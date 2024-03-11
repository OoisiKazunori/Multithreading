#pragma once
#include"../Buffer/DrawFuncData.h"
#include"../KazLibrary/Camera/CameraMgr.h"

namespace DrawFunc
{
	static CoordinateSpaceMatData GetMatrixBufferData(KazMath::Transform3D& arg_transform)
	{
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		DirectX::XMVECTOR pos, scale, rotaQ;
		DirectX::XMMatrixDecompose(&pos, &scale, &rotaQ, arg_transform.GetMat());
		DirectX::XMMATRIX rotaMat = DirectX::XMMatrixRotationQuaternion(rotaQ);
		transData.m_rotaion = rotaMat;
		return transData;
	}

	enum RayTracingType
	{
		NONE,		//�������Ȃ�
		REFRACTION,	//���C�g�����ŃI�u�W�F�N�g�����܂�����
		REFLECTION	//���C�g�����ŃI�u�W�F�N�g�𔽎˂�����
	};

	static void DrawTextureIn2D(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform2D& arg_transform, const KazBufferHelper::BufferData& arg_texture)
	{
		//�s����
		DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//�e�N�X�`�����
		arg_callData.extraBufferArray[1] = arg_texture;
		arg_callData.extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}

	static void DrawTextureIn2D(DrawFuncData::DrawCallData& arg_callData, const KazMath::Transform2D& arg_transform, const KazBufferHelper::BufferData& arg_texture, const KazMath::Color& arg_color)
	{
		//�s����
		DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetOrthographicMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//�F
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
		//�e�N�X�`�����
		arg_callData.extraBufferArray[2] = arg_texture;
		arg_callData.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}


	static void DrawModelInRaytracing(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, RayTracingType arg_type)
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//�N�H�[�^�j�I���ɒl�������Ă��� or �N�H�[�^�j�I�����P�ʍs�񂶂�Ȃ�������N�H�[�^�j�I���ŉ�]�s������߂�B
		transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);


		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		//ID
		UINT num = static_cast<UINT>(arg_type);
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(UINT));
	}

	static void Test(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, RayTracingType arg_type)
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//�N�H�[�^�j�I���ɒl�������Ă��� or �N�H�[�^�j�I�����P�ʍs�񂶂�Ȃ�������N�H�[�^�j�I���ŉ�]�s������߂�B
		transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);

	}

	static void DrawModelInRaytracing(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, RayTracingType arg_type, const KazMath::Color& arg_emissiveColor, const KazMath::Color& arg_albedoColor = KazMath::Color(255, 255, 255, 255))
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//�N�H�[�^�j�I���ɒl�������Ă��� or �N�H�[�^�j�I�����P�ʍs�񂶂�Ȃ�������N�H�[�^�j�I���ŉ�]�s������߂�B
		transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		//ID
		UINT num = static_cast<UINT>(arg_type);
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&num, sizeof(UINT));

		//AlbedoColor
		arg_callData.extraBufferArray[2].bufferWrapper->TransData(&arg_albedoColor.GetColorRate(), sizeof(DirectX::XMFLOAT4));

		//EmissiveColor
		arg_callData.extraBufferArray[3].bufferWrapper->TransData(&arg_emissiveColor.GetColorRate(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModel(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		//�s����
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//�N�H�[�^�j�I���ɒl�������Ă��� or �N�H�[�^�j�I�����P�ʍs�񂶂�Ȃ�������N�H�[�^�j�I���ŉ�]�s������߂�B
		transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));

		arg_callData.extraBufferArray[2].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModelDisolve(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		//�s����
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//�N�H�[�^�j�I���ɒl�������Ă��� or �N�H�[�^�j�I�����P�ʍs�񂶂�Ȃ�������N�H�[�^�j�I���ŉ�]�s������߂�B
		transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));

		arg_callData.extraBufferArray[2].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModel(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazBufferHelper::BufferData& arg_boneBuffer, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		//�s����
		CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		//�N�H�[�^�j�I���ɒl�������Ă��� or �N�H�[�^�j�I�����P�ʍs�񂶂�Ȃ�������N�H�[�^�j�I���ŉ�]�s������߂�B
		transData.m_rotaion = DirectX::XMMatrixRotationQuaternion(arg_transform.quaternion);
		

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));

		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		arg_callData.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		arg_callData.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		arg_callData.extraBufferArray[3] = arg_boneBuffer;
		arg_callData.extraBufferArray[3].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		arg_callData.extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA4;
	}

	static void DrawLine(DrawFuncData::DrawCallData& arg_callData, std::vector<KazMath::Vec3<float>>arg_limitPosArray, RESOURCE_HANDLE arg_vertexHandle, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255))
	{
		VertexBufferMgr::Instance()->GetVertexBuffer(arg_vertexHandle).vertBuffer->bufferWrapper->TransData(
			arg_limitPosArray.data(),
			KazBufferHelper::GetBufferSize<UINT>(arg_limitPosArray.size(), sizeof(DirectX::XMFLOAT3))
		);

		//�s����
		DirectX::XMMATRIX mat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawModelLight(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, KazMath::Vec3<float>& lightDir, const KazMath::Color& arg_color)
	{
		//�s����
		static CoordinateSpaceMatData transData(arg_transform.GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		transData.m_world = arg_transform.GetMat();
		transData.m_projective = CameraMgr::Instance()->GetPerspectiveMatProjection();
		transData.m_view = CameraMgr::Instance()->GetViewMatrix();

		DirectX::XMVECTOR pos, scale, rotaQ;
		DirectX::XMMatrixDecompose(&pos, &scale, &rotaQ, arg_transform.GetMat());
		DirectX::XMMATRIX rotaMat = DirectX::XMMatrixRotationQuaternion(rotaQ);
		transData.m_rotaion = rotaMat;

		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&lightDir, sizeof(DirectX::XMFLOAT3));
		arg_callData.extraBufferArray[2].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
	}

	static void DrawTextureIn3DBillBoard(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazBufferHelper::BufferData& arg_texture, const KazMath::Color& arg_color)
	{
		//�s����
		DirectX::XMMATRIX mat = arg_transform.GetMat(CameraMgr::Instance()->GetMatBillBoard()) * CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//�F
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
		//�e�N�X�`�����
		arg_callData.extraBufferArray[2] = arg_texture;
		arg_callData.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}

	static void DrawTextureIn3D(DrawFuncData::DrawCallData& arg_callData, KazMath::Transform3D& arg_transform, const KazBufferHelper::BufferData& arg_texture, const KazMath::Color& arg_color)
	{
		//�s����
		DirectX::XMMATRIX mat = arg_transform.GetMat() * CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		arg_callData.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
		//�F
		arg_callData.extraBufferArray[1].bufferWrapper->TransData(&arg_color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
		//�e�N�X�`�����
		arg_callData.extraBufferArray[2] = arg_texture;
		arg_callData.extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		arg_callData.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}
}