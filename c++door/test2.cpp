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
int main(void)
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "ScreenCaptureReceiverWindowClass";
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);
	RegisterClass(&wc);
	HWND EYE_hwnd = CreateWindowEx(0, wc.lpszClassName, "EYE", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, nWidth*3/4, nHeight*3/4, NULL, NULL, wc.hInstance, NULL);
	ShowWindow(EYE_hwnd, SW_SHOW);
	
	//int32_t ScrWidth = 0, ScrHeight = 0;
	//const char *filename="G:\\github_clone\\FL\\c++door\\121.bmp";
	//ScrWidth = GetDeviceCaps(hdc, HORZRES)/2;
	//ScrHeight = GetDeviceCaps(hdc, VERTRES)/2;
	
	
	
	
	
	HDC hdc_see = GetDC(EYE_hwnd);
	HDC hmdc_see = CreateCompatibleDC(hdc_see);
	HBITMAP hBmpScreen_see = CreateCompatibleBitmap(hmdc_see, nWidth, nHeight);
	SelectObject(hmdc_see, hBmpScreen_see);
	BITMAP bm_see;
	GetObject(hBmpScreen_see, sizeof(bm_see), &bm_see);
	BITMAPINFOHEADER bi_see = { 0 };
	bi_see.biSize = sizeof(BITMAPINFOHEADER);
	bi_see.biWidth = bm_see.bmWidth;
	bi_see.biHeight = bm_see.bmHeight;
	bi_see.biPlanes = 1;
	bi_see.biBitCount = 24;
	bi_see.biCompression = BI_RGB;
	bi_see.biSizeImage = bm_see.bmHeight * bm_see.bmWidthBytes;
	
	char *buf = new char[bi_see.biSizeImage];
	//BitBlt 函数执行与像素矩形相对应的颜色数据的位块传输，从指定的源设备上下文传输到目标设备上下文。
	//     目标HDC                         源HDC
	//BitBlt(hdc_see, 0, 0, nWidth, nHeight, hdc, 0, 0, SRCCOPY);
	//GetDIBits 函数检索指定兼容位图的位，并使用指定格式将其作为 DIB 复制到缓冲区中。远程共享需要
	//        HDC句柄   位图的句柄                     缓冲区   指定格式
	//GetDIBits(hdc_see, hBmpScreen, 0L, (DWORD)nHeight, buf, (LPBITMAPINFO)&bi_see, (DWORD)DIB_RGB_COLORS);
	HDC hdc =  GetWindowDC(NULL);
	cout<<hdc<<endl;
	HDC hmdc = CreateCompatibleDC(hdc);
	HBITMAP hBmpScreen = CreateCompatibleBitmap(hdc, nWidth, nHeight);
	//SelectObject(hmdc, hBmpScreen);
	int len = nWidth * nHeight * 3;
	//recv(s, (char*)bBits, len, 0);
	SetStretchBltMode(hdc_see, HALFTONE);
	StretchBlt(hdc_see,0,0,nWidth*3/5,nHeight*3/5,hdc,0,0,nWidth, nHeight, SRCCOPY);
	//StretchBlt(hdc_see,0,0,nWidth,nHeight,hdc,0,0,nWidth, nHeight, SRCCOPY);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	


	
	
}

