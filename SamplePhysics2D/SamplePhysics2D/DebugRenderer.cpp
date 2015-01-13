#include "Precomp.h"
#include "DebugRenderer.h"
#include "DebugRendererVS.h"
#include "DebugRendererPS.h"

using namespace Microsoft::WRL;

const Color DebugRenderer::DefaultPointColor(1.0f, 0.0f, 0.0f, 1.0f);
const Color DebugRenderer::DefaultLineColor(1.0f, 1.0f, 1.0f, 1.0f);

DebugRenderer::DebugRenderer()
{
}

DebugRenderer::~DebugRenderer()
{
}

bool DebugRenderer::Initialize(HWND hwnd)
{
    // Get the window's client rect to use for the swapchain size
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // Create D3D11 device and swapchain targeting the hwnd
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Width = clientRect.right;
    scd.BufferDesc.Height = clientRect.bottom;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &_swapChain,
        &_device, nullptr, &_context);
    if (FAILED(hr))
    {
        return false;
    }

    // Get back buffer pointer & create a render target view to it
    ComPtr<ID3D11Texture2D> texture;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr))
    {
        return false;
    }

    hr = _device->CreateRenderTargetView(texture.Get(), nullptr, &_renderTargetView);
    if (FAILED(hr))
    {
        return false;
    }

    // Create our shaders
    hr = _device->CreateVertexShader(DebugRendererVS, sizeof(DebugRendererVS), nullptr, &_vertexShader);
    if (FAILED(hr))
    {
        return false;
    }

    hr = _device->CreatePixelShader(DebugRendererPS, sizeof(DebugRendererPS), nullptr, &_pixelShader);
    if (FAILED(hr))
    {
        return false;
    }

    // Create input layout describing our vertex data
    D3D11_INPUT_ELEMENT_DESC elems[2] = {};
    elems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[0].SemanticName = "POSITION";
    elems[1].AlignedByteOffset = sizeof(Vector2);
    elems[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    elems[1].SemanticName = "COLOR";

    hr = _device->CreateInputLayout(elems, _countof(elems), DebugRendererVS, sizeof(DebugRendererVS), &_inputLayout);
    if (FAILED(hr))
    {
        return false;
    }

    // Create vertex & constant buffer. All drawing uses the same vertex buffer.
    // We just copy new vertices into it and draw using it again.
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(Vertex) * MaxVerticesPerDraw;
    bd.StructureByteStride = sizeof(Vertex);
    bd.Usage = D3D11_USAGE_DEFAULT;

    hr = _device->CreateBuffer(&bd, nullptr, &_vertexBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(Constants);
    bd.StructureByteStride = sizeof(Constants);
    bd.Usage = D3D11_USAGE_DEFAULT;

    hr = _device->CreateBuffer(&bd, nullptr, &_constantBuffer);
    if (FAILED(hr))
    {
        return false;
    }

    // Set up the graphics pipeline
    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;

    _context->IASetInputLayout(_inputLayout.Get());
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    _context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

    _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)clientRect.right;
    vp.Height = (float)clientRect.bottom;
    vp.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &vp);

    SetViewport(Vector2(), Vector2(20, 20));

    return true;
}

void DebugRenderer::SetViewport(const Vector2& position, const Vector2& size)
{
    Constants constants;
    constants.viewportPosition = position;
    constants.viewportSize = size;
    _context->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &constants, sizeof(constants), 0);
}

void DebugRenderer::DrawPoint(const Vector2& position, const Color& color)
{
    // Draw the point as a tiny hollow square
    float size = 0.1f;

    Vector2 offsets[] =
    {
        Vector2(-size, -size),
        Vector2(size, -size),
        Vector2(size, size),
        Vector2(-size, size),
    };

    for (int i = 0; i < _countof(offsets); ++i)
    {
        DrawLine(position + offsets[i], position + offsets[(i + 1) % _countof(offsets)], color);
    }
}

void DebugRenderer::DrawLine(const Vector2& start, const Vector2& end, const Color& color)
{
    _vertices.push_back(Vertex(start, color));
    _vertices.push_back(Vertex(end, color));
}

void DebugRenderer::DrawBox(const Vector2& position, const Vector2& widths, float rotation, const Color& color)
{
    Vector2 size = widths * 0.5f;
    Vector2 offsets[] =
    {
        Vector2(-size.x, -size.y),
        Vector2(size.x, -size.y),
        Vector2(size.x, size.y),
        Vector2(-size.x, size.y),
    };

    // Rotate the offset vectors
    float sinA = sinf(rotation);
    float cosA = cosf(rotation);
    for (int i = 0; i < _countof(offsets); ++i)
    {
        offsets[i] = Vector2(cosA * offsets[i].x + -sinA * offsets[i].y, sinA * offsets[i].x + cosA * offsets[i].y);
    }

    // Draw lines
    for (int i = 0; i < _countof(offsets); ++i)
    {
        DrawLine(position + offsets[i], position + offsets[(i + 1) % _countof(offsets)], color);
    }
}

void DebugRenderer::DrawCircle(const Vector2& position, float radius, float rotation, const Color& color)
{
    static const int numPoints = 32;

    Vector2 first = position + Vector2(radius, 0.0f);
    Vector2 prev = first;
    float step = 2.0f * (float)M_PI / (float)numPoints;
    float angle = 0.0f;

    for (int i = 1; i < numPoints; ++i)
    {
        angle += step;
        Vector2 p = position + Vector2(cosf(angle) * radius, sinf(angle) * radius);
        DrawLine(prev, p, color);
        prev = p;
    }

    // Last segment
    DrawLine(prev, first, color);

    // Rotation line
    DrawLine(position, position + Vector2(cosf(rotation) * radius, sinf(rotation) * radius), color);
}

void DebugRenderer::Render()
{
    static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _context->ClearRenderTargetView(_renderTargetView.Get(), clearColor);

    D3D11_BOX box = {};
    box.back = 1;
    box.bottom = 1;

    // Draw lines
    uint32_t numPoints = (uint32_t)_vertices.size();
    Vertex* points = _vertices.data();
    while (numPoints > 0)
    {
        uint32_t num = min(numPoints, MaxVerticesPerDraw);
        box.right = sizeof(Vertex) * num;
        _context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, points, box.right, 0);
        _context->Draw(num, 0);
        numPoints -= num;
        points += num;
    }

    _swapChain->Present(1, 0);

    _vertices.clear();
}
