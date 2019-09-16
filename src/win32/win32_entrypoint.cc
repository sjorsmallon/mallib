#ifndef UNICODE
#define UNICODE
#endif 
// if platform == windows, include this file.
#include <windows.h>
#include <GL/GL.h>

#include <Wingdi.h>
#undef max
#undef min
#include <fmt/core.h>
#include <fmt/printf.h>
#include <limits> // for fmt error
#include "redirect_output.h" // for redirecting output to console window.


LRESULT CALLBACK win32_main_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR command_line, int command_show)
{
    // output redirection for a new console. This calls AllocConsole().
    redirect::output_to_console();


    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Window_Class";
    
    WNDCLASS window_class = {};
    window_class.lpfnWndProc   = win32_main_window_callback; // application defined function. forward reference.
    window_class.hInstance     = instance;  // handle to application instance
    // wc.hIcon 		 = 		   // handle to the class icon
    //wc.hCursor     	 =		   // handle to the cursor resource.
    //wc.hBrush 		 =         // handle to the background brush (?)

    window_class.lpszClassName = CLASS_NAME; // string that identifies the window class. 
    RegisterClass(&window_class); //@Log: can this fail?

    // Create the window.
    HWND window = CreateWindowEx(
        CS_OWNDC,                              // Optional window styles. 0 for standard, but CS_OWNDC for
        CLASS_NAME,                     // Window class
        L"game",  		    // Window text
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

    if (window)
    {
        // start loading everything.
        game::audio_setup();
        // game::video_setup(); cannot do this here. need to do in the getmessage
        // part?

        // Run the message loop.
	    MSG message = {};
	    while (GetMessage(&message, NULL, 0, 0)) // null here means we get all messages on this tread.
	    {
	        TranslateMessage(&message);
	        DispatchMessage(&message);
            game::main_loop(); // enter game loop for one cycle
	    }
    }
    else
    {
        game::shutdown();


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
        case WM_CREATE:
        {

            PIXELFORMATDESCRIPTOR pixel_format_descriptor = 
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // flags
                PFD_TYPE_RGBA,              // kind of framebuffer, RGBA or palette(?)
                32,                        // colordepth of the framebuffer
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,                     // number of bits for the depthbuffer
                8,                     // nnumber of bits for the stencilbuffer
                0,                     // number of AUX buffers in the framebuffer
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            HDC device_context = GetDC(window); // @Todo: getDCEx? 
            int letWindowsChooseThisPixelFormat = ChoosePixelFormat(device_context, &pixel_format_descriptor); 
            SetPixelFormat(device_context, letWindowsChooseThisPixelFormat, &pixel_format_descriptor);

            HGLRC gl_context = wglCreateContext(device_context);
            wglMakeCurrent(device_context, gl_context);
            renderer::gl_context = gl_context;

            MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);

            break;
        }

		case WM_SIZE:
    	{
    		// resize!
    		const int width = LOWORD(lParam);
    		const int height = HIWORD(lParam);

			on_size_changed(window, static_cast<UINT>(wParam), width, height);
    		break;
    	}
    	case WM_CLOSE:
    	{
			DestroyWindow(window);
			break;
    	}
    	case WM_DESTROY:
	    {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(renderer::current_context);
              // wglDeleteContext ();

	        PostQuitMessage(0);
	        break;
	    }
	    
	    case WM_PAINT:
        {
            PAINTSTRUCT paint_struct;
            HDC device_context = BeginPaint(window, &paint_struct);
            int x = paint_struct.rcPaint.left;
            int y = paint_struct.rcPaint.right;
            int width  = paint_struct.rcPaint.right  - paint_struct.rcPaint.left;
            int height = paint_struct.rcPaint.bottom  - paint_struct.rcPaint.top;
            // painting occurs between beginpaint and endpaint.
            // FillRect(hdc, &paint_struct.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            PatBlt(device_context, x, y, width, height, WHITENESS);
            EndPaint(window, &paint_struct);	

            break;
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
