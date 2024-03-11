#include "SplineRail.h"

SplineRail::SplineRail() :m_vertexBufferHandle(-1)
{
	Init();

	m_jsonData.LoadFile("Resource/Stage/json/Stage.json");
	//JSON�t�@�C���ŗp�ӂ���Ă��鐧��_�̐��̍ő吔
	m_posArray.resize(m_jsonData.doc["LIMIT_POS_MAX"].GetInt());
	Import();
}

void SplineRail::Init()
{
	m_timer = 0;
	m_nowIndex = 1;
}

void SplineRail::Update()
{
	//����_��0�Ȃ�Q�[���i�s�s�\�̂��߃A�T�[�g���͂�����
	if (m_splineRailPosArray.size() == 0)
	{
		assert(0);
	}

	//++m_timer;
	if (m_maxTimer <= m_timer)
	{
		m_timer = 0;
		++m_nowIndex;

		if (IsEnd())
		{
			m_nowIndex = static_cast<int>(m_splineRailPosArray.size() - 2);
		}
	}
	m_nowPosition = KazMath::SplinePosition(m_splineRailPosArray, m_nowIndex, static_cast<float>(m_timer) / static_cast<float>(m_maxTimer), false);
}

void SplineRail::DebugDraw(DrawingByRasterize& arg_rasterize)
{
	bool importFlag = false;
	bool exportFlag = false;

	ImGui::Begin("SplineLine");
	//���W�̕\��
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < m_limitPosMaxNum; ++i)
	{
		std::string label = "limitPosIndex : " + std::to_string(i);
		KazImGuiHelper::InputVec3(label, &m_posArray[i]);
		ImGui::NewLine();
	}
	ImGui::EndChild();
	//���[�v���邩�ǂ���
	ImGui::Checkbox("isLoop", &m_isLoopFlag);
	//���[���̍ő�t���[��
	ImGui::DragInt("Timer", &m_maxTimer);
	//�g�p���鐧��_�̐�
	ImGui::SliderInt("limitPosMaxNum", &m_limitPosMaxNum, 2, static_cast<int>(m_posArray.size()));
	importFlag = ImGui::Button("Import");
	ImGui::SameLine();
	exportFlag = ImGui::Button("Export");
	ImGui::End();

	if (importFlag)
	{
		Import();
	}
	if (exportFlag)
	{
		Export();
	}
	DrawFunc::DrawLine(m_splineDrawCall, m_splineRailPosArray, m_vertexBufferHandle);
	//arg_rasterize.ObjectRender(m_splineDrawCall);
}

bool SplineRail::IsEnd()
{
	return (m_splineRailPosArray.size() - 2) <= m_nowIndex;
}

KazMath::Vec3<float> SplineRail::GetPosition()
{
	return m_nowPosition + KazMath::Vec3<float>(0,20,0);
}

void SplineRail::Import()
{
	//����_�̐�
	m_limitPosMaxNum = m_jsonData.doc["limitPosNum"].GetInt();
	//���[�v���邩�ǂ���
	m_isLoopFlag = m_jsonData.doc["isLoop"].GetBool();
	//�ő厞��
	m_maxTimer = m_jsonData.doc["flame"].GetInt();
	//�ǂݍ���
	for (int limitPosIndex = 0; limitPosIndex < m_posArray.size(); ++limitPosIndex)
	{
		//���W�̓ǂݍ���
		KazMath::Vec3<float>pos =
		{
			m_jsonData.doc["limitPosTransform"][limitPosIndex]["translation"][0].GetFloat(),
			m_jsonData.doc["limitPosTransform"][limitPosIndex]["translation"][1].GetFloat(),
			m_jsonData.doc["limitPosTransform"][limitPosIndex]["translation"][2].GetFloat()
		};
		m_posArray[limitPosIndex] = pos;
	}

	Reflect();
}

void SplineRail::Export()
{
	//��������
	for (int limitPosIndex = 0; limitPosIndex < m_posArray.size(); ++limitPosIndex)
	{
		m_jsonData.doc["limitPosTransform"][limitPosIndex]["translation"][0].SetFloat(m_posArray[limitPosIndex].x);
		m_jsonData.doc["limitPosTransform"][limitPosIndex]["translation"][1].SetFloat(m_posArray[limitPosIndex].y);
		m_jsonData.doc["limitPosTransform"][limitPosIndex]["translation"][2].SetFloat(m_posArray[limitPosIndex].z);
	}
	//����_�̐�
	m_jsonData.doc["limitPosNum"].SetInt(m_limitPosMaxNum);
	//���[�v���邩�ǂ���
	m_jsonData.doc["isLoop"].SetBool(m_isLoopFlag);
	//�ő厞��
	m_jsonData.doc["flame"].SetInt(m_maxTimer);

	Reflect();


	m_jsonData.ExportFile("Resource/Stage/json/Stage.json");
}

void SplineRail::Reflect()
{
	//�Q�[�����ɔ��f
	m_splineRailPosArray.clear();
	m_splineRailPosArray.emplace_back(m_posArray[0]);
	for (int i = 0; i < m_limitPosMaxNum; ++i)
	{
		m_splineRailPosArray.emplace_back(m_posArray[i]);
	}
	m_splineRailPosArray.emplace_back(m_posArray[m_limitPosMaxNum - 1]);

	if (m_vertexBufferHandle != -1)
	{
		VertexBufferMgr::Instance()->ReleaseVeretexBuffer(m_vertexBufferHandle);
	}
	VertexGenerateData generateData(m_splineRailPosArray.data(), sizeof(DirectX::XMFLOAT3), m_splineRailPosArray.size(), sizeof(m_splineRailPosArray[0]));
	m_vertexBufferHandle = VertexBufferMgr::Instance()->GenerateBuffer(generateData, false);
	m_splineDrawCall = DrawFuncData::SetLine(m_vertexBufferHandle);
}
