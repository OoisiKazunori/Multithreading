#pragma once
#include "../KazLibrary/Helper/ISinglton.h"
#include <memory>
#include <vector>
#include <DirectXCommon/Base.h>
#include <Helper/KazBufferHelper.h>

namespace Raytracing {

	class Blas;

	class BlasDataContainer : public ISingleton<BlasDataContainer> {

	public:

		struct BlasMesh
		{
			std::vector<std::shared_ptr<Raytracing::Blas>> m_mesh;
			int m_vertexHandle;
			bool m_isOpaque;
		};

	private:

		std::vector<BlasMesh> m_meshDatas;		//�ۑ�����Ă���BLAS�̏���B

	public:

		std::vector<std::shared_ptr<Raytracing::Blas>> SetBlas(bool arg_isOpaque, int arg_vertexHandle, std::vector<std::vector<std::shared_ptr<KazBufferHelper::BufferData>>> arg_materialBuffer);

	};

}