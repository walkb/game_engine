#pragma once
#include "glm/glm.hpp"
#include "string"
#include "rapidjson/document.h"
#include "SDL2_image/SDL_image.h"
#include "ImageDB.h"
#include <optional>
#include <unordered_map>
#include "Component.h"
#include "Rigidbody.h"

class Collision;

class Actor
{
public:
	uint32_t id;
	std::string name = "";
	std::string template_name = "";
	std::unordered_map<std::string, std::shared_ptr<Component>> components;
	bool deleted = false;
	bool dontdelete = false;

	std::vector<std::shared_ptr<Component>> OnStartComponentQueue;
	std::vector<std::shared_ptr<Component>> OnUpdateComponentQueue;
	std::vector<std::shared_ptr<Component>> OnLateUpdateComponentQueue;

	std::vector<std::shared_ptr<Component>> OnCollisionQueue;
	std::vector<std::shared_ptr<Component>> OnTriggerQueue;

	std::vector<std::shared_ptr<Component>> OnDeleteQueue;

	Actor();
	//Actor(const Actor& other);
	Actor(rapidjson::Value& arr);

	// Create an actor and initialize components given a rapidjson::Value
	void CreateActorFromValue(rapidjson::Value& arr);

	// Add component to queues
	void AddComponentToQueues(Component& c);

	// ---------[RUN FUNCTIONS]---------
	void Start();
	void Update();
	void LateUpdate();

	// ---------[LUA RUNTIME FUNCTIONS]-----------

	// Adds a component to components and returns a ref to it
	luabridge::LuaRef AddComponent(std::string type_name);

	// Sets enabled to false, meaning GetComponent, etc., returns nil for this component
	void RemoveComponent(luabridge::LuaRef ref);

	void OnTriggerEnter(Collision c);

	void OnTriggerExit(Collision c);

	void OnCollisionEnter(Collision c);

	void OnCollisionExit(Collision c);


	// ---------[LUA REFERENCE FUNCTIONS]----------
	// Makes a raw pointer of the owning Actor available in Lua
	void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref);

	// Returns owning actor's ID
	uint32_t GetID();

	// Returns owning actor's name
	std::string GetName();

	// Returns a LuaRef to component or nil from key
	luabridge::LuaRef GetComponentByKey(std::string key);

	// Returns a LuaRef to the first component of [type_name] in key order, or nil
	luabridge::LuaRef GetComponent(std::string type_name);

	// Creates an indexed table (or empty table) of components sorted by key
	luabridge::LuaRef GetComponents(std::string type_name);

private:
	// Create a component helper function using rapidjson::Value of components object
	void CreateComponents(const rapidjson::Value& comp_arr);

	// Helper function to create a component, used only in CreateComponents
	void CreateComponent(rapidjson::Value::ConstMemberIterator itr, std::string type, std::string name);

	// Helper function to create a Rigidbody component, used only in Create Components
	void CreateRigidbody(rapidjson::Value::ConstMemberIterator itr, std::string name);

	// Helper function to create a ParticleSystem component, used only in Create Components
	void CreateParticleSystem(rapidjson::Value::ConstMemberIterator itr, std::string name);
};

