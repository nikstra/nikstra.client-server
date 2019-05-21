#ifndef WINMAIN_H
#define WINMAIN_H

#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
	version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

enum
{
	ID_FILE_EXIT,
	ID_FILE_MSG,
	IDC_EDIT_CTRL,
	IDC_BUTTON_SHOW_MSG,
	IDC_LISTBOX_CTRL
};

#define ADDPOPUPMENU(hmenu, string) \
	HMENU hSubMenu = CreatePopupMenu(); \
	AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, string);

#define ADDMENUITEM(hmenu, ID, string) \
	AppendMenu(hSubMenu, MF_STRING, ID, string);

LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CreateWin32Window(HINSTANCE hInstance, int width, int height);
int fInsertListViewColumn(HWND hwndListView,
	long lCol, int iPercent, unsigned char* text);
int fInsertListViewItem(HWND hwndListView, long lLin,
	long lCol, int iSubItemYesNo, unsigned char* text);

#endif /* !WINMAIN_H */
