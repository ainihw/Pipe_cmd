#include <Windows.h>
#include "resource.h"

BOOL CALLBACK _DialogMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void _stdcall _DealPipe(HWND hWnd, HANDLE hMyWrite, HANDLE hCmdRead, HANDLE hMyRead, HANDLE hCmdWrite);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pszCmdLine, int iCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, _DialogMain, 0);
	return 0;
}

BOOL CALLBACK _DialogMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HANDLE				hMyWrite, hCmdRead, hMyRead, hCmdWrite;
	static STARTUPINFO			si;
	static PROCESS_INFORMATION	pi;
	static SECURITY_ATTRIBUTES	as;
	HANDLE hIcon;
	char szCmdLine[256] = TEXT("121212");
	int n;
	switch (message)
	{

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			_DealPipe(hWnd, hMyWrite, hCmdRead, hMyRead, hCmdWrite);
			break;
		}
		break;
	case WM_INITDIALOG:
		as.nLength			= sizeof(SECURITY_ATTRIBUTES);
		as.bInheritHandle	= TRUE;								//管道句柄可被继承
		CreatePipe(&hCmdRead, &hMyWrite, &as, 0);				//创建my  ->  cmd的管道
		CreatePipe(&hMyRead, &hCmdWrite, &as, 0);				//创建my  <-  cmd的管道
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = hCmdRead;
		si.hStdOutput = hCmdWrite;
		si.hStdError = hCmdWrite;
	
		CreateProcess("C:\\Windows\\System32\\cmd.exe",
			szCmdLine,
			NULL,
			NULL,
			TRUE,	//继承
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&si,
			&pi);
		

		hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, LPARAM(hIcon));
		break;
	case WM_CLOSE:
		CloseHandle(hMyRead);
		CloseHandle(hMyWrite);
		CloseHandle(hCmdRead);
		CloseHandle(hCmdWrite);
		EndDialog(hWnd, 0);
		break;
	default:
		return FALSE;
	}
	
	return TRUE;
}




void _stdcall _DealPipe(HWND hWnd, HANDLE hMyWrite, HANDLE hCmdRead, HANDLE hMyRead, HANDLE hCmdWrite)
{
	DWORD	dwSize;
	TCHAR	szBuffer[256] = {0};
	TCHAR	szBuffer1[0x256] = { 0 };
	TCHAR	szBuffer2[0x1000] = { 0 };
	GetDlgItemText(hWnd, IDC_EDIT1, szBuffer, sizeof(szBuffer));
	lstrcat(szBuffer, TEXT("\r\n"));
	
	WriteFile(hMyWrite, szBuffer, lstrlen(szBuffer), NULL, NULL);
	Sleep(100);
	int i = 0;
	while (1)
	{
		memset(szBuffer1, 0, sizeof(szBuffer1));
		ReadFile(hMyRead, szBuffer1, 256, &dwSize, NULL);
		lstrcat(szBuffer2, szBuffer1);
		if (dwSize!=256)
			break;
		i++;
	}
	


	//实现edit追加写
	SendMessage(GetDlgItem(hWnd, IDC_EDIT2), EM_SETSEL, -2, -1);					//选中文本末
	SendMessage(GetDlgItem(hWnd, IDC_EDIT2), EM_REPLACESEL, true, (long)szBuffer2);	//进行文本替换

}