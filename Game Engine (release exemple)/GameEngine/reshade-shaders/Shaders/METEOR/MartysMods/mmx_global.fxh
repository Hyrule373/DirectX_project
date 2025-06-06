/*=============================================================================

    Copyright (c) Pascal Gilcher. All rights reserved.

 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 
=============================================================================*/

#pragma once 

//Helpers for all sorts of device queries

#define GPU_VENDOR_NVIDIA      0x10DE
#define GPU_VENDOR_AMD         0x1002
#define GPU_VENDOR_INTEL       0x8086

#define RENDERER_D3D9          0x9000
#define RENDERER_D3D10         0xA000 //>=
#define RENDERER_D3D11         0xB000 //>=
#define RENDERER_D3D12         0xC000 //>=
#define RENDERER_OPENGL       0x10000 //>=
#define RENDERER_VULKAN       0x20000 //>=

#if __RENDERER__ >= RENDERER_D3D11
 #define _COMPUTE_SUPPORTED          1
#else 
 #define _COMPUTE_SUPPORTED          0
#endif

#if __RENDERER__ >= RENDERER_D3D10
 #define _BITWISE_SUPPORTED          1
#else 
 #define _BITWISE_SUPPORTED          0
#endif

//Frequently used things / ReShade FX extensions

static const float2 BUFFER_PIXEL_SIZE = float2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT);
static const uint2 BUFFER_SCREEN_SIZE = uint2(BUFFER_WIDTH, BUFFER_HEIGHT);
static const float2 BUFFER_ASPECT_RATIO = float2(1.0, BUFFER_WIDTH * BUFFER_RCP_HEIGHT);

//DLSS/FSR/XeSS/TAAU compatibility features

#define DLSS_QUALITY           0.66666666 //    1 / 1.5
#define DLSS_BALANCED          0.58000000 //    1 / 1.72
#define DLSS_PERFORMANCE       0.50000000 //    1 / 2.0 
#define DLSS_ULTRA_PERFORMANCE 0.33333333 //    1 / 3.0

#define FSR_ULTRA_QUALITY      0.77000000 //    1 / 1.3
#define FSR_QUALITY            0.66666666 //    1 / 1.5
#define FSR_BALANCED           0.58823529 //    1 / 1.7
#define FSR_PERFORMANCE        0.50000000 //    1 / 2.0

//if we write it this way instead of ifdef, ReShade won't add this to the GUI
#ifdef _MARTYSMODS_TAAU_SCALE //this works with both the "enum" above and actual literals like 0.5
       
    //use the shit we have
    #define BUFFER_WIDTH_DLSS       int(BUFFER_WIDTH  * _MARTYSMODS_TAAU_SCALE + 0.5)
    #define BUFFER_HEIGHT_DLSS      int(BUFFER_HEIGHT * _MARTYSMODS_TAAU_SCALE + 0.5)
    #define BUFFER_RCP_WIDTH_DLSS   (1.0 / (BUFFER_WIDTH_DLSS))
    #define BUFFER_RCP_HEIGHT_DLSS  (1.0 / (BUFFER_HEIGHT_DLSS))   

#else 

    #define BUFFER_WIDTH_DLSS       BUFFER_WIDTH 
    #define BUFFER_HEIGHT_DLSS      BUFFER_HEIGHT
    #define BUFFER_RCP_WIDTH_DLSS   BUFFER_RCP_WIDTH
    #define BUFFER_RCP_HEIGHT_DLSS  BUFFER_RCP_HEIGHT

#endif

static const float2 BUFFER_PIXEL_SIZE_DLSS   = float2(BUFFER_RCP_WIDTH_DLSS, BUFFER_RCP_HEIGHT_DLSS);
static const uint2 BUFFER_SCREEN_SIZE_DLSS   = uint2(BUFFER_WIDTH_DLSS, BUFFER_HEIGHT_DLSS);
static const float2 BUFFER_ASPECT_RATIO_DLSS = float2(1.0, BUFFER_WIDTH_DLSS * BUFFER_RCP_HEIGHT_DLSS);

void FullscreenTriangleVS(in uint id : SV_VertexID, out float4 vpos : SV_Position, out float2 uv : TEXCOORD)
{
	uv = id.xx == uint2(2, 1) ? 2.0.xx : 0.0.xx;  
	vpos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
}

struct PSOUT1 
{ 
    float4 t0 : SV_Target0; 
};
struct PSOUT2 
{ 
    float4 t0 : SV_Target0, 
           t1 : SV_Target1;
};
struct PSOUT3 
{ 
    float4 t0 : SV_Target0, 
           t1 : SV_Target1, 
           t2 : SV_Target2; 
};
struct PSOUT4 
{ 
    float4 t0 : SV_Target0, 
           t1 : SV_Target1, 
           t2 : SV_Target2, 
           t3 : SV_Target3; 
};

//why is smoothstep a thing but not this also...
#define linearstep(_a, _b, _x) saturate(((_x) - (_a)) * rcp((_b) - (_a)))
//why is log10 a thing but not this also...
#define exp10(_x) pow(10.0, (_x))
//why 1e-8? On some platforms the compiler truncates smaller constants? idfk, caused lots of trouble before...
#define safenormalize(_x) ((_x) * rsqrt(max(1e-8, dot((_x), (_x)))))
//condition true selects left in ternary but right in lerp, lerp is more intuitive but produces more instructions
//this is (the?) solution for that
#define select(_lhs, _rhs, _cond) (_cond)?(_rhs):(_lhs)
//quite often I need to dot a vector with itself but don't necessarily want to create a new variable for it
#define dot2(_x) dot((_x), (_x))

#define MAX3(_type) _type max3(_type a, _type b, _type c){ return max(max(a, b), c);}
#define MAX4(_type) _type max4(_type a, _type b, _type c, _type d){ return max(max(a, b), max(c, d));}
#define MIN3(_type) _type min3(_type a, _type b, _type c){ return min(min(a, b), c);}
#define MIN4(_type) _type min4(_type a, _type b, _type c, _type d){ return min(min(a, b), min(c, d));}
#define MED3(_type) _type med3(_type a, _type b, _type c) { return clamp(a, min(b, c), max(b, c));}

MAX3(float)MAX3(float2)MAX3(float3)MAX3(float4)MAX3(int)MAX3(int2)MAX3(int3)MAX3(int4)
MAX4(float)MAX4(float2)MAX4(float3)MAX4(float4)MAX4(int)MAX4(int2)MAX4(int3)MAX4(int4)
MIN3(float)MIN3(float2)MIN3(float3)MIN3(float4)MIN3(int)MIN3(int2)MIN3(int3)MIN3(int4)
MIN4(float)MIN4(float2)MIN4(float3)MIN4(float4)MIN4(int)MIN4(int2)MIN4(int3)MIN4(int4)
MED3(float)MED3(int)

#undef MAX3
#undef MAX4
#undef MIN3
#undef MIN4
#undef MED3

float maxc(float  t) {return t;}
float maxc(float2 t) {return max(t.x, t.y);}
float maxc(float3 t) {return max3(t.x, t.y, t.z);}
float maxc(float4 t) {return max4(t.x, t.y, t.z, t.w);}
float minc(float  t) {return t;}
float minc(float2 t) {return min(t.x, t.y);}
float minc(float3 t) {return min3(t.x, t.y, t.z);}
float minc(float4 t) {return min4(t.x, t.y, t.z, t.w);}
float medc(float3 t) {return med3(t.x, t.y, t.z);}

float4 tex2Dlod(sampler s, float2 uv, float mip)
{
    return tex2Dlod(s, float4(uv, 0, mip));
}

//log2 macro for uints up to 16 bit, inefficient in runtime but preprocessor doesn't care
#define T1(x,n) ((uint(x)>>(n))>0)
#define T2(x,n) (T1(x,n)+T1(x,n+1))
#define T4(x,n) (T2(x,n)+T2(x,n+2))
#define T8(x,n) (T4(x,n)+T4(x,n+4))
#define LOG2(x) (T8(x,0)+T8(x,8))

#define CEIL_DIV(num, denom) ((((num) - 1) / (denom)) + 1)