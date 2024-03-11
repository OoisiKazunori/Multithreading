/*
�P�F�\���̏���
*/

//���ʏ���---------------------------------------
cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D�ϊ��s��
}
cbuffer ColorBuffer : register(b1)
{
    float4 color; //�F
}
//���ʏ���---------------------------------------



//�P�F�Ή�---------------------------------------
struct ColorOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 color : COLOR;
};    

ColorOutput VSmain(float4 pos : POSITION)
{
    ColorOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    return op;
}

float4 PSmain(ColorOutput input) : SV_TARGET
{
    return input.color;
}
//�P�F�Ή�---------------------------------------



//�F�A�@���Ή�---------------------------------------
struct ColorNormalOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 color : COLOR;
    float3 normal : NORMAL; //�@���x�N�g��
};

ColorNormalOutput VSPosNormalmain(float4 pos : POSITION,float3 normal : NORMAL)
{
    ColorNormalOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    op.normal = normal;
    return op;
}

float4 PSPosNormalmain(ColorNormalOutput input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1));
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;

    return input.color * float4(brightness, brightness, brightness, 1);
}
//�F�A�@���Ή�---------------------------------------