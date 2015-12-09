/*
 * ishot.h
 *
 *  Created on: 9 dec 2015
 *      Author: Mattias Larsson Sköld
 */


#pragma once
#include <memory>


class IShot {
public:
	virtual ~IShot() {};

	virtual void update() = 0;
	virtual void render() = 0;
};

std::shared_ptr<IShot> create_shot(class IScreen *screen, float x, float y, float vx,
		float vy, std::shared_ptr<class IShip> = 0, float range = 100, bool from_explosion = false);



