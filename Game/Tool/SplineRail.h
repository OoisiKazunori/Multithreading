#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Imgui/imgui.h"
#include"../Game/Debug/ParameterMgr.h"

class SplineRail
{
public:
	SplineRail();

	void Init();
	void Update();
	void DebugDraw(DrawingByRasterize& arg_rasterize);

	bool IsEnd();
	/// <summary>
	/// �X�v���C���Ȑ��ɉ����������𓾂�
	/// </summary>
	KazMath::Vec3<float> GetPosition();
private:
	//�X�v���C���Ȑ��̋���---------------------------------------
	std::vector<KazMath::Vec3<float>>m_splineRailPosArray;
	int m_timer, m_maxTimer;
	int m_nowIndex;
	KazMath::Vec3<float>m_nowPosition;
	//�X�v���C���Ȑ��̋���---------------------------------------

	//�f�o�b�N�p�̏���---------------------------------------
	ParameterMgr m_jsonData;
	std::vector<DrawFuncData::DrawCallData> m_boxArray;
	std::vector<KazMath::Vec3<float>>m_posArray;
	int m_limitPosMaxNum;
	bool m_isLoopFlag;
	
	int m_vertexBufferHandle;//�f�o�b�N�p�̃X�v���C���Ȑ��`��̒��_�o�b�t�@
	DrawFuncData::DrawCallData m_splineDrawCall;
	//�f�o�b�N�p�̏���---------------------------------------

	void Import();
	void Export();
	void Reflect();
};

