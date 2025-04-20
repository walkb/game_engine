#pragma once
#include "box2d/box2d.h"
#include "Rigidbody.h"
#include <vector>

// --- [ RAYCASTING ] ---
struct HitResult {
	Actor* actor = nullptr;
	b2Vec2 point;
	b2Vec2 normal;
	bool is_trigger;
	float fraction;
};

class RaycastCallback : public b2RayCastCallback {
public:


	std::vector<HitResult> hits;


	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
};

class World
{
public:
	// ---- [ DECON ] ----
	~World();

	// ---- [ FUNCS ] ----

	// Returns whether the world has been instantiated
	static bool Exists();
	
	// Creates a new world with either default gravity or args gravity
	static void Instantiate(float _x = 0.0f, float _y = 9.8f);

	// Returns a pointer to the world, which could be nullptr
	static b2World* Get();

	// If a world exists, calls step on the object with default parameters (1.0f / 60.0f, 8, 3)
	static void Step();

	// --- [ LUA FUNCS ] ---

	static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist);

	static luabridge::LuaRef RayCastAll(b2Vec2 pos, b2Vec2 dir, float dist);

	// ---- [ CONFIG ] ----
	inline static b2Vec2 gravity;
	inline static b2World* world;
	inline static ContactListener* listener;
};

