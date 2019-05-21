#include <stdio.h>
#include <winsock.h>
#include "WinMain.h"
#include "net_client.h"

#pragma comment(lib,"ws2_32.lib")
#define MSG_LEN 200

const char *clsName = "WinAPI";
char* title = "Data Client";
BOOL running = TRUE;
HWND hWnd = NULL;
ClientConnection* client_conn;
#if false
SOCKET ConnectToServer(char* server_address, int port)
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char buf[MSG_LEN];

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		sprintf_s(buf, MSG_LEN, "Failed. Error Code: % d", WSAGetLastError());
		MessageBox(hWnd, buf, "WinSock Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		sprintf_s(buf, MSG_LEN, "Could not create socket: % d", WSAGetLastError());
		MessageBox(hWnd, buf, "WinSock Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	server.sin_addr.s_addr = inet_addr(server_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(s, (struct sockaddr*) &server, sizeof(server)) < 0)
	{
		sprintf_s(buf, MSG_LEN, "Could not connect to server: % d", WSAGetLastError());
		MessageBox(hWnd, buf, "WinSock Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	return s;
}

void CloseConnection(SOCKET s)
{
	closesocket(s);
	WSACleanup();
}

int SendCommand(SOCKET s, char* command)
{
	char* server_reply[2000];
	char buf[MSG_LEN];
	int recv_size;

	if (send(s, command, strlen(command), 0) < 0)
	{
		sprintf_s(buf, MSG_LEN, "Send failed: % d", WSAGetLastError());
		MessageBox(hWnd, buf, "WinSock Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
	{
		sprintf_s(buf, MSG_LEN, "Receive failed: % d", WSAGetLastError());
		MessageBox(hWnd, buf, "WinSock Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	//Add a NULL terminating character to make it a proper string before printing
	server_reply[recv_size] = '\0';

	return 0;
}
#endif

int fInsertListViewColumn(HWND hwndListView, long lCol,
	int iPercent, unsigned char* text)
{
	/*********************
	* Local declarations *
	*********************/

	// LVCOLUMN struct (see MSDN for content):
	LVCOLUMN lvcolumn;

	// RECT struct, used in column width and 
	// WM_PAINT jobs:
	RECT rect;
	// Enter the client area of the ListView 
	//into the RECT structure:
	GetClientRect(hwndListView, &rect);

	// To appear in the caption of the MessageBox:
	static unsigned char* sYourApplicationsName = "you name it :)";
	// Error 'string' you can use to 'MessageBox' and/or to 
	// 'fprintf' into a logfile:
	unsigned char sErrorString[5000];

	// Value to be returned after the function tried 
	// to insert a column:
	int iResult;

	/*************************
	* Minimum error checking *
	*************************/

	// iPercent adjustement before display, e.g.:
	iPercent = iPercent > 10 ? min(iPercent, 90) : 10;

	// Column width, with 'rect.right' the x-coordinate of 
	// the lower-right corner of the RECT:
	int iWidth = (int)(rect.right * (iPercent / 100.0));

	// Check ListView handle:
	if (hwndListView == NULL)
	{
		sprintf_s(sErrorString, sizeof(sErrorString),
			"! Handle of ListView NULL (fInsertListViewColumn)");
		MessageBox(NULL, (LPSTR)sErrorString,
			(LPSTR)sYourApplicationsName, MB_OK | MB_ICONEXCLAMATION);
		return(0);
	}

	/***********************************************
	* Use a LVCOLUMN struct to describe the column *
	* that you want to insert into the ListView    *
	***********************************************/

	// Initialize the LVCOLUMN struct (see LVCF_xxx and 
	// LVCFMT_xxx values in MSDN):
	memset(&lvcolumn, 0, sizeof(lvcolumn));
	// Necessary mask values for this job:
	lvcolumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	// Left-align the text:
	lvcolumn.fmt = LVCFMT_LEFT;
	// Point to the right address:
	lvcolumn.pszText = text;
	// Nota bene: you might want to check lCol value first
	lvcolumn.iSubItem = lCol;
	// Set column width:
	lvcolumn.cx = iWidth;

	// SendMessage returns the number (zero based) of the 
	// column inserted, 
	// or -1 if it fails:
	if (SendMessage((HWND)hwndListView, (UINT)LVM_INSERTCOLUMN,
		(WPARAM)(int)lCol, (LPARAM)& lvcolumn) == -1)
		iResult = 0; else iResult = 1;

	// Paints (updates) the client area of the ListView control:
	InvalidateRect(hwndListView, &rect, TRUE);

	// Returns either 1 (OK) or 0 (failed to insert column - e.g., 
	// lCol too big):
	return(iResult);
}

int fInsertListViewItem(HWND hwndListView, long lLin,
	long lCol, int iSubItemYesNo, unsigned char* text)
{
	/*********************
	* Local declarations *
	*********************/

	// LVITEM struct (see MSDN for content):
	LVITEM lvi;

	// RECT struct, used in column width and WM_PAINT jobs:
	RECT rect;
	// Enter the client area of the ListView into
	// the RECT structure:
	GetClientRect(hwndListView, &rect);

	// To appear in the caption of the MessageBox:
	static unsigned char* sYourApplicationsName = "you name it :)";
	// Error string you can use to 'MessageBox' and/or to 'fprintf' 
	// into a logfile:
	unsigned char sErrorString[5000];

	// Value to be returned after the function tried to insert 
	// an item / subitem:
	int iResult;

	/*************************
	* Minimum error checking *
	*************************/

	// Check ListView handle:
	if (hwndListView == NULL)
	{
		sprintf_s(sErrorString, sizeof(sErrorString),
			"! Handle of ListView NULL (fInsertListViewItem)");
		MessageBox(NULL, (LPSTR)sErrorString,
			(LPSTR)sYourApplicationsName, MB_OK | MB_ICONEXCLAMATION);
		return(0);
	}

	/*****************************************************
	* Use a LVITEM struct to describe the item / subitem *
	* that you want to insert into the ListView          *
	*****************************************************/

	// Initialize the LVITEM struct (see LVIF_xxx reference in MSDN):
	memset(&lvi, 0, sizeof(lvi));
	// Minimum mask value for this job:
	lvi.mask = LVIF_TEXT;
	lvi.state = 0;
	lvi.stateMask = 0;
	// Point to the right address:
	lvi.pszText = text;
	// Nota bene: you might want to check lLin and lCol values first
	lvi.iItem = lLin;
	lvi.iSubItem = lCol;

	switch (iSubItemYesNo)
	{
	case 0:
		// Send LVM_INSERTITEM message if you want to 
		// insert an item (returns -1 if it fails):
		if (SendMessage((HWND)hwndListView,
			(UINT)LVM_INSERTITEM, (WPARAM)0, (LPARAM)& lvi) == -1)
			iResult = 0; else iResult = 1;
		break;
	case 1:
		// Send LVM_SETITEM message if you want to insert 
		// a subitem (returns FALSE if it fails):
		if (SendMessage((HWND)hwndListView,
			(UINT)LVM_SETITEM, (WPARAM)0, (LPARAM)& lvi) == FALSE)
			iResult = 0; else iResult = 1;
		break;
	default:
		sprintf_s(sErrorString, sizeof(sErrorString),
			"! Unexpected iSubItemYesNo value: %d (fInsertListViewItem)",
			iSubItemYesNo);
		MessageBox(NULL, (LPSTR)sErrorString,
			(LPSTR)sYourApplicationsName, MB_OK | MB_ICONEXCLAMATION);
		return(0);
		break;
	}

	// Paints (updates) the client area of the ListView control:
	InvalidateRect(hwndListView, &rect, TRUE);

	// Returns either 1 (OK) or 0 (failed to insert an item / subitem):
	return(iResult);
}

void ShowErrorAndExit(char* message, char* title)
{
	MessageBox(hWnd, message, title, MB_OK | MB_ICONERROR);
	PostQuitMessage(0);
	running = FALSE;
}

void CreateMainMenu(HWND hWnd)
{
	HMENU hMenu = CreateMenu();

	ADDPOPUPMENU(hMenu, "&File");
	ADDMENUITEM(hMenu, ID_FILE_MSG, "&Show Message");
	ADDMENUITEM(hMenu, ID_FILE_EXIT, "&Exit");

	SetMenu(hWnd, hMenu);
}

void CreateControls(HWND hWnd, HINSTANCE hInstance)
{
	HWND hStaticText = NULL;
	HWND hButton = NULL;
	HWND hEdit = NULL;
	HWND hGroupBox = NULL;
	HWND hListBox = NULL;
	HWND hListView = NULL;

	INITCOMMONCONTROLSEX icex;
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	if ((hGroupBox = CreateWindow("Button",
		"Data search",
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		20, 20, 740, 500,
		hWnd, NULL,
		hInstance, NULL)) == NULL)
	{
		ShowErrorAndExit("Failed to create control.", "Error");
	}

	if ((hEdit = CreateWindow("Edit",
		NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		40, 50, 300, 25,
		hWnd, (HMENU)IDC_EDIT_CTRL,
		hInstance, NULL)) == NULL)
	{
		ShowErrorAndExit("Failed to create control.", "Error");
	}

	if ((hButton = CreateWindow("Button",
		"Search",
		BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
		350, 50, 120, 28,
		hWnd, (HMENU)IDC_BUTTON_SHOW_MSG,
		hInstance, NULL)) == NULL)
	{
		ShowErrorAndExit("Failed to create control.", "Error");
	}

	if ((hStaticText = CreateWindow("Static",
		"Search results",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		40, 85, 700, 20,
		hWnd, NULL, hInstance, NULL)) == NULL)
	{
		ShowErrorAndExit("Failed to create control.", "Error");
	}
	// TODO: https://docs.microsoft.com/en-us/windows/desktop/controls/create-a-simple-list-box
	//if ((hListBox = CreateWindow("ListBox",
	//	NULL,
	//	WS_VISIBLE | WS_CHILD | WS_BORDER,
	//	40, 115, 700, 400,
	//	hWnd, (HMENU)IDC_LISTBOX_CTRL,
	//	hInstance, NULL)) == NULL)
	//{
	//	ShowErrorAndExit("Failed to create control.", "Error");
	//}

	if ((hListView = CreateWindow(WC_LISTVIEW,
		NULL,
		/*WS_VISIBLE | WS_CHILD | WS_BORDER,*/
		WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE | WS_BORDER,
		40, 115, 700, 380,
		hWnd, (HMENU)IDC_LISTBOX_CTRL,
		hInstance, NULL)) == NULL)
	{
		ShowErrorAndExit("Failed to create control.", "Error");
	}

	int iColumnsToInsert = 5, iLinesToInsert = 8, i, j;
	unsigned char sListViewText[200];
	// Remember 65 is ASCII decimal for 'A', 66 is 'B', etc.:
	for (i = 0; i < iColumnsToInsert; i++)
	{
		sprintf_s(sListViewText, 200, "%c (column)", 65 + i);
		// Columns are labeled 'A', 'B', ... , 'E':
		fInsertListViewColumn(hListView, i, 100 / iColumnsToInsert, sListViewText);
	}
	for (i = 0; i < iColumnsToInsert; i++)
	{
		for (j = 0; j < iLinesToInsert; j++)
		{
			// Items / subitems are labeled 'A1', 'A2', ... , 'E8':
			if (i == 0) sprintf_s(sListViewText, 200, "%c%d (item)", 65 + i, j + 1);
			else sprintf_s(sListViewText, 200, "%c%d (subitem)", 65 + i, j + 1);
			// Here iSubItemYesNo=0 if i is 0; iSubItemYesNo=1 otherwise:
			fInsertListViewItem(hListView, j, i, i == 0 ? 0 : 1, sListViewText);
		}
	}
}

/*
 * WinMain()
 */
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	MSG msg;
	
	if(!CreateWin32Window(hInstance, 800, 600))
		return 1;

	CreateMainMenu(hWnd);
	CreateControls(hWnd, hInstance);
	ShowWindow(hWnd, SW_SHOW);
	client_conn = new_NetClient("127.0.0.1", 8888);

	/* Main message loop */
	while (running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	NetClient_free(client_conn);
	return msg.wParam;
}

BOOL CreateWin32Window(HINSTANCE hInstance, int width, int height)
{
	WNDCLASSEX WndEx;

	WndEx.cbSize = sizeof(WNDCLASSEX);
	WndEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WndEx.lpfnWndProc = (WNDPROC)WndProc;
	WndEx.cbClsExtra = 0;
	WndEx.cbWndExtra = 0;
	WndEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndEx.hbrBackground = NULL;
	WndEx.lpszMenuName = NULL;
	WndEx.lpszClassName = clsName;
	WndEx.hInstance = hInstance;
	WndEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&WndEx))
	{
		MessageBox(NULL, "Failed to register class.", "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!(hWnd = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		clsName,
		title,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		MessageBox(NULL, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

/* Callback */
LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		running = FALSE;
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			running = FALSE;
			break;
		case ID_FILE_MSG:
			MessageBox(hWnd, "Hello World!", "Message", MB_ICONINFORMATION);
			break;
		case IDC_BUTTON_SHOW_MSG:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				if (NetClient_connect(client_conn) != 0)
				{
					MessageBox(hWnd, NetClient_read_status(client_conn), "NetClient", MB_ICONERROR);
				}
				static char text[256];
				GetWindowText(GetDlgItem(hWnd, IDC_EDIT_CTRL), (LPSTR)text, 256);

				if (NetClient_query(client_conn, text) != 0)
				{
					MessageBox(hWnd, NetClient_read_status(client_conn), "NetClient", MB_ICONERROR);
				}

				MessageBox(hWnd, text, "Click", MB_ICONINFORMATION);
			}
			break;
		case IDC_EDIT_CTRL:
			break;
			if (HIWORD(wParam) == EN_CHANGE)
			{
				static char text[256];
				GetWindowText(GetDlgItem(hWnd, IDC_EDIT_CTRL), (LPSTR)text, 256);
				SetWindowText(hWnd, (LPSTR)text);
			}
		}
		break;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (INT_PTR)(HBRUSH)GetStockObject(WHITE_BRUSH);
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
