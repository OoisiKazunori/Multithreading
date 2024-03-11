#include "ProgressBar.h"

ProgressBar::ProgressBar()
{
	Init();
}

void ProgressBar::Init()
{
	m_loadMaxBar = 0;
	m_loadNowBar = 0;
}

void ProgressBar::SetMaxBar(size_t arg_num)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_loadMaxBar += arg_num;
}

void ProgressBar::IncreNowNum(int arg_num)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_loadNowBar += static_cast<size_t>(arg_num);
}

float ProgressBar::GetNowRate()
{
	return static_cast<float>(m_loadNowBar) / static_cast<float>(m_loadMaxBar);
}

bool ProgressBar::IsDone()
{
	const bool isDoneFlag = 1.0f <= GetNowRate();
	const bool isRate0Flag = m_loadMaxBar <= 0;
	return isDoneFlag || isRate0Flag;
}
