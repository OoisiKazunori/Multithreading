#include "InGame.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

InGame::InGame(DrawingByRasterize& arg_rasterize) :
	m_sponzaModelRender(arg_rasterize, "Resource/DefferdRendering/Sponza/", "Sponza.gltf"),
	m_announceSceneUI(arg_rasterize, "Resource/SceneUI/Title/SceneUI_0.png", true),
	m_intractUI(arg_rasterize, "Resource/SceneUI/Title/SceneUI_1.png", true),
	m_loadSponzaUI(arg_rasterize, "Resource/SceneUI/Title/SceneUI_2.png", true)
{
	m_sceneNum = -1;
}

InGame::~InGame()
{
}

void InGame::Init()
{
}

void InGame::PreInit()
{
}

void InGame::Finalize()
{
}

void InGame::Input()
{
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_SPACE))
	{
		m_sceneNum = 1;
	}
}

void InGame::Update(DrawingByRasterize& arg_rasterize)
{
	m_camera.Update();
}

void InGame::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	m_announceSceneUI.m_tex.Draw2D(arg_rasterize, KazMath::Transform2D({ WIN_X / 2.0f,WIN_Y / 2.0f - 100.0f }, { 1.0f,1.0f }));
	m_intractUI.m_tex.Draw2D(arg_rasterize, KazMath::Transform2D({ WIN_X / 2.0f + 70.0f,WIN_Y / 2.0f + 50.0f }, { 1.0f,1.0f }));
	m_loadSponzaUI.m_tex.Draw2D(arg_rasterize, KazMath::Transform2D({ WIN_X / 2.0f + 0.0f,WIN_Y / 2.0f + 150.0f }, { 1.0f,1.0f }));
}

int InGame::SceneChange()
{
	if (m_sceneNum != SCENE_NONE)
	{
		int tmp = m_sceneNum;
		m_sceneNum = SCENE_NONE;
		return tmp;
	}
	return SCENE_NONE;
}
