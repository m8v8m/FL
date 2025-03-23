//#include "values.h"
#include <iostream>
#include<cstring>
using namespace std;
int main(){
	char send_msg[1024];
	char recv_msg[]="@ipconfig";
	char dd[1024];
	string fuk;
	cout<<sizeof(recv_msg)<<endl;
	for(int i=0; i+1<sizeof(recv_msg);i=i+1)
	{
		recv_msg[i]=recv_msg[i+1];
	}
	cout<<sizeof(recv_msg)<<endl;
	FILE *fp = _popen(recv_msg, "r");

	if ((fp) == NULL) {
		cout<<"file"<<endl;
	}else{
		fuk=send_msg;
		while (fgets(send_msg, 1024, fp) != NULL) {
			//cout<<send_msg<<endl;
			strcat(dd,send_msg);
			//memcpy(dd,send_msg,sizeof(send_msg+1));
			//printf("%s\n", send_msg);
			//cout<<dd<<endl;
			cout<<666<<endl;
		}
		cout<<dd<<endl;
		memcpy(send_msg,dd,sizeof(dd));
		printf("%s\n", send_msg);
	}
	_pclose(fp);
	//cout<<send_msg<<endl;
	cin.getline(send_msg,10);
	cout<<send_msg;
	return 0;
}


