/*
 * iship.h
 *
 *  Created on: 9 dec 2015
 *      Author: Mattias Larsson Sköld
 */


#pragma once
#include <memory>


class IShip {
public:
	virtual ~IShip() {}
	virtual void update() = 0;
	virtual void render() = 0;

	virtual void ai(int) = 0;
	virtual void fire(float fire_rate, std::shared_ptr<class IShip> = 0) = 0;

	virtual float x() { return 0; }
	virtual float y() { return 0; }

	virtual void x(float val) {}
	virtual void y(float val) {}

	virtual float hp() { return 0; }
	virtual void hp(float) {}
	virtual void damage( float value ) {}


	virtual void x_vel(float val) {}
	virtual void y_vel(float val) {}

	virtual float x_vel() { return 0; }
	virtual float y_vel() { return 0; }


	virtual void add_x_vel(float val) {}
	virtual void add_y_vel(float val) {}
};


std::shared_ptr<IShip> create_ship(class IScreen*, float x, float y, int ai);

