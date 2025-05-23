/*------------------.
| :: Description :: |
'-------------------/

    RCAS
    
    Version 1.2
    Author: Barbatos Bachiko
    License: MIT
    About: Adaptive Sharpening filter.

    History:
    (*) Feature (+) Improvement	(x) Bugfix (-) Information (!) Compatibility

    Version 1.2
    * Removed code referring to JakobPCoder, inserted source code "40163650
FSRForReShade"

*/
namespace RCASisCool
{
#define FSR_RCAS_LIMIT (0.18 - (1.0/16.0))
#include "ReShade.fxh"
/*---------------.
| :: Settings :: |
'---------------*/

    uniform float sharpness <
    ui_type = "slider";
    ui_label = "Sharpness Intensity";
    ui_tooltip = "Adjust Sharpness Intensity";
    ui_min = 0.0;
    ui_max = 1.0;
    ui_default = 0.15;
> = 0.6;

    uniform float contrastThreshold <
    ui_type = "slider";
    ui_label = "Contrast Threshold";
    ui_tooltip = "Threshold to enable sharpening on low-contrast areas";
    ui_min = 0.0;
    ui_max = 1.0;
    ui_default = 0.0;
> = 0.0;

/*----------------.
| :: Functions :: |
'----------------*/

    float4 FsrRcasLoadF(sampler2D samp, float2 p)
    {
        return tex2Dlod(samp, float4(p * ReShade::PixelSize, 0, 0));
    }

    float FsrRcasCon(float sharpness)
    {
        return exp2(sharpness);
    }

    float3 FsrRcasF(
    float2 ip, // Integer pixel position in output.
    float con
)
    {
    // Constant generated by RcasSetup().
    // Algorithm uses minimal 3x3 pixel neighborhood.
    //    b 
    //  d e f
    //    h
        float2 sp = float2(ip);
        float3 b = FsrRcasLoadF(ReShade::BackBuffer, sp + float2(0, -1)).rgb;
        float3 d = FsrRcasLoadF(ReShade::BackBuffer, sp + float2(-1, 0)).rgb;
        float3 e = FsrRcasLoadF(ReShade::BackBuffer, sp).rgb;
        float3 f = FsrRcasLoadF(ReShade::BackBuffer, sp + float2(1, 0)).rgb;
        float3 h = FsrRcasLoadF(ReShade::BackBuffer, sp + float2(0, 1)).rgb;
    
    // Luma times 2.
        float bL = mad(.5, b.b + b.r, b.g);
        float dL = mad(.5, d.b + d.r, d.g);
        float eL = mad(.5, e.b + e.r, e.g);
        float fL = mad(.5, f.b + f.r, f.g);
        float hL = mad(.5, h.b + h.r, h.g);

     // Contrast Threshold (Check if there is enough contrast to apply sharpening)
        float diffBD = max(abs(bL - eL), abs(dL - eL));
        float diffFH = max(abs(fL - eL), abs(hL - eL));
        float maxDiff = max(diffBD, diffFH);
        if (maxDiff < contrastThreshold)
        {
            return e;
        }
        
    // Noise detection.
        float nz = mad(.25, bL + dL + fL + hL, -eL);

        nz = clamp(abs(nz) / (max(max(bL, dL), max(eL, max(fL, hL))) - min(min(bL, dL), min(eL, min(fL, hL)))), 0., 1.);
        nz = mad(-.5, nz, 1.);
    // Min and max of ring.
        float3 mn4 = min(b, min(f, h));
        float3 mx4 = max(b, max(f, h));
    // Immediate constants for peak range.
        float2 peakC = float2(1., -4.);
    // Limiters, these need to be high precision RCPs.
        float3 hitMin = mn4 / (4. * mx4);
        float3 hitMax = (peakC.x - mx4) / mad(4., mn4, peakC.y);
        float3 lobeRGB = max(-hitMin, hitMax);
        float lobe = max(-FSR_RCAS_LIMIT, min(max(lobeRGB.r, max(lobeRGB.g, lobeRGB.b)), 0.)) * con;

    // Resolve, which needs the medium precision rcp approximation to avoid visible tonality changes.
        return mad(lobe, b + d + h + f, e) / mad(4., lobe, 1.);
    }

    float4 Rcas(sampler2D samp, float2 texcoord, float sharpness)
    {
        float2 fragCoord = texcoord * tex2Dsize(samp);
        float con = FsrRcasCon(sharpness);
        float3 col = FsrRcasF(fragCoord, con);
        return float4(col, 0);
    }

    float4 Out(float4 pos : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
    {
        return Rcas(ReShade::BackBuffer, texcoord, sharpness);
    }

/*-----------------.
| :: Techniques :: |
'-----------------*/

    technique RCAS
    {
        pass
        {
            VertexShader = PostProcessVS;
            PixelShader = Out;
        }
    }
}
