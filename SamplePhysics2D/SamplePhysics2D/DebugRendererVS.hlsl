cbuffer Constants
{
    float2 ViewportPosition;
    float2 ViewportSize;
};

struct VertexIn
{
    float2 Position : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    output.Position = float4((input.Position - ViewportPosition) / ViewportSize, 0, 1);
    output.Color = input.Color;
    return output;
}