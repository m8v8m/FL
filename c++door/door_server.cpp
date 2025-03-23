#include "values.h"
SOCKET we_socket;            //创建socket对象 
void initialization() {
	WSADATA wsd;         //定义WSADATA对象
	WSAStartup(MAKEWORD(2, 2), &wsd);
          
	sockaddr_in we_addr_info;         //创建sockaddr_in对象储存自身信息(当有多个端口，可以多个绑定)
	//初始化套接字库
	we_addr_info.sin_family = AF_INET;             //设置服务器地址家族
	we_addr_info.sin_port = htons(6369);           //设置服务器端口号
	we_addr_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	we_socket = socket(AF_INET, SOCK_STREAM, 0);

	int len = sizeof(we_addr_info);
	int i = bind(we_socket, (sockaddr*)&we_addr_info, len);    //把名字和套接字绑定
	if (i!=0){printf("WSAStartup failed: %d\n", WSAGetLastError());}
	if (listen(we_socket, 0) < 0){
		std::cout << "设置监听状态失败！" << std::endl;
		WSACleanup();
	}
	we_socket=accept(we_socket, (sockaddr*)&we_socket, &len); 
	std::cout<<"new connect:"<<inet_ntoa(we_addr_info.sin_addr)<<std::endl;

}
int main(){
	initialization();
	
	char recv_msg[1024];
	char send_msg[1024];
	
	while (1){
		memset(recv_msg, 0, sizeof(recv_msg));
		memset(send_msg, 0, sizeof(send_msg));
		std::cout<<"          input***";
		std::cin.getline(send_msg,65);
		send(we_socket, send_msg, sizeof(send_msg), 0);
		
		recv(we_socket, recv_msg, sizeof(recv_msg), 0); 
		std::cout<<sizeof(recv_msg)<<std::endl;
		printf("[recv_msg]-->%s\n",recv_msg);

		}
	WSACleanup();
	
	
	return 0;
}
//if (!strcmp(send_msg, "exit"))                     //如果输入的是exit则断开连接

