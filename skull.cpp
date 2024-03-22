// skull.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "skull.h"
#include "windowsx.h"
#include "psapi.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
char processName[MAX_PATH];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HWND g_hWnd = nullptr;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SKULL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SKULL));

    MSG msg;

    POINT prev{ 0, 0 };
    DWORD pidPrev = 0;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        short rbut = GetAsyncKeyState(VK_RBUTTON);
        short esc = GetAsyncKeyState(VK_ESCAPE);
        if (rbut != 0 || esc != 0)
            break;
        short lbut = GetAsyncKeyState(VK_LBUTTON);
        short lshift = GetAsyncKeyState(VK_LSHIFT);
        if (lbut && lshift)
        {
            HANDLE Handle = OpenProcess(
                PROCESS_TERMINATE,
                FALSE,
                pidPrev /* This is the PID, you can find one from windows task manager */
            );
            if (Handle != nullptr)
            {
                TerminateProcess(Handle, -1);
            }
        }
        POINT p;
        GetCursorPos(&p);
        if (p.x != prev.x || p.y != prev.y)
        {
            HWND hWnd = WindowFromPoint(p);
            DWORD pid;
            GetWindowThreadProcessId(hWnd, &pid);
            if (pid != pidPrev)
            {
                HANDLE Handle = OpenProcess(
                    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                    FALSE,
                    pid /* This is the PID, you can find one from windows task manager */
                );
                if (Handle)
                {
                    if (GetModuleFileNameExA(Handle, 0, processName, MAX_PATH))
                    {
                        InvalidateRect(g_hWnd, nullptr, true);
                    }
                    else
                    {
                        // You better call GetLastError() here
                    }
                    CloseHandle(Handle);
                }

                pidPrev = pid;
            }
            prev = p;
        }

    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SKULL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   g_hWnd = CreateWindowW(szWindowClass, szTitle, 0,
      0, 0, 100, 100, nullptr, nullptr, hInstance, nullptr);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   SetTimer(g_hWnd,             // handle to main window 
       1,            // timer identifier 
       10,
       (TIMERPROC)NULL);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            size_t len = strlen(processName);
            RECT r;
            GetClientRect(hWnd, &r);
            DrawTextA(hdc, processName, -1, &r, 0);
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
