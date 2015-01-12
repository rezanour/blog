#pragma once

struct Color
{
    Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

    float r, g, b, a;
};

// Simple debug renderer for visualizing shapes
class DebugRenderer
{
public:
    DebugRenderer();
    ~DebugRenderer();

    // Initialize debug renderer, outputing to the specified hwnd
    bool Initialize(HWND hwnd);

    // Defines what portion of the world coordinate system gets rendered to the window
    void SetViewport(const Vector2& position, const Vector2& size);

    // Draw a point, optionally providing a color. Otherwise, uses default color
    void DrawPoint(const Vector2& position, const Color& color = DefaultPointColor);

    // Draw a line, optionally providing a color. Otherwise, uses default color
    void DrawLine(const Vector2& start, const Vector2& end, const Color& color = DefaultLineColor);

    // Draw a box, optionally providing a color. Otherwise, uses default color
    void DrawBox(const Vector2& position, const Vector2& widths, float rotation, const Color& color = DefaultLineColor);

    // Draw a circle, optionally providing a color. Otherwise, uses default color
    // This also draws a line from the center out to the surface based on rotation to visualize roll.
    void DrawCircle(const Vector2& position, float radius, float rotation, const Color& color = DefaultLineColor);

    // Called once a frame to render all batched commands to the window
    void Render();

private:
    // Prevent copy
    DebugRenderer(const DebugRenderer&);
    DebugRenderer& operator= (const DebugRenderer&);

private:
    static const Color DefaultLineColor;
    static const Color DefaultPointColor;
    static const uint32_t MaxVerticesPerDraw = 16384;

    Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
    Microsoft::WRL::ComPtr<ID3D11Device> _device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;

    struct Vertex
    {
        Vertex() {}
        Vertex(const Vector2& position, const Color& color) : position(position), color(color) {}

        Vector2 position;
        Color   color;
    };

    struct Constants
    {
        Vector2 ViewportPosition;
        Vector2 ViewportSize;
    };

    std::vector<Vertex> _vertices;
};
