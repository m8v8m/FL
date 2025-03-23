
#include "values.h"


void initialization();

int main()
{
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	initialization();
	while(1){

		if (listen(m_SockServer, 0) < 0){
			std::cout << "设置监听状态失败！" << std::endl;
			WSACleanup();
		}
		*m_Server = accept(m_SockServer, (sockaddr*)&serveraddrfrom, &len);
		int ires = send(*m_Server, target_msg, sizeof(target_msg), 0); 
	}
	closesocket(*m_Server);
	WSACleanup();
	return 0;
}




