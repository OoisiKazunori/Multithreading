#include "Bloom.h"
#include "Buffer/DescriptorHeapMgr.h"
#include "Buffer/UavViewHandleMgr.h"
#include "../PostEffect/GaussianBlur.h"

PostEffect::Bloom::Bloom(KazBufferHelper::BufferData arg_bloomTargetTexture)
{

	/*===== �R���X�g���N�^ =====*/

	//�u���[����������Ώۂ̃e�N�X�`����ۑ����Ă����B
	m_targetTexture = arg_bloomTargetTexture;

	//�u���[���Ώۂ̈�U�ۑ��p�e�N�X�`����p�ӁB
	m_targetCopyTexture = KazBufferHelper::SetUAVTexBuffer(TEXSIZE.x, TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_targetCopyTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_targetCopyTexture.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), TEXSIZE.x * TEXSIZE.y),
		m_targetCopyTexture.bufferWrapper->GetBuffer().Get()
	);
	//�u���[���̉ߒ��ŕK�v�ȃe�N�X�`����p�ӁB
	for (auto& tex : m_blurTextures) {
		tex = KazBufferHelper::SetUAVTexBuffer(TEXSIZE.x, TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		tex.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			tex.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), TEXSIZE.x * TEXSIZE.y),
			tex.bufferWrapper->GetBuffer().Get()
		);
	}

	//�ŏI�����p�̃e�N�X�`����p�ӁB
	{
		//�u���[���p�̃V�F�[�_�[��p�ӁB
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_targetCopyTexture,
			 m_blurTextures[0],
			 m_blurTextures[1],
			 m_blurTextures[2],
			 m_blurTextures[3],
			 m_targetTexture,
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
		extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
		extraBuffer[3].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[3].rootParamType = GRAPHICS_PRAMTYPE_TEX4;
		extraBuffer[4].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[4].rootParamType = GRAPHICS_PRAMTYPE_TEX5;
		extraBuffer[5].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[5].rootParamType = GRAPHICS_PRAMTYPE_TEX6;
		m_composeShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/Bloom/" + "ComposeShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
	}

	//�u���[�p�X
	m_blur[0] = std::make_shared<GaussianBlur>(m_targetCopyTexture);
	m_blur[1] = std::make_shared<GaussianBlur>(m_blurTextures[0]);
	m_blur[2] = std::make_shared<GaussianBlur>(m_blurTextures[1]);
	m_blur[3] = std::make_shared<GaussianBlur>(m_blurTextures[2]);
	m_blur[4] = std::make_shared<GaussianBlur>(m_blurTextures[3]);

}

void PostEffect::Bloom::Apply()
{

	/*===== �u���[���������� =====*/

	//�u���[����������Ώۂ̃e�N�X�`�����R�s�[���ĕۑ����Ă����B
	CopyTexture(m_targetCopyTexture, m_targetTexture);

	//�R�s�[��̃e�N�X�`���Ƀu���[��������B
	m_blur[0]->ApplyBlur();

	//�R�s�[��̃e�N�X�`����쐣���u���[���p�̃e�N�X�`���ɓ���Ă����B
	CopyTexture(m_blurTextures[0], m_targetCopyTexture);

	//�쐣���u���[���p�e�N�X�`���Ƀu���[��������B
	m_blur[1]->ApplyBlur();

	//�R�s�[��̃e�N�X�`����쐣���u���[���p�̃e�N�X�`���ɓ���Ă����B
	CopyTexture(m_blurTextures[1], m_blurTextures[0]);

	//�쐣���u���[���p�e�N�X�`���Ƀu���[��������B
	m_blur[2]->ApplyBlur();

	//�R�s�[��̃e�N�X�`����쐣���u���[���p�̃e�N�X�`���ɓ���Ă����B
	CopyTexture(m_blurTextures[2], m_blurTextures[1]);

	//�쐣���u���[���p�e�N�X�`���Ƀu���[��������B
	m_blur[3]->ApplyBlur();

	//�R�s�[��̃e�N�X�`����쐣���u���[���p�̃e�N�X�`���ɓ���Ă����B
	CopyTexture(m_blurTextures[3], m_blurTextures[2]);

	//�쐣���u���[���p�e�N�X�`���Ƀu���[��������B
	m_blur[4]->ApplyBlur();

	//�S�Ẵe�N�X�`������������B
	DispatchData finalPath;
	finalPath.x = static_cast<UINT>(TEXSIZE.x / 16) + 1;
	finalPath.y = static_cast<UINT>(TEXSIZE.y / 16) + 1;
	finalPath.z = static_cast<UINT>(1);
	m_composeShader.Compute(finalPath);

}

void PostEffect::Bloom::CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture)
{

	/*===== �����Y�t���A�e�N�X�`���̃R�s�[���쐬 =====*/

	//�����Y�t���A��������Ώۂ̃X�e�[�^�X��ύX�B
	BufferStatesTransition(arg_srcTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	//��U�R�s�[���Ă����p�̃e�N�X�`���̃X�e�[�^�X��ύX�B
	BufferStatesTransition(arg_destTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

	//�R�s�[�����s
	DirectX12CmdList::Instance()->cmdList->CopyResource(arg_destTexture.bufferWrapper->GetBuffer().Get(), arg_srcTexture.bufferWrapper->GetBuffer().Get());

	//�����Y�t���A��������Ώۂ̃X�e�[�^�X�����ɖ߂��B
	BufferStatesTransition(arg_srcTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//�R�s�[��̃X�e�[�^�X�����ɖ߂��B
	BufferStatesTransition(arg_destTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

}

void PostEffect::Bloom::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
{
	D3D12_RESOURCE_BARRIER barriers[] = {
	CD3DX12_RESOURCE_BARRIER::Transition(
		arg_resource,
		arg_before,
		arg_after),
	};
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
}
