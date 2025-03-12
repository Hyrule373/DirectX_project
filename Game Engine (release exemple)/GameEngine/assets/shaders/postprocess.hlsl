//////////////////////////////////////////////////
// postprocess.hlsl
//////////////////////////////////////////////////

// ------------------------------------------------
// Vertex Shader (PPVS)
// ------------------------------------------------

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PS_INPUT PPVS(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = float4(input.pos, 1.0);
    output.uv = input.uv;
    return output;
}

// ------------------------------------------------
// Pixel Shader (PPPS) - Effet Glow / Bloom
// ------------------------------------------------

// La texture de la sc�ne rendue (offscreen)
Texture2D SceneTexture : register(t0);
SamplerState SceneSampler : register(s0);

// Param�tres uniformes pour le post processing
cbuffer PostProcessParams : register(b0)
{
    float2 resolution; // R�solution de l'�cran (largeur, hauteur)
    float bloomIntensity; // Intensit� du glow (bloom)
    float brightnessThreshold; // Seuil pour extraire les zones lumineuses
};

float4 PPPS(PS_INPUT input) : SV_TARGET
{
    // Calcul de la taille d'un texel
    float2 texelSize = 1.0 / resolution;

    // �chantillonnage de la couleur originale
    float4 originalColor = SceneTexture.Sample(SceneSampler, input.uv);

    // Calcul de la luminance (per�ue) de la couleur
    float luminance = dot(originalColor.rgb, float3(0.299, 0.587, 0.114));

    // Cr�ation d'un masque : seuls les pixels au-dessus du seuil seront amplifi�s
    float mask = step(brightnessThreshold, luminance);

    // Accumulation des voisins pour simuler un flou (3x3)
    float4 blurColor = float4(0, 0, 0, 0);
    [unroll]
    for (int x = -1; x <= 1; x++)
    {
        [unroll]
        for (int y = -1; y <= 1; y++)
        {
            float2 offset = float2(x, y) * texelSize;
            blurColor += SceneTexture.Sample(SceneSampler, input.uv + offset);
        }
    }
    blurColor /= 9.0; // Moyenne sur 9 �chantillons

    // Combinaison du r�sultat du flou avec la couleur originale
    // Seules les zones lumineuses (mask=1) amplifient l'effet de bloom
    float4 finalColor = originalColor + bloomIntensity * blurColor * mask;

    // Retourne la couleur finale, born�e entre 0 et 1
    return saturate(finalColor);
}
