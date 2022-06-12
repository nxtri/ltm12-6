#include "framework.h"
#include "ChatClient.h"
#include <WinSock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define MAX_LOADSTRING 100

SOCKET client;
bool isLoggedIn = false;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitLoginInstance(HINSTANCE, int);
BOOL                InitChatClientInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int LoginWindow(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow);
int ChatClientWindow(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow);
void AddUser(HWND hWnd, char* user_id);
void DelUser(HWND hWnd, char* user_id);
void RecvMessageFromUser(HWND hWnd, char* user_id, char* mess);
void RecvMessageFromGroup(HWND hWnd, char* user_id, char* mess);


int LoginWindow(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    // Perform application initialization:
    if (!InitLoginInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

int ChatClientWindow(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR    lpCmdLine,
                    _In_ int       nCmdShow) 
{
    // Perform application initialization:
    if (!InitChatClientInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // TODO: Place code here.

    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8000);

    connect(client, (sockaddr*) &addr, sizeof(addr));

    

    LoginWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    if (isLoggedIn) {
        ChatClientWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

    return 0;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitLoginInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, 400, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   WSAAsyncSelect(client, hWnd, WM_SOCKET_LOGIN, FD_READ | FD_CLOSE);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
       40, 30, 300, 40, hWnd, (HMENU)IDC_EDIT_LOGIN, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Log in"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
       120, 80, 120, 40, hWnd, (HMENU)IDC_BUTTON_LOGIN, GetModuleHandle(NULL), NULL);

   return TRUE;
}

BOOL InitChatClientInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    WSAAsyncSelect(client, hWnd, WM_SOCKET_CHATCLIENT, FD_READ | FD_CLOSE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
        10, 10, 400, 350, hWnd, (HMENU)IDC_LIST_MESSAGE, GetModuleHandle(NULL), NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
        420, 10, 150, 350, hWnd, (HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 360, 400, 40, hWnd, (HMENU)IDC_EDIT_MESSAGE, GetModuleHandle(NULL), NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("SEND"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        420, 360, 150, 40, hWnd, (HMENU)IDC_BUTTON_SEND, GetModuleHandle(NULL), NULL);

    SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0,
        (LPARAM)"ALL");

    return TRUE;
}

LRESULT CALLBACK LoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SOCKET_LOGIN:
    {
        if (WSAGETSELECTERROR(lParam)) {
            closesocket(wParam);
        }
        else {
            if (WSAGETSELECTEVENT(lParam) == FD_READ) {
                char buf[1024];
                int ret = recv(client, buf, sizeof(buf), 0);

                if (ret == SOCKET_ERROR) {
                    closesocket(client);
                }

                buf[ret] = 0;

                char protocol[25], status[25];
                sscanf(buf, "%s %s", protocol, status);

                if (strcmp(protocol, CONNECT) == 0 && strcmp(status, "OK") == 0) {
                    DestroyWindow(hWnd);
                    isLoggedIn = true;
                }
                
            }
            if (WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
                closesocket(client);
            }
        }
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
        {
            DestroyWindow(hWnd);
            break;
        }
        case IDC_BUTTON_LOGIN:
        {
            char user_id[256];
            GetDlgItemTextA(hWnd, IDC_EDIT_LOGIN, user_id, sizeof(user_id));

            char buf[1024];
            sprintf(buf, "[CONNECT] %s", user_id);

            send(client, buf, strlen(buf), 0);
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SOCKET_LOGIN:
    {
        if (WSAGETSELECTERROR(lParam)) {
            closesocket(wParam);
        }
        else {
            if (WSAGETSELECTEVENT(lParam) == FD_READ) {
                char buf[1024];
                int ret = recv(client, buf, sizeof(buf), 0);

                if (ret == SOCKET_ERROR) {
                    closesocket(client);
                }

                buf[ret] = 0;

                char protocol[25], status[25];
                sscanf(buf, "%s %s", protocol, status);

                if (strcmp(protocol, CONNECT) == 0 && strcmp(status, "OK") == 0) {
                    DestroyWindow(hWnd);
                    isLoggedIn = true;
                }

            }
            if (WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
                closesocket(client);
            }
        }
        break;
    }
    case WM_SOCKET_CHATCLIENT:
    {
        if (WSAGETSELECTERROR(lParam)) {
            closesocket(wParam);
        }
        else {
            if (WSAGETSELECTEVENT(lParam) == FD_READ) {
                char buf[1024];
                int ret = recv(client, buf, sizeof(buf), 0);

                if (ret == SOCKET_ERROR) {
                    closesocket(client);
                }

                buf[ret] = 0;

                char protocol[25], user_id[25];
                sscanf(buf, "%s %s", protocol, user_id);

                if (strcmp(protocol, USER_CONNECT) == 0) {
                    
                    AddUser(hWnd, user_id);
                }
                else if (strcmp(protocol, USER_DISCONNECT) == 0) {

                    DelUser(hWnd, user_id);
                }
                else if (strcmp(protocol, MESSAGE) == 0) {
                    
                    RecvMessageFromUser(hWnd, user_id, buf + strlen(MESSAGE) + strlen(user_id) + 2);
                }
                else if (strcmp(protocol, MESSAGE_ALL) == 0) {
                    
                    RecvMessageFromGroup(hWnd, user_id, buf + strlen(MESSAGE_ALL) + strlen(user_id) + 2);
                }
            }
            if (WSAGETSELECTEVENT(lParam) == FD_CLOSE) {
                closesocket(client);
            }
        }
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
            {
                send(client, DISCONNECT, strlen(DISCONNECT), 0);
                closesocket(client);
                DestroyWindow(hWnd);
                break;
            }
            case IDC_BUTTON_LOGIN:
            {
                char user_id[256];
                GetDlgItemTextA(hWnd, IDC_EDIT_LOGIN, user_id, sizeof(user_id));

                char buf[1024];
                sprintf(buf, "[CONNECT] %s", user_id);

                send(client, buf, strlen(buf), 0);
                break;
            }
            case IDC_BUTTON_SEND:
            {
                //Gui tin nhan [SEND]
                char message[256], user_id[256];
                GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, message, sizeof(message));

                int user_index = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETCURSEL, 0, 0);
                int ret = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETTEXT, user_index, (LPARAM)user_id);

                if (ret <= 0)
                    break;

                char buf[1024];
                sprintf(buf, "[SEND] %s %s", user_id, message);

                send(client, buf, strlen(buf), 0);
                break;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void AddUser(HWND hWnd, char* user_id) {
    SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)user_id);

    char buf[256];
    sprintf(buf, "%s connected!", user_id);

    SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);
}

void DelUser(HWND hWnd, char* user_id) {
    char user_name[25];
    int user_index = 0;

    while (1) {
        int ret = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETTEXT, user_index, (LPARAM)user_name);
        if (ret <= 0) break;

        if (strcmp(user_name, user_id) == 0) {
            SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_DELETESTRING, user_index, (LPARAM)user_name);

            char buf[256];
            sprintf(buf, "%s disconnected!", user_id);

            SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);

            break;
        }

        user_index++;
    }
    
}

void RecvMessageFromUser(HWND hWnd, char* user_id, char* mess) {

    char buf[1024];
    sprintf(buf, "From %s to You: %s", user_id, mess);

    SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);
}

void RecvMessageFromGroup(HWND hWnd, char* user_id, char* mess) {

    char buf[1024];
    sprintf(buf, "From %s to Group: %s", user_id, mess);

    SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);
}