cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    
    // Correction 1: Utilisation des majuscules pour les matrices
    // Correction 2: Ordre de multiplication correct
    float4 pos = mul(float4(input.pos, 1.0f), World);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    
    output.pos = pos;
    output.color = input.color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return input.color;
}