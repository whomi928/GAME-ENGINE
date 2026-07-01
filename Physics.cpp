#include "Header.h"
#include <algorithm>
#include <vector>

using namespace std;

void natural_force(vector<Entity> &entity, float dt) {
	for (Entity &e : entity) {
		e.velocity_y -= e.acceleration_y * dt;
		e.y -= e.velocity_y * dt;
	}
}