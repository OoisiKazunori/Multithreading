#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Math/KazMath.h"
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

/// <summary>
/// �`��̐������
/// </summary>
class GenerateRender
{
public:
	//�`����̐������߂�ς�
	const DrawFuncData::DrawData *SetPipeline(DrawFuncData::DrawCallData &arg_drawData, bool arg_deleteInSceneFlag = false);
	/// <summary>
	/// �X�^�b�N���ꂽ�`����̐���(�}���`�X���b�h�Ή��ׂ̈Ɉꊇ�Ő������鏈��)
	/// </summary>
	void GeneratePipeline();
	/// <summary>
	/// �O�V�[���̕`�施�ߔj��
	/// </summary>
	void ReleasePipeline();

	void ObjectRender(const DrawFuncData::DrawData *arg_drawData);
	void UIRender(const DrawFuncData::DrawData *arg_drawData);

private:
	//���O��������-----
	//�`�搶�����߂̃L���[
	std::list<DrawFuncData::DrawCallData *>m_drawCallStackDataArray;
	//�`�施�߂̃f�[�^
	std::vector<std::unique_ptr<DrawFuncData::DrawData>>m_drawCallArray;
	//�`�施�߂̃L���[
	std::list<const DrawFuncData::DrawData *>m_stackDataArray;
	std::list<const DrawFuncData::DrawData *>m_uiStackDataArray;


	//�p�C�v���C���̏��----------------------------------------

	PipelineDuplicateBlocking piplineBufferMgr;
	ShaderDuplicateBlocking shaderBufferMgr;
	RootSignatureDuplicateBlocking rootSignatureBufferMgr;

	//�p�C�v���C���̏��----------------------------------------
};
