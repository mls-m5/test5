/*
 * Sound.h
 *
 *  Created on: 22 aug 2012
 *      Author: mattias
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <string>
#include <SDL/SDL.h>
#include <memory>

using std::string;

class SoundIntance {
public:
    Uint8 *data;
    double dpos; //Double to be able to change frequency
    Uint32 dlen;
    bool playing = false;
    bool looping;
};

//Removes data when noone uses it anymore
struct DataContainer {
	Uint8 *data;

	~DataContainer() {
		delete [] data;
	}
};

class Sound {
public:

	Sound(string fname, bool loadNew = false);
	virtual ~Sound();

	unsigned int sampleFrequency = 44100;
	double playPos = 0;
	double volume = 1;
	bool loop = 0;

	void clear();

	void play(bool loop = false);
	void playLoop();
	void stop();
	void setVolume(double v);

	static void InitSound();
	static void setGeneralFrequency(double f);

protected:

//    Uint8 *data;
    Uint32 dlen;

    std::shared_ptr<DataContainer> dataContainer;
};

//Class that locks and unlocks when going out of scope
class AudioLock{
public:
	AudioLock() {
		SDL_LockAudio();
	}

	~AudioLock() {
		SDL_UnlockAudio();
	}
};

#endif /* SOUND_H_ */
