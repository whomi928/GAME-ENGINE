#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

extern int tilesize;

//this is for level editor. this will be used to store the tile data and also to save and load the tile map. 
// this will be used in the editor and also in the game to render the tile map.
struct tile {
	int x;
	int y;
	float rotation;
	std::string type;
	bool solid;
	bool flip;
	int tilesize;
};

//this is for making the player, enemy and entities.
enum Entity_Type {
    PLAYER,
    ENEMY,
    TILES,
    PROJECTILE,
    ENTITY
};

struct Entity {
    int id;
    Entity_Type type;

    float x, y;
    float velocity_x, velocity_y;
    float acceleration_x, acceleration_y;
    float orientation;

    float mass;         // Mass = 0.0f makes it a static, unmovable tile/object, all though this will not be the case as tile are separate struct
    float restitution;
    float friction;

    // Visuals & Geometry
    std::string texture_key; // Pass this to your getTexture() system!
    int num_sides;           //If applicable 
    int size;

    // The actual raw geometry for your SAT collision code
    std::vector<std::pair<float, float>> vertices;

    bool is_active;
};

int editor(std::string location_of_txt, struct tile t);
int editor(std::string location_of_txt, struct Entity e);

void renderer_ghost(std::string current_type, float current_rotation, std::string location_of_tiles, SDL_Renderer* Renderer, bool flip, int Tilesize, bool mapped);
SDL_Texture* getTexture(std::string filepath, SDL_Renderer* Renderer);
void renderer(std::vector <tile> tiles, std::string location_of_tiles, SDL_Renderer* Renderer);
void renderer_entity(std::vector <Entity> entity, std::string location_of_tiles_for_entity, SDL_Renderer* Renderer);

std::vector <tile> loader(std::string location_of_txt);
std::vector <Entity> loader_entity(std::string location_of_txt_for_entity);

void saveMap(std::vector <tile>& tiles, std::string location_of_txt);
void saveMap(std::vector<Entity>& entity, std::string location_of_txt);

void size_reset();
void tilesize_adjuster(int t);
void rotation_reset(float& rotation);
void tile_deleter(int x1, int y1, int x2, int y2, std::vector<tile>& current_map);



struct Entity Entity_maker(Entity_Type type, float x, float y, float vel_x, float vel_y, float orientation,
    int id, float restitution, float friction, int size, int sides, int mass, std::string filepath);

// this is for physics. this will be used to make the physics for the player, enemy and entities.
void natural_force(std::vector<Entity>& entity, float dt);

//renderer functions or other stuff
void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
