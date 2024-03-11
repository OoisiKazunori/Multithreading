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

class InGame :public SceneBase
{
public:
	InGame(DrawingByRasterize& arg_rasterize);
	~InGame();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update(DrawingByRasterize& arg_rasterize);
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

private:
	int m_sceneNum;

	//�J����--------------------------
	DebugCamera m_camera;

	//���f���̕`��--------------------------
	BasicDraw::BasicModelRender m_sponzaModelRender;//�X�|���U�`��
	KazMath::Transform3D m_sponzaModelTransform;

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

	//UI------------------------------------------
	BasicDraw::BasicTextureRender m_announceSceneUI;
	BasicDraw::BasicTextureRender m_intractUI;
	BasicDraw::BasicTextureRender m_loadSponzaUI;
};

