//Test5 c++ remake
//Copyright Mattias Larsson Sköld 2015

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <SDL2/SDL.h>
#include <iship.h>
#include <ishot.h>
#include <string>
#include <sstream>
#include "iscreen.h"
#include "sound.h"

using namespace std;

float random_uniform(float min, float max) {
	return (float) rand() / RAND_MAX * (max - min) + min;
}


class Screen : public IScreen{
public:

	struct Star {
		float x;
		float y;
	};

	vector<shared_ptr<Sound>> sounds;

	Screen() {
		cout << "hej" << endl;



		{
			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = 640;
			r.h = 480;
			screen_rect = r;

			window = SDL_CreateWindow("Test 5",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					r.w, r.h, SDL_WINDOW_SHOWN);
		}

		Sound::InitSound();

		sounds = {
				shared_ptr<Sound>(new Sound("exp.wav")),
				shared_ptr<Sound>(new Sound("launch.wav")),
				shared_ptr<Sound>(new Sound("crash.wav")),
				shared_ptr<Sound>(new Sound("gunshot.wav")),
				shared_ptr<Sound>(new Sound("enemy.wav")),
		};


		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


		{
			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = 32;
			r.h = 32;
			for (int i = 1; i <= 10; ++i) {
				ostringstream ss;
				ss << "data/" << i << ".bmp";
				const string fname = ss.str();

				auto sdl_surface = SDL_LoadBMP(fname.c_str());
				SDL_SetColorKey(sdl_surface, 1, 0);
				textures.push_back(SDL_CreateTextureFromSurface(renderer, sdl_surface));
				SDL_FreeSurface(sdl_surface);
			}
			image_rect = r;
		}


		max_x = screen_rect.w;
		max_y = screen_rect.h;

		player = create_ship(this, max_y / 2, max_y, -1);
		player->hp(100);
		ships.push_back(player);

		create_group();


		init_stars();

		SDL_ShowCursor(0);
		//        init_stars()
	}


	void init_stars() {
		for (int i = 0; i < 10; ++i) {
			stars.push_back(Star({random_uniform(0, max_x), random_uniform(0, max_y)}));
		}
	}


	void create_group() {
        for (int i = 0; i < 5 + int(time_elapsed / 3); ++i) {
        	auto s = create_ship(this, random_uniform(50, max_x - 50), random_uniform(-20, 80), 1);
            s->hp(10);
			ships.push_back(s);
        }

        auto s = create_ship(this, 300, -20,  2);
        s->hp(10);
        ships.push_back(s);

        time_elapsed += 1;

        if (time_elapsed < 10) {
            return;
        }

        if (random_uniform(0, 1) > .8) {
            s = create_ship(this, random_uniform(0, max_x), 0, 3);
            s->hp(50);
            ships.push_back(s);
        }
	}


	void run() {
		auto running = true;

		while (running) {
			SDL_Event event;
			while (SDL_PollEvent(&event) != 0) {

				switch (event.type) {
				case SDL_QUIT:
					running = false;
					return;
				case SDL_MOUSEMOTION:
					if (player) {
						player->x(event.motion.x);
						player->y(event.motion.y);
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1) {
						firing = true;
					}
					else {
						firing_missile = true;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button == 1) {
						firing = false;
					}
					else {
						firing_missile = false;
					}
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_RETURN) {
						shotgun = not shotgun;
					}
					else if ( event.key.keysym.sym == SDLK_SPACE) {
						asrc = not asrc;
					}
					break;
				}


			}

			update();
			render_scene();
		}

	}

	void update() {
		for (auto &s: ships) {
			s->update();
		}

		for (auto &o: objects) {
			o->update();
		}

        game_logic();

        for (auto s: ships_to_remove) {
        	for (auto it = ships.begin(); it != ships.end();) {
        		if (&**it == s) {
        			it = ships.erase(it);
        		}
        		else {
        			++it;
        		}
        	}
        }
        ships_to_remove.clear();

        for (auto s: shots_to_remove) {
        	for (auto it = objects.begin(); it != objects.end();) {
        		if (&**it == s) {
        			it = objects.erase(it);
        		}
        		else {
        			++it;
        		}
        	}
        }

        shots_to_remove.clear();


        SDL_Delay(30);
	}

	void play_sound(SoundEnum sound) {
		sounds[sound]->play();
	}

	void game_logic() {
        if (firing and fire_rate < 0 and player) {
            fire_rate = 1;
            if (shotgun) {
                auto from_x = player->x() + 16 * fire_side;
                auto from_y = player->y() - 18;
                small_explosion(from_x, from_y, 0, -16);
                fast_display(from_x, from_y, 5);
                fire_side *= -1;
                play_sound(sndMissileLaunch);
            }
            else if (asrc) {
                fire_asrc();
            }
            else {
                fire_laser();
            }
        }
        else {
            fire_rate -= 1;
        }

        missile_rate -= 1;
        if (firing_missile and missile_rate < 0 and player) {
        	missile_rate = 4;

            auto from_x = player->x() + 16 * fire_side;
            auto from_y = player->y() - 18;
            fire_side *= -1;
            create_shot(from_x, from_y, 0, -16, r_target.lock(), 100);
            play_sound(sndLaser);
        }

        auto target = r_target.lock();

        if (target and target->hp() < 0) {
        	r_target.reset();
        }

        if (asrc and target) {
        	c_x += (target->x() - c_x) / 3;
        	c_y += (target->y() - c_y) / 3;
        }

        next_wave -= 1;
		if ((ships.size() == 1 or next_wave < 0) and player){
			create_group();
			next_wave = 100 + 250 / time_elapsed;
		}

		if (player) {
			if (player->hp() < 0) {
				player.reset();
			}
		}
	}

	void fire_asrc() {
		if (not player) {
			return;
		}
        auto x = player->x();

        for (auto &s: ships) {
            if (s != player and s->x() - 40 < x and x < s->x() + 40) {
                r_target = s;
                small_explosion(c_x + random_uniform(-30, 30),
                		c_y + random_uniform(-30, 30), 0, 0);
            }
        }
        play_sound(sndLaser);
	}

	void fire_laser() {
        play_sound(sndLaser);
        visible_laser = true;
        auto x = player->x();
		for (auto &s: ships) {
            if (s != player and s->x() - 15 < x and x < s->x() + 15) {
                laser_y = s->y();
                s->damage(3);
                return;
            }
		}
        laser_y = 0;
	}

	void small_explosion(float x, float y, float vx, float vy) {
		for (int i = 0; i < 10; ++i) {
			create_shot(x, y, random_uniform(-2, 2) + vx, random_uniform(-2, 2) + vy,
					0, random_uniform(10, 20), true);
		}
		play_sound(sndCrash);
	}

	virtual std::weak_ptr<IShip> is_hit(float x, float y) {
		constexpr float ship_size2 = 16 * 16;
		for (auto &s: ships) {
			auto dx = s->x() - x;
			auto dy = s->y() - y;
			if (dx * dx + dy * dy < ship_size2) {
				return s;
			}
		}
		return weak_ptr<IShip>();
	}

	void render_scene() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        if (visible_laser) {
            draw_laser();
            visible_laser = false;
        }

        draw_stars();

        fast_display(planet.x, planet.y, 9, .3);

        for (auto &s: ships) {
            s->render();
        }

        for (auto &o: objects) {
            o->render();
        }

        if (asrc) {
            fast_display(c_x, c_y, 6);
        }

        planet.y += .4;
        if (planet.y > max_y * 1.1) {
        	planet.y = -max_y * .1;
        	planet.x = random_uniform(0, max_x);
        }

        SDL_RenderPresent(renderer);
	}

	void draw_stars() {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (auto &s: stars) {
			SDL_RenderDrawPoint(renderer, int(s.x), int(s.y));
			s.y += 1;
			if (s.y > max_y) {
				s.y = 0;
			}
		}
	}

	void draw_laser() {
		SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
		if (not player) {
			return;
		}
		else {
			SDL_RenderDrawLine(renderer, int(player->x()), int(player->y()), int(player->x()), int(laser_y));
		}
	}


	std::shared_ptr<class IShip> get_player() {
		return player;
	}

	virtual void fast_display(float x, float y, int num, float alpha = 1, bool center = true) override {
        auto dst_rect = image_rect;
        dst_rect.x = x;
        dst_rect.y = y;
        if (center) {
            dst_rect.x -= dst_rect.w / 2;
            dst_rect.y -= dst_rect.h / 2;
        }

        SDL_SetTextureAlphaMod(textures[num], 255 * alpha);
        SDL_RenderCopy(renderer, textures[num], &image_rect, &dst_rect);
	}

	virtual void explosion(float x, float y) override {

		for (int i = 0; i < 20; ++i) {
            create_shot(x, y, random_uniform(-10, 10), random_uniform(-10, 10),
            		0, random_uniform(10, 20), true);
		}

        play_sound(sndExp);
	}

	virtual float get_max_x() override {
		return max_x;
	}
	virtual float get_max_y() override {
		return max_y;
	}

	virtual void remove_ship(class IShip* s) override {
		ships_to_remove.push_back(s);
	}
	virtual void remove_shot(class IShot* s) override {
		shots_to_remove.push_back(s);
	}

    virtual void create_shot(float x, float y, float vx, float vy,
    		std::shared_ptr<IShip> target = 0, int range = 100, int from_explosion = false) override {

        auto shot = ::create_shot(this, x, y, vx, vy, target, range,  from_explosion);
        objects.push_front(shot);

    }


	float time_elapsed = 0;

	SDL_Window *window;
	SDL_Renderer *renderer;

	vector<SDL_Texture*> textures;

	SDL_Rect screen_rect;
	float max_y;
	float max_x;
	float laser_y = 0;
	float next_wave = 250;
	bool visible_laser = false;
	bool firing = false;
	bool firing_missile = false;
	float fire_rate = 0;
	float missile_rate = 0;
	bool shotgun = false;
	bool asrc = true;
	float c_x = 0;
	float c_y = 0;
	bool display_cursor = false;
	int fire_side = 1;
	SDL_Rect image_rect;

	Star planet = {100, 100};

	list<shared_ptr<IShip>> ships;
	list<shared_ptr<IShot>> objects;
	list<IShip *> ships_to_remove;
	list<IShot *> shots_to_remove;
	weak_ptr<IShip> r_target;
	shared_ptr<IShip> player;

	vector<Star> stars;

};




int main() {
	Screen screen;

	screen.run();
}
