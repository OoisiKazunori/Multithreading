#pragma once
#include<string>
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include <source_location>

struct DispatchData
{
	UINT x, y, z;
};

class ComputeShader
{
public:
	//�p�C�v���C���A���[�g�V�O�l�`���[����
	ComputeShader();
	void Generate(const ShaderOptionData& arg_shader, std::vector<KazBufferHelper::BufferData> arg_extraBuffer);
	void Generate(const ShaderOptionData& arg_shader, const RootSignatureDataTest& arg_rootsignature);
	void Compute(const DispatchData &arg_dispatch);


	std::vector<KazBufferHelper::BufferData>m_extraBufferArray;
private:
	//�`��ɕK�v�ȃo�b�t�@���R�}���h���X�g�ɐς�
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData>& BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);
	PipelineDuplicateBlocking m_piplineBuffer;
	ShaderDuplicateBlocking m_shaderBuffer;
	RootSignatureDuplicateBlocking m_rootSignatureBuffer;
	bool m_initFlag;
};