#include "ShaderCommon.hlsl"

VS_OUT main(VS_IN input)
{
	VS_OUT output;
	output.pos = mul(input.pos, mul(mul(matWorld, matView), matProjection));
	output.tex = input.tex;

	float3 normal = normalize(mul(input.normal, mul(matWorld, matView)));
	float3 light = float3(0.0f, 0.2f, -0.3f);
	float I = normalize(dot(normal, light));
	output.diffuse = float4(I, I, I, 1.0f) * fColor;

	return output;
}