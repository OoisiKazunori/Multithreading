#include "SoundManager.h"
#include"../Helper/KazHelper.h"



Microsoft::WRL::ComPtr<IXAudio2> SoundManager::xAudio2{};
IXAudio2MasteringVoice* SoundManager::masterVoice{};

//XAudio2VoiceCallback�̃C���X�^���X���O���[�o���ϐ��Ƃ��ăC���X�^���X�����
XAudio2VoiceCallback voiceCallback{};

void SoundManager::SettingSoundManager()
{
	//xaudio2�̃G���W���𐶐�
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//�}�X�^�[�{�C�X�𐶐�
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

SoundData SoundManager::SoundLoadWave(char* arg_fileName)
{

	//�@�t�@�C���I�[�v��
	//�t�@�C�����̓X�g���[���̃C���X�^���X
	ifstream file{};
	//.wav���o�C�i�����[�h�ŊJ��
	file.open(arg_fileName, ios_base::binary);
	//�t�@�C���I�[�v���̎��s�����o����
	assert(file.is_open());

	//�A.wav�̓ǂݍ��݂ƍĐ�
	//RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//�t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//�^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Format�`�����N�̓ǂݍ���
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//�`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNK�`�����N�����o�����ꍇ
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		//�ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "LIST ", 4) == 0) {
		//�ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}
	//Data�`�����N�̃f�[�^��(�g�`�f�[�^)�̓ǂݍ���
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//Wave�t�@�C�������
	file.close();

	//return���邽�߂̉����f�[�^
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.volume = 1.0f;

	return soundData;

	////�B�T�E���h�Đ� �ǂݍ���.wav�t�@�C���̏�񂩂特�����Đ�����
	//WAVEFORMATEX wfex{};
	////�g�`�t�H�[�}�b�g�̐ݒ�
	//memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	//wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;
	////�g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
	//IXAudio2SourceVoice* pSourceVoice = nullptr;
	//result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	//if FAILED(result) {
	//	delete[] pBuffer;
	//	return;
	//}
	////�Đ�����g�`�f�[�^�̐ݒ�
	//XAUDIO2_BUFFER buf{};
	//buf.pAudioData = (BYTE*)pBuffer;
	//buf.pContext = pBuffer;
	//buf.Flags = XAUDIO2_END_OF_STREAM;
	//buf.AudioBytes = data.size;
	////�g�`�f�[�^�̍Đ�
	//result = pSourceVoice->SubmitSourceBuffer(&buf);
	//result = pSourceVoice->Start();
}

void SoundManager::SoundUnload(SoundData* arg_soundData)
{
	//�o�b�t�@�����������
	delete[]  arg_soundData->pBuffer;

	arg_soundData->pBuffer = 0;
	arg_soundData->bufferSize = 0;
	arg_soundData->wfex = {};
}

void SoundManager::SoundPlayerWave(SoundData& arg_soundData, int arg_loopCount)
{
	HRESULT result;

	//�g�`�t�H�[�}�b�g�����Ƃ�SourceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &arg_soundData.wfex);
	assert(SUCCEEDED(result));

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = arg_soundData.pBuffer;
	buf.AudioBytes = arg_soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = arg_loopCount;

	pSourceVoice->SetVolume(arg_soundData.volume);

	arg_soundData.source = pSourceVoice;

	//�g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();

}



//
//SoundManager::SoundManager()
//{
//	//XAudio�G���W���̃C���X�^���X�𐶐�
//	XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
//	//�}�X�^�[�{�C�X�𐶐�
//	xAudio2->CreateMasteringVoice(&masterVoice);
//}
//
//void SoundManager::Finalize()
//{
//}
//
//RESOURCE_HANDLE SoundManager::LoadSoundMem(std::string FILE_PASS, bool BGM_FLAG)
//{
//	//�ǂݍ��ݏd���h�~
//	for (int i = 0; i < masterSoundData.size(); i++)
//	{
//		if (FILE_PASS == masterSoundData[i].filePass)
//		{
//			return static_cast<RESOURCE_HANDLE>(i);
//		}
//	}
//
//
//	std::ifstream file;
//	file.open(FILE_PASS, std::ios_base::binary);
//	if (file.fail())
//	{
//		FailCheck(FILE_PASS + "�̓ǂݍ��݂Ɏ��s���܂���\n");
//		return -1;
//	}
//
//	//RIFF------------------------------------------
//	RiffHeader riff;
//	file.read((char *)&riff, sizeof(riff));
//
//	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "�̓ǂݍ��݂Ɏ��s���܂���\n");
//		return -1;
//	}
//	if (strncmp(riff.type.data(), "WAVE", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "�̓ǂݍ��݂Ɏ��s���܂���\n");
//		return -1;
//	}
//	//RIFF------------------------------------------
//
//
//	//FormatChunk------------------------------------------
//	FormatChunk format = {};
//	file.read((char *)&format, sizeof(ChunkHeader));
//
//	if (strncmp(format.chunk.id.data(), "fmt ", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "�̓ǂݍ��݂Ɏ��s���܂���\n");
//		return -1;
//	}
//
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char *)&format.fmt, format.chunk.size);
//	//FormatChunk------------------------------------------
//
//	//ChunkHeader------------------------------------------
//	ChunkHeader data;
//	file.read((char *)&data, sizeof(data));
//	if (strncmp(data.id.data(), "JUNK", 4) == 0)
//	{
//		file.seekg(data.size, std::ios_base::cur);
//		file.read((char *)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id.data(), "LIST ", 4) == 0)
//	{
//		file.seekg(data.size, std::ios_base::cur);
//		file.read((char *)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id.data(), "data ", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "�̓ǂݍ��݂Ɏ��s���܂���\n");
//		return -1;
//	}
//	//ChunkHeader------------------------------------------
//
//
//	//�t�@�C���ǂݍ��ݐ���
//	RESOURCE_HANDLE lHandle = handle.GetHandle();
//	
//	//true...�z��ȏ�Ȃ�push,false...�z����Ȃ炻�̂܂ܒu������
//	if (masterSoundData.size() <= lHandle)
//	{
//		//masterSoundData.push_back({});
//		masterSoundData[lHandle].soundData = std::make_unique<SoundData>();
//		masterSoundData[lHandle].filePass = FILE_PASS;
//	}
//	else
//	{
//		masterSoundData[lHandle].soundData = std::make_unique<SoundData>();
//		masterSoundData[lHandle].filePass = FILE_PASS;
//	}
//
//
//
//	masterSoundData[lHandle].soundData->wfex= format.fmt;
//	masterSoundData[lHandle].soundData->bufferSize= data.size;	
//	masterSoundData[lHandle].soundData->pBuffer = new BYTE[data.size];
//	file.read(reinterpret_cast<char *>(masterSoundData[lHandle].soundData->pBuffer), data.size);
//	file.close();
//
//
//	//�T�E���h�\�[�X����
//	if (BGM_FLAG)
//	{
//		xAudio2->CreateSourceVoice(&masterSoundData[lHandle].soundSorce, &masterSoundData[lHandle].soundData->wfex);
//	}
//
//
//
//	SucceedCheck(FILE_PASS + "�̓ǂݍ��݂ɐ������܂���\n");
//
//	return lHandle;
//}
//
//void SoundManager::PlaySoundMem(RESOURCE_HANDLE HANDLE, int VOLUME, bool LOOP_FLAG)
//{
//	if (KazHelper::IsitInAnArray(HANDLE, masterSoundData.size()))
//	{
//		if (masterSoundData[HANDLE].soundData == nullptr)
//		{
//			FailCheck("���̍Đ��Ɏ��s���܂���\n");
//			return;
//		}
//
//		//���Đ�
//		if (masterSoundData[HANDLE].soundSorce == nullptr)
//		{
//			HRESULT result;
//			IXAudio2SourceVoice *pSourceVoice = nullptr;
//			result = xAudio2->CreateSourceVoice(&pSourceVoice, &masterSoundData[HANDLE].soundData->wfex);
//			XAUDIO2_BUFFER buf{};
//			buf.pAudioData = masterSoundData[HANDLE].soundData->pBuffer;
//			buf.AudioBytes = masterSoundData[HANDLE].soundData->bufferSize;
//			buf.Flags = XAUDIO2_END_OF_STREAM;
//			if (LOOP_FLAG)
//			{
//				buf.LoopCount = XAUDIO2_LOOP_INFINITE;
//			}
//			else
//			{
//				buf.LoopCount = XAUDIO2_NO_LOOP_REGION;
//			}
//
//			//���ʒ���
//			float volumeTmp = VOLUME / 100.0f;
//			if (1 <= volumeTmp)
//			{
//				volumeTmp = 1;
//			}
//			pSourceVoice->SetVolume(volumeTmp);
//			result = pSourceVoice->SubmitSourceBuffer(&buf);
//			result = pSourceVoice->Start();
//		}
//		else
//		{
//			XAUDIO2_BUFFER buf{};
//			buf.pAudioData = masterSoundData[HANDLE].soundData->pBuffer;
//			buf.AudioBytes = masterSoundData[HANDLE].soundData->bufferSize;
//			buf.Flags = XAUDIO2_END_OF_STREAM;
//			if (LOOP_FLAG)
//			{
//				buf.LoopCount = XAUDIO2_LOOP_INFINITE;
//			}
//			else
//			{
//				buf.LoopCount = XAUDIO2_NO_LOOP_REGION;
//			}
//
//			//���ʒ���
//			float volumeTmp = VOLUME / 100.0f;
//			if (1 <= volumeTmp)
//			{
//				volumeTmp = 1;
//			}
//			masterSoundData[HANDLE].soundSorce->SetVolume(volumeTmp);
//			masterSoundData[HANDLE].soundSorce->SubmitSourceBuffer(&buf);
//			masterSoundData[HANDLE].soundSorce->Start();
//		}
//	}
//	else
//	{
//		return;
//	}	
//}
//
//void SoundManager::ChangeSoundMem(RESOURCE_HANDLE HANDLE, int VOLUME)
//{
//	float volumeTmp = VOLUME / 100.0f;
//	masterSoundData[HANDLE].soundSorce->SetVolume(volumeTmp);
//}
//
//void SoundManager::StopSoundMem(RESOURCE_HANDLE HANDLE)
//{
//	if (KazHelper::IsitInAnArray(HANDLE, masterSoundData.size()))
//	{
//		if (masterSoundData[HANDLE].soundData != nullptr)
//		{
//			masterSoundData[HANDLE].soundSorce->Stop();
//			masterSoundData[HANDLE].soundSorce->FlushSourceBuffers();
//			masterSoundData[HANDLE].soundData->wfex;
//		}
//	}
//}
//
//void SoundManager::ReleaseSoundMem(RESOURCE_HANDLE HANDLE)
//{
//	if (KazHelper::IsitInAnArray(HANDLE, masterSoundData.size()))
//	{
//		if (masterSoundData[HANDLE].soundData != nullptr)
//		{	
//			masterSoundData[HANDLE].soundSorce->Stop();
//			masterSoundData[HANDLE].soundSorce->DestroyVoice();			
//			//�T�E���h�f�[�^�̍폜
//			masterSoundData[HANDLE].soundData.reset();
//			masterSoundData[HANDLE].soundData.reset();
//			masterSoundData[HANDLE].soundData = nullptr;
//			//�t�@�C���p�X�̍폜
//			masterSoundData[HANDLE].filePass = "";
//			//�n���h���̍폜
//			handle.DeleteHandle(HANDLE);
//		}
//	}
//}
