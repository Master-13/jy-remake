/*
==========================================================================================
File: audio_sdl2.c
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
            throw ResourceException(va("cannot open file: %s", filename));
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

