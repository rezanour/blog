#include "Precomp.h"
#include "DebugRenderer.h"

// Name used to register our window class, and set our window title.
static const wchar_t AppClassName[] = L"SamplePhysics2D";

// Initialize application, creates window.
static HWND AppInitialize(HINSTANCE instance, int width, int height);

// Application window message processing callback
static LRESULT CALLBACK AppWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main entry point
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    // Create our main application window
    HWND hwnd = AppInitialize(instance, 800, 600);
    if (!hwnd)
    {
        return -1;
    }

    std::unique_ptr<DebugRenderer> renderer(new DebugRenderer);
    if (!renderer->Initialize(hwnd))
    {
        return -2;
    }

    // Make window visible and active
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Message pump
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Idle
            renderer->Render();
        }
    }

    return 0;
}

HWND AppInitialize(HINSTANCE instance, int width, int height)
{
    // Register a window class for our application
    WNDCLASSEX wcx = {};
    wcx.cbSize = sizeof(wcx);
    wcx.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wcx.hInstance = instance;
    wcx.lpfnWndProc = AppWindowProc;
    wcx.lpszClassName = AppClassName;
    if (!RegisterClassEx(&wcx))
    {
        return nullptr;
    }

    // Based on our desired style, determine how big to make the window so the
    // client area is the size requested.
    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    RECT clientRect = { 0, 0, width, height };
    AdjustWindowRect(&clientRect, style, FALSE);

    return CreateWindow(AppClassName, AppClassName, style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        clientRect.right - clientRect.left, clientRect.bottom - clientRect.top,
        nullptr, nullptr, instance, nullptr);
}

LRESULT CALLBACK AppWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        // If the user closes the window, quit the application
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

