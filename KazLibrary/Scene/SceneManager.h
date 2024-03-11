#pragma once
#include"../DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"../Scene/SceneChange.h"
#include"../RenderTarget/RenderTargetStatus.h"
#include<memory>
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include "../KazLibrary/Sound/SoundManager.h"
#include "../KazLibrary/Raytracing/RayPipeline.h"
#include"../KazLibrary/Scene/LoadScene.h"
#include"../Game/Scene/RenderScene.h"
#include"../Game/Scene/InGame.h"

class SceneManager
{
public:
	SceneManager(DirectX12*arg_directX12);
	~SceneManager();
	void Update();
	void Draw();

	bool endGameFlag;
private:
	std::shared_ptr<SceneBase>m_nowScene;
	std::unique_ptr<ChangeScene::SceneChange> m_sceneChange;
	int m_nowSceneNumber, m_nextSceneNumber;
	bool initGameFlag;

	bool sceneChangeFlag;

	bool itisInArrayFlag;
	bool gameFirstInitFlag;



	//�{�����[���t�H�O�p3D�e�N�X�`��
	KazBufferHelper::BufferData m_volumeFogTextureBuffer;
	ComputeShader m_volumeNoiseShader;
	struct NoiseParam
	{
		KazMath::Vec3<float> m_worldPos;
		float m_timer;
		float m_windSpeed;
		float m_windStrength;
		float m_threshold;
		float m_skydormScale;
		int m_octaves;
		float m_persistence;
		float m_lacunarity;
		float m_pad;
	}m_noiseParam;
	KazBufferHelper::BufferData m_noiseParamData;

	//�{�����[���t�H�O�p�萔�o�b�t�@
	struct RaymarchingParam
	{
		KazMath::Vec3<float> m_pos; //�{�����[���e�N�X�`���̃T�C�Y
		float m_gridSize; //�T���v�����O����O���b�h�̃T�C�Y
		KazMath::Vec3<float> m_color; //�t�H�O�̐F
		float m_wrapCount; //�T���v�����O���W���͂ݏo�����ۂɉ���܂�Wrap���邩
		float m_sampleLength; //�T���v�����O����
		float m_density; //�Z�x�W��
		int m_isSimpleFog;
		int m_isActive;
	}m_raymarchingParam;
	KazBufferHelper::BufferData m_raymarchingParamData;

	//OnOff�f�o�b�O�p
	struct DebugRaytracingParam
	{
		int m_debugReflection;
		int m_debugShadow;
		float m_sliderRate;
		float m_skyFacter;
	}m_debugRaytracingParam;
	KazBufferHelper::BufferData m_debugRaytracingParamData;

	//�f�o�b�O�p��OnOff�؂�ւ����C��
	bool m_isOldDebugRaytracing;
	float m_debugLineScale;

	//�f�o�b�O�֘A�̕ϐ�
	bool m_isDebugCamera;
	bool m_isDebugRaytracing;
	bool m_isDebugTimeZone;
	bool m_isDebugVolumeFog;
	bool m_isDebugSea;


	//���X�^���C�U�`��
	DrawingByRasterize m_rasterize;

	//���C�g���Ƀ��f����g�ݍ��ޗp�̔z��N���X
	Raytracing::BlasVector m_blasVector;
	//���C�g���ŕ`�悷�邽�߂̏��
	Raytracing::Tlas m_tlas;
	//���C�g���p�p�C�v���C��
	std::vector<Raytracing::RayPipelineShaderData> m_pipelineShaders;
	std::unique_ptr<Raytracing::RayPipeline> m_rayPipeline;

	std::shared_ptr<SceneBase>GetScene(int arg_sceneNum)
	{
		switch (arg_sceneNum)
		{
		case 0:
			return std::make_shared<InGame>(m_rasterize);
			break;
		case 1:
			return std::make_shared<RenderScene>(m_rasterize);
			break;
		case 2:
			break;
		default:
			break;
		}
		return std::make_shared<RenderScene>(m_rasterize);
	}
	void Release();
	void AssetLoad();
	void GeneratePipeline();

	DirectX12 *m_directX12;

	//���[�h��ʂ̕`��
	LoadScene m_loadScene;
	void LoadScene();

	BasicDraw::BasicModelRender m_sponzaModelRender;//�X�|���U�`��
	bool m_skipDrawForLoadFlag;	//���[�h��̍ŏ���1F��Draw�R�}���h���X�L�b�v������


	KazBufferHelper::BufferData m_bufferDataDest;
	KazBufferHelper::BufferData m_bufferDataSource;

	const KazBufferHelper::BufferData& Get()
	{
		return m_bufferDataDest;
	}
};