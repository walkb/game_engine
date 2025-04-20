#include "Rigidbody.h"
#include "World.h"

void ContactListener::BeginContact(b2Contact* contact) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	Actor* a = reinterpret_cast<Actor*>(contact->GetFixtureA()->GetUserData().pointer);
	Actor* b = reinterpret_cast<Actor*>(contact->GetFixtureB()->GetUserData().pointer);

	Collision c;
	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);
	b2Vec2 relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();

	c.relative_velocity = relative_velocity;
	c.normal = world_manifold.normal;
	c.point = world_manifold.points[0];

	if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
		c.other = b;
		a->OnTriggerEnter(c);
		c.other = a;
		b->OnTriggerEnter(c);
	}
	else if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
		c.other = b;
		a->OnCollisionEnter(c);
		c.other = a;
		b->OnCollisionEnter(c);
	}
}

void ContactListener::EndContact(b2Contact* contact) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	Actor* a = reinterpret_cast<Actor*>(contact->GetFixtureA()->GetUserData().pointer);
	Actor* b = reinterpret_cast<Actor*>(contact->GetFixtureB()->GetUserData().pointer);

	Collision c;
	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);
	b2Vec2 relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();

	c.relative_velocity = relative_velocity;
	c.normal = world_manifold.normal;
	c.point = world_manifold.points[0];

	// TODO: Potentially remove and set these to the sentinel values.
	// ALSO: Check Actor::305

	if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
		c.other = b;
		a->OnTriggerExit(c);
		c.other = a;
		b->OnTriggerExit(c);
	}
	else if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
		c.other = b;
		a->OnCollisionExit(c);
		c.other = a;
		b->OnCollisionExit(c);
	}
}

void Rigidbody::OnDestroy() {
	World::world->DestroyBody(body);
}

void Rigidbody::OnStart() {
	// Check if world is created
	if (!World::Exists()) {
		World::Instantiate();

	}

	// create bodydef
	b2BodyDef body_def;
	if (body_type == "dynamic")
		body_def.type = b2_dynamicBody;
	else if (body_type == "kinematic")
		body_def.type = b2_kinematicBody;
	else if (body_type == "static")
		body_def.type = b2_staticBody;
	body_def.position = b2Vec2(x, y);
	body_def.bullet = precise;
	body_def.angularDamping = angular_friction;
	body_def.gravityScale = gravity_scale;
	body_def.angle = rotation * (b2_pi / 180.0f);

	body = World::world->CreateBody(&body_def);

	// Create phantom senesor to make bodies move without trigger or collider
	if (!has_collider && !has_trigger) {
		b2PolygonShape phantom_shape;
		//shape.SetAsBox(width * 0.5f, height * 0.5f);
		phantom_shape.SetAsBox(0.5f * width, 0.5f * height);

		b2FixtureDef fixture;
		fixture.shape = &phantom_shape;
		fixture.density = density;

		// Set category
		fixture.filter.categoryBits = BOX2D_CATEGORY_PHANTOM;
		// Categories allowed to collide with
		fixture.filter.maskBits = 0;

		// No collisions will occur as a sensor
		fixture.isSensor = true;
		//fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
		body->CreateFixture(&fixture);
	}
	else if (has_collider || has_trigger) {
		if (has_collider) {
			b2FixtureDef collider_fixture;
			collider_fixture.friction = friction;
			collider_fixture.restitution = bounciness;
			collider_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
			collider_fixture.isSensor = false;
			collider_fixture.density = density;
			// Set category
			collider_fixture.filter.categoryBits = BOX2D_CATEGORY_COLLIDER;
			// Categories allowed to collide with
			collider_fixture.filter.maskBits = BOX2D_CATEGORY_COLLIDER;
			if (collider_type == "box") {
				b2PolygonShape collider_shape;
				collider_shape.SetAsBox(0.5f * width, 0.5f * height);
				collider_fixture.shape = &collider_shape;
				body->CreateFixture(&collider_fixture);
			}
			else if (collider_type == "circle") {
				b2CircleShape collider_shape;
				collider_shape.m_radius = radius;
				collider_fixture.shape = &collider_shape;
				body->CreateFixture(&collider_fixture);
			}
		}
		if (has_trigger) {
			b2FixtureDef trigger_fixture;
			trigger_fixture.friction = friction;
			trigger_fixture.restitution = bounciness;
			trigger_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
			trigger_fixture.isSensor = true;
			trigger_fixture.density = density;
			// Set category
			trigger_fixture.filter.categoryBits = BOX2D_CATEGORY_TRIGGER;
			// Categories allowed to collide with
			trigger_fixture.filter.maskBits = BOX2D_CATEGORY_TRIGGER;
			if (trigger_type == "box") {
				b2PolygonShape trigger_shape;
				trigger_shape.SetAsBox(0.5f * trigger_width, 0.5f * trigger_height);
				trigger_fixture.shape = &trigger_shape;
				body->CreateFixture(&trigger_fixture);
			}
			else if (trigger_type == "circle") {
				b2CircleShape trigger_shape;
				trigger_shape.m_radius = trigger_radius;
				trigger_fixture.shape = &trigger_shape;
				body->CreateFixture(&trigger_fixture);
			}
		}
	}
}


// ---- [ LUA FUNCTIONS ] ----

b2Vec2 Rigidbody::GetPosition() {
	if (body == nullptr) {
		return b2Vec2(x, y);
	}
	return body->GetPosition();
}

// Convert to degrees
float Rigidbody::GetRotation() {
	if (body == nullptr) {
		return rotation;
	}
	return body->GetAngle() * (180.0f / b2_pi);
}

void Rigidbody::AddForce(b2Vec2 force) {
	body->ApplyForceToCenter(force, true);
}

void Rigidbody::SetVelocity(b2Vec2 velocity) {
	body->SetLinearVelocity(velocity);
}

void Rigidbody::SetPosition(b2Vec2 position) {
	if (body == nullptr) {
		x = position.x;
		y = position.y;
		return;
	}
	body->SetTransform(position, body->GetAngle());
}

void Rigidbody::SetRotation(float degrees_clockwise) {
	if (body == nullptr) {
		rotation = degrees_clockwise;
		return;
	}
	float radians = degrees_clockwise * (b2_pi / 180.0f);
	body->SetTransform(body->GetPosition(), radians);
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise) {
	float radians = degrees_clockwise * (b2_pi / 180.0f);
	body->SetAngularVelocity(radians);
}

void Rigidbody::SetGravityScale(float scale) {
	body->SetGravityScale(scale);
}

void Rigidbody::SetUpDirection(b2Vec2 direction) {
	direction.Normalize();
	float angle = glm::atan(direction.x, -direction.y);
	body->SetTransform(body->GetPosition(), angle);
}

void Rigidbody::SetRightDirection(b2Vec2 direction) {
	direction.Normalize();
	float angle = glm::atan(direction.x, -direction.y) - (b2_pi / 2.0f);
	body->SetTransform(body->GetPosition(), angle);
}

b2Vec2 Rigidbody::GetVelocity() {
	return body->GetLinearVelocity();
}

float Rigidbody::GetAngularVelocity() {
	return body->GetAngularVelocity() * (180.0f / b2_pi);
}

float Rigidbody::GetGravityScale() {
	return body->GetGravityScale();
}

b2Vec2 Rigidbody::GetUpDirection() {
	float angle = body->GetAngle();
	b2Vec2 result = b2Vec2(sin(angle), -cos(angle));
	result.Normalize();
	return result;
}

b2Vec2 Rigidbody::GetRightDirection() {
	float angle = body->GetAngle();
	b2Vec2 result = b2Vec2(cos(angle), sin(angle));
	result.Normalize();
	return result;
}