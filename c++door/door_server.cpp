#include "values.h"
int mian(){
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	initialization();
	return 0;
}
