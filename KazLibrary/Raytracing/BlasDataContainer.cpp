#include "BlasDataContainer.h"
#include "Blas.h"

std::vector<std::shared_ptr<Raytracing::Blas>> Raytracing::BlasDataContainer::SetBlas(bool arg_isOpaque, int arg_vertexHandle, std::vector<std::vector<std::shared_ptr<KazBufferHelper::BufferData>>> arg_materialBuffer)
{

	//�f�[�^���ۑ�����Ă��邩���`�F�b�N
	for (auto& index : m_meshDatas) {

		//���łɐ����ς݂̃f�[�^���H
		bool isOpaque = arg_isOpaque == index.m_isOpaque;
		bool isVertexHandle = arg_vertexHandle == index.m_vertexHandle;
		if (isOpaque && isVertexHandle) {

			//�����ς݂������炻�̃f�[�^��Blas��Ԃ��B
			return index.m_mesh;

		}

	}

	//�ۑ�����Ă��Ȃ������琶������B
	const int BLAS_COUNT = static_cast<int>(arg_materialBuffer.size());
	BlasMesh mesh;
	mesh.m_mesh.resize(BLAS_COUNT);
	for (int counter = 0; counter < BLAS_COUNT; ++counter) {
		mesh.m_mesh[counter] = std::make_shared<Raytracing::Blas>(arg_isOpaque, arg_vertexHandle, counter, arg_materialBuffer[counter][0]->bufferWrapper->GetViewHandle(), static_cast<int>(m_meshDatas.size()));
	}
	mesh.m_isOpaque = arg_isOpaque;
	mesh.m_vertexHandle = arg_vertexHandle;
	m_meshDatas.emplace_back(mesh);

	return m_meshDatas.back().m_mesh;
}
