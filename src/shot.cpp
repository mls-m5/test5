/*
 * shot.cpp
 *
 *  Created on: 9 dec 2015
 *      Author: Mattias Larsson Sköld
 */


#include "ishot.h"
#include "iship.h"
#include "iscreen.h"
#include <cmath>
using namespace std;

class Shot : public IShot{
public:
	Shot(class IScreen *screen, float x, float y, float vx,
		float vy, std::shared_ptr<class IShip> target, float range, bool from_explosion):
			screen(screen),
			x(x),
			y(y),
			vx(vx),
			vy(vy),
			target(target),
			range(range),
			from_explosion(from_explosion){

	}

	void render() override {
        auto s = screen;
        s->fast_display(x - vx * 2, y - vy * 2, 3, .2);
        s->fast_display(x - vx * 3, y - vy * 3, 3, .4);
        s->fast_display(x - vx * 4, y - vy * 4, 4, .6);
        s->fast_display(x - vx, y - vy, 2, .8);
        s->fast_display(x, y, 2);
	}

	void update() override {
        auto h = screen->is_hit(x, y);
        auto hit = h.lock();
        if (hit) {
            hit->damage(3);
            screen->remove_shot(this);
            if( not from_explosion) {
                screen->small_explosion(x - vx, y - vy, -vx / 3, -vy / 3);
                screen->play_sound(IScreen::sndCrash);
            }
        }
        if (target) {
            auto &t = target;

            if (t->x() != x and y != t->y()) {
                auto dx = t->x() - x;
                auto dy = t->y() - y;
                if (dx * vx + dy * vy > 0) {  // if the target is in front of the shot
                    auto dist = sqrt(dx * dx + dy * dy);
                    if (dist > 140) {
                    	vx = dx / dist * 10 + t->x_vel();
                    	vy = dy / dist * 10 + t->y_vel();
                    }
                }
            }
        }
        x += vx;
        y += vy;

        range -= 1;
        if (range < 0) {
            screen->remove_shot(this);
            screen->fast_display(x, y, 5);
        }

//        if (from_explosion == 2) {
//            screen->explosion(x, y);
//        }
	}

protected:
	float x, y;
	float vx, vy;
	shared_ptr<IShip> target;
	float range = 10;
	bool from_explosion;
	IScreen *screen;

};

std::shared_ptr<IShot> create_shot(class IScreen *screen, float x, float y, float vx,
		float vy, std::shared_ptr<class IShip> target, float range, bool from_explosion) {
	return shared_ptr<IShot>(new Shot(screen, x, y, vx, vy, target, range, from_explosion));
}

