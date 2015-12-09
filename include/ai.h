/*
 * ai.h
 *
 *  Created on: 9 dec 2015
 *      Author: Mattias Larsson Sköld
 */


#pragma once

#include "iship.h"


class AI {
protected:

	IScreen *screen;
	IShip *ship;
	int ai;
	int data = 0;

public:
	AI(class IScreen* ascreen, class IShip* aship, int aAI) {
		screen = ascreen;
		ship = aship;
		ai = aAI;
	}

	void move() {
		switch (ai) {
		case 1:
			strategy1();
			break;
		case 2:
			strategy2();
			break;
		case 3:
			strategy3();
			break;
		}
	}

    void strategy1(){
        auto &s = *ship;
        auto player = screen->get_player();

        auto bottom = screen->get_max_y();
        auto width = screen->get_max_x();

		switch (data) {
		case 6:
            if (s.y() > bottom / 3){
                s.add_y_vel(-1);
            }
            else{
            	s.add_y_vel(1);
            }
            if (player) {
            	if (s.x() > player->x()){
            		s.add_x_vel(-1);
            	}
            	else{
            		s.add_x_vel(1);
            	}
            }
            else {
            	s.x_vel(0);
            }
            s.fire(40);
            if (s.x() < 0){
                s.x_vel(1);
            }
            if (s.x() > width){
                s.x_vel(-1);
            }
            break;
		case 5:
            s.y_vel(2);
            s.x_vel(0);
            if (s.y() > bottom / 1.5){
                data = 4;
            }
            break;
		case 4:
            s.y_vel(0);
            data = 6;
            break;
		case 3:
			s.x_vel(0);
			s.y_vel(2);
            s.fire(50);
            if (s.y() > bottom / 4){
                data = 5;
            }
            break;
		case 2:
			s.y_vel(3);
			if (player) {
				if (player->x() > s.x()){
					s.x_vel(1);
				}
				else{
					s.x_vel(-1);
				}
			}
            s.fire(50);
            if (s.y() > bottom / 5){
                data = 3;
            }
            break;
		case 1:
			s.y_vel(4);
            if (s.y() > 150){
                data = 2;
            }
            break;
		case 0:
			s.y_vel(6);
            if (s.y() > bottom / 12){
                data = 1;
            }
            break;
		}
    }

    void strategy2() {
        auto &s = *ship;
        auto player = screen->get_player();
        auto width = screen->get_max_x();
        auto bottom = screen->get_max_y();

		switch (data) {
		case 0:
			s.y_vel(2);
            if (s.y() > bottom / 4){
                data = 1;
                s.x_vel(-3);
                s.y_vel(0);
            }
            break;
		case 1:
            if (s.x() > width - 32){
                s.x_vel(-3);
            }
            else if (s.x() < 0){
                s.x_vel(3);
            }
            if (rand() > RAND_MAX * 0.7){
                s.fire(10, player);
            }
            else {
                s.fire(10);
            }
            break;
		}
    }

    void strategy3(){
        auto &s = *ship;
        auto player = screen->get_player();
        if (not player) {
        	return;
        }
        float r = (float) rand() / RAND_MAX;
        s.y_vel(6);
        screen->create_shot(s.x() - 15, s.y() + 15, 20 * r - 10, 20, 0, 20, 1);
        screen->create_shot(s.x() + 15, s.y() + 15, 20 * r - 10, 20, 0, 20, 1);
        if (s.x() > player->x()){
        	s.x_vel(-10);
        }
        else{
        	s.x_vel(10);
        }
        if (player->x() - 16 < s.x() and s.x() < player->y() + 16){
        	s.x_vel(0);
        }
    }

};

