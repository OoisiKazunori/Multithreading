#pragma once
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include "../KazLibrary/Raytracing/RayPipeline.h"
#include "../KazLibrary/Render/BasicDraw.h"

/// <summary>
/// �}���`�X���b�h�����̃��[�h��ʂ̕`��
/// </summary>
class LoadScene
{
public:
	LoadScene();
	void Update();
	void Draw(DirectX12* arg_directX12);

private:

	//���X�^���C�U�`��
	DrawingByRasterize m_rasterize;

	BasicDraw::BasicTextureRender m_bar;
	BasicDraw::BasicTextureRender m_barFlame;
	BasicDraw::BasicTextureRender m_loadText;

	KazBufferHelper::BufferData m_rateBuffer;

	float m_timer;
};

