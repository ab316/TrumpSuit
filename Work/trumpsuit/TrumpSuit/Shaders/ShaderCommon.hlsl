struct VS_IN
{
	float4 pos		:	POSITION;
	float3 normal	:	NORMAL;
	float2 tex		:	TEXCOORD;
};

struct VS_OUT
{
	float4 pos		:	SV_POSITION;
	float4 diffuse	:	COLOR0;
	float2 tex		:	TEXCOORD;
};


Texture2D textureColorMap : register(t0);
sampler samplerLinear : register(s0);


cbuffer cbTransformation : register(b0)
{
	matrix	matWorld;
	matrix	matView;
	matrix	matProjection;
	float4	fColor;
}