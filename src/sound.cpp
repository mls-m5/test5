/*
 * sound.cpp
 *
 *  Created on: 22 aug 2012
 *      Author: mattias
 */




#include "sound.h"
#include <SDL/SDL_audio.h>
#include <map>
#include <list>
#include <iostream>
#include <climits>

using std::map;
using std::list;
using std::cout;
using std::endl;

#define MAXPLAYINGSOUNDS 16

static map<string, Sound*> soundBufferMap;
static list<Sound*> soundBufferList;

SoundIntance playingSounds[MAXPLAYINGSOUNDS];

double generalFrequency = 1;
double generalVolume = 1. / 16.;

void mixaudio(void* unused, Uint8* stream, int len) {

	int i;
	auto streamData = (Sint16*) stream;
	auto length = len / sizeof(Sint16);

	for (int i = 0; i < length; ++i) {
		streamData[i] = 0;
	}

	for ( i=0; i<MAXPLAYINGSOUNDS; ++i ) {
		if (not playingSounds[i].playing){
			continue;
		}

		auto bufferLen = playingSounds[i].dlen / sizeof(Sint16);
		auto &dpos = playingSounds[i].dpos;
		auto data = (Sint16*)playingSounds[i].data;

		auto t = (int) dpos;
		for (int j = 0; j < length; ++j){
			t = ((int) (dpos / 2)) * 2;
			streamData[j] += data[t] * generalVolume;
			++j; ++t;
			streamData[j] += data[t] * generalVolume;

			dpos += generalFrequency * 2; //times 2 because of stereo
			if (dpos > bufferLen){
				playingSounds[i].playing = false;
				break;
			}
		}
	}
}

Sound::Sound(string fname, bool loadNew) {
	auto it = soundBufferMap.find(fname);

	if (it == soundBufferMap.end() || loadNew){
		SDL_AudioSpec wave;
		Uint8 *tdata;
		Uint32 dlen;
		SDL_AudioCVT cvt;


		/* Load the sound file and convert it to 16-bit stereo at 22kHz */
		if ( SDL_LoadWAV(("sound/" + fname).c_str(), &wave, &tdata, &dlen) == NULL ) {
			cout << "Couldn't load " << fname << " " << SDL_GetError() << endl;
            tdata = 0;
			return;
		}
		SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
				AUDIO_S16,   2,             44100);
//		cvt.buf = (Uint8*)malloc(dlen*cvt.len_mult);
		cvt.buf = new Uint8[dlen*cvt.len_mult];
		memcpy(cvt.buf, tdata, dlen);
		cvt.len = dlen;
		SDL_ConvertAudio(&cvt);
		SDL_FreeWAV(tdata);

//		this->data = cvt.buf;
		this->dlen = cvt.len_cvt;

		dataContainer = std::shared_ptr<DataContainer>(new DataContainer);
		dataContainer->data = cvt.buf;

		soundBufferMap[fname] = this;
		cout << "laddar ljud " << fname << endl;
	}
	else{
//		this->data = it->second->data;
		this->dlen = it->second->dlen;
		this->dataContainer = it->second->dataContainer;

		cout << "återanvänder ljud " << fname << endl;
	}
	soundBufferList.push_back(this);
	cout << "antal ljud: " << soundBufferList.size() << endl;
}



Sound::~Sound() {
	stop();
	clear();
	for (auto it = soundBufferMap.begin(); it != soundBufferMap.end();) {
		if (it->second == this) {
			soundBufferMap.erase(it);
		}
		else {
			++it;
		}
	}
}

void Sound::play(bool looping) {
	if (not dataContainer) {
		return;
	}
	SDL_LockAudio();
	int index;
	for ( index=0; index<MAXPLAYINGSOUNDS; ++index ) {
		if (!playingSounds[index].playing) {
			break;
		}
	}
	if ( index == MAXPLAYINGSOUNDS ){
		return;
	}
	playingSounds[index].data = dataContainer->data;
	playingSounds[index].dlen = dlen;
	playingSounds[index].dpos = 0;
	playingSounds[index].looping = looping;
	playingSounds[index].playing = true;

	SDL_UnlockAudio();
}

void Sound::playLoop() {
	play(true);
}

void Sound::setVolume(double v) {
}

void Sound::clear() {
	for (auto it: soundBufferMap){
		if (it.second == this){
			soundBufferMap.erase(it.first);
			break;
		}
	}

	soundBufferList.remove(this);
	cout << "removing sound " << soundBufferList.size() << " remains" << endl;
	//Check if other sound is using the same sound data
//	for (auto it: soundBufferList){
//		if (it->data == dataContainer->data){
//			return;
//		}
//	}

	if (dataContainer) {
		//otherwise no other object point at the data
		for (int i = 0; i < MAXPLAYINGSOUNDS; ++i){
			if (playingSounds[i].data == dataContainer->data){
				SDL_LockAudio();
				playingSounds[i].playing = false;
				playingSounds[i].data = 0;
				SDL_UnlockAudio();
			}

		}
	}
//    if (dataContainer->data){
//        delete data;
//    }
}

void Sound::InitSound() {

    SDL_AudioSpec fmt;

    /* Set 16-bit stereo audio at 22Khz */
    fmt.freq = 44100;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = mixaudio;
    fmt.userdata = NULL;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_PauseAudio(0);
}

void Sound::stop() {

}

void Sound::setGeneralFrequency(double f) {
	generalFrequency = f;
}
