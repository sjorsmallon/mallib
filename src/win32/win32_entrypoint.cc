#ifndef UNICODE
#define UNICODE
#endif 

// if platform == windows, include this file.
#include <windows.h>

#define GL_LITE_IMPLEMENTATION
#include "../graphics/gl_lite.h"

// #include <GL/GL.h>

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
        const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

//See https: www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

#include "../game/game.h"
#include "../graphics/graphics.h"
#include "../input/input.h"

#include <Wingdi.h>
#include <iostream> // redirect_output
#include <algorithm> // std::swap
#include "fmt/core.h" // 


//@Memory: these are allocated statically now?
static input::Key_Input windows_key_array[255] = {};
static unsigned char previous_keyboard_state[256] = {};
static unsigned char current_keyboard_state[256] = {};

static void init_windows_key_array()
{
    using namespace input;

    windows_key_array[0x57]       = Key_Input::KEY_W;
    windows_key_array[0x41]       = Key_Input::KEY_A;
    windows_key_array[0x53]       = Key_Input::KEY_S;
    windows_key_array[0x44]       = Key_Input::KEY_D;
    windows_key_array[VK_UP]      = Key_Input::KEY_UP;
    windows_key_array[VK_DOWN]    = Key_Input::KEY_DOWN;
    windows_key_array[VK_LEFT]    = Key_Input::KEY_LEFT;
    windows_key_array[VK_RIGHT]   = Key_Input::KEY_RIGHT;
    windows_key_array[VK_LBUTTON] = Key_Input::MOUSE_LEFT;
    windows_key_array[VK_RBUTTON] = Key_Input::MOUSE_RIGHT;
}


static void fatal_error(const char *msg)
{
    MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONEXCLAMATION);
    exit(EXIT_FAILURE);
}


LRESULT CALLBACK win32_main_window_callback(HWND window,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam);

static void redirect_output_to_console(bool use_parent)
{
    //Create the console window and set the window title.
    if (use_parent)
    {
        AttachConsole(ATTACH_PARENT_PROCESS);
    }
    else
    {
        if (AllocConsole() == 0)
        {
            fatal_error("redirect_output: allocConsole failed.");
        }
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


static void init_opengl_extensions()
{
    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.
    WNDCLASSA window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = DefWindowProcA;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = "Dummy_WGL_djuasiodwa";
    if (!RegisterClassA(&window_class))
    {
        fatal_error("Failed to register dummy OpenGL window.");
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window_class.hInstance,
        0);

    if (!dummy_window) {
        fatal_error("Failed to create dummy OpenGL window.");
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(pfd),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32, // color bits
        0, 0,
        0, 0,
        0, 0,
        8, 0,
        0,
        0, 0, 0, 0,
        24, // depth bits
        8, //stencil bits
        0,
        PFD_MAIN_PLANE, // layer type
        0,
        0,
        0,
        0
    };

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) {
        fatal_error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        fatal_error("Failed to set the pixel format.");
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        fatal_error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        fatal_error("Failed to activate dummy OpenGL rendering context.");
    }

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
        "wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
        "wglChoosePixelFormatARB");

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}


static HGLRC init_opengl(HDC device_context)
{
     init_opengl_extensions();

    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(device_context, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        fatal_error("Failed to set the OpenGL 3.3 pixel format.");
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(device_context, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(device_context, pixel_format, &pfd)) {
        fatal_error("Failed to set the OpenGL 3.3 pixel format.");
    }

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC opengl_context = wglCreateContextAttribsARB(device_context, 0, gl33_attribs);
    if (!opengl_context) {
        fatal_error("Failed to create OpenGL 3.3 context.");
    }

    if (!wglMakeCurrent(device_context, opengl_context)) {
        fatal_error("Failed to activate OpenGL 3.3 rendering context.");
    }


    return opengl_context;
} 


static HWND create_window(HINSTANCE instance)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Window_Class";
    
    WNDCLASS window_class = {};
    window_class.lpfnWndProc   = win32_main_window_callback; // application defined function. forward reference.
    window_class.hInstance     = instance;  // handle to application instance

    window_class.lpszClassName = CLASS_NAME; // string that identifies the window class. 
    RegisterClass(&window_class); //@Log: can this fail?

    // Create the window.
    HWND window = CreateWindowEx(
        CS_OWNDC,   // Optional window styles. 0 for standard, but CS_OWNDC for
        CLASS_NAME, // Window class
        L"game",    // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window style
        // position (x, y) and size (x,y). we use the default.
        CW_USEDEFAULT,
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        instance,   // Instance handle
        NULL        // Additional application data
        );

    if (!window)
    {
        // @Log: help.
    }
    return window;
}

static void insert_input_in_queue()
{
    //@refactor: the input queue is maximally the size of the buttons we want to check.
    // I think? or do we want buffered input?
    //@Platform:
    int keys[6] = {VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_LBUTTON, VK_RBUTTON};
    std::swap(current_keyboard_state, previous_keyboard_state);

    for (auto key: keys)
    {
        if(GetAsyncKeyState(key))
        {
            current_keyboard_state[key] = 1;
             if (!previous_keyboard_state[key])
                input::input_queue().push_back(windows_key_array[key]);
        }
        else
            current_keyboard_state[key] = 0;
    }
}


int WINAPI wWinMain(HINSTANCE instance,
                    HINSTANCE prev_instance,
                    PWSTR command_line,
                    int command_show)
{
    
    auto arguments = std::wstring(command_line);
    bool use_parent = true;
    if ( arguments.find(L"create_console") != std::string::npos)
        use_parent = false;

    redirect_output_to_console(use_parent);

    // else
    //     exit(1);

    init_windows_key_array();

    HWND window = create_window(instance);
    HDC device_context = GetDC(window);
    HGLRC gl_context = init_opengl(device_context);
    graphics::global_Win32_context().gl_context = gl_context;
    graphics::global_Win32_context().device_context = device_context;
    // context.gl_context = gl_context;
    // context.device_context = device_context;
    // gl_lite_init();

    ShowWindow(window, command_show);
    UpdateWindow(window);

    if (window)
    {
        // change window size
        const int width =  1920; // @hardcoded
        const int height = 1080;
        auto& settings = graphics::window_settings();
        settings.width = width;
        settings.height = height;

        SetWindowPos(
          window,
          HWND_TOP,
          500,
          500,
          width,
          height,
          SWP_NOMOVE
        );

        // start loading everything.
        game::init_everything();

        // Run the message loop.
        bool running = true;
        while (running)
        {
            MSG message = {};
            while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) // null here means we get all messages on this tread.
            {
                if (message.message == WM_QUIT)
                {
                    running = false;
                }
                else
                {
                 TranslateMessage(&message);
                 DispatchMessage(&message);
                }
            }
            //@TODO: when do we look at input?
            insert_input_in_queue();
            game::main_loop();
        }               
    }
    
    return 0;
}

static void on_size_changed(HWND hwnd, UINT flag, const int width, const int height)
{
    // fmt::print("on_size_changed is not yet implemented.\n");
}

static LRESULT CALLBACK win32_main_window_callback(HWND window,
                                                   UINT message, 
                                                   WPARAM wParam,
                                                   LPARAM lParam)
{
    LRESULT result = 0;
    switch (message)
    {
        // all  keyboard nonsense
        case WM_KEYUP:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                {
                    exit(1);
                    break;
                }
                case VK_RIGHT:
                {
                    break;
                }
                case VK_UP:
                {
                    break;
                }
                case VK_DOWN:
                {
                    break; 
                }
            }
        }
        case WM_CHAR:
        {

        }
        case WM_CREATE:
        {
            // MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
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
            graphics::Win32_Context& context = graphics::global_Win32_context();
            wglDeleteContext(context.gl_context);

            PostQuitMessage(0);
            break;
        }
        
        case WM_PAINT:
        {
            // we need to start painting here, otherwise WM_PAINT stays
            // in the message queue, and we can never break the peekMessage loop.
            // sigh. 
            PAINTSTRUCT ps = {};
            HDC dc = BeginPaint(window,&ps);
            // do drawing to 'dc' here -- or don't
            EndPaint(window,&ps);

            break;
        }
        case WM_ACTIVATEAPP:
        {
            break;
        }
        default: 
        {
            result = DefWindowProc(window, message, wParam, lParam); 
            break;
        }
    }
    // this is for all messages that are not handled. the default is returned.
    return result;
}

void toggle_fullscreen(HWND window_handle)
{

    DWORD style = GetWindowLong(window_handle, GWL_STYLE);
    WINDOWPLACEMENT window_position = {};
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        

        if(GetWindowPlacement(window_handle, &window_position) &&
           GetMonitorInfo(MonitorFromWindow(window_handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
        {
            SetWindowPos(window_handle, HWND_TOP,
                         monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowPlacement(window_handle, &window_position);
        SetWindowPos(window_handle, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}
