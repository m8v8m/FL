import socket
socket_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket_client.connect(('localhost', 5010))
while True:
    data = input('客户端：')
    if data == 'exit':
        break
    socket_client.send(data.encode('UTF-8'))
    reply = socket_client.recv(1024)#会阻塞,即一直等待服务端的消息,就会卡死,可以使用异步
    try:
        print(reply.decode('gbk'))
    except UnicodeDecodeError:
        print("编码发生错误,自动适应utf-8")
        print(reply.decode('utf-8'))

socket_client.close()
