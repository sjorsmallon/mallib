#ifndef UNICODE
#define UNICODE
#endif 
#include <windows.h> // I mean...
#include <Wingdi.h> // createDC 
#include <iostream> // redirect_output, string(for CLI arguments)
#include <algorithm> // std::swap (for keyboard vectors.)

#include "io.h" // Keyboard_State, Platform_Key_Map 
#include "globals.h"  // globals.window_width, globals.window_height
#include "../game/game.h" 
#include "../graphics/graphics.h" // this is a test.

//@NOTE: go to create_window in order to skip the openGL / input handling 
// stuff.

// The input header needs to provide an input queue which all platform files insert content in.

// hmm. maybe the input queue can be something that we do not necessarily need.

// it can be implemented later. whatever. at game level. Maybe we don't want 
// an input queue, but want to only listen to the latest actions.
// then we need to make the key map array agreeable.

//@TODO: find an alternative for the global_device_context and the global_gl_context.

// WHAT THIS IS: this file is to be used for projects that require an openGL
// context, as well as any type of input handling. The context creation should
// work fine for any general application (at least, it suits my needs).

// THIS DEPENDS ON: the input header. also provided.

// WHAT THIS IS NOT: an all-encompassing, real-world-ready robust entrypoint.
// It's a cludgy part of my own applications.

// HOW DO I USE IT:
// INIT: 
    // for any initialization needs of your application, use the predefined
    // application_init function (which is empty).
// LOOP:
    // for the main loop of your application, use the predefined        
    // application_loop function (which is empty).


// OpenGL: this file provides a valid  opengl_context and the device_context. // These can be acquired through 
    // global_device_context
    //  global_gl_context
// (which are available after application_init is invoked).   

// Call gl_lite_init() before using any OpenGL function and after you have a
//  valid OpenGL context.

///-----------------------------------------------------------------------
///  openGL setup.

// gl_lite for extension loading.
#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
        const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

//See https: www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_type(
    HDC hdc,
    const int *piAttribIList,
    const FLOAT *pfAttribFList,
    UINT nMaxFormats,
    int *piFormats,
    UINT *nNumFormats);

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


// private
static void fatal_error(const char *msg)
{
    MessageBoxA(NULL, msg, "Fatal Error. Exiting.", MB_OK | MB_ICONEXCLAMATION);
    exit(EXIT_FAILURE);
}

static void init_opengl_extensions()
{
    // Before we can load extensions, we need a dummy OpenGL context, created // using a dummy window. We use a dummy window because you can only set  // the pixel format for a window once. For the real window, we want to   // use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and 
    // use that before we have a context.

    WNDCLASSA window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = DefWindowProcA;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = "Dummy_WGL_TEMP";
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

    // the PIXELFORMATDESCRIPTOR is instrumental in describing the properties
    // of the drawing "canvas". These are some defaults I took from some 
    // website.
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

// private
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
    wglChoosePixelFormatARB(
        device_context,
        pixel_format_attribs,
        0,
        1,
        &pixel_format,
        &num_formats);

    if (!num_formats) {
        fatal_error("Failed to set the OpenGL 4.1 pixel format.");
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(device_context, pixel_format, sizeof(pfd), &pfd);

    if (!SetPixelFormat(device_context, pixel_format, &pfd)) {
        fatal_error("Failed to set the OpenGL 4.1 pixel format.");
    }

    // Specify that we want to create an OpenGL 4.1 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC opengl_context = wglCreateContextAttribsARB(device_context, 0, gl33_attribs);
    if (!opengl_context) {
        fatal_error("Failed to create OpenGL 4.1 context.");
    }

    if (!wglMakeCurrent(device_context, opengl_context)) {
        fatal_error("Failed to activate OpenGL 4.1 rendering context.");
    }
    return opengl_context;
} 

// private
static void redirect_output_to_console(bool use_parent)
{
    //Create the console window and set the window title.
    if (use_parent)
    {
        //  attach to the launch console.
        AttachConsole(ATTACH_PARENT_PROCESS);
    }
    else
    {
        // create a new console to write to (useful for renderDoc)
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


// init_windows_key_array creates a mapping between the windows key presses (hexadecimal 0-255) and the input events that any application can process.
// This mapping needs to be established in the relevant starting points for the application. The mapping is used to "return" the correct input key that we can listen to. I don't know how to do this properly cross platform. 



// private
static void init_key_mapping_array(io::Platform_Key_Map& platform_key_map)
{
    using  namespace io;
    // 0x57: 87
    // 0x41: 65
    // 0x53: 83
    // 0x44: 68
    platform_key_map[0x57]       = Button::KEY_W;
    platform_key_map[0x41]       = Button::KEY_A;
    platform_key_map[0x53]       = Button::KEY_S;
    platform_key_map[0x44]       = Button::KEY_D;
    platform_key_map[VK_UP]      = Button::KEY_UP;
    platform_key_map[VK_DOWN]    = Button::KEY_DOWN;
    platform_key_map[VK_LEFT]    = Button::KEY_LEFT;
    platform_key_map[VK_RIGHT]   = Button::KEY_RIGHT;
    platform_key_map[VK_LBUTTON] = Button::MOUSE_LEFT;
    platform_key_map[VK_RBUTTON] = Button::MOUSE_RIGHT;
}


// private
static void insert_input_in_queue()
{
    // keys are the keys we are watching. we evaluate the state of the keyboard.    
    int keys[6] = {
        VK_UP,
        VK_DOWN,
        VK_LEFT,
        VK_RIGHT,
        VK_LBUTTON,
        VK_RBUTTON
    };

    std::swap(io::keyboard_state(), io::prev_keyboard_state());

    for (auto key: keys)
    {
        if(GetAsyncKeyState(key))
        {
            io::keyboard_state()[key] = true;
             if (!io::prev_keyboard_state()[key])
             {
                io::input_queue().push_back(io::platform_key_map()[key]);
             }
        }
        else
            io::keyboard_state()[key] = false;
    }
}

///---------------------------------------------------------------------------
/// entrypoint & openGL stuff.
HDC   global_device_context;
HGLRC global_gl_context;

static void application_init()
{
    game::init();
}

static void application_loop()
{
    game::main_loop();
}

///---------------------------------------------------------------------------
/// Windows API stuff.


// actual windows related stuff.
// this is necessary as it is not available at the first passthrough. (will be dynamically)
// the callback is invoked for listening to the messages that are received 
// from the operating system or from user interaction. This is defined later.
LRESULT CALLBACK win32_main_window_callback(HWND window,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam);

// private
static HWND create_window(HINSTANCE instance)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Window_Class";

    WNDCLASS window_class = {};
    // application defined function. forward reference.
    window_class.lpfnWndProc   = win32_main_window_callback; 
    window_class.hInstance     = instance;  // handle to application instance
    window_class.lpszClassName = CLASS_NAME; // string that identifies the window class. 
    RegisterClass(&window_class); //@Log: can this fail?

    // Create the window.
    HWND window = CreateWindowEx(
        CS_OWNDC,   // Optional window styles. 0 for standard, but CS_OWNDC for
        CLASS_NAME, // Window class
        L"Application",    // Window text
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
        fatal_error("CreateWindowEx Failed. Exiting.");
    }

    return window;
}



int WINAPI wWinMain(HINSTANCE instance,
                    HINSTANCE prev_instance,
                    PWSTR command_line,
                    int command_show)
{
    // set up console output. per default, attach to parent console.
    // unite this in a struct?
    const auto cli_arguments = std::wstring(command_line);
    bool use_parent = true;
    if ( cli_arguments.find(L"create_console") != std::string::npos)
        use_parent = false;
    redirect_output_to_console(use_parent);
 
    // set up keyboard input. These are defined in the IO header.
    init_key_mapping_array(io::platform_key_map());

    // actual window stuff.
    HWND window = create_window(instance);
    HDC device_context = GetDC(window);
    HGLRC gl_context = init_opengl(device_context);


    // how do we avoid bi-dependency here?
    global_gl_context = gl_context;
    global_device_context = device_context;
    //@Global
    globals.device_context = device_context;
    globals.gl_context = gl_context;

    ShowWindow(window, command_show);
    UpdateWindow(window);
    if (window)
    {
        // change window size
        const int width =  1920; // @hardcoded
        const int height = 1080;

        //@global:
        globals.window_width = width;
        globals.window_height = height;
        SetWindowPos(
          window,
          HWND_TOP,
          0,
          0,
          width,
          height,
          SWP_NOMOVE
        );

        // at this point, the user can insert their init functions.
        application_init();

        // @Note: remember that we look at input after processing the OS 
        // messages. I have no idea yet what kind of repercussions that has
        // on input delay (if any), or whether we should reverse the ordering.
        // Additionally, we could terminate after a fixed amount of time
        // in order to be in time for the next frame to be processed (and not
        // be stuck processing operating system messages.)
        // @Incomplete: can we escape from the peekmessage loop after n loops?

        // Run the message & input loop.
        POINT screen_cursor_pos= {};
        int cursor_x = 0;
        int cursor_y = 0;   
   
        MSG message = {};             
        bool running = true;
        while (running)
        {
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
            GetCursorPos(&screen_cursor_pos);

            // device coordinates (I think?)
            ScreenToClient(window, &screen_cursor_pos);
            cursor_x = screen_cursor_pos.x;
            cursor_y = screen_cursor_pos.y;

            // this is TMI I think. We force the inclusion of Vec2i here.
            // how do we publish the new mouse coordinates properly?
            io::update_mouse_coords(cursor_x, cursor_y);
            insert_input_in_queue();
            application_loop();
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
            }
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

            // //@INCOMPLETE: DESTROY THE GL_CONTEXT!
            // graphics::Win32_Context& context = graphics::global_Win32_context();
            // wglDeleteContext(context.gl_context);

            PostQuitMessage(0);
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
