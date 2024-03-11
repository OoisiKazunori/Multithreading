#pragma once
#include"SceneData.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"Raytracing/BlasVector.h"

class SceneBase
{
public:
	SceneBase() :endGameFlag(false)
	{};
	virtual ~SceneBase() = default;
	//�C���Q�[���Ɋւ�����e�̏�����(�`������g���ď����������胊�X�^�[�g�p�̏���)
	virtual void Init() = 0;
	virtual void PreInit() {};
	virtual void Finalize() = 0;
	virtual void Input() = 0;
	virtual void Update(DrawingByRasterize &arg_rasterize) = 0;
	virtual void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec) = 0;

	virtual int SceneChange() = 0;
	virtual bool OrderGeneratePipeline()
	{
		return false;
	}

	bool endGameFlag;
	bool skipTurtorialFlag;
	bool firstGenerateFlag;
protected:

};