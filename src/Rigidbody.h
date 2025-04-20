#pragma once
#include <string>
#include "box2d/box2d.h"
#include "Component.h"
#include "Actor.h"

class Actor;


// --- [ Collision Class ] --
class ContactListener : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
};

class Collision {
public:
	Actor* other;
	b2Vec2 point;
	b2Vec2 relative_velocity;
	b2Vec2 normal;
};

enum _entityCategory {
	BOX2D_CATEGORY_COLLIDER = 0x0001,
	BOX2D_CATEGORY_TRIGGER = 0x0002,
	BOX2D_CATEGORY_PHANTOM = 0x0004
};

class Rigidbody
{
public:
	// Used to determine whether or not functions should fire in the frame
	bool enabled = true;

	// Helper reference to owner
	Actor* actor = nullptr;

	// Component variables
	std::string key = "???";
	std::string type = "Rigidbody";

	// Initial rigidbody variables
	float x = 0.0f; // only used for creation, must later use body->GetPosition().x
	float y = 0.0f; // only used for creation, must later use body->GetPosition().y
	std::string body_type = "dynamic"; // b2_dynamicBody, static, or kinematic with b2BodyDef.type
	bool precise = true; // b2BodyDef.bullet
	float gravity_scale = 1.0f; // b2BodyDef.gravityScale
	float density = 1.0f; // ...
	float angular_friction = 0.3f; // use b2BodyDef.angularDamping
	float rotation = 0.0f; // degrees, not radians, must convert to radians for box2d

	bool has_collider = true;
	std::string collider_type = "box";
	float width = 1.0f;
	float height = 1.0f;
	float radius = 0.5f;
	float friction = 0.3f;
	float bounciness = 0.3f;

	bool has_trigger = true;
	std::string trigger_type = "box";
	float trigger_width = 1.0f;
	float trigger_height = 1.0f;
	float trigger_radius = 0.5f;

	// --- [ LUA FUNCTIONS ] ---
	
	// Returns Vector2 of position
	b2Vec2 GetPosition();

	// Returns angle in degrees
	float GetRotation();

	// Adds force to the center of the body
	void AddForce(b2Vec2 force);

	// Sets linear velocity of the body
	void SetVelocity(b2Vec2 velocity);

	// Sets position of body
	void SetPosition(b2Vec2 position);

	// Set rotation in degrees clockwise of body
	void SetRotation(float degrees_clockwise);

	// Set angular velocity in degrees clockwise of body
	void SetAngularVelocity(float degrees_clockwise);

	// Set gravity scale of body
	void SetGravityScale(float scale);

	// Normalize direction vector and set up direction
	void SetUpDirection(b2Vec2 direction);

	// Normalize direction vector and set local "right" east-pointing vector
	void SetRightDirection(b2Vec2 direction);

	b2Vec2 GetVelocity();

	float GetAngularVelocity();

	float GetGravityScale();

	b2Vec2 GetUpDirection();

	b2Vec2 GetRightDirection();

	// --- [ LIFE CYCLE FUNC ] ---

	void OnStart();

	void OnDestroy();

	// --- [ Box2D BODY ] ---
	b2Body* body;


};

