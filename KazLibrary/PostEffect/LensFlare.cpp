#include "LensFlare.h"
#include "Buffer/GBufferMgr.h"
#include "Buffer/DescriptorHeapMgr.h"
#include "Buffer/UavViewHandleMgr.h"
#include "../PostEffect/GaussianBlur.h"
#include "../PostEffect/Bloom.h"

namespace PostEffect {

	LensFlare::LensFlare(KazBufferHelper::BufferData arg_lnesflareTargetTexture, KazBufferHelper::BufferData arg_emissiveTexture)
	{

		/*===== �R���X�g���N�^ =====*/

		//�����Y�t���A��������Ώۂ̃e�N�X�`����ۑ����Ă����B
		m_lensFlareTargetTexture = arg_lnesflareTargetTexture;

		//�����Y�t���A�Ώۂ̈�U�ۑ��p�e�N�X�`����p�ӁB
		m_lensFlareTargetCopyTexture = KazBufferHelper::SetUAVTexBuffer(COPY_TEXSIZE.x, COPY_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_lensFlareTargetCopyTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_lensFlareTargetCopyTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), COPY_TEXSIZE.x * COPY_TEXSIZE.y),
			m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get()
		);
		//�����Y�t���A�̌��ʏo�͗p�̃e�N�X�`����p�ӁB
		m_lensFlareTexture = KazBufferHelper::SetUAVTexBuffer(LENSFLARE_TEXSIZE.x, LENSFLARE_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_lensFlareTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_lensFlareTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), LENSFLARE_TEXSIZE.x * LENSFLARE_TEXSIZE.y),
			m_lensFlareTexture.bufferWrapper->GetBuffer().Get()
		);
		//�u���[���p�̃e�N�X�`����p�ӁB
		m_bloomTexture = arg_emissiveTexture;
		m_bloom = std::make_shared<PostEffect::Bloom>(m_bloomTexture);
		//�����Y�̐F�e�N�X�`�������[�h
		m_lensColorTexture = *TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensColor.png");
		m_lendDirtTexture = *TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensDirt.png");
		m_lensStarTexture = *TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensStar.png");
		//�X�^�[�o�[�X�g��]�p�̃o�b�t�@��p�ӁB
		m_cametaVecConstBuffer = KazBufferHelper::SetConstBufferData(sizeof(CameraVec));
		{
			//�����Y�t���A�p�̃V�F�[�_�[��p�ӁB
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_lensColorTexture,
				 m_lensFlareTargetCopyTexture,
				 m_lensFlareTexture,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
			m_lensFlareShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "LensFlareShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}
		{
			//�ŏI���H and �����p�X��p�ӁB
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_lendDirtTexture,
				 m_lensStarTexture,
				 m_lensFlareTexture,
				 m_bloomTexture,
				 m_lensFlareTargetTexture,
				 m_cametaVecConstBuffer,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
			extraBuffer[3].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[3].rootParamType = GRAPHICS_PRAMTYPE_TEX4;
			extraBuffer[4].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[4].rootParamType = GRAPHICS_PRAMTYPE_TEX5;
			extraBuffer[5].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
			extraBuffer[5].rootParamType = GRAPHICS_PRAMTYPE_DATA;
			m_finalProcessingShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "FinalProcessingShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}

		//�u���[�p�X
		m_blurPath = std::make_shared<GaussianBlur>(m_lensFlareTexture);

	}

	void LensFlare::Apply() {

		/*===== �����Y�t���A�������� =====*/


		/*- �@�����Y�t���A��������Ώۂ���U�R�s�[���Ă����B -*/

		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "LensFlare Path");
		GenerateCopyOfLensFlareTexture();


		/*- �A�����Y�t���A�p�X -*/

		//�����Y�t���A��������B
		DispatchData lensFlareData;
		lensFlareData.x = static_cast<UINT>(LENSFLARE_TEXSIZE.x / 16) + 1;
		lensFlareData.y = static_cast<UINT>(LENSFLARE_TEXSIZE.y / 16) + 1;
		lensFlareData.z = static_cast<UINT>(1);
		m_lensFlareShader.Compute(lensFlareData);
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


		//�u���[�����������Ⴄ�B
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Bloom");
		//CopyTexture(m_bloomTexture, m_lensFlareTargetCopyTexture);
		m_bloom->Apply();
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


		/*- �B�u���[�p�X -*/

		//�����Y�t���A�Ƀu���[��������B
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Blur Path");
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

		/*- �C�ŏI���H�p�X -*/

		//�J�����̏���ۑ����ē]���B
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Final Path");
		m_cameraVec.m_cameraZVec = CameraMgr::Instance()->GetCameraAxis().z;
		m_cameraVec.m_cameraXVec = CameraMgr::Instance()->GetCameraAxis().x;
		KazMath::Vec3<float> yVec = m_cameraVec.m_cameraZVec.Cross(m_cameraVec.m_cameraXVec);
		m_cametaVecConstBuffer.bufferWrapper->TransData(&m_cameraVec, sizeof(CameraVec));

		//�ŏI���H and �������s���B
		DispatchData finalPath;
		finalPath.x = static_cast<UINT>(BACKBUFFER_SIZE.x / 16) + 1;
		finalPath.y = static_cast<UINT>(BACKBUFFER_SIZE.y / 16) + 1;
		finalPath.z = static_cast<UINT>(1);
		m_finalProcessingShader.Compute(finalPath);
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


	}

	void LensFlare::GenerateCopyOfLensFlareTexture()
	{

		/*===== �����Y�t���A�e�N�X�`���̃R�s�[���쐬 =====*/

		//�����Y�t���A��������Ώۂ̃X�e�[�^�X��ύX�B
		BufferStatesTransition(m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		//��U�R�s�[���Ă����p�̃e�N�X�`���̃X�e�[�^�X��ύX�B
		BufferStatesTransition(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		//�R�s�[�����s
		DirectX12CmdList::Instance()->cmdList->CopyResource(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get());

		//�����Y�t���A��������Ώۂ̃X�e�[�^�X�����ɖ߂��B
		BufferStatesTransition(m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		//�R�s�[��̃X�e�[�^�X�����ɖ߂��B
		BufferStatesTransition(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

	void LensFlare::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
	{
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			arg_resource,
			arg_before,
			arg_after),
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
	}

	void LensFlare::CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture)
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

}