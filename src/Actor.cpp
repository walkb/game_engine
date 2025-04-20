#include "Actor.h"
#include "Helper.h"
#include "ComponentDB.h"
#include "ComponentManager.h"
#include "Component.h"
#include "SceneDB.h"
#include "EngineUtils.h"
#include "Rigidbody.h"
#include "ParticleSystem.h"
#include <memory>

Actor::Actor() {
	id = -1;
}

void Actor::Start() {
	if (OnStartComponentQueue.empty()) {
		return;
	}

	// run each component
	for (std::shared_ptr<Component> c : OnStartComponentQueue) {
		try {
			if (c && c->isEnabled()) {
				luabridge::LuaRef* ref = c->componentRef.get();
				if (c->type == "Rigidbody") {
					Rigidbody* r = ref->cast<Rigidbody*>();
					r->OnStart();
				}
				else if (c->type == "ParticleSystem") {
					ParticleSystem* p = ref->cast<ParticleSystem*>();
					p->OnStart();
				}
				else {
					(*ref)["OnStart"](*ref);
				}
			}
		}
		catch (luabridge::LuaException e) {
			EngineUtils::ReportError(name, e);
		}
	}
	OnStartComponentQueue.clear();
}

void Actor::Update() {
	if (OnUpdateComponentQueue.empty()) {
		return;
	}

	// run each component's update function
	for (std::shared_ptr<Component> c : OnUpdateComponentQueue) {
		try {
			if (c && c->isEnabled()) {
				luabridge::LuaRef* ref = c->componentRef.get();
				if (c->type == "ParticleSystem") {
					ParticleSystem* p = ref->cast<ParticleSystem*>();
					p->OnUpdate();
				}
				else {
					(*ref)["OnUpdate"](*ref);
				}
			}
		}
		catch (luabridge::LuaException e) {
			EngineUtils::ReportError(name, e);
		}
	}
}

void Actor::LateUpdate() {
	if (OnLateUpdateComponentQueue.empty()) {
		return;
	}
	// run each component's update function
	for (std::shared_ptr<Component> c : OnLateUpdateComponentQueue) {
		try {
			if (c && c->isEnabled()) {
				luabridge::LuaRef* ref = c->componentRef.get();
				(*ref)["OnLateUpdate"](*ref);
			}
		}
		catch (luabridge::LuaException e) {
			EngineUtils::ReportError(name, e);
		}
	}
}

void Actor::AddComponentToQueues(Component& c) {
	// Handle rigidbody separately, called only during runtime
	if (c.type == "Rigidbody") {
		c.hasStart = true;
		c.hasDestroy = true;
		OnStartComponentQueue.push_back(std::make_shared<Component>(c));
		OnDeleteQueue.push_back(std::make_shared<Component>(c));
		return;
	}
	// Handle separately, called only during runtime
	if (c.type == "ParticleSystem") {
		c.hasStart = true;
		c.hasUpdate = true;
		//c.hasDestroy = true;
		OnStartComponentQueue.push_back(std::make_shared<Component>(c));
		OnUpdateComponentQueue.push_back(std::make_shared<Component>(c));
		//OnDeleteQueue.push_back(std::make_shared<Component>(c));
		return;
	}

	// Queue component for OnStart function next update (frame)
	if ((*c.componentRef)["OnStart"].isFunction()) {
		OnStartComponentQueue.push_back(std::make_shared<Component>(c));
		c.hasStart = true;
	}

	// Queue component for OnUpdate function
	if ((*c.componentRef)["OnUpdate"].isFunction()) {
		OnUpdateComponentQueue.push_back(std::make_shared<Component>(c));
		c.hasUpdate = true;
	}

	// Queue component for LateUpdate
	if ((*c.componentRef)["OnLateUpdate"].isFunction()) {
		OnLateUpdateComponentQueue.push_back(std::make_shared<Component>(c));
		c.hasLateUpdate = true;
	}

	if ((*c.componentRef)["OnCollisionEnter"].isFunction() || (*c.componentRef)["OnCollisionExit"].isFunction()) {
		OnCollisionQueue.push_back(std::make_shared<Component>(c));
	}
	if ((*c.componentRef)["OnTriggerEnter"].isFunction() || (*c.componentRef)["OnTriggerExit"].isFunction()) {
		OnTriggerQueue.push_back(std::make_shared<Component>(c));
	}

	// Queue component for OnDestroy lifecycle function
	if ((*c.componentRef)["OnDestroy"].isFunction()) {
		c.hasDestroy = true;
		OnDeleteQueue.push_back(std::make_shared<Component>(c));
	}
}


// We use a separate derived class for Rigidbody
void Actor::CreateRigidbody(rapidjson::Value::ConstMemberIterator compItr, std::string name) {
	auto itr = components.find(name);
	Rigidbody* r = new Rigidbody();
	Component c;
	if (itr != components.end()) {
		c = *itr->second; // this component already exists (by key, not type)
	}
	else {
		// init a rigidbody from the value
		c = ComponentManager::MakeInstance("Rigidbody", name);
	}
	// Init Rigidbody
	if (compItr->value.HasMember("x") ) {
		r->x = compItr->value["x"].GetFloat();
	}
	if (compItr->value.HasMember("y")) {
		r->y = compItr->value["y"].GetFloat();
	}
	if (compItr->value.HasMember("body_type")) {
		r->body_type = compItr->value["body_type"].GetString();
	}
	if (compItr->value.HasMember("precise")) {
		r->precise = compItr->value["precise"].GetBool();
	}
	if (compItr->value.HasMember("gravity_scale")) {
		r->gravity_scale = compItr->value["gravity_scale"].GetFloat();
	}
	if (compItr->value.HasMember("density")) {
		r->density = compItr->value["density"].GetFloat();
	}
	if (compItr->value.HasMember("angular_friction")) {
		r->angular_friction = compItr->value["angular_friction"].GetFloat();
	}
	if (compItr->value.HasMember("rotation")) {
		r->rotation = compItr->value["rotation"].GetFloat();
	}
	if (compItr->value.HasMember("has_collider")) {
		r->has_collider = compItr->value["has_collider"].GetBool();
	}
	if (compItr->value.HasMember("has_trigger")) {
		r->has_trigger = compItr->value["has_trigger"].GetBool();
	}
	if (compItr->value.HasMember("collider_type")) {
		r->collider_type = compItr->value["collider_type"].GetString();
	}
	if (compItr->value.HasMember("width")) {
		r->width = compItr->value["width"].GetFloat();
	}
	if (compItr->value.HasMember("height")) {
		r->height = compItr->value["height"].GetFloat();
	}
	if (compItr->value.HasMember("radius")) {
		r->radius = compItr->value["radius"].GetFloat();
	}
	if (compItr->value.HasMember("friction")) {
		r->friction = compItr->value["friction"].GetFloat();
	}
	if (compItr->value.HasMember("bounciness")) {
		r->bounciness = compItr->value["bounciness"].GetFloat();
	}
	if (compItr->value.HasMember("trigger_type")) {
		r->trigger_type = compItr->value["trigger_type"].GetString();
	}
	if (compItr->value.HasMember("trigger_radius")) {
		r->trigger_radius = compItr->value["trigger_radius"].GetFloat();
	}
	if (compItr->value.HasMember("trigger_height")) {
		r->trigger_height = compItr->value["trigger_height"].GetFloat();
	}
	if (compItr->value.HasMember("trigger_width")) {
		r->trigger_width = compItr->value["trigger_width"].GetFloat();
	}

	// set Rigidbody as LuaRef
	c.componentRef = std::make_shared<luabridge::LuaRef>(luabridge::LuaRef(ComponentManager::lua_state, r));
	c.hasDestroy = true;
	if (itr == components.end()) {
		// This is a new component, so inject the convenience references
		r->actor = this;
		r->key = name;
		r->type = "Rigidbody";
		r->enabled = true;
		// Insert a shared pointer of the component into map
		components.insert(std::make_pair(name, std::make_shared<Component>(c)));
		//AddComponentToQueues(c);

		// push it into the OnStart queue
		OnStartComponentQueue.push_back(std::make_shared<Component>(c));
		c.hasStart = true;
	}
}


// We use a separate derived class for Rigidbody
void Actor::CreateParticleSystem(rapidjson::Value::ConstMemberIterator compItr, std::string name) {
	auto itr = components.find(name);
	ParticleSystem* p = new ParticleSystem();
	Component c;
	if (itr != components.end()) {
		c = *itr->second; // this component already exists (by key, not type)
	}
	else {
		// init a particlesystem from the value
		c = ComponentManager::MakeInstance("ParticleSystem", name);
	}
	// Init Rigidbody
	if (compItr->value.HasMember("x")) {
		p->x = compItr->value["x"].GetFloat();
	}
	if (compItr->value.HasMember("y")) {
		p->y = compItr->value["y"].GetFloat();
	}
	if (compItr->value.HasMember("frames_between_bursts")) {
		p->frames_between_bursts = compItr->value["frames_between_bursts"].GetInt();
		// Ensure the frames between bursts is at least 1
		if (p->frames_between_bursts < 1) {
			p->frames_between_bursts = 1;
		}
	}
	if (compItr->value.HasMember("burst_quantity")) {
		p->burst_quantity = compItr->value["burst_quantity"].GetInt();
		// Ensure the frames between bursts is at least 1
		if (p->burst_quantity < 1) {
			p->burst_quantity = 1;
		}
	}
	if (compItr->value.HasMember("start_scale_min")) {
		p->start_scale_min = compItr->value["start_scale_min"].GetFloat();
	}
	if (compItr->value.HasMember("start_scale_max")) {
		p->start_scale_max = compItr->value["start_scale_max"].GetFloat();
	}
	if (compItr->value.HasMember("rotation_min")) {
		p->rotation_min = compItr->value["rotation_min"].GetFloat();
	}
	if (compItr->value.HasMember("rotation_max")) {
		p->rotation_max = compItr->value["rotation_max"].GetFloat();
	}
	if (compItr->value.HasMember("emit_radius_min")) {
		p->emit_radius_min = compItr->value["emit_radius_min"].GetFloat();
	}
	if (compItr->value.HasMember("emit_radius_max")) {
		p->emit_radius_max = compItr->value["emit_radius_max"].GetFloat();
	}
	if (compItr->value.HasMember("emit_angle_min")) {
		p->emit_angle_min = compItr->value["emit_angle_min"].GetFloat();
	}
	if (compItr->value.HasMember("emit_angle_max")) {
		p->emit_angle_max = compItr->value["emit_angle_max"].GetFloat();
	}
	
	if (compItr->value.HasMember("start_color_r")) {
		p->start_color_r = compItr->value["start_color_r"].GetInt();
	}
	if (compItr->value.HasMember("start_color_g")) {
		p->start_color_g = compItr->value["start_color_g"].GetInt();
	}
	if (compItr->value.HasMember("start_color_b")) {
		p->start_color_b = compItr->value["start_color_b"].GetInt();
	}
	if (compItr->value.HasMember("start_color_a")) {
		p->start_color_a = compItr->value["start_color_a"].GetInt();
	}
	if (compItr->value.HasMember("end_color_r")) {
		p->end_color_r = static_cast<int>(compItr->value["end_color_r"].GetFloat());
		p->has_end_color = true;
	}
	if (compItr->value.HasMember("end_color_g")) {
		p->end_color_g = static_cast<int>(compItr->value["end_color_g"].GetFloat());
		p->has_end_color = true;
	}
	if (compItr->value.HasMember("end_color_b")) {
		p->end_color_b = static_cast<int>(compItr->value["end_color_b"].GetFloat());
		p->has_end_color = true;
	}
	if (compItr->value.HasMember("end_color_a")) {
		p->end_color_a = static_cast<int>(compItr->value["end_color_a"].GetFloat());
		p->has_end_color = true;
	}

	if (compItr->value.HasMember("image")) {
		p->image = compItr->value["image"].GetString();
	}

	if (compItr->value.HasMember("duration_frames")) {
		p->duration_frames = compItr->value["duration_frames"].GetInt();
		if (p->duration_frames < 1) {
			p->duration_frames = 1;
		}
	}
	if (compItr->value.HasMember("start_speed_min")) {
		p->start_speed_min = compItr->value["start_speed_min"].GetFloat();
	}
	if (compItr->value.HasMember("start_speed_max")) {
		p->start_speed_max = compItr->value["start_speed_max"].GetFloat();
	}
	if (compItr->value.HasMember("rotation_speed_min")) {
		p->rotation_speed_min = compItr->value["rotation_speed_min"].GetFloat();
	}
	if (compItr->value.HasMember("rotation_speed_max")) {
		p->rotation_speed_max = compItr->value["rotation_speed_max"].GetFloat();
	}
	if (compItr->value.HasMember("gravity_scale_x")) {
		p->gravity_scale_x = compItr->value["gravity_scale_x"].GetFloat();
	}
	if (compItr->value.HasMember("gravity_scale_y")) {
		p->gravity_scale_y = compItr->value["gravity_scale_y"].GetFloat();
	}
	if (compItr->value.HasMember("drag_factor")) {
		p->drag_factor = compItr->value["drag_factor"].GetFloat();
	}
	if (compItr->value.HasMember("angular_drag_factor")) {
		p->angular_drag_factor = compItr->value["angular_drag_factor"].GetFloat();
	}
	if (compItr->value.HasMember("end_scale")) {
		p->end_scale = compItr->value["end_scale"].GetFloat();
		p->has_end_scale = true;
	}
	if (compItr->value.HasMember("sorting_order")) {
		p->sorting_order = compItr->value["sorting_order"].GetInt();
	}



	// set Rigidbody as LuaRef
	c.componentRef = std::make_shared<luabridge::LuaRef>(luabridge::LuaRef(ComponentManager::lua_state, p));
	if (itr == components.end()) {
		// This is a new component, so inject the convenience references
		p->actor = this;
		p->key = name;
		p->type = "ParticleSystem";
		p->enabled = true;
		// Insert a shared pointer of the component into map
		components.insert(std::make_pair(name, std::make_shared<Component>(c)));

		c.hasStart = true;
		c.hasUpdate = true;
		//c.hasDestroy = true;
		// push it into the OnStart queue
		OnStartComponentQueue.push_back(std::make_shared<Component>(c));
		OnUpdateComponentQueue.push_back(std::make_shared<Component>(c));

	}
}

void Actor::CreateComponent(rapidjson::Value::ConstMemberIterator compItr, std::string type, std::string name) {
	auto itr = components.find(name);
	Component c;
	if (itr != components.end()) {
		c = *itr->second; // this component already exists (by key, not type)
	}
	else {
		c = ComponentManager::MakeInstance(type, name); // this component doesn't exist, so make a new one
		(*c.componentRef)["enabled"] = true; // set the new component to enabled
	}
	// fetch the rest of the variables if they exist
	std::shared_ptr<luabridge::LuaRef> ref = c.componentRef;
	for (auto varItr = compItr->value.MemberBegin(); varItr != compItr->value.MemberEnd(); varItr++) {
		std::string varName = varItr->name.GetString();
		if (varItr->value.IsString()) {
			(*ref)[varName] = varItr->value.GetString();
		}
		else if (varItr->value.IsBool()) {
			(*ref)[varName] = varItr->value.GetBool();
		}
		else if (varItr->value.IsInt()) {
			(*ref)[varName] = varItr->value.GetInt();
		}
		else if (varItr->value.IsFloat()) {
			(*ref)[varName] = varItr->value.GetFloat();
		}
	}
	if (itr == components.end()) {
		// This is a new component, so inject the convenience references
		InjectConvenienceReferences(ref);
		// Insert a shared pointer of the component into map
		AddComponentToQueues(c);
		components.insert(std::make_pair(name, std::make_shared<Component>(c)));
	}
}

void Actor::CreateComponents(const rapidjson::Value& comp_arr) {
	for (auto compItr = comp_arr.MemberBegin(); compItr != comp_arr.MemberEnd(); compItr++) {
		std::string componentName = compItr->name.GetString();
		std::string type = "";
		if (compItr->value.HasMember("type") && compItr->value["type"].IsString()) {
			type = compItr->value["type"].GetString();
		}
		if (type == "Rigidbody") {
			//CreateRigidbody(compItr, componentName);
			CreateRigidbody(compItr, componentName);
		}
		else if (type == "ParticleSystem") {
			CreateParticleSystem(compItr, componentName);
		}
		else {
			CreateComponent(compItr, type, componentName);
		}
		
	}
}

void Actor::CreateActorFromValue(rapidjson::Value& arr) {
	for (auto itr = arr.MemberBegin(); itr != arr.MemberEnd(); ++itr) {
		const std::string key = itr->name.GetString();  // Get the key once
		const rapidjson::Value& value = itr->value;
		if (key == "name") {
			name = value.GetString();
		}
		if (key == "components" && itr->value.IsObject()) {
			CreateComponents(value);
		}
	}
	// Sort our queues so we don't have to every time we use them
	std::sort(OnStartComponentQueue.begin(), OnStartComponentQueue.end(), ComponentManager::CompareComponents);
	std::sort(OnUpdateComponentQueue.begin(), OnUpdateComponentQueue.end(), ComponentManager::CompareComponents);
	std::sort(OnLateUpdateComponentQueue.begin(), OnLateUpdateComponentQueue.end(), ComponentManager::CompareComponents);
	std::sort(OnDeleteQueue.begin(), OnDeleteQueue.end(), ComponentManager::CompareComponents);
}

// ---------- [ LUA RUNTIME FUNCS ] ------
luabridge::LuaRef Actor::AddComponent(std::string type_name) {
	std::string key = "r" + std::to_string(ComponentManager::n);
	ComponentManager::n++;
	Component c = ComponentManager::MakeInstance(type_name, key);
	// set the new component to enabled

	if (type_name == "Rigidbody") {
		Rigidbody* r = new Rigidbody();
		// set Rigidbody as LuaRef
		c.componentRef = std::make_shared<luabridge::LuaRef>(luabridge::LuaRef(ComponentManager::lua_state, r));
		c.hasDestroy = true;
		// This is a new component, so inject the convenience references
		r->actor = this;
		r->key = name;
		r->type = "Rigidbody";
		r->enabled = true;
		// Insert a shared pointer of the component into map
		components.insert(std::make_pair(name, std::make_shared<Component>(c)));

		c.hasStart = true;
	}
	if (type_name == "ParticleSystem") {
		ParticleSystem* p = new ParticleSystem();
		// set Rigidbody as LuaRef
		c.componentRef = std::make_shared<luabridge::LuaRef>(luabridge::LuaRef(ComponentManager::lua_state, p));
		c.hasDestroy = true;
		// This is a new component, so inject the convenience references
		p->actor = this;
		p->key = name;
		p->type = "Rigidbody";
		p->enabled = true;
		// Insert a shared pointer of the component into map
		components.insert(std::make_pair(name, std::make_shared<Component>(c)));

		c.hasStart = true;
	}
	else {
		(*c.componentRef)["enabled"] = true;	
		// This is a new component, so inject the convenience references
		InjectConvenienceReferences(c.componentRef);
	}
	std::shared_ptr<Component> comp_ptr = std::make_shared<Component>(c);
	SceneDB::ComponentsToAdd[this].push_back(comp_ptr);
	SceneDB::ActorsComponentChanged.push_back(this);

	return *c.componentRef;
}

void Actor::RemoveComponent(luabridge::LuaRef ref) {
	ref["enabled"] = false;
	SceneDB::ComponentsToRemove[this].push_back(ref["key"]);
	SceneDB::ActorsComponentChanged.push_back(this);
}

void Actor::OnTriggerEnter(Collision c) {
	c.point = b2Vec2(-999.0f, -999.0f);
	c.normal = b2Vec2(-999.0f, -999.0f);
	for (std::shared_ptr<Component>& comp_ptr : OnTriggerQueue) {
		Component* comp = comp_ptr.get();
		luabridge::LuaRef* ref = comp->componentRef.get();
		if ((*ref)["OnTriggerEnter"].isFunction()) {
			(*ref)["OnTriggerEnter"](*ref, c);
		}
	}
}

void Actor::OnTriggerExit(Collision c) {
	c.point = b2Vec2(-999.0f, -999.0f);
	c.normal = b2Vec2(-999.0f, -999.0f);
	for (std::shared_ptr<Component>& comp_ptr : OnTriggerQueue) {
		Component* comp = comp_ptr.get();
		luabridge::LuaRef* ref = comp->componentRef.get();
		if ((*ref)["OnTriggerExit"].isFunction()) {
			(*ref)["OnTriggerExit"](*ref, c);
		}
	}
}

void Actor::OnCollisionEnter(Collision c) {
	for (std::shared_ptr<Component>& comp_ptr : OnCollisionQueue) {
		Component* comp = comp_ptr.get();
		luabridge::LuaRef* ref = comp->componentRef.get();
		if ((*ref)["OnCollisionEnter"].isFunction()) {
			(*ref)["OnCollisionEnter"](*ref, c);
		}
	}
}

void Actor::OnCollisionExit(Collision c) {
	c.point = b2Vec2(-999.0f, -999.0f);
	c.normal = b2Vec2(-999.0f, -999.0f);
	for (std::shared_ptr<Component>& comp_ptr : OnCollisionQueue) {
		Component* comp = comp_ptr.get();
		luabridge::LuaRef* ref = comp->componentRef.get();
		if ((*ref)["OnCollisionExit"].isFunction()) {
			(*ref)["OnCollisionExit"](*ref, c);
		}
	}
}

// ---------- [ LUA REFERENCE FUNCTIONS ] -----------

void Actor::InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref) {
	(*component_ref)["actor"] = this;
}

uint32_t Actor::GetID() {
	return id;
}
std::string Actor::GetName() {
	return name;
}

Actor::Actor(rapidjson::Value& arr) {
	CreateActorFromValue(arr);
}

luabridge::LuaRef Actor::GetComponentByKey(std::string key) {
	auto itr = components.find(key);
	if (itr == components.end()) {
		return luabridge::LuaRef(ComponentManager::lua_state);
	}
	else {
		// TODO: check if we need to error check this or something bcuz it's a smart pointer
		return *itr->second->componentRef;
	}
}

luabridge::LuaRef Actor::GetComponent(std::string type_name) {
	std::string key = "";
	for (const auto& component : components) {
		const std::shared_ptr<Component>& comp_ptr = component.second;
		// TODO: Check if removing isEnabled is necessary
		if (comp_ptr->type == type_name && comp_ptr->isEnabled()) {
			if (key == "" || comp_ptr->key < key) {
				key = comp_ptr->key;
			}
		}
	}
	if (key == "") {
		return luabridge::LuaRef(ComponentManager::lua_state);
	}
	else {
		luabridge::LuaRef ref = *components[key]->componentRef;
		return ref;
	}
}

luabridge::LuaRef Actor::GetComponents(std::string type_name) {
	// Find all matches and sort them by key order
	std::vector<std::shared_ptr<Component>> matches;
	for (const auto& component : components) {
		const std::shared_ptr<Component>& comp_ptr = component.second;
		if (comp_ptr->type == type_name && comp_ptr->isEnabled()) {
			matches.push_back(comp_ptr);
		}
	}
	std::sort(matches.begin(), matches.end(), ComponentManager::CompareComponents);

	// Make a table and insert all matches into it
	lua_State* lua_state = ComponentManager::lua_state;

	luabridge::LuaRef table = luabridge::newTable(lua_state);

	// Add {i : LuaRef to Component} to table
	for (uint32_t i = 0; i < matches.size(); i++) {
		// Lua tables are 1-indexed
		table[i + 1] = *matches[i]->componentRef;
	}

	return table;
}

