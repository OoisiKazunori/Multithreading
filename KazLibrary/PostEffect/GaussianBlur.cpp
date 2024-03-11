#include "GaussianBlur.h"
#include "Buffer/UavViewHandleMgr.h"
#include "Buffer/DescriptorHeapMgr.h"

namespace PostEffect {

	GaussianBlur::GaussianBlur(KazBufferHelper::BufferData arg_blurTargetTexture)
	{

		/*===== �R���X�g���N�^ =====*/

		//�u���[��������Ώۂ�ۑ��B
		m_blurTargetTexture = arg_blurTargetTexture;

		//�e�u���[�̏o�͌��ʗp�̃e�N�X�`���𐶐��B
		m_blurXResultTexture = KazBufferHelper::SetUAVTexBuffer(BLURX_TEXSIZE.x, BLURX_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_blurXResultTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_blurXResultTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), BLURX_TEXSIZE.x * BLURX_TEXSIZE.y),
			m_blurXResultTexture.bufferWrapper->GetBuffer().Get()
		);
		m_blurYResultTexture = KazBufferHelper::SetUAVTexBuffer(BLURY_TEXSIZE.x, BLURY_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_blurYResultTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_blurYResultTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), BLURY_TEXSIZE.x * BLURY_TEXSIZE.y),
			m_blurYResultTexture.bufferWrapper->GetBuffer().Get()
		);

		//�K�E�V�A���u���[�̋����p�o�b�t�@��p�ӁB
		m_blurPowerConstBuffer = KazBufferHelper::SetConstBufferData(sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT);

		//�e��R���s���[�g�V�F�[�_�[�𐶐�����B
		{
			//X�u���[�̃V�F�[�_�[
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_blurTargetTexture,
				 m_blurXResultTexture,
				 m_blurPowerConstBuffer,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
			m_blurXShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/GaussianBlur/" + "BlurX.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}
		{
			//Y�u���[�̃V�F�[�_�[
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_blurXResultTexture,
				 m_blurYResultTexture,
				 m_blurPowerConstBuffer,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
			m_blurYShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/GaussianBlur/" + "BlurY.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}
		{
			//�ŏI�����p�̃V�F�[�_�[
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_blurYResultTexture,
				 m_blurTargetTexture,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			m_composeShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/GaussianBlur/" + "BlurFinal.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}

	}

	void GaussianBlur::ApplyBlur()
	{

		/*===== �u���[�����s =====*/

		//�u���[�̏d�݂��Z�b�g�B
		CalcBloomWeightsTableFromGaussian();
		m_blurPowerConstBuffer.bufferWrapper->TransData(m_blurWeight.data(), sizeof(float) * BLOOM_GAUSSIAN_WEIGHTS_COUNT);

		//�o���A��\��B
		{
			D3D12_RESOURCE_BARRIER barrier[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			m_blurTargetTexture.bufferWrapper->GetBuffer().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			m_blurXResultTexture.bufferWrapper->GetBuffer().Get())
			};
			DirectX12CmdList::Instance()->cmdList->ResourceBarrier(2, barrier);
		}

		//X�u���[
		DispatchData dispatchData;
		dispatchData.x = static_cast<UINT>(BLURX_TEXSIZE.x / 16) + 1;
		dispatchData.y = static_cast<UINT>(BLURX_TEXSIZE.y / 16) + 1;
		dispatchData.z = static_cast<UINT>(1);
		m_blurXShader.Compute(dispatchData);

		//�o���A��\��B
		{
			D3D12_RESOURCE_BARRIER barrier[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			m_blurXResultTexture.bufferWrapper->GetBuffer().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			m_blurYResultTexture.bufferWrapper->GetBuffer().Get())
			};
			DirectX12CmdList::Instance()->cmdList->ResourceBarrier(2, barrier);
		}

		//Y�u���[
		dispatchData.x = static_cast<UINT>(BLURY_TEXSIZE.x / 16) + 1;
		dispatchData.y = static_cast<UINT>(BLURY_TEXSIZE.y / 16) + 1;
		dispatchData.z = static_cast<UINT>(1);
		m_blurYShader.Compute(dispatchData);

		//�o���A��\��B
		{
			D3D12_RESOURCE_BARRIER barrier[] = { CD3DX12_RESOURCE_BARRIER::UAV(
			m_blurYResultTexture.bufferWrapper->GetBuffer().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
			m_blurTargetTexture.bufferWrapper->GetBuffer().Get())
			};
			DirectX12CmdList::Instance()->cmdList->ResourceBarrier(2, barrier);
		}

		//�����p
		dispatchData.x = static_cast<UINT>(1280.0f / 16) + 1;
		dispatchData.y = static_cast<UINT>(740.0f / 16) + 1;
		dispatchData.z = static_cast<UINT>(1);
		m_composeShader.Compute(dispatchData);

	}

	void GaussianBlur::CalcBloomWeightsTableFromGaussian()
	{

		/*===== �u���[�̏d�݂��v�Z =====*/

		//�d�݂̍��v���L�^����ϐ����`����B
		float total = 0;

		//��������K�E�X�֐���p���ďd�݂��v�Z���Ă���B
		//���[�v�ϐ���x����e�N�Z������̋����B
		for (int x = 0; x < BLOOM_GAUSSIAN_WEIGHTS_COUNT; ++x)
		{
			m_blurWeight[x] = expf(-0.5f * static_cast<float>(x * x) / BLUR_POWER);
			total += 2.0f * m_blurWeight.at(x);
		}

		//�d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���B
		for (int i = 0; i < BLOOM_GAUSSIAN_WEIGHTS_COUNT; ++i)
		{
			m_blurWeight[i] /= total;
		}

	}

}