#pragma once
#include"Scene/SceneBase.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Render/DrawFuncHelper.h"
#include"../Game/Debug/DebugCamera.h"
#include"../KazLibrary/Render/DrawFuncHelper.h"
#include"../KazLibrary/Render/BasicDraw.h"

class RenderScene :public SceneBase
{
public:
	RenderScene(DrawingByRasterize& arg_rasterize);
	~RenderScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update(DrawingByRasterize& arg_rasterize);
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

private:
	//�J����--------------------------
	DebugCamera m_camera;

	//���f���̕`��--------------------------
	BasicDraw::BasicModelRender m_sponzaModelRender;//�X�|���U�`��
	KazMath::Transform3D m_sponzaModelTransform;

	/// <summary>
	/// ���f����XY��ɕ��ׂ��z�u����N���X
	/// </summary>
	class ParallelModels
	{
		static const int X_ARRAY = 10;
		static const int Y_ARRAY = 1;
	public:
		void Load(DrawingByRasterize& arg_rasterize, std::string arg_filePass, std::string arg_fileName, const KazMath::Transform3D& arg_baseTransform);
		void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

		std::array<KazMath::Vec3<float>, X_ARRAY* Y_ARRAY> GetPosArray();
	private:
		//���f���̔z�u
		std::array<std::array<BasicDraw::BasicModelRender, Y_ARRAY>, X_ARRAY>m_modelDrawArray;
		//���f����transform
		std::array<std::array<KazMath::Transform3D, Y_ARRAY>, X_ARRAY>m_modelTransformArray;
		//���W�݂̂̒��o
		std::array<KazMath::Vec3<float>, X_ARRAY* Y_ARRAY>m_posArray;
	};
	static const int MODEL_MAX_NUM = 7;
	std::array<ParallelModels, MODEL_MAX_NUM> m_models;

	//���C�g�̈ʒu
	struct LightData
	{
		float m_lightRadius;
	};
	LightData m_lightData;
	KazBufferHelper::BufferData m_uploadLightBuffer, m_defaultLightBuffer;
	std::array<ParallelModels, 6> m_lights;
	bool m_drawLightFlag;

	//G-Buffer�̕`��--------------------------
	enum GBufferTexEnum
	{
		GBUFFER_ALBEDO,
		GBUFFER_NORMAL,
		GBUFFER_MR,
		GBUFFER_WORLD,
		GBUFFER_MAX
	};
	int m_gBufferType;
	KazMath::Transform2D m_renderTransform;
	std::array<BasicDraw::BasicTextureRender, GBUFFER_MAX>m_gBufferRender;//GBuffer�ɏ������܂ꂽ�e�N�X�`���̕`��
	DrawFuncHelper::TextureRender m_finalRender;
	int m_sceneNum;

	//�����f��
	KazMath::Transform3D m_alphaTransform;
	BasicDraw::BasicModelRender m_alphaModel;

	//UI------------------------------------------
	BasicDraw::BasicTextureRender m_announceSceneUI;
	BasicDraw::BasicTextureRender m_intractUI;
};

