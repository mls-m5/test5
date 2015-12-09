/*
 * ship.cpp
 *
 *  Created on: 9 dec 2015
 *      Author: Mattias Larsson Sköld
 */



#include "iship.h"
#include "iscreen.h"
#include "ishot.h"
#include <cmath>
#include "ai.h"

using namespace std;

class Ship : public IShip{
protected:

	IScreen *_screen;
	float _x, _y;
	float _xvel = 0, _yvel = 0;
	int _ai;
	int _pic = 1;
	float _fire_rate = 0;
	AI _ai_object;
	float _hp = 10;
	bool _dead = false;

public:
	Ship(IScreen *screen, float x, float y, int ai):
	_screen(screen),
	_x(x),
	_y(y),
	_ai(ai),
	_ai_object(screen, this, ai){

		switch(ai) {
		case -1:
			set_pic(0);
			break;
		case 1:
			set_pic(1);
			break;
		case 2:
			set_pic(7);
			break;
		case 3:
			set_pic(8);
			break;
		}

	}



	void ai(int val) {

	}


    void set_pic(int number) {
        _pic = number;
    }

    void set_player() {
        _ai = 0;
		_pic = 0;
    }

    void render() override{
        _screen->fast_display(_x, _y, _pic);
    }

    void update() {
        _x += _xvel;
        _y += _yvel;

        if (_ai >= 0) {
            _ai_object.move();
        }

        if (_y > _screen->get_max_y()) {
            _screen->explosion(_x, _y);
        }
    }

    void damage(float value) {
        _hp -= value;
        if (_hp < 0) {
        	if (not _dead) {
        		_screen->remove_ship(this);
        		_screen->explosion(_x, _y);
        		_dead = true;
        	}
        }
    }

    void fire(float rate, shared_ptr<IShip> target) {
        if (_fire_rate > 0) {
            _fire_rate -= 1;
            return;
        }

        _fire_rate = rate;


        auto player = _screen->get_player();

        if (not player) {
        	return;
        }
        auto dx = player->x() - _x;
        auto dy = player->y() - _y - 15;

        auto total = sqrt(dx * dx + dy * dy);
        auto xvel = dx / total * 15.;
        auto yvel = dy / total * 15.;

        _screen->create_shot(_x, _y + 40, xvel, yvel, target, 100);

        _screen->play_sound(IScreen::sndEnemy);
    }




	virtual float x() { return _x; }
	virtual float y() { return _y; }

	virtual void x(float val) { _x = val; };
	virtual void y(float val) { _y = val; };


	virtual void add_x_vel(float val) { _xvel += val; }
	virtual void add_y_vel(float val) { _yvel += val; }


	virtual void x_vel(float val) { _xvel = val; };
	virtual void y_vel(float val) { _yvel = val; };

	virtual float x_vel() { return _xvel; }
	virtual float y_vel() { return _yvel; }

	virtual float hp() { return _hp; };
	virtual void hp(float value) { _hp = value; }

};

std::shared_ptr<IShip> create_ship(IScreen* screen, float x, float y, int ai) {
	return std::shared_ptr<IShip>(new Ship(screen, x, y, ai));
}
