#pragma once
#include"../Helper/KazBufferHelper.h"
#include"../Helper/KazRenderHelper.h"

/// <summary>
/// ���_���݂̂̃|���S��
/// </summary>
struct PolygonInstanceData
{
	std::shared_ptr<KazBufferHelper::BufferData> vertBuffer;
	KazRenderHelper::DrawInstanceCommandData instanceData;
};

/// <summary>
/// ���_���ƃC���f�b�N�X�̃|���S��
/// </summary>
struct PolygonIndexData
{
	std::shared_ptr<KazBufferHelper::BufferData> vertBuffer;
	std::shared_ptr<KazBufferHelper::BufferData> indexBuffer;
	KazRenderHelper::DrawIndexInstanceCommandData index;
};

/// <summary>
/// �������b�V���Ή��̒��_���ƃC���f�b�N�X�̃|���S��
/// </summary>
struct PolygonMultiMeshedIndexData
{
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>> vertBuffer;
	std::vector<std::shared_ptr<KazBufferHelper::BufferData>> indexBuffer;
	KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData index;
};

/// <summary>
/// �I�u�W�F�N�g��`�悷��ׂɕK�v�Ȓ��_�ƃC���f�b�N�X�̏�������
/// </summary>
class PolygonBuffer
{
public:
	std::shared_ptr<KazBufferHelper::BufferData> GenerateVertexBuffer(void *verticesPos, int structureSize, size_t arraySize);
	std::shared_ptr<KazBufferHelper::BufferData> GenerateIndexBuffer(std::vector<UINT>indices);

	PolygonIndexData GenerateBoxBuffer(float scale);
	PolygonIndexData GenerateBoxNormalBuffer(float scale);
	PolygonIndexData GeneratePlaneBuffer(float scale);
	PolygonIndexData GeneratePlaneTexBuffer(const KazMath::Vec2<float> &scale, const KazMath::Vec2<int> &texSize);

	struct VertexData
	{
		DirectX::XMFLOAT3 pos;
	};
	struct VertUvData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

private:
	std::shared_ptr<KazBufferHelper::BufferData> cpuVertBuffer;
	std::shared_ptr<KazBufferHelper::BufferData> cpuIndexBuffer;

	std::shared_ptr<KazBufferHelper::BufferData> vertBuffer;
	std::shared_ptr<KazBufferHelper::BufferData> indexBuffer;



	std::vector<DirectX::XMFLOAT3>GetPlaneVertices(const KazMath::Vec2<float> &anchorPoint, const KazMath::Vec2<float> &scale, const KazMath::Vec2<int> &texSize);

	std::vector<DirectX::XMFLOAT3> GetBoxNormal(std::vector<DirectX::XMFLOAT3>VERT,std::vector<UINT>INDEX_LEN);
};