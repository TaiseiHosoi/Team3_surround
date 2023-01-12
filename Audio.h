#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <map>
#include <string>
#include <cassert>
#include <vector>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <propvarutil.h>


class Audio
{
private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];		//�`�����N���Ƃ�id
		int32_t size;	//�`�����N�T�C�Y
	};

	//RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};
	//FMT�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMATEX fmt;	//�g�`�t�H�[�}�b�g
	};
	//�����f�[�^
	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		BYTE* pBuffer;
		//�o�b�t�@�̃T�C�Y
		unsigned int bufferSize;
	};
public:
	//������
	void Initialize(const std::string directoryPath = "Resources/");

	//�I������
	void Finalize();

	/// <summary>
	/// WAV�����ǂݍ���
	/// </summary>
	/// <param name="filename">WAV�t�@�C����</param>
	void LoadWave(const std::string filename);


	/// <summary>
	///	�T�E���h�f�[�^�̊J��
	/// </summary>
	void Unload(SoundData* soundData);

	/// <summary>
	/// �T�E���h�f�[�^��
	/// </summary>
	/// <param name="filename">WAV�t�@�C����</param>
	void PlayWave(const std::string filename, bool LoopFlag = false, float sourceRate=1.0f, float targetRate=1.0f);

	void SetPitch(const std::string filename,float sourceRate,float targetRate);

private:
	//xAudio�̃C���X�^���X
	ComPtr<IXAudio2>xAudio2_;
	//�T�E���h�f�[�^�̔z��
	std::map<std::string, SoundData> soundDatas_;

	//�T�E���h�i�[�f�B���N�g��
	std::string directoryPath_;
};
