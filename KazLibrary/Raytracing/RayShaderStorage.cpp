#include "RayShaderStorage.h"
#include "RayShaderData.h"
#include <assert.h>

#pragma comment(lib, "dxcompiler.lib")

namespace Raytracing {

	Microsoft::WRL::ComPtr<ID3DBlob> RayShaderStorage::LoadShader(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint)
	{

		/*-- �V�F�[�_�[�̃��[�h���� --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//�V�F�[�_�[�̐������[�v���āA���[�h�ς݂̃V�F�[�_�[�����`�F�b�N����B
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//�V�F�[�_�̖��O������Ă����玟�ցB
			if (m_shaderData[index]->GetShaderPath() != arg_shaderPath) continue;

			//����index�̃V�F�[�_�[�����^�[������B
			return m_shaderData[index]->GetShaderBlob();

		}

		//�V�F�[�_�[�����[�h���ĕۑ��B
		m_shaderData.emplace_back(std::make_unique<RayShaderData>(arg_shaderPath, arg_entryPoint, arg_shaderModel));

		//�Ō���̃f�[�^�����^�[������B
		return m_shaderData[m_shaderData.size() - 1]->GetShaderBlob();
	}

	Microsoft::WRL::ComPtr<ID3DBlob> RayShaderStorage::LoadShaderForDXC(const std::string& arg_shaderPath, const std::string& arg_shaderModel, const std::string& arg_entryPoint)
	{

		/*-- �V�F�[�_�[�̃��[�h���� --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//�V�F�[�_�[�̐������[�v���āA���[�h�ς݂̃V�F�[�_�[�����`�F�b�N����B
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//�V�F�[�_�̖��O������Ă����玟�ցB
			if (m_shaderData[index]->GetShaderPath() != arg_shaderPath) continue;

			//����index�̃V�F�[�_�[�����^�[������B
			return m_shaderData[index]->GetShaderBlob();

		}

		//�V�F�[�_�[�����[�h���ĕۑ��B
		m_shaderData.emplace_back(std::make_unique<RayShaderData>(arg_shaderPath, arg_entryPoint, arg_shaderModel, true));

		//�Ō���̃f�[�^�����^�[������B
		return m_shaderData[m_shaderData.size() - 1]->GetShaderBlob();

	}

	Microsoft::WRL::ComPtr<ID3DBlob> RayShaderStorage::GetShaderData(const std::string& arg_shaderPath)
	{

		/*-- �V�F�[�_�[�f�[�^��Ԃ����� --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//�S�ẴV�F�[�_�[�f�[�^����������B
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//�����̗v�f�������Ă��邩���`�F�b�N����B
			if (!(m_shaderData[index]->GetShaderPath() == arg_shaderPath)) continue;

			return m_shaderData[index]->GetShaderBlob();

		}

		return Microsoft::WRL::ComPtr<ID3DBlob>();
	}

	Microsoft::WRL::ComPtr<IDxcBlob> RayShaderStorage::GetShaderDataForDXC(const std::string& arg_shaderPath)
	{
		/*-- �V�F�[�_�[�f�[�^��Ԃ����� --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//�S�ẴV�F�[�_�[�f�[�^����������B
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//�����̗v�f�������Ă��邩���`�F�b�N����B
			if (!(m_shaderData[index]->GetShaderPath() == arg_shaderPath)) continue;

			return m_shaderData[index]->GetShaderBlobDXC();

		}

		return Microsoft::WRL::ComPtr<IDxcBlob>();
	}

	std::vector<char>& RayShaderStorage::GetShaderBin(const std::string& arg_shaderPath)
	{
		/*-- �V�F�[�_�[�f�[�^��Ԃ����� --*/

		const int SHADER_COUNT = static_cast<int>(m_shaderData.size());

		//�S�ẴV�F�[�_�[�f�[�^����������B
		for (int index = 0; index < SHADER_COUNT; ++index) {

			//�����̗v�f�������Ă��邩���`�F�b�N����B
			if (!(m_shaderData[index]->GetShaderPath() == arg_shaderPath)) continue;

			return m_shaderData[index]->GetShaderBin();

		}

		//�V�F�[�_�[�����[�h����Ă��Ȃ��B
		assert(0);
		return m_shaderData[0]->GetShaderBin();
	}

}