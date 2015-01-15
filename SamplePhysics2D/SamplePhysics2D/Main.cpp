#include "Precomp.h"
#include "DebugRenderer.h"
#include "PhysicsWorld.h"
#include "RigidBody.h"
#include "Shape.h"

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

    // Create debug renderer, which we'll use to visualize our physics code
    std::unique_ptr<DebugRenderer> renderer(new DebugRenderer);
    if (!renderer->Initialize(hwnd))
    {
        return -2;
    }

    // Create a physics world object to run our simulation
    std::unique_ptr<PhysicsWorld> world(new PhysicsWorld(Vector2(0.0f, -20.0f), 100));

    // Create an assortment of random objects
    srand(0);

    std::vector<std::unique_ptr<RigidBody>> bodies;

    bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(new BoxShape(2, 2), 5.0f)));
    for (int i = 0; i < 10; ++i)
    {
        if (rand() % 2 == 0)
        {
            bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(new CircleShape((rand() % 5 + 1) * 0.4f), 5.0f)));
        }
        else
        {
            bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(new BoxShape(rand() % 2 + 1.0f, rand() % 2 + 1.0f), 5.0f)));
        }
        bodies[bodies.size() - 1]->Position() = Vector2(rand() % 20 - 10, rand() % 50 + 2);
    }

    // Walls
    bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(new BoxShape(20.0f, 1.0f), FLT_MAX)));
    bodies[bodies.size() - 1]->Position() = Vector2(0.0f, -10.0f);
    bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(new BoxShape(1.0f, 20.0f), FLT_MAX)));
    bodies[bodies.size() - 1]->Position() = Vector2(-10.0f, 0.0f);
    bodies[bodies.size() - 1]->Rotation() = 0.3f;
    bodies.push_back(std::unique_ptr<RigidBody>(new RigidBody(new BoxShape(1.0f, 20.0f), FLT_MAX)));
    bodies[bodies.size() - 1]->Position() = Vector2(10.0f, 0.0f);
    bodies[bodies.size() - 1]->Rotation() = -0.3f;

    // Add them to the world
    for (auto& body : bodies)
    {
        world->AddBody(body.get());
    }

    SetWindowText(hwnd, L"ESC=Exit, ArrowKeys=Move Object_0");

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
            // Simulate (NOTE: currently hard coded to 60Hz update time)
            static const float dt = 1.0f / 60.0f;

            // Some basic input for testing purposes
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            {
                PostQuitMessage(0);
            }

            if (GetAsyncKeyState(VK_LEFT) & 0x8000)
            {
                bodies[0]->Force() += Vector2(-100.0f, 0.0f);
            }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
            {
                bodies[0]->Force() += Vector2(100.0f, 0.0f);
            }
            if (GetAsyncKeyState(VK_UP) & 0x8000)
            {
                bodies[0]->Force() += Vector2(0.0f, 300.0f);
            }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000)
            {
                bodies[0]->Force() += Vector2(0.0f, -100.0f);
            }

            world->Update(dt);
            world->Draw(renderer.get());

            // Refresh
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

