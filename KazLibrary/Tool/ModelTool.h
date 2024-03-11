#pragma once
#include"../KazLibrary/Render/DrawFunc.h"
#include<filesystem>
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Helper/Select.h"
#include"../KazLibrary/Animation/AnimationInRaytracing.h"

class ModelTool
{
public:
	ModelTool(std::string arg_fileDir);

	void Load();
	void LoadFileName(std::string arg_folderPath, std::vector<std::string>& arg_file_names, std::vector<std::string>& fileDir);
	void Update();
	void Draw(DrawingByRasterize& render);

private:
	struct ModelData
	{
		ModelData(std::string arg_fileName, std::shared_ptr<ModelInfomation>arg_modelInfomation) :m_fileName(arg_fileName), m_modelInfomation(arg_modelInfomation)
		{
			if (arg_modelInfomation->skelton->animations.size() == 0)
			{
				DrawFuncData::PipelineGenerateData pipelineData;
				pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
				pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvLightMain", "vs_6_4", SHADER_TYPE_VERTEX);
				pipelineData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvLightMain", "ps_6_4", SHADER_TYPE_PIXEL);
				pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
				m_drawCall = DrawFuncData::SetDrawGLTFIndexMaterialLightData(*arg_modelInfomation, pipelineData);
			}
			else
			{
				m_drawCall = DrawFuncData::SetDefferdRenderingModelAnimationInRaytracing(arg_modelInfomation);
			}
		};
		std::string m_fileName;
		KazMath::Transform3D m_transform;
		std::shared_ptr<ModelInfomation>m_modelInfomation;
		DrawFuncData::DrawCallData m_drawCall;
	};
	//�t�@�C�����������ƂɎ擾�������f�����
	std::vector<ModelData>m_modelInfomationArray;
	//�m�F�p�̃��f���������Ă���t�@�C���̃p�X
	std::string m_fileDir;
	//�I�����Ă��郂�f���̃C���f�b�N�X
	int m_selectNum;

	//���C�g
	KazMath::Vec3<float>m_directionalLight;


	//�w�i---------------------------------------
	std::array<DrawFuncData::DrawCallData, 10> m_gridCallDataX;
	std::array<DrawFuncData::DrawCallData, 10> m_gridCallDataZ;
	void DrawGrid(DrawingByRasterize& render);

	std::vector<std::shared_ptr<ModelAnimator>>m_modelAnimation;
	std::vector<std::shared_ptr<AnimationInRaytracing>>m_modelAnimationInRaytracing;

	KazBufferHelper::BufferData m_meshParticle;
	KazBufferHelper::BufferData m_particle;
	KazBufferHelper::BufferData m_counterBuffer;
	DrawFuncData::DrawCallData m_particleRender;
};

