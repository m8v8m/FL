#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <vector>
#include<iostream>
using namespace std;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
std::vector<int> getxy() {
	HWND hWnd = GetDesktopWindow();//根据需要可以替换成自己程序的句柄 
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	
	DEVMODE dm;
	MONITORINFOEX miex;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	// 获取监视器物理宽度与高度
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	int cyPhysical = dm.dmPelsHeight;
	vector<int> ret;
	ret.push_back(cxPhysical);
	ret.push_back(cyPhysical);
	
	return ret;
}
class xyz {
public:
	int x;
	int y;
};
int main(void)
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "ScreenCaptureReceiverWindowClass";
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);
	vector<int> xy = getxy();
	int screenWidth = xy[0];
	int screenHeight = xy[1];
	xyz data;
	data.x = screenWidth;
	data.y = screenHeight;
	int xyd[2]={nWidth,nHeight};

	printf("%d\n",xyd);
	cout<<xyd[0];
	
	
	
	
}

