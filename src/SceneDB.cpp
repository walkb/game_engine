#include "SceneDB.h"
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Helper.h"
#include <iostream>
#include "Actor.h"
#include <chrono>
#include <algorithm>
#include <thread>
#include "ComponentManager.h"



Actor* SceneDB::InitializeActor(rapidjson::Value& arr) {
	rapidjson::Value::ConstMemberIterator itr = arr.FindMember("template");
	if (itr != arr.MemberEnd()) {
		// Use a template, then overwrite it with arr
		std::string template_name = itr->value.GetString();
		if (!TemplateDB::CheckTemplateExists(template_name)) {
			// Template does not exist
			std::cout << "error: template " << template_name << " is missing";
			exit(0);
		}
		// Template exists
		rapidjson::Document& d = TemplateDB::templates[template_name];

		Actor* a = new Actor();
		// create actor from template
		a->CreateActorFromValue(d);
		// add in overrides from json
		a->CreateActorFromValue(arr);
		a->id = next_id++;
		a->template_name = template_name;
		return a;
	}
	else {
		Actor* a = new Actor(arr);
		a->id = next_id++;
		return a;
	}
}


// Returns a pointer to the player
bool SceneDB::LoadScene(std::string scene) {
	change = false;
	next_scene_name = "";
	// Reset everything for new scene load
	for (Actor* a : actors) {
		if (!a->dontdelete) {
			SceneDB::Destroy(a);
		}
	}
	std::string path = "./resources/scenes/" + scene + ".scene";
	if (!EngineUtils::CheckPathExists(path)) {
		// Scene not found
		std::cout << "error: scene " << scene << " is missing";
		exit(0);
	}
	current_scene_name = scene;
	rapidjson::Document scene_json;
	EngineUtils::ReadJsonFile(path, scene_json);
	if (scene_json.HasMember("actors") && scene_json["actors"].IsArray()) {
		// reserve space for actors
		actors.reserve(scene_json["actors"].Size());
		for (auto& actor : scene_json["actors"].GetArray()) {
			Actor* a = InitializeActor(actor);
			actors.push_back((a));

			// If actors have relevant components, flag them and add to queue
			if (a->OnStartComponentQueue.size() > 0) {
				OnStartActorQueue.push_back(actors.back());
			}
			if (a->OnUpdateComponentQueue.size() > 0) {
				OnUpdateActorQueue.push_back(actors.back());
			}
			if (a->OnLateUpdateComponentQueue.size() > 0) {
				OnLateUpdateActorQueue.push_back(actors.back());
			}
		}
	}
	return true;
}

void SceneDB::ProcessComponentUpdates(Actor* a) {
	// Check for removals
	if (ComponentsToRemove.count(a) != 0) {
		for (std::string comp : ComponentsToRemove[a]) {
			std::shared_ptr<Component> c = a->components[comp];
			if (a->components[comp]->hasStart) {
				a->OnStartComponentQueue.erase(std::remove(a->OnStartComponentQueue.begin(), a->OnStartComponentQueue.end(), c), a->OnStartComponentQueue.end());
			}
			if (a->components[comp]->hasUpdate) {
				a->OnUpdateComponentQueue.erase(std::remove(a->OnUpdateComponentQueue.begin(), a->OnUpdateComponentQueue.end(), c), a->OnUpdateComponentQueue.end());
			}
			if (a->components[comp]->hasLateUpdate) {
				a->OnLateUpdateComponentQueue.erase(std::remove(a->OnLateUpdateComponentQueue.begin(), a->OnLateUpdateComponentQueue.end(), c), a->OnLateUpdateComponentQueue.end());
			}
			if (a->components[comp]->hasDestroy) {
				// call onDestroy
				if (c->type == "Rigidbody") {
					Rigidbody* r = c->componentRef->cast<Rigidbody*>();
					r->OnDestroy();
				}
				else {
					std::shared_ptr<luabridge::LuaRef> ref = (a->components[comp])->componentRef;
					(*ref)["OnDestroy"](*ref);
				}
				// remove from queues
				a->OnDeleteQueue.erase(std::remove(a->OnDeleteQueue.begin(), a->OnDeleteQueue.end(), c), a->OnDeleteQueue.end());
			}
			a->components.erase(comp);
		}
		ComponentsToRemove.clear();
	}
	// Check for additions
	if (ComponentsToAdd.count(a) != 0) {
		for (std::shared_ptr<Component>& comp : ComponentsToAdd[a]) {
			a->AddComponentToQueues(*comp);
			a->components.insert(std::make_pair(comp->key, comp));
		}
		ComponentsToAdd.clear();
	}
}

void SceneDB::RemoveActorFromQueue(Actor* a, std::vector<Actor*>& queue) {
	auto it = std::find(queue.begin(), queue.end(), a);
	if (it != queue.end()) {
		queue.erase(it);
	}
}

void SceneDB::ProcessActorUpdates() {
	while (!ActorsToAdd.empty()) {
		// Add actor to actors
		Actor* a = ActorsToAdd.back();
		ActorsToAdd.pop_back();

		actors.push_back(a);

		// If actors have relevant components, flag them and add to queue
		if (a->OnStartComponentQueue.size() > 0) {
			OnStartActorQueue.push_back(actors.back());
		}
		if (a->OnUpdateComponentQueue.size() > 0) {
			OnUpdateActorQueue.push_back(actors.back());
		}
		if (a->OnLateUpdateComponentQueue.size() > 0) {
			OnLateUpdateActorQueue.push_back(actors.back());
		}
	}
	while (!ActorsToRemove.empty()) {
		Actor* a = ActorsToRemove.back();
		ActorsToRemove.pop_back();

		// remove actor from actors
		auto it = std::remove(actors.begin(), actors.end(), a);
		if (it != actors.end()) {
			actors.erase(it, actors.end());
		}
		// remove actor from all queues
		RemoveActorFromQueue(a, OnStartActorQueue);
		RemoveActorFromQueue(a, OnUpdateActorQueue);
		RemoveActorFromQueue(a, OnLateUpdateActorQueue);

		// Call all of actor's OnDestroy components
		for (auto ptr : a->OnDeleteQueue) {
			(*ptr->componentRef)["OnDestroy"](*ptr->componentRef);
		}

		delete a;
	}
	for (Actor* a : ActorsComponentChanged) {
		// Check if actor gains a new component type to queue for
		int start = a->OnStartComponentQueue.size();
		int update = a->OnUpdateComponentQueue.size();
		int late = a->OnLateUpdateComponentQueue.size();
		ProcessComponentUpdates(a);
		if (start == 0 && a->OnStartComponentQueue.size() > 0) {
			OnStartActorQueue.push_back(a);
		}
		if (update == 0 && a->OnUpdateComponentQueue.size() > 0) {
			OnUpdateActorQueue.push_back(a);
		}
		if (late == 0 && a->OnLateUpdateComponentQueue.size() > 0) {
			OnLateUpdateActorQueue.push_back(a);
		}
	}
}


// ------- [ LUA? ] ---------


luabridge::LuaRef SceneDB::Instantiate(std::string template_name) {
	// Use a template, then overwrite it with arr
	if (!TemplateDB::CheckTemplateExists(template_name)) {
		// Template does not exist
		std::cout << "error: template " << template_name << " is missing";
		exit(0);
	}
	// Template exists
	rapidjson::Document& d = TemplateDB::templates[template_name];

	Actor* a = new Actor();
	// create actor from template
	a->CreateActorFromValue(d);
	// add in overrides from json
	a->id = next_id++;
	a->template_name = template_name;
	ActorsToAdd.push_back(a);
	return luabridge::LuaRef(ComponentManager::lua_state, a);
}

void SceneDB::Destroy(Actor* a) {
	ActorsToRemove.push_back(a);
	a->deleted = true;
	// set all components' enabled to false
	for (auto& entry : a->components) {
		std::shared_ptr<Component> c = entry.second;
		(*c->componentRef)["enabled"] = false;
	}
}


luabridge::LuaRef SceneDB::Find(std::string name) {
	for (Actor* a : actors) {
		if (a->name == name && !a->deleted) {
			// TODO: Check if we need to make this a shared_ptr instead.
			return luabridge::LuaRef(ComponentManager::lua_state, a);
		}
	}
	for (Actor* a : ActorsToAdd) {
		if (a->name == name && !a->deleted) {
			// TODO: Check if we need to make this a shared_ptr instead.
			return luabridge::LuaRef(ComponentManager::lua_state, a);
		}
	}
	return luabridge::LuaRef(ComponentManager::lua_state);
}

luabridge::LuaRef SceneDB::FindAll(std::string name) {
	// Find all matches and sort them by key order
	std::vector<Actor*> matches;
	for (Actor* actor : actors) {
		if (actor->name == name && !actor->deleted) {
			matches.push_back(actor);
		}
	}
	for (Actor* actor : ActorsToAdd) {
		if (actor->name == name && !actor->deleted) {
			matches.push_back(actor);
		}
	}
	//std::sort(matches.begin(), matches.end(), [](const Actor& a, const Actor& b) { ret);

	// Make a table and insert all matches into it
	lua_State* lua_state = ComponentManager::lua_state;

	luabridge::LuaRef table = luabridge::newTable(lua_state);

	// Add {i : LuaRef to Component} to table
	for (uint32_t i = 0; i < matches.size(); i++) {
		// Lua tables are 1-indexed
		table[i + 1] = luabridge::LuaRef(ComponentManager::lua_state, matches[i]);
	}

	return table;
}

// --- [ APPLICATION FUNCTIONS ] ---
void SceneDB::Quit() {
	exit(0);
}

void SceneDB::Sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int SceneDB::GetFrame() {
	return Helper::GetFrameNumber();
}

void SceneDB::OpenURL(const std::string& url) {
	std::string cmd;
	#ifdef _WIN32
	cmd = "start " + url;
	#elif __linux__
	cmd = "open " + url;
	#elif __APPLE__ && __MACH__
	cmd = "xdg-open " + url;
	#endif
	std::system(cmd.c_str());
}

// --- [ SCENE FUNC ] ---
void SceneDB::Load(std::string scene_name) {
	change = true;
	next_scene_name = scene_name;
 }

std::string SceneDB::GetCurrent() {
	return current_scene_name;
 }

void SceneDB::DontDestroy(Actor* a) {
	a->dontdelete = true;
 }