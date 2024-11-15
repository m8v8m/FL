import socket,os,struct
 
# 实例化
sk = socket.socket()
# 定义连接的ip和port
server_ip = input("请输入目标ip")
if server_ip=="":
    server_ip='127.0.0.1'

# 连接到服务器
sk.connect((server_ip, 5555))
 
while True:
    send_msg = input("请输入要发送给服务端的消息：")
    if send_msg == "":
        send_msg="hi"
    # 发送消息
    elif  "upload" in send_msg:
        send_file=send_msg.replace("@upload ","")
        print("you try upload file--->"+send_file)
        if os.path.isfile(send_file):
                    # 定义文件头信息，包含文件名和文件大小
                    #fhead_msg = struct.pack(str(len(send_file))+"s", os.path.basename(send_file).encode("utf-8"))
                    sk.send(send_msg.encode("UTF-8"))
                    if sk.recv(4)==b'ok':
                        print("start send")
                        sk.send(str(os.stat(send_file).st_size).encode("UTF-8"))
                        fp = open(send_file, 'rb')
                        while 1:
                            data = fp.read(1024)
                            if not data and sk.recv(1024)==b'success':
                                print("send succes")
                                break
                            sk.send(data)
    else:
        sk.send(send_msg.encode("UTF-8"))
    # 接受消息
        recv_data = sk.recv(1024).decode("gbk")    # 1024是缓冲区大小，一般就填1024， recv是阻塞式
        print(f"服务端回复的消息是：{recv_data}")
 
# 关闭连接

