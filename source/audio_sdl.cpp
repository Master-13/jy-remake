/*
==========================================================================================
File: audio_sdl.c
Note: ʹ��SDL_mixerʵ�ֵ���Ƶģ�顣���Ǹ����ư汾��SDL_mixer��������FluidSynth��
Author: Zgames
==========================================================================================
*/

#include <string.h>
#include <SDL.h>
#include <string>
#include "libzaudio/SDL_mixer.h"
#include "util.h"
#include "audio.h"

static const int kMaxWaveChunks = 5;
// Max volume is 128
static const int kMusicVolume = 64;
static const int kSoundVolume = 64;

static Mix_Music *s_current_music = NULL;	//�����������ݣ�����ͬʱֻ����һ������һ������
static Mix_Chunk *s_wave_chunks[kMaxWaveChunks];	//������Ч���ݣ�����ͬʱ���ż��������������
static int s_current_wave = 0;		//��ǰ���ŵ���Ч
static int s_sound_enabled = SDL_FALSE;

//////////////////////////////////////////////////////////////////////////////////////////
// Sdl2AudioIniter
//////////////////////////////////////////////////////////////////////////////////////////

class Sdl2AudioIniter {
public:
    Sdl2AudioIniter();
    ~Sdl2AudioIniter();
    bool inited() const;
};

Sdl2AudioIniter::~Sdl2AudioIniter()
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

Sdl2AudioIniter::Sdl2AudioIniter()
{
    if (!inited()) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            throw HardwareException("SDL2 Audio Subsystem initialization failed.");
        }
    }
}

bool Sdl2AudioIniter::inited() const
{
    return SDL_WasInit(SDL_INIT_AUDIO);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////

class WavePool {
public:
    void load(const char *filename);
private:
    static const int kMaxWaveChunks = 20;
    Mix_Chunk *_waveChunks[kMaxWaveChunks];
};

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////

class Sdl2Audio : public Audio {
public:
    Sdl2Audio();
    ~Sdl2Audio();
    virtual void playMidi(const char *filename);
    virtual void playWave(const char *filename);
    virtual void fadeOut(Uint32 msec);
    virtual void stopMidi();
private:
    static const int kMaxWaveChunks = 5;
    // Max volume is 128
    static const int kMusicVolume = 64;
    static const int kSoundVolume = 64;
    Sdl2AudioIniter _sdl2AudioIniter;
    Mix_Music *_pCurrentMusic;	//�����������ݣ�����ͬʱֻ����һ������һ������
    Mix_Chunk *_waveChunks[kMaxWaveChunks];	//������Ч���ݣ�����ͬʱ���ż��������������
    std::string _lastMusicFname;
    int _iCurrentWave;		//��ǰ���ŵ���Ч
    bool _soundEnabled;
};

Sdl2Audio::~Sdl2Audio()
{
}

Sdl2Audio::Sdl2Audio()
    : _pCurrentMusic(NULL),
    _iCurrentWave(0),
    _soundEnabled(false)
{
    Mix_SetSoundFonts("data/sb.sf2");
    // ������̫�����������ӳ٣�2048�ȽϺ��ʡ�
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        throw HardwareException("Mix_OpenAudio() failed.");
    }
	for (int i = 0; i < kMaxWaveChunks; i++)
		_waveChunks[i] = NULL;
    _soundEnabled = true;
}

void Sdl2Audio::stopMidi()
{
	if (_pCurrentMusic != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(_pCurrentMusic);
		_pCurrentMusic = NULL;
	}
}

void Sdl2Audio::fadeOut(Uint32 msec)
{
    Mix_FadeOutChannel(-1, (int)msec);
    Mix_FadeOutMusic((int)msec);
}

void Sdl2Audio::playMidi(const char *filename)
{
    if (_soundEnabled) {
        if (strlen(filename) == 0) {	//�ļ���Ϊ�գ�ֹͣ����
            stopMidi();
            _lastMusicFname = "";
            return;
        }
        if (_lastMusicFname == filename)
            return;
        stopMidi();
        _pCurrentMusic = Mix_LoadMUS(filename);
        if (_pCurrentMusic == NULL) {
            throw ResourceException(filename);
        }
        Mix_VolumeMusic(kMusicVolume);
        Mix_PlayMusic(_pCurrentMusic, -1);
        _lastMusicFname = filename;
    }
}

void Sdl2Audio::playWave(const char *filename)
{
    if (_soundEnabled) {
        if (_waveChunks[_iCurrentWave]) {
            Mix_FreeChunk(_waveChunks[_iCurrentWave]);
            _waveChunks[_iCurrentWave] = NULL;
        }
        _waveChunks[_iCurrentWave] = Mix_LoadWAV(filename);
        if (_waveChunks[_iCurrentWave]) {
            Mix_VolumeChunk(_waveChunks[_iCurrentWave], kSoundVolume);
            Mix_PlayChannel(1, _waveChunks[_iCurrentWave], 0);
            ++_iCurrentWave;
            if (_iCurrentWave >= kMaxWaveChunks)
                _iCurrentWave = 0;
        } else {
            Log("cannot open file: %s", filename);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// class Audio
//////////////////////////////////////////////////////////////////////////////////////////

Audio *Audio::_instance = NULL;

Audio *Audio::getInstance()
{
    if (_instance == NULL) {
        _instance = new Sdl2Audio();
    }
    return _instance;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////

//ֹͣ��Ч
static int StopMIDI()
{
	if (s_current_music != NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(s_current_music);
		s_current_music = NULL;
	}
	return 0;
}

int Audio_Init()
{
    Audio::getInstance();
}

void Audio_Quit()
{
    delete Audio::getInstance();
}

int Audio_PlayMIDI(const char *filename)
{
    Audio::getInstance()->playMidi(filename);
}

int Audio_PlayWAV(const char *filename)
{
    Audio::getInstance()->playWave(filename);
}


void Audio_FadeOut(Uint32 msec)
{
    Audio::getInstance()->fadeOut(msec);
}

#if 0
int Audio_Init(void)
{
	int i;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        goto out;
    }
    Mix_SetSoundFonts("data/sb.sf2");
    // ������̫�����������ӳ٣�2048�ȽϺ��ʡ�
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        goto out;
    }
	s_current_wave = 0;
	for (i = 0; i < kMaxWaveChunks; i++)
		s_wave_chunks[i] = NULL;
    s_sound_enabled = SDL_TRUE;
	return 0;

out:
    return 1;
}


void Audio_Quit(void)
{
	int i;
	StopMIDI();
	for (i = 0; i < kMaxWaveChunks; i++) {
		if (s_wave_chunks[i]) {
			Mix_FreeChunk(s_wave_chunks[i]);
			s_wave_chunks[i] = NULL;
		}
	}
	Mix_CloseAudio();
}


int Audio_PlayMIDI(const char *filename)
{
	static char currentfile[255] = "";

	if (!s_sound_enabled)
		return 1;
	if (strlen(filename) == 0) {	//�ļ���Ϊ�գ�ֹͣ����
		StopMIDI();
		strcpy(currentfile, filename);
		return 0;
	}
	if (strcmp(currentfile, filename) == 0)	//�뵱ǰ�����ļ���ͬ��ֱ�ӷ���
		return 0;
	StopMIDI();
	s_current_music = Mix_LoadMUS(filename);
	if (s_current_music == NULL) {
        Log("cannot open file: %s", filename);
		return 1;
	}
	Mix_VolumeMusic(kMusicVolume);
	Mix_PlayMusic(s_current_music, -1);
	strcpy(currentfile, filename);

	return 0;
}

int Audio_PlayWAV(const char *filename)
{

	if (!s_sound_enabled)
		return 1;

	if (s_wave_chunks[s_current_wave]) {	//�ͷŵ�ǰ��Ч
		Mix_FreeChunk(s_wave_chunks[s_current_wave]);
		s_wave_chunks[s_current_wave] = NULL;
	}

	s_wave_chunks[s_current_wave] = Mix_LoadWAV(filename);	//���ص���ǰ��Ч

	if (s_wave_chunks[s_current_wave]) {
		Mix_VolumeChunk(s_wave_chunks[s_current_wave], kSoundVolume);
		Mix_PlayChannel(-1, s_wave_chunks[s_current_wave], 0);	//������Ч
		s_current_wave++;
		if (s_current_wave >= kMaxWaveChunks)
			s_current_wave = 0;
	} else {
        Log("cannot open file: %s", filename);
	}

	return 0;
}


void Audio_FadeOut(float msec)
{
    // fade out all channels
    Mix_FadeOutChannel(-1, (int)msec);
    Mix_FadeOutMusic((int)msec);
}

#endif
