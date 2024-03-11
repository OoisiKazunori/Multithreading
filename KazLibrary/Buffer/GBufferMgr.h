#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

struct DessolveOutline {
	KazMath::Vec4<float> m_dissolve;
	KazMath::Vec4<float> m_outline;
};

namespace PostEffect {
	class GaussianBlur;
	class Outline;
}
class ComputeShader;

/// <summary>
/// G-Buffer�̊Ǘ��N���X
/// </summary>
class GBufferMgr :public ISingleton<GBufferMgr>
{
public:
	enum BufferType
	{
		NONE = -1,
		ALBEDO,
		NORMAL,
		R_M_S_ID,
		WORLD,
		EMISSIVE,
		MAX
	};
	GBufferMgr();

	std::vector<RESOURCE_HANDLE> GetRenderTarget();
	std::vector<DXGI_FORMAT> GetRenderTargetFormat();
	void SetCameraPos(DirectX::XMFLOAT3 arg_pos, DirectX::XMMATRIX arg_viewMat, DirectX::XMMATRIX arg_projMat)
	{
		m_cameraEyePosData.m_eyePos = KazMath::Vec3<float>(arg_pos.x, arg_pos.y, arg_pos.z);
		m_cameraEyePosData.m_viewMat = DirectX::XMMatrixInverse(nullptr, arg_viewMat);
		m_cameraEyePosData.m_projMat = DirectX::XMMatrixInverse(nullptr, arg_projMat);;
		m_cameraPosBuffer.bufferWrapper->TransData(&m_cameraEyePosData,sizeof(CameraEyePosBufferData));
	};
	const KazBufferHelper::BufferData &GetEyePosBuffer()
	{
		return m_cameraPosBuffer;
	}


	/// <summary>
	/// �f�o�b�N�p�̕`��m�F
	/// </summary>
	void DebugDraw();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(BufferType arg_type);

	const KazBufferHelper::BufferData &GetFinalBuffer()
	{
		return m_finalGBuffer;
	};

	const KazBufferHelper::BufferData &GetRayTracingBuffer()
	{
		return m_raytracingGBuffer;
	};

	const KazBufferHelper::BufferData& GetLensFlareBuffer()
	{
		return m_lensFlareLuminanceGBuffer;
	};

	const KazBufferHelper::BufferData& GetEmissiveGBuffer()
	{
		return m_emissiveGBuffer;
	};

	const KazBufferHelper::BufferData& GetBackBufferCopyBuffer()
	{
		return m_backBufferCopyBuffer;
	};

	const KazBufferHelper::BufferData& GetBackBufferCompositeBuffer()
	{
		return m_backBufferCompositeBuffer;
	};

	//�o�b�N�o�b�t�@������
	void ComposeBackBuffer();

	//�����Y�t���A�p��GBuffer�Ƀu���[��������B
	void ApplyLensFlareBlur();

	//�����Y�t���A�ƃV�[���摜����������B
	void ComposeLensFlareAndScene();

	//�o�b�t�@�̃X�e�[�^�X��J�ځB
	void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

	//���C�g�p�\����
	struct DirLight {
		KazMath::Vec3<float> m_dir;
		int m_isActive;
	};
	struct PointLight {
		KazMath::Vec3<float> m_pos;
		float m_power;		//���C�g���e����^����ő勗��
		KazMath::Vec3<float> m_pad;	//�p�����[�^�[���K�v�ɂȂ�����K���ɖ��O���ĕϐ���ǉ����Ă��������I
		int m_isActive;
	};
	struct LightConstData {
		DirLight m_dirLight;
		std::array<PointLight, 10> m_pointLight;
	}m_lightConstData;

	KazBufferHelper::BufferData m_lightBuffer;

	struct CameraEyePosBufferData {
		DirectX::XMMATRIX m_viewMat;
		DirectX::XMMATRIX m_projMat;
		KazMath::Vec3<float> m_eyePos;
		float m_noiseTimer;
	}m_cameraEyePosData;

	//�����Y�t���A�p�u���[
	std::shared_ptr<PostEffect::GaussianBlur> m_lensFlareBlur;
	std::shared_ptr<PostEffect::Outline> m_outline;

	KazBufferHelper::BufferData m_outlineBuffer;


	KazBufferHelper::BufferData m_chromaticAberrationData;
	KazMath::Vec4<float> m_iroShuusa;

private:
	//G-Buffer�p�̃����_�[�^�[�Q�b�g
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;

	KazBufferHelper::BufferData m_cameraPosBuffer;

	//�ŏI��������
	KazBufferHelper::BufferData m_finalGBuffer;
	KazBufferHelper::BufferData m_raytracingGBuffer;			//���C�g���̏o�͌���
	KazBufferHelper::BufferData m_backBufferCopyBuffer;			//���C�g�����ʂ�Copy����ۂɏ㏑�����Ă��܂�Ȃ��悤�ɂ��邽�߂̃o�b�t�@
	KazBufferHelper::BufferData m_backBufferCompositeBuffer;				//���C�g�����ʂ�Copy����ۂɏ㏑�����Ă��܂�Ȃ��悤�ɂ��邽�߂̃o�b�t�@
	std::shared_ptr<ComputeShader> m_backBufferRaytracingCompositeShader;	//�o�b�N�o�b�t�@�ƃ��C�g���o�͌��ʂ���������o�b�t�@

	//�����Y�t���A�֘A
	KazBufferHelper::BufferData m_lensFlareLuminanceGBuffer;	//�����Y�t���A�Ɏg�p����GBuffer ���C�g�������s����Ə������܂��B
	KazBufferHelper::BufferData m_lensFlareConposeBuffTexture;	//�����Y�t���A����������Ƃ��Ɉ�U�ۑ�����e�N�X�`���B
	KazBufferHelper::BufferData m_emissiveGBuffer;				//�����Y�t���A�Ŏg�p����u���[���pGBuffer ���C�g�������s����Ə������܂��B
	std::shared_ptr<ComputeShader> m_lensFlareComposeShader;
};

