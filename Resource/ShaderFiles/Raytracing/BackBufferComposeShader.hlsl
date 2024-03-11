
//���͏��
RWTexture2D<float4> BackBuffer : register(u0);

//���͏��
RWTexture2D<float4> Raytracing : register(u1);

//�o�͐�UAV  
RWTexture2D<float4> OutputImg : register(u2);

//�F�����pCBV
struct IroShuusaData
{
    float4 m_facter;
};
ConstantBuffer<IroShuusaData> IroShuusa : register(b0);

float EaseInExp(float arg_rate)
{
    return arg_rate == 0.0f ? 0.0f : pow(2.0f, 10.0f * arg_rate - 10.0f);
}
float EaseInQuint(float arg_rate)
{
    return arg_rate * arg_rate * arg_rate * arg_rate * arg_rate;
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //�w�i�F�������烌�C�g���̌��ʂ�`��
    bool isBackGroundColor = BackBuffer[DTid.xy].a <= 0.0f;
    if (isBackGroundColor)
    {
        
        //�F�������������Ⴄ�B
        float2 center = float2(1280.0f / 2.0f, 720.0f / 2.0f);
        float2 position = float2(DTid.xy);
        float2 dir = normalize(center - position);
        float posLength = length(center - position);
        float lengthRate = posLength / center;
        
        OutputImg[DTid.xy].x = Raytracing[DTid.xy + dir * (EaseInExp(lengthRate) * IroShuusa.m_facter.x)].x;
        OutputImg[DTid.xy].y = Raytracing[DTid.xy + dir * (EaseInExp(lengthRate) * IroShuusa.m_facter.y)].y;
        OutputImg[DTid.xy].z = Raytracing[DTid.xy + dir * (EaseInExp(lengthRate) * IroShuusa.m_facter.z)].z;
        
    }
    else
    {
        float alpha = BackBuffer[DTid.xy].w;
        OutputImg[DTid.xy] = BackBuffer[DTid.xy] + Raytracing[DTid.xy] * (1.0f - alpha);
    
    }
    
}