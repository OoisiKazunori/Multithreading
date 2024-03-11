#include"../Scene/SceneChange.h"
#include"../Scene/SceneChange.h"
#include"../Helper/ResourceFilePass.h"
#include"../Loader/TextureResourceMgr.h"
#include "../Easing/easing.h"
#include "../Imgui/MyImgui.h"

using namespace ChangeScene;

SceneChange::SceneChange(DrawingByRasterize& arg_rasterize) :allHidenFlag(false), startFlag(false)
{
	m_render = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaDepthAlwaysShader());
	texBuffer = TextureResourceMgr::Instance()->StaticLoadGraphBuffer("Resource/Test/Black.png");
	//DrawFunc::DrawTextureIn2D(m_render, m_transform, *texBuffer, KazMath::Color(255, 255, 255, 255));
	m_renderCallData = arg_rasterize.GenerateSceneChangePipeline(&m_render);
	m_transform.pos.x = WIN_X / 2.0f;
	m_transform.pos.y = WIN_Y / 2.0f;
	m_transform.scale = { 1280.0f, 720.0f };
}

void SceneChange::Init()
{
	startFlag = false;
}

void SceneChange::Finalize()
{
	startFlag = false;
}

void SceneChange::Update()
{
	if (startFlag)
	{
		//ì¸ÇË
		if (startOutInT[0] < 1.0f)
		{
			Rate(&startOutInT[0], 0.03f, 1.0f);
			m_alpha = static_cast<int>(EasingMaker(Out, Cubic, startOutInT[0]) * 255.0f);
		}
		//èIÇÌÇË
		else
		{
			//âBÇÍÇΩÉtÉâÉO
			if (!initFlag)
			{
				allHidenFlag = true;
				initFlag = true;
			}

			Rate(&startOutInT[1], 0.03f, 1.0f);
			m_alpha = static_cast<int>((1.0f - EasingMaker(In, Cubic, startOutInT[1])) * 255.0f);
		}

		if (1.0 <= startOutInT[1])
		{
			startFlag = false;
		}
	}
	else
	{
		initFlag = false;
		startOutInT[0] = 0;
		startOutInT[1] = 0;
	}
}

void SceneChange::Draw(DrawingByRasterize& arg_rasterize)
{
	DrawFunc::DrawTextureIn2D(m_render, m_transform, *texBuffer, KazMath::Color(255, 255, 255, m_alpha));
	arg_rasterize.StaticUIRender(m_renderCallData);
}

void SceneChange::Start()
{
	startFlag = true;
}

bool SceneChange::AllHiden()
{
	if (allHidenFlag)
	{
		allHidenFlag = false;
		return true;
	}
	return false;
}
