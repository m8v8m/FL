
#include "values.h"

SOCKET he_socket;            //创建socket对象           
sockaddr_in he_addr_info; 
void initialization() {
	WSADATA wsd;         //定义WSADATA对象
	WSAStartup(MAKEWORD(2, 2), &wsd);
	he_addr_info.sin_family = AF_INET;             //设置服务器地址家族
	he_addr_info.sin_port = htons(6369);           //设置服务器端口号
	he_addr_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	he_socket = socket(AF_INET, SOCK_STREAM, 0);
}

int main()
{

	initialization();
	
	int con_status=connect(he_socket, (sockaddr*)&he_addr_info,sizeof(he_addr_info));
	cout<<con_status;
	if (con_status<0){printf("con_status failed: %d\n", WSAGetLastError());}
	while(con_status==0){
		//memset(recv_msg, 0, sizeof(recv_msg));
		memset(recv_msg, 0, 1024);
		memset(send_msg, 0, 1024);
		recv(he_socket, recv_msg, sizeof(recv_msg), 0); 
		std::cout<<"[recv_msg]-->"<<recv_msg<<std::endl;
		switch (recv_msg[0]) {
		case *"@":
			extend_cmd(recv_msg);
			break;
		case *"$":
			send_file(he_socket,recv_msg);
			if (recv(he_socket, recv_msg, sizeof(recv_msg), 0)>0){
				cout<<recv_msg;
				strcpy(send_msg,"file download success!");	
			break;
		default:
			strcpy(send_msg,"default msg");		
			break;
		}

		std::cout<<"###############################################"<<std::endl;
		send(he_socket, send_msg, sizeof(send_msg), 0); 
		cout<<"send:"<<send_msg<<endl;
		
	}
	closesocket(he_socket);
	WSACleanup();
	return 0;
}




