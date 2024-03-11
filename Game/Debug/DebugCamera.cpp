#include "DebugCamera.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"

DebugCamera::DebugCamera() :m_distance(20.0f), m_matRot(DirectX::XMMatrixIdentity()), m_target(0.0f, 1.0f, 0.0f)
{
	DirectX::XMVECTOR r1 = { -0.91f , -0.04f,-0.01f,0.0f };
	DirectX::XMVECTOR r2 = { -0.03f , 0.95f, -0.26f,0.0f };
	DirectX::XMVECTOR r3 = {  0.03f ,-0.26f, -0.95f,0.0f };
	DirectX::XMVECTOR r4 = { 0.0f ,0.0f,0.0f,1.0f };
	m_matRot.r[0] = r1;
	m_matRot.r[1] = r2;
	m_matRot.r[2] = r3;
	m_matRot.r[3] = r4;

	m_distance = 117.0f;

	// �����_���王�_�ւ̃x�N�g���ƁA������x�N�g��
	DirectX::XMVECTOR vTargetEye = { 0.0f, 0.0f, -m_distance, 1.0f };
	DirectX::XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	// �x�N�g������]
	vTargetEye = DirectX::XMVector3Transform(vTargetEye, m_matRot);
	vUp = DirectX::XMVector3Transform(vUp, m_matRot);

	// �����_���炸�炵���ʒu�Ɏ��_���W������
	const DirectX::XMFLOAT3& target = m_target.ConvertXMFLOAT3();
	m_eye = { target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1], target.z + vTargetEye.m128_f32[2] };
	m_up = { vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] };
	CameraMgr::Instance()->Camera(m_eye, m_target, m_up);
}

void DebugCamera::Update()
{
	Camera(
		KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_MIDDLE),
		KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_LEFT),
		KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_RIGHT),
		KeyBoradInputManager::Instance()->GetMouseVel()
	);
}

void DebugCamera::Camera(bool arg_zoomFlag, bool arg_targetPosFlag, bool arg_rotationFlag, const KazMath::Vec3<float>& arg_mouseVel)
{
	bool dirty = false;
	float angleX = 0;
	float angleY = 0;

	const KazMath::Vec2<float> SCALE = { 0.001f,0.001f };


	// �}�E�X�̍��{�^����������Ă�����J��������]������
	if (arg_rotationFlag)
	{
		float dy = arg_mouseVel.x * SCALE.x;
		float dx = arg_mouseVel.y * SCALE.y;
		angleX = -dx * KazMath::PI_2F;
		angleY = -dy * KazMath::PI_2F;
		dirty = true;
	}

	// �}�E�X�̒��{�^����������Ă�����J��������s�ړ�������
	if (arg_zoomFlag)
	{
		float dx = arg_mouseVel.x / 100.0f;
		float dy = arg_mouseVel.y / 100.0f;

		DirectX::XMVECTOR move = { -dx, +dy, 0, 0 };
		move = DirectX::XMVector3Transform(move, m_matRot);

		MoveVector(move);
		dirty = true;
	}

	// �z�C�[�����͂ŋ�����ύX
	if (arg_mouseVel.z != 0)
	{
		m_distance -= arg_mouseVel.z / 100.0f;
		m_distance = max(m_distance, 1.0f);
		dirty = true;
	}

	if (dirty)
	{
		// �ǉ���]���̉�]�s��𐶐�
		DirectX::XMMATRIX matRotNew = DirectX::XMMatrixIdentity();
		matRotNew *= DirectX::XMMatrixRotationX(-angleX);
		matRotNew *= DirectX::XMMatrixRotationY(-angleY);
		// �ݐς̉�]�s�������
		// ����]�s���ݐς��Ă����ƁA�덷�ŃX�P�[�����O��������댯�������
		// �N�H�[�^�j�I�����g�p��������]�܂���
		m_matRot = matRotNew * m_matRot;

		// �����_���王�_�ւ̃x�N�g���ƁA������x�N�g��
		DirectX::XMVECTOR vTargetEye = { 0.0f, 0.0f, -m_distance, 1.0f };
		DirectX::XMVECTOR vUp = { 0.0f, 1.0f, 0.0f, 0.0f };

		// �x�N�g������]
		vTargetEye = DirectX::XMVector3Transform(vTargetEye, m_matRot);
		vUp = DirectX::XMVector3Transform(vUp, m_matRot);

		// �����_���炸�炵���ʒu�Ɏ��_���W������
		const DirectX::XMFLOAT3& target = m_target.ConvertXMFLOAT3();
		m_eye = { target.x + vTargetEye.m128_f32[0], target.y + vTargetEye.m128_f32[1], target.z + vTargetEye.m128_f32[2] };
		m_up = { vUp.m128_f32[0], vUp.m128_f32[1], vUp.m128_f32[2] };
	}

	CameraMgr::Instance()->Camera(m_eye, m_target, m_up);
};