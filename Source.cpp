#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Header.h"
#include <string>
#include <vector>

using namespace std;
int tilesize = 32;

int main(int argc, char* argv[])
{
    int origin_x = 800;
    int origin_y = 640;
    int tile_id = 1;
    int max_tile = 7;
    int entity_id = tile_id;
    int max_entity = max_tile;
    float rotation = 0;
    bool flip = 0;
	int startX = 0, startY = 0;
	int endX = 0, endY = 0;
    bool mapped = 1;

    //for now gravity is here
    float gravity = 9.8;
    float dt = 0.1;

    enum Entity_Type type_entity = PLAYER;

    bool editing = 1;

	string location_of_txt = "C:\\Users\\shaur\\Desktop\\Project\\Physics eng\\LIVEOUT\\tilemap.txt";
	string location_of_tiles = "C:\\Users\\shaur\\Desktop\\Project\\Physics eng\\LIVEOUT\\tiles\\";
    string location_of_txt_for_entity ="C:\\Users\\shaur\\Desktop\\Project\\Physics eng\\LIVEOUT\\entity.txt";

    SDL_Window* window = SDL_CreateWindow(
        "LIVEOUT",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        origin_x,
        origin_y,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL) {
        cout << "Failed to create window: " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Event e;
    bool quit = false;
    vector<tile> current_map = loader(location_of_txt);
    vector<Entity> current_loader = loader_entity(location_of_txt_for_entity);

    vector<Entity> current_loader_copy = current_loader;

    for (Entity& e : current_loader) {
        e.acceleration_y = gravity; 
    }

    //struct Entity player=Entity_maker(PLAYER, 350, 350, 0, 0, 1, 4, 1, 0, 10, 4, 0, location_of_tiles);
    //editor(location_of_txt, { 350, 350, 0, to_string(4), 1, flip, tilesize });
    //current_map.push_back({ 350, 350, 0, to_string(4), 1, flip, tilesize });

    if (TTF_Init() == -1) {
        cout << "Failed to initialize TTF: " << TTF_GetError() << endl;
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 16);
    SDL_Color textColor = { 255, 255, 255, 255 };

    string type[] = {"TILE", "ENTITY"};
    string editing_state[] = {"Playing", "Editing" };
    bool type_num = 0;

    // 1. Define your rigid physics timestep (60 Hz)
    const float PHYSICS_DT = 1.0f / 60.0f;

    // 2. Setup your time tracking variables before the main loop
    Uint64 currentTime = SDL_GetPerformanceCounter();
    float accumulator = 0.0f;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
				//Editor will be called here with the tile data and add if nothing is there 
                //and delete if there is already  tile.
                //will add the tile for type_num = 0 and entity for type_num = 1
                if (type_num == 0) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);

                        int snapX = mapped ? ((mouseX / tilesize) * tilesize) : mouseX;
                        int snapY = mapped ? ((mouseY / tilesize) * tilesize) : mouseY;

                        auto it = std::find_if(current_map.begin(), current_map.end(),
                            [snapX, snapY, tile_id](const tile& t) {
                                return (t.x == snapX && t.y == snapY && t.type == to_string(tile_id));
                            }
                        );
                        if (it != current_map.end()) {
                            // 1. Swap the tile we want to delete with the very last tile
                            *it = current_map.back();

                            // 2. Delete the last tile (which is now a duplicate)
                            current_map.pop_back();
                        }
                        else {
                            editor(location_of_txt, { snapX, snapY, rotation, to_string(tile_id), 1, flip, tilesize });
                            current_map.push_back({ snapX, snapY, rotation, to_string(tile_id), 1, flip, tilesize });
                        }
                    }
                    else if (e.button.button == SDL_BUTTON_RIGHT) {
                        SDL_GetMouseState(&startX, &startY);
                    }
                }
                else {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);

                        int snapX = mapped ? ((mouseX / tilesize) * tilesize) : mouseX;
                        int snapY = mapped ? ((mouseY / tilesize) * tilesize) : mouseY;

                        auto it = std::find_if(current_loader.begin(), current_loader.end(),
                            [snapX, snapY, entity_id](const Entity& e) {
                                return (e.x == snapX && e.y == snapY && e.id == (entity_id));
                            }
                        );
                        if (it != current_loader.end()) {
                            size_t index = std::distance(current_loader.begin(), it);

                            // Erase from both arrays at the same position to keep them perfectly in sync
                            current_loader.erase(current_loader.begin() + index);
                            current_loader_copy.erase(current_loader_copy.begin() + index);
                        }
                        else {//re write these for entity
                            if (type_entity == PLAYER) {
                                editor(location_of_txt_for_entity, { entity_id, PLAYER, (float)snapX, (float)snapY, 0, 0, 0, 0, 0, 10, 1, 0,
                                location_of_tiles + to_string(entity_id) + ".png", 4, tilesize,
                                {{snapX, snapY},{snapX + tilesize, snapY},{snapX, snapY + tilesize},{snapX + tilesize, snapY + tilesize}}, 1 });
                             
                                current_loader.push_back({ entity_id, PLAYER, (float)snapX, (float)snapY, 0, 0, 0, 0, 0, 10, 1, 0,
                                location_of_tiles + to_string(entity_id) + ".png", 4, tilesize,
                                { {snapX, snapY},{snapX + tilesize, snapY},{snapX, snapY + tilesize},{snapX + tilesize, snapY + tilesize} }, 1 });

                                current_loader_copy.push_back({ entity_id, PLAYER, (float)snapX, (float)snapY, 0, 0, 0, 0, 0, 10, 1, 0,
                                location_of_tiles + to_string(entity_id) + ".png", 4, tilesize,
                                { {snapX, snapY},{snapX + tilesize, snapY},{snapX, snapY + tilesize},{snapX + tilesize, snapY + tilesize} }, 1 });
                            }
                            else if (type_entity == ENEMY) {
                                editor(location_of_txt_for_entity, { entity_id, ENEMY, (float)snapX, (float)snapY, 0, 0, 0, 0, 0, 10, 1, 0,
                                location_of_tiles + to_string(entity_id) + ".png", 4, tilesize,
                                {{snapX, snapY},{snapX + tilesize, snapY},{snapX, snapY + tilesize},{snapX + tilesize, snapY + tilesize}}, 1 });

                                current_loader.push_back({ entity_id, ENEMY, (float)snapX, (float)snapY, 0, 0, 0, 0, 0, 10, 1, 0,
                                location_of_tiles + to_string(entity_id) + ".png", 4, tilesize,
                                { {snapX, snapY},{snapX + tilesize, snapY},{snapX, snapY + tilesize},{snapX + tilesize, snapY + tilesize} }, 1 });

                                current_loader_copy.push_back({ entity_id, ENEMY, (float)snapX, (float)snapY, 0, 0, 0, 0, 0, 10, 1, 0,
                                location_of_tiles + to_string(entity_id) + ".png", 4, tilesize,
                                { {snapX, snapY},{snapX + tilesize, snapY},{snapX, snapY + tilesize},{snapX + tilesize, snapY + tilesize} }, 1 });
                            }
                        }
                    }
                    else if (e.button.button == SDL_BUTTON_RIGHT) {
                        SDL_GetMouseState(&startX, &startY);
                    }
                }
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.button == SDL_BUTTON_RIGHT) {
                    SDL_GetMouseState(&endX, &endY); 
					tile_deleter(startX, startY, endX, endY, current_map);
                }
            }
			if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RIGHT && tile_id < max_tile && !editing) {
                    move_the_player(current_loader, 1, 1);
                }
                else if (e.key.keysym.sym == SDLK_LEFT && tile_id > 1 && !editing) {
                    move_the_player(current_loader, 1, -1);
                }
                if (e.key.keysym.sym == SDLK_RIGHT && tile_id < max_tile && editing) {
                    tile_id++;
                }
                else if (e.key.keysym.sym == SDLK_LEFT && tile_id > 1 && editing) {
                    tile_id--;
                }
                else if (e.key.keysym.sym == SDLK_m && editing) {
                    mapped = !mapped;
                }
                else if (e.key.keysym.sym == SDLK_r && editing) {
                    //rotation = (int)(rotation + 0.1) % 4 + (rotation - (int)rotation);
                    rotation = fmodf(rotation + 0.1f, 4.0f);
                }
                else if (e.key.keysym.sym == SDLK_f && editing) {
                    flip = !flip;
                }
                else if (e.key.keysym.sym == SDLK_t && editing) {
                    size_reset();
                }
                else if (e.key.keysym.sym == SDLK_e) {
                    editing = !editing;
                }
				else if (e.key.keysym.sym == SDLK_SPACE && editing) {
					rotation_reset(rotation);
                }
                else if (e.key.keysym.sym == SDLK_c && editing) {
                    type_num = !type_num;
                }
                else if (e.key.keysym.sym == SDLK_UP && !editing) {
                    move_the_player(current_loader, 2, -1);
                }
                else if (e.key.keysym.sym == SDLK_DOWN && !editing) {
                    move_the_player(current_loader, 2, 1);
                }
                else if (e.key.keysym.sym == SDLK_UP && editing) {
                    type_entity = ENEMY;
                }
                else if (e.key.keysym.sym == SDLK_DOWN && editing) {
                    type_entity = PLAYER;
                }
            }
            if (e.type == SDL_MOUSEWHEEL) {
                tilesize_adjuster(e.wheel.y);
            }
        }

        // 3. Measure real time passed since the last frame
        Uint64 newTime = SDL_GetPerformanceCounter();
        float frameTime = (newTime - currentTime) / (float)SDL_GetPerformanceFrequency();
        currentTime = newTime;

        // 4. Prevent the "Spiral of Death"
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        // 5. Pour the real time into the accumulator
        accumulator += frameTime;

        // 6. Drain the accumulator in fixed chunks
        while (accumulator >= PHYSICS_DT) {
            // Run forces and kinematics using the fixed DT
            natural_force(current_loader, PHYSICS_DT);

            // Run your collision relaxation loop
            for (int iteration = 0; iteration < 4; iteration++) {
                collision_detection_entity_tiles(current_loader, current_map);
                collision_detection_entity_entity(current_loader);
            }

            // Subtract the fixed chunk from the bucket
            accumulator -= PHYSICS_DT;
        }

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        if ((currentKeyStates[SDL_SCANCODE_RSHIFT] || currentKeyStates[SDL_SCANCODE_LSHIFT]) && currentKeyStates[SDL_SCANCODE_R]) {
            rotation = (int)(rotation + 1) % 4 + (rotation - (int)rotation);
            SDL_Delay(100);
        }

        SDL_SetRenderDrawColor(Renderer, 40, 40, 40, 255);
        SDL_RenderClear(Renderer);

		renderer(current_map, location_of_tiles, Renderer);
        renderer_entity(current_loader, location_of_tiles, Renderer);
        if (editing) {
            renderer_ghost(to_string(tile_id), rotation, location_of_tiles, Renderer, flip, tilesize, mapped);
        }

        if (font) {
            int x, y;
            SDL_GetWindowSize(window, &x, &y);
            if(editing) {RenderText(Renderer, font, type[type_num], x - 80, 15, textColor);}
            RenderText(Renderer, font, editing_state[editing], x - 80, 38, textColor);
        }

        SDL_RenderPresent(Renderer);
        SDL_Delay(16);
    }
    saveMap(current_map, location_of_txt);
    saveMap(current_loader_copy, location_of_txt_for_entity);

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(window);
    if (font) TTF_CloseFont(font);
    TTF_Quit();

    return 0;
    
}