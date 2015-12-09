/*
 * iscreen.h
 *
 *  Created on: 9 dec 2015
 *      Author: Mattias Larsson Sköld
 */


#pragma once
#include <memory>


class IScreen {
public:

	enum SoundEnum {

		sndExp = 0,
		sndMissileLaunch = 1,
		sndCrash = 2,
		sndLaser = 3,
		sndEnemy = 4,
		sndCound

	};

	virtual ~IScreen() {}

	virtual std::shared_ptr<class IShip> get_player() = 0;

	virtual void fast_display(float x, float y, int num, float alpha = 1, bool center = true) = 0;

	virtual void play_sound(SoundEnum) = 0;

	virtual void explosion(float x, float y) = 0;
	virtual void small_explosion(float x, float y, float vx, float vy) = 0;

	virtual float get_max_x() = 0;
	virtual float get_max_y() = 0;

	virtual void remove_ship(class IShip*) = 0;
	virtual void remove_shot(class IShot*) = 0;

	virtual std::weak_ptr<IShip> is_hit(float x, float y) = 0;


    virtual void create_shot(float x, float y, float vx, float vy,
    		std::shared_ptr<IShip> target = 0, int range = 100, int from_explosion = false) = 0;
};


