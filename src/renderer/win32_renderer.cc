#ifndef UNICODE
#define UNICODE
#endif 
// if platform == windows, include this file.
#include <windows.h>
#pragma comment(lib,"user32.lib") // this is needed for unresolved symbols elsewhere? weird.
#undef max
#undef min
#include <fmt/core.h>
#include <fmt/printf.h>
#include <limits> // for fmt error

// set_std_out_to_console();
#include "guicon.h"
#include <iostream>

// #include <GL/gl.h>
// #include <GLEW.h>

// this works!
static void redirect_output()
{
	// Create the console window and set the window title.
	if (AllocConsole() == 0)
	{
	// Handle error here. Use ::GetLastError() to get the error.
	}

	// Redirect CRT standard input, output and error handles to the console window.
	FILE * pNewStdout = nullptr;
	FILE * pNewStderr = nullptr;
	FILE * pNewStdin = nullptr;

	::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
	::freopen_s(&pNewStdin, "CONIN$", "r", stdin);

	// Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
	// to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
	// which seems to occur in newer version of Visual Studio even when the console has not been read from or written
	// to yet.
	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();

	std::wcout.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Window_Class";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc   = WindowProc; // application defined function. forward reference.
    wc.hInstance     = hInstance;  // handle to application instance
    // wc.hIcon 		 = 		   // handle to the class icon
    //wc.hCursor     	 =		   // handle to the cursor resource.
    //wc.hBrush 		 =         // handle to the background brush (?)

    wc.lpszClassName = CLASS_NAME; // string that identifies the window class. 

    redirect_output();

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Draw Something!",  		    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT,
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }
    //mine:

 	

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


static void on_size_changed(HWND hwnd, UINT flag, const int width, const int height)
{
	// handle resize.
	fmt::printf("Hello, %s!", "world");

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    	case WM_CLOSE:
    	{
			DestroyWindow(hwnd);
    	}

    	case WM_SIZE:
    	{
    		// resize!
    		const int width = LOWORD(lParam);
    		const int height = HIWORD(lParam);
    		std::cout << "test";
			fmt::printf("Hello, %s!", "world");

			on_size_changed(hwnd, static_cast<UINT>(wParam), width, height);
    		break;
    	}

	    case WM_DESTROY:
	    {
	        PostQuitMessage(0);
	        return 0;
	    }

	    case WM_PAINT:
        {
	            PAINTSTRUCT ps;
	            HDC hdc = BeginPaint(hwnd, &ps);
	            // painting occurs between beginpaint and endpaint.
	            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

	            EndPaint(hwnd, &ps);
        }

        return 0;
    }
    // this is for all messages that are not handled. the default is returned.
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
