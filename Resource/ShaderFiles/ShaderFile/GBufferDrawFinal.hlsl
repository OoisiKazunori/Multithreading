bool IsEnableToUseMaterialTex(float4 texColor)
{
    return texColor.a <= 0.0f;
};


struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer MatBuffer : register(b0)
{
    matrix Mat;
}

cbuffer LightNum : register(b1)
{
    int lightArrayNum;
}

cbuffer LightStatus : register(b2)
{
    float lightRadius;
}

RWStructuredBuffer<float3>LightBuffer:register(u0);

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(Mat, pos);
	op.uv = uv;
	return op;
}

Texture2D<float4> AlbedoTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
Texture2D<float4> WorldTex : register(t2);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 albedoColor = AlbedoTex.Sample(smp, input.uv);
    float4 worldNormalVec = NormalTex.Sample(smp, input.uv);
    float4 worldPos = WorldTex.Sample(smp, input.uv);

    //法線マップを使わないならAlbedoを出力する
    if(IsEnableToUseMaterialTex(worldNormalVec))
    {
        return albedoColor;
    }
    
    float3 lightOutput = float3(0.5f,0.5f,0.5f);
    for(int i = 0; i < lightArrayNum; ++i)
    {
        float3 lightV = LightBuffer[i] - worldPos.xyz;
        float len = length(lightV);
        //距離が20以上なら計算しない。
        if(lightRadius <= len)
        {
            continue;
        }
 
        lightV = normalize(lightV);
        float3 attenVec = float3(0.0f,0.0f,0.0f);
        float atten = saturate(1.0f / (attenVec.x + attenVec.y * len + attenVec.z * len * len));
        float bright = dot(normalize(worldNormalVec.xyz),lightV);
        float3 lightColor = float3(1.0f,1.0f,1.0f);

        float ambient = 0.5f;
        bright = clamp(bright,ambient,1.0f);
        float3 light = (bright * atten + ambient) * lightColor;
        lightOutput = saturate(light);
        break;
    }
    float4 outputColor = float4(albedoColor.xyz * lightOutput, 1.0f);
    return outputColor;
}