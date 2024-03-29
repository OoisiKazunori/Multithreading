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
		/// シーン遷移を開始します
		/// </summary>
		void Start();

		/// <summary>
		/// 画面が隠れた瞬間の判定を取ります
		/// 必ずトリガー判定で取る事
		/// </summary>
		/// <returns>true...隠れた,false...隠れてない</returns>
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