struct VS_OUT
{
	float4 pos		:	SV_POSITION;
	float2 uv		:	TEXCOORD0;
	float4 color	:	COLOR;
};


cbuffer cbTransformation : register(b0)
{
	matrix matWVP;
}


VS_OUT VS( float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR)
{
	VS_OUT output;
	//output.pos = mul(pos, matWVP);
	output.pos = pos;
	output.uv = uv;
	output.color = color;

	return output;
}



Texture2D colorMapTex : register(t0);
sampler samplerLinear : register(s0);

float4 PS( VS_OUT input) : SV_TARGET
{
	float4 color = input.color * colorMapTex.Sample(samplerLinear, input.uv);
	return color;
};