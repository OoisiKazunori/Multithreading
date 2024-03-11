#pragma once
#include"../DirectXCommon/Base.h"
#include"../Math/KazMath.h"
#include"../Helper/DirtyFlag.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Render/BasicDraw.h"

namespace ChangeScene {

	class SceneChange
	{
	public:
		SceneChange(DrawingByRasterize& arg_rasterize);

		void Init();
		void Finalize();
		void Update();
		void Draw(DrawingByRasterize& arg_rasterize);

		/// <summary>
		/// �V�[���J�ڂ��J�n���܂�
		/// </summary>
		void Start();

		/// <summary>
		/// ��ʂ��B�ꂽ�u�Ԃ̔�������܂�
		/// �K���g���K�[����Ŏ�鎖
		/// </summary>
		/// <returns>true...�B�ꂽ,false...�B��ĂȂ�</returns>
		bool AllHiden();

	private:
		bool startFlag, finishFlag;
		bool allHidenFlag;
		bool initFlag;

		float tmp;
		float startOutInT[2];
		int m_alpha;

		KazMath::Transform2D m_transform;
		DrawFuncData::DrawCallData m_render;
		const DrawFuncData::DrawData *m_renderCallData;
		std::shared_ptr<KazBufferHelper::BufferData> texBuffer;
	};
}