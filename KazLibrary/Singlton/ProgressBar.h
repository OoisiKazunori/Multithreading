#pragma once
#include"Helper/ISinglton.h"
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

/// <summary>
/// ロード画面のゲージの数値管理
/// </summary>
class ProgressBar :public ISingleton<ProgressBar>
{
public:
	ProgressBar();
	void Init();
	void SetMaxBar(size_t arg_num);
	void IncreNowNum(int arg_num);
	float GetNowRate();

	bool IsDone();
private:
	size_t m_loadMaxBar;
	size_t m_loadNowBar;
	//排他処理
	std::mutex m_mtx;
};

