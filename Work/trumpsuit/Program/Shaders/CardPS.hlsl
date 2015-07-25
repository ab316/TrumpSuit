#include "ShaderCommon.hlsl"


float4 main(VS_OUT input) : SV_Target
{
	return input.diffuse * textureColorMap.Sample(samplerLinear, input.tex);
}