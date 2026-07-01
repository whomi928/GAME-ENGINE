#include "Header.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

map<string, SDL_Texture*> textureCache;

//tile editor. Only used while making the tile map. only txt needed
//this will be called when the mouse is clicked and the tile is placed.
int editor(string location_of_txt, struct tile t) {//this should have the tile data
	std::ofstream file(location_of_txt, std::ios::app);
	if (file.is_open()) {
		file << t.x << " " << t.y << " " << t.rotation << " " << t.type << " " << t.solid << " " << t.flip << " " << t.tilesize<<endl;
		file.close();
	}
	else {
		cout << "ERROR: Could not open file for writing." << endl;
	}
	return 0;
}

int editor(string location_of_txt, struct Entity e) {//this should have the entity data
	std::ofstream file(location_of_txt, std::ios::app);
	if (file.is_open()) {
		file << e.x << " " << e.y << " " << e.orientation << " " << e.mass << " " << e.num_sides << " " << e.size << " " << e.friction << " " << e.restitution << " " << e.id << endl;
		file.close();
	}
	else {
		cout << "ERROR: Could not open file for writing." << endl;
	}
	return 0;
}

SDL_Texture* getTexture(string filepath, SDL_Renderer* Renderer) {
	// If the texture isn't in our cache yet, load it
	if (textureCache.find(filepath) == textureCache.end()) {
		SDL_Surface* surface = IMG_Load(filepath.c_str());
		if (!surface) {
			cout << "ERROR: Failed to load image " << filepath << ". SDL_image Error: " << IMG_GetError() << endl;
			return nullptr;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer, surface);
		SDL_FreeSurface(surface);

		if (!texture) {
			cout << "ERROR: Failed to create texture from " << filepath << ". SDL Error: " << SDL_GetError() << endl;
			return nullptr;
		}
		// Save to cache
		textureCache[filepath] = texture;
	}
	// Return the cached texture
	return textureCache[filepath];
}

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
	if (!surface) return;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect dest = { x, y, surface->w, surface->h };

	SDL_RenderCopy(renderer, texture, NULL, &dest);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

//map renderer. txt and files location both are needed. well the txt is not needed instead the 
// vector is given but still let it be there.
//will alway be called in the program to show the ghost tiles near the the mouse cursor
void renderer_ghost(string current_type, float current_rotation, string location_of_tiles, SDL_Renderer* Renderer, bool flip, int Tilesize, bool mapped) {

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	int snapX = mapped ? ((mouseX / Tilesize) * Tilesize) : mouseX;
	int snapY = mapped ? ((mouseY / Tilesize) * Tilesize) : mouseY;

	string fullPath = location_of_tiles + current_type + ".png";
	SDL_Texture* tex = getTexture(fullPath, Renderer);

	if (tex != nullptr) {
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

		SDL_SetTextureAlphaMod(tex, 128);

		SDL_Rect destRect;
		destRect.x = snapX;
		destRect.y = snapY;
		destRect.w = Tilesize;
		destRect.h = Tilesize;

		SDL_RenderCopyEx(
			Renderer,
			tex,
			NULL,
			&destRect,
			current_rotation * 90,
			NULL,
			flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
		);

		SDL_SetTextureAlphaMod(tex, 255);
	}
}

//map renderer for the solid tiles
//will be called always to show the tile map
void renderer(vector <tile> tiles, string location_of_tiles, SDL_Renderer* Renderer) {
	for (auto t : tiles) {
		//render the tile at t.x, t.y with rotation t.rotation and type t.type
		string fullPath = location_of_tiles + t.type + ".png";
		SDL_Texture* tex = getTexture(fullPath, Renderer);

		if (tex != nullptr) {

			SDL_Rect destRect;
			destRect.x = t.x;
			destRect.y = t.y;
			destRect.w = t.tilesize;
			destRect.h = t.tilesize;

			SDL_RenderCopyEx(
				Renderer,
				tex,
				NULL,            // Source rect (NULL = draw the whole image)
				&destRect,       
				t.rotation * 90, 
				NULL,            // Center of rotation (NULL = center of the tile)
				t.flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE  // Flip state (none/mirrored)
			);
		}
	}
}

void renderer_entity(vector <Entity> entity, string location_of_tiles, SDL_Renderer* Renderer) {
	for (auto e : entity) {
		//render the tile at t.x, t.y with rotation t.rotation and type t.type
		string fullPath = location_of_tiles + to_string(e.id) + ".png";
		SDL_Texture* tex = getTexture(fullPath, Renderer);

		if (tex != nullptr) {

			SDL_Rect destRect;
			destRect.x = e.x;
			destRect.y = e.y;
			destRect.w = e.size;
			destRect.h = e.size;

			SDL_RenderCopyEx(
				Renderer,
				tex,
				NULL,            // Source rect (NULL = draw the whole image)
				&destRect,
				e.orientation * 90,
				NULL,            // Center of rotation (NULL = center of the tile)
				SDL_FLIP_NONE   // Flip state (none) for now
			);
		}
	}
}

//map loader only txt need
vector<tile> loader(string location_of_txt) {
	vector<tile> tiles;
	std::ifstream file(location_of_txt);

	if (!file.is_open()) {
		cout << "ERROR: Could not open file for reading." << endl;
		return tiles;
	}
	
	tile t;
	while (file >> t.x >> t.y >> t.rotation >> t.type >> t.solid >> t.flip >> t.tilesize) {
		tiles.push_back(t);
	}
	file.close();
	cout << "Tile map loaded successfully." << endl;
	return tiles;
}
//entity loader
vector<Entity> loader_entity(string location_of_txt_for_entity) {
	vector<Entity> entity;
	std::ifstream file(location_of_txt_for_entity);

	if (!file.is_open()) {
		cout << "ERROR: Could not open file for reading." << endl;
		return entity;
	}

	Entity e;
	while (file >> e.x >> e.y >> e.orientation >> e.mass >> e.num_sides >> e.size >> e.friction >> e.restitution >> e.id) {
		e.velocity_x = 0;
		e.velocity_y = 0;
		e.acceleration_x = 0;
		e.acceleration_y = 0;
		entity.push_back(e);
	}
	file.close();
	cout << "Entity map loaded successfully." << endl;
	return entity;
}

//saves map when some change is made and the tiles are also deleted
//called only once when terminationn takes place
void saveMap(vector<tile> &tile, string location_of_txt) {
	std::ofstream file(location_of_txt, std::ios::trunc);

	if (file.is_open()) {
		for (auto t : tile) {
			file << t.x << " " << t.y << " " << t.rotation << " " << t.type << " " << t.solid << " " << t.flip <<" " << t.tilesize << "\n";
		}
		cout << "file saved successfully \n";
	}
	else {
		cout << "Some error ocurred in saving" << endl;
	}
}
//for the entity
void saveMap(vector<Entity>& entity, string location_of_txt) {
	std::ofstream file(location_of_txt, std::ios::trunc);

	if (file.is_open()) {
		for (auto e : entity) {
			file << e.x << " " << e.y << " " << e.orientation << " " << e.mass << " " << e.num_sides << " " << e.size << " " << e.friction << " " << e.restitution << " " << e.id << endl;
		}
		cout << "file saved successfully \n";
	}
	else {
		cout << "Some error ocurred in saving" << endl;
	}
}

void tilesize_adjuster(int t) {
	t>0 ? (tilesize < 320? tilesize += 5 : tilesize = 320):( tilesize > 10 ?tilesize -=5 : tilesize = 10);
}
void size_reset() {
	tilesize = 32;
}
void rotation_reset(float &rotation) {
	rotation = 0;
}
void tile_deleter(int x1, int y1, int x2, int y2, vector<tile>& current_map) {

	int rightEdge = max(x1, x2);
	int leftEdge = min(x1, x2);

	int bottomEdge = max(y1, y2);
	int topEdge = min(y1, y2);

	// Iterate through your vector of tiles and remove any that fall within the defined rectangle
	std::erase_if(current_map, [leftEdge, rightEdge, topEdge, bottomEdge](const tile& t) { 
		return (t.x >= leftEdge && t.x <= rightEdge &&
			t.y >= topEdge && t.y <= bottomEdge);
		});
	
}