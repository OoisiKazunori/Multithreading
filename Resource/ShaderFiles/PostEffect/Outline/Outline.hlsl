//���͏��
RWTexture2D<float4> OutlineMap : register(u0);

//�o�͐�UAV  
RWTexture2D<float4> Albedo : register(u1);
RWTexture2D<float4> Emissive : register(u2);

cbuffer Dissolve : register(b0)
{
    float4 m_outlineColor;
}

float4 SamplingPixel(uint2 arg_uv)
{
    return OutlineMap[uint2(clamp(arg_uv.x, 0, 1280), clamp(arg_uv.y, 0, 720))].xyzw;
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    float4 samplingPos = SamplingPixel(DTid.xy);
    if (0.0f < length(samplingPos.xyz))
    {
        Albedo[DTid.xy] = float4(0, 0, 0, 0);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
        return;
    }
    
    const int OUTLINE_THICKNESS = 2;
    
    //����ɎQ�����Ă��Ȃ��~�l����
    bool isNoGatheringMineral = false;
    //�G��
    bool isEnemy = false;
    //�F�D
    bool isPlayer = false;
    //�U����Ԃ̃~�l����
    bool isMineralAttackMode = false;
    //���ނ⌚�z���Ȃ�
    bool isbuildingAndMaterial = false;
    
    //�E�����`�F�b�N
    float4 sample = SamplingPixel(DTid.xy + uint2(OUTLINE_THICKNESS, 0));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�������`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(-OUTLINE_THICKNESS, 0));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�㑤���`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(0, -OUTLINE_THICKNESS));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�������`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(0, OUTLINE_THICKNESS));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�E�����`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(OUTLINE_THICKNESS, OUTLINE_THICKNESS));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�E�����`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(-OUTLINE_THICKNESS, -OUTLINE_THICKNESS));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�E�����`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(-OUTLINE_THICKNESS, OUTLINE_THICKNESS));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    
    //�E�����`�F�b�N
    sample = SamplingPixel(DTid.xy + uint2(OUTLINE_THICKNESS, -OUTLINE_THICKNESS));
    isPlayer |= abs(0.1f - sample.x) < 0.01f;
    isEnemy |= abs(0.2f - sample.x) < 0.01f;
    isNoGatheringMineral |= abs(0.3f - sample.x) < 0.01f;
    isMineralAttackMode |= abs(0.4f - sample.x) < 0.01f;
    isbuildingAndMaterial |= abs(0.5f - sample.x) < 0.01f;
    

    if (isPlayer)
    {
        Albedo[DTid.xy] = float4(0.59f, 0.84f, 0.31f, 1.0f);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
    }
    else if (isEnemy)
    {
        Albedo[DTid.xy] = float4(0.70f, 0.21f, 0.34f, 1.0f);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
    }
    else if (isNoGatheringMineral)
    {
        Albedo[DTid.xy] = float4(0.90f, 0.94f, 0.94f, 1.0f);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
    }
    else if (isMineralAttackMode)
    {
        Albedo[DTid.xy] = float4(0.94f, 0.95f, 0.49f, 1.0f);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
    }
    else if (isbuildingAndMaterial)
    {
        Albedo[DTid.xy] = float4(0.88f, 0.66f, 0.27f, 1.0f);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
    }
    else
    {
        Albedo[DTid.xy] = float4(0, 0, 0, 0);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
    }

}