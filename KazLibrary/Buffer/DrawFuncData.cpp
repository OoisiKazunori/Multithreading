#include "DrawFuncData.h"
#include "../Raytracing/Blas.h"
#include "Raytracing/BlasDataContainer.h"

void DrawFuncData::DrawCallData::SetupRaytracing(bool arg_isOpaque)
{

	/*-- ���C�g���[�V���O�̏����֐� --*/

	//Blas���\�z�B
	//m_raytracingData.m_blas = Raytracing::BlasDataContainer::Instance()->SetBlas(arg_isOpaque, m_modelVertDataHandle, materialBuffer);

	//���C�g�����Z�b�g�A�b�v�ς݂̏�Ԃɂ���B
	m_raytracingData.m_isRaytracingInitialized = true;

	//���C�g����L���ɂ���B
	m_raytracingData.m_isRaytracingEnable = true;

}

void DrawFuncData::ExcuteIndirectArgumentData::GenerateArgumentBuffer()
{
}

DrawFuncData::DrawCallData DrawFuncData::SetRaytracingData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
{
	DrawCallData lDrawCallData;

	//���_���
	lDrawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
	lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
	lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
	for (auto& obj : MODEL_DATA.modelData)
	{
		lDrawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
	}

	lDrawCallData.SetupRaytracing(true);

	return lDrawCallData;
};
