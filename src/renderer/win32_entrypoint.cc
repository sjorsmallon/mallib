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

#include "redirect_output.h"

// #include <GL/gl.h>
// #include <GLEW.h>

// this works!


LRESULT CALLBACK win32_main_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR command_line, int command_show)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Window_Class"; 
    WNDCLASS window_class = {};
    window_class.lpfnWndProc   = win32_main_window_callback; // application defined function. forward reference.
    window_class.hInstance     = instance;  // handle to application instance
    // wc.hIcon 		 = 		   // handle to the class icon
    //wc.hCursor     	 =		   // handle to the cursor resource.
    //wc.hBrush 		 =         // handle to the background brush (?)

    window_class.lpszClassName = CLASS_NAME; // string that identifies the window class. 

    redirect::redirect_output();

    RegisterClass(&window_class); //@Log: can this fail?


    // Create the window.
    HWND window_handle = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Draw Something!",  		    // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style
        // position (x, y) and size (x,y)
        CW_USEDEFAULT,
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        instance,  // Instance handle
        NULL        // Additional application data
        );

    if (window_handle)
    {
        // Run the message loop.
	    MSG message = {};
	    while (GetMessage(&message, NULL, 0, 0)) // null here means we get all messages on this tread.
	    {
	        TranslateMessage(&message);
	        DispatchMessage(&message);
	    }
    }
    else
    {
    	return 0;
    	//@Log:
    }

 	



    return 0;
}


static void on_size_changed(HWND hwnd, UINT flag, const int width, const int height)
{
	// handle resize.
	// fmt::printf("Hello, %s!", "world");

}

static LRESULT CALLBACK win32_main_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
    switch (message)
    {
		case WM_SIZE:
    	{
    		// resize!
    		const int width = LOWORD(lParam);
    		const int height = HIWORD(lParam);

			on_size_changed(hwnd, static_cast<UINT>(wParam), width, height);
    		break;
    	}
    	case WM_CLOSE:
    	{
			DestroyWindow(window);
			break;
    	}
    	case WM_DESTROY:
	    {
	        PostQuitMessage(0);
	        break;
	    }
	    
	    case WM_PAINT:
        {
            PAINTSTRUCT paint_struct;
            HDC hdc = BeginPaint(window, &paint_struct);
            // painting occurs between beginpaint and endpaint.
            FillRect(hdc, &paint_struct.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(window, &paint_struct);	

            break.;
        }
        case WM_ACTIVATEAPP:
        {
        	fmt::printf("WM_ACTIVATEAPP");

        	break;
        }
        default: 
        {
        	result = DefWindowProc(window, message, wParam, lParam); 

        	break;
        }
    }

    return result;
    // this is for all messages that are not handled. the default is returned.
}
