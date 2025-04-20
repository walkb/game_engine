#include "ComponentManager.h"
#include "ComponentDB.h"
#include "Actor.h"
#include "SceneDB.h"
#include "Input.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "Renderer.h"
#include "Rigidbody.h"
#include "box2d/box2d.h"
#include "World.h"
#include "EventBus.h"
#include "ParticleSystem.h"


void ComponentManager::Initialize() {
	InitializeState();
}

bool ComponentManager::CompareComponents(const std::shared_ptr<Component> a, const std::shared_ptr<Component> b) {
	return a->key < b->key;
}

Component ComponentManager::MakeInstance(std::string type, std::string name) {
	if (ComponentDB::CheckComponentExists(type)) {
		Component c = Component(type);
		c.componentRef = std::make_shared<luabridge::LuaRef>(luabridge::newTable(lua_state));
		ComponentDB::EstablishInheritance(c, ComponentDB::globalComponents[type]);
		c.key = name;
		c.type = type;
		(*c.componentRef)["key"] = c.key;
		return c;
	}
	else if (type == "Rigidbody" || type == "ParticleSystem") {
		Component c = Component(type);
		c.key = name;
		c.type = type;
		return c;
	}
	else {
		std::cout << "error: failed to locate component " << type;
		exit(0);
	}
}

//Component ComponentManager::CreateRigidbody() {
//	Component c = Component("Rigidbody");
//}

void ComponentManager::InitializeState() {
	lua_state = luaL_newstate();

	luaL_openlibs(lua_state);

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", ComponentManager::CppLog)
		.endNamespace();

	// --- [ HITRESULT CLASS ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<HitResult>("HitResult")
		.addData("actor", &HitResult::actor)
		.addData("point", &HitResult::point)
		.addData("normal", &HitResult::normal)
		.addData("is_trigger", &HitResult::is_trigger)
		.endClass();

	// --- [ PHYSICS FUNCTIONS ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", &World::Raycast)
		.addFunction("RaycastAll", &World::RayCastAll)
		.endNamespace();

	// --- [ EVENTBUS ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Event")
		.addFunction("Subscribe", &EventBus::Subscribe)
		.addFunction("Unsubscribe", &EventBus::Unsubscribe)
		.addFunction("Publish", &EventBus::Publish)
		.endNamespace();

	// --- [ RIGIDBODY CLASS ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Rigidbody>("Rigidbody")
		.addData("enabled", &Rigidbody::enabled)
		.addData("key", &Rigidbody::key)
		.addData("type", &Rigidbody::type)
		.addData("actor", &Rigidbody::actor)
		.addData("x", &Rigidbody::x)
		.addData("y", &Rigidbody::y)
		.addData("rotation", &Rigidbody::rotation)
		.addData("gravity_scale", &Rigidbody::gravity_scale)
		.addFunction("GetPosition", &Rigidbody::GetPosition)
		.addFunction("GetRotation", &Rigidbody::GetRotation)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.endClass();

	// --- [ PARTICLE SYSTEM CLASS ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<ParticleSystem>("ParticleSystem")
		.addData("enabled", &ParticleSystem::enabled)
		.addData("key", &ParticleSystem::key)
		.addData("type", &ParticleSystem::type)
		.addData("actor", &ParticleSystem::actor)
		.addData("x", &ParticleSystem::x)
		.addData("y", &ParticleSystem::y)
		.addData("start_color_r", &ParticleSystem::start_color_r)
		.addData("start_color_g", &ParticleSystem::start_color_g)
		.addData("start_color_b", &ParticleSystem::start_color_b)
		.addData("start_color_a", &ParticleSystem::start_color_a)
		.addData("end_color_r", &ParticleSystem::end_color_r)
		.addData("end_color_g", &ParticleSystem::end_color_g)
		.addData("end_color_b", &ParticleSystem::end_color_b)
		.addData("end_color_a", &ParticleSystem::end_color_a)
		.addFunction("Stop", &ParticleSystem::Stop)
		.addFunction("Play", &ParticleSystem::Play)
		.addFunction("Burst", &ParticleSystem::LuaBurst)
		.endClass();


	// --- [ COLLISION CLASS ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Collision>("Collision")
		.addData("normal", &Collision::normal)
		.addData("other", &Collision::other)
		.addData("point", &Collision::point)
		.addData("relative_velocity", &Collision::relative_velocity)
		.endClass();

	// --- [ ACTOR CLASS ] ---

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass();

	// --- [ ACTOR NAMESPACE ] ---

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Actor")
		.addFunction("Find", &SceneDB::Find)
		.addFunction("FindAll", &SceneDB::FindAll)
		.addFunction("Instantiate", &SceneDB::Instantiate)
		.addFunction("Destroy", &SceneDB::Destroy)
		.endNamespace();

	// --- [ APPLICATION NAMESPACE ] ---

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", &SceneDB::Quit)
		.addFunction("Sleep", &SceneDB::Sleep)
		.addFunction("GetFrame", &SceneDB::GetFrame)
		.addFunction("OpenURL", &SceneDB::OpenURL)
		.endNamespace();

	// --- [ glm::VEC2 ] ---

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();

	// --- [ box2d::b2Vec2 AKA Vector ] ---

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addProperty("x", &b2Vec2::x)
		.addProperty("y", &b2Vec2::y)
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.addStaticFunction("Distance", &b2Distance)
		.addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endClass();


	// --- [ INPUT NAMESPACE ] ---

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Input")
		.addFunction("GetKey", &Input::GetKey)
		.addFunction("GetKeyDown", &Input::GetKeyDown)
		.addFunction("GetKeyUp", &Input::GetKeyUp)
		.addFunction("GetMousePosition", &Input::GetMousePosition)
		.addFunction("GetMouseButton", &Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
		.addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
		.addFunction("HideCursor", &Input::HideCursor)
		.addFunction("ShowCursor", &Input::ShowCursor)
		.endNamespace();

	// --- [ TEXT ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", &TextDB::Draw)
		.endNamespace();

	// --- [ AUDIO ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", &AudioDB::Play)
		.addFunction("Halt", &AudioDB::Halt)
		.addFunction("SetVolume", &AudioDB::SetVolume)
		.endNamespace();

	// --- [ IMAGE ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Image")
		.addFunction("DrawUI", &Renderer::DrawUI)
		.addFunction("DrawUIEx", &Renderer::DrawUIEx)
		.addFunction("Draw", &Renderer::Draw)
		.addFunction("DrawEx", &Renderer::DrawEx)
		.addFunction("DrawPixel", &Renderer::DrawPixel)
		.endNamespace();

	// --- [ CAM ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", &Renderer::SetPosition)
		.addFunction("GetPositionX", &Renderer::GetPositionX)
		.addFunction("GetPositionY", &Renderer::GetPositionY)
		.addFunction("SetZoom", &Renderer::SetZoom)
		.addFunction("GetZoom", &Renderer::GetZoom)
		.endNamespace();

	// --- [ SCENE ] ---
	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", &SceneDB::Load)
		.addFunction("GetCurrent", &SceneDB::GetCurrent)
		.addFunction("DontDestroy", &SceneDB::DontDestroy)
		.endNamespace();


	ComponentDB::Initialize(lua_state);
}


void ComponentManager::CppLog(const std::string& message) {
	std::cout << message << std::endl;
}

void ComponentManager::CppLogError(const std::string& message) {
	std::cout << message << std::endl;
}
