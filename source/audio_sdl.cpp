/*
==========================================================================================
File: audio_sdl.c
Note: ʹ��SDL_mixerʵ�ֵ���Ƶģ�顣���Ǹ����ư汾��SDL_mixer��������FluidSynth��
Author: Zgames
==========================================================================================
*/

#include <string.h>
#include <SDL.h>
#include "libzaudio/SDL_mixer.h"
#include "util.h"

static const int kMaxWaveChunks = 5;
// Max volume is 128
static const int kMusicVolume = 64;
static const int kSoundVolume = 64;

static Mix_Music *s_current_music = NULL;	//�����������ݣ�����ͬʱֻ����һ������һ������
static Mix_Chunk *s_wave_chunks[kMaxWaveChunks];	//������Ч���ݣ�����ͬʱ���ż��������������
static int s_current_wave = 0;		//��ǰ���ŵ���Ч
static int s_sound_enabled = SDL_FALSE;

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

