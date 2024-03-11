#pragma once
#include"Render/DrawFunc.h"
#include"Render/DrawingByRasterize.h"
#include"Loader/ModelLoader.h"
#include"Raytracing/BlasVector.h"

namespace DrawFuncHelper
{
	//�X�v���C�g�`��̃��b�p�[
	struct TextureRender
	{
		DrawFuncData::DrawCallData m_drawCommand;
		const DrawFuncData::DrawData* m_drawCommandData;
		std::shared_ptr<KazBufferHelper::BufferData> m_textureBuffer;
		KazMath::Vec2<float> m_textureSize;

		/// <summary>
		/// �e�N�X�`���ǂݍ���
		/// </summary>
		/// <param name="arg_textureFilePass">�e�N�X�`���̃t�@�C���p�X</param>
		/// <param name="arg_alphaFlag">���ߗL�薳��</param>
		TextureRender(DrawingByRasterize& arg_rasterize, const std::string& arg_textureFilePass, bool arg_isUIFlag, bool arg_isStaticFlag = false);
		/// <summary>
		/// �e�N�X�`���ǂݍ��݂ƕ`����̐ݒ�
		/// </summary>
		/// <param name="arg_textureFilePass">�e�N�X�`���̃t�@�C���p�X</param>
		/// <param name="arg_drawCall">�`����̐���</param>
		TextureRender(DrawingByRasterize& arg_rasterize, const std::string& arg_textureFilePass, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_isUIFlag, bool arg_isStaticFlag = false);
		TextureRender(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_isUIFlag);
		TextureRender(DrawingByRasterize& arg_rasterize, bool arg_isUIFlag);
		TextureRender();

		void Load(DrawingByRasterize& arg_rasterize, const std::string& arg_textureFilePass, bool arg_isUIFlag);
		void Load(DrawingByRasterize& arg_rasterize, const KazBufferHelper::BufferData& arg_textureBuffer, bool arg_isUIFlag);
		void Load(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_isUIFlag);

		void operator=(const KazBufferHelper::BufferData& rhs);

		/// <summary>
		/// �X�v���C�g2D�`��
		/// </summary>
		void Draw2D(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));
		void Draw2D(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D, const KazBufferHelper::BufferData& arg_textureBuffer, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));
		/// <summary>
		/// �X�v���C�g3D�`��
		/// </summary>
		void Draw3D(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));
		void Draw3D(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazBufferHelper::BufferData& arg_textureBuffer, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));
		void Draw3DBillBoard(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));
		void Draw3DBillBoard(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Transform3D& arg_trasform3D, const KazBufferHelper::BufferData& arg_textureBuffer, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));

		void DrawGaussian(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D);

		void Error();
		void StackOnBlas(Raytracing::BlasVector& arg_blasVec, const DirectX::XMMATRIX& arg_worldMat);

		KazMath::Vec2<float>GetTexSize()
		{
			m_textureSize =
			{
				static_cast<float>(m_textureBuffer->bufferWrapper->GetBuffer().Get()->GetDesc().Width),
				static_cast<float>(m_textureBuffer->bufferWrapper->GetBuffer().Get()->GetDesc().Height)
			};
			return m_textureSize;
		}
	};


	struct ModelRender
	{
		DrawFuncData::DrawCallData m_drawCommand;
		const DrawFuncData::DrawData* m_drawCommandData;
		std::shared_ptr<ModelInfomation> m_modelInfo;
		std::shared_ptr<ModelAnimator> m_animator;

		ModelRender(const std::string& arg_fileDir, const std::string& arg_filePass, bool arg_deletePipelineInScene);
		ModelRender(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall, bool arg_deletePipelineInScene);
		ModelRender();

		void Load(const std::string& arg_fileDir, const std::string& arg_filePass);
		void Load(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall);

		bool LoadAnimation();

		void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255), float arg_timeScale = 1.0f);
		void Error();

	};

	struct LineRender
	{
		DrawFuncData::DrawCallData m_drawCommand;
		const DrawFuncData::DrawData* m_drawCommandData;


		LineRender(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall);
		LineRender();
		void Generate(DrawingByRasterize& arg_rasterize);
		void Generate(DrawingByRasterize& arg_rasterize, const DrawFuncData::DrawCallData& arg_drawCall);
		void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec, const KazMath::Vec3<float>& arg_startPos, const KazMath::Vec3<float>& arg_endPos, const KazMath::Color& arg_color = KazMath::Color(255, 255, 255, 255));

	private:
		std::array<KazMath::Vec3<float>, 2>posArray;
		RESOURCE_HANDLE m_vertexHandle;
	};

}