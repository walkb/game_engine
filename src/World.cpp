#include "World.h"
#include "ComponentManager.h"
#include <algorithm>
#include "Helper.h"

// [RAYCAST]

float RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {

	// Ignore phantom fixtures (have no actor)
	Actor* a = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
	if (a == nullptr) return -1.0f;


	HitResult result;
	result.actor = a;
	result.is_trigger = fixture->IsSensor();
	result.normal = normal;
	result.point = point;
	result.fraction = fraction;

	hits.push_back(result);

	return 1.0f;
}



// [WORLD}

luabridge::LuaRef World::Raycast(b2Vec2 pos, b2Vec2 dir, float dist) {
	if (!Exists() || dist <= 0) {
		return luabridge::LuaRef(ComponentManager::lua_state);
	}



	RaycastCallback callback;
	b2Vec2 end = pos + (dist * dir);
	world->RayCast(&callback, pos, end);


	if (callback.hits.size() == 0) {
		return luabridge::LuaRef(ComponentManager::lua_state);
	}

	std::sort(callback.hits.begin(), callback.hits.end(), 
		[](const auto& a, const auto& b) { return a.fraction < b.fraction; }
	);

	// Return the closest
	HitResult& closest = callback.hits.front();
	luabridge::LuaRef result = luabridge::LuaRef(ComponentManager::lua_state, closest);

	return result;
}

luabridge::LuaRef World::RayCastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
	if (!Exists() || dist <= 0) {
		return luabridge::LuaRef(ComponentManager::lua_state);
	}

	RaycastCallback callback;
	b2Vec2 end = pos + (dist * dir);
	world->RayCast(&callback, pos, end);

	std::sort(callback.hits.begin(), callback.hits.end(), 
		[](const auto& a, const auto& b) { return a.fraction < b.fraction; }
	);


	// Return all hits
	luabridge::LuaRef result = luabridge::LuaRef::newTable(ComponentManager::lua_state);

	for (int i = 0; i < callback.hits.size(); i++) {
		result[i + 1] = callback.hits[i];
	}

	return result;
}



World::~World() {
	if (Exists()) {
		delete listener;
		delete world;
	}
}

bool World::Exists() {
	if (world == nullptr) {
		return false;
	}
	return true;
}

void World::Instantiate(float _x, float _y) {
	// Prevent a duplicate call to Instantiate
	if (Exists()) {
		return;
	}
	// Otherwise, make a new world and set a listener
	gravity.x = _x;
	gravity.y = _y;
	world = new b2World(gravity);
	listener = new ContactListener();
	world->SetContactListener(listener);
}

b2World* World::Get() {
	return world;
}

void World::Step() {
	// Ignore step calls if we don't have an instantiated world
	if (!Exists()) return;

	world->Step(1.0f / 60.0f, 8, 3);
}