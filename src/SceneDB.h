#pragma once
#include <string>
#include <array>
#include "Actor.h"
#include <vector>
#include "rapidjson/document.h"
#include "TemplateDB.h"
#include "glm/glm.hpp"
#include "Component.h"
#include <set>


class SceneDB
{
public:
	inline static std::vector<Actor*> actors;

	static bool LoadScene(std::string scene);
	static Actor* InitializeActor(rapidjson::Value& arr);
	inline static uint32_t next_id = 1;
	inline static bool change = false;
	inline static std::string current_scene_name = "";
	inline static std::string next_scene_name = "";

	// Processes all actors to add, remove, and component updates
	static void ProcessActorUpdates();
	static void ProcessComponentUpdates(Actor* a);
	static void RemoveActorFromQueue(Actor* a, std::vector<Actor*>& queue);

	inline static std::vector<Actor*> OnStartActorQueue;
	inline static std::vector<Actor*> OnUpdateActorQueue;
	inline static std::vector<Actor*> OnLateUpdateActorQueue;

	inline static std::vector<Actor*> ActorsComponentChanged;
	inline static std::unordered_map<Actor*, std::vector<std::shared_ptr<Component>>> ComponentsToAdd;
	inline static std::unordered_map<Actor*, std::vector<std::string>> ComponentsToRemove;

	inline static std::vector<Actor*> ActorsToAdd;
	inline static std::vector<Actor*> ActorsToRemove;

	static luabridge::LuaRef Find(std::string name);

	static luabridge::LuaRef FindAll(std::string name);

	static luabridge::LuaRef Instantiate(std::string template_name);

	static void Destroy(Actor* a);

	// --- [ APPLICATION FUNCTIONS ] ---
	static void Quit();

	static void Sleep(int ms);

	static int GetFrame();

	static void OpenURL(const std::string& url);

	// --- [ SCENE FUNCTIONS ] ---
	static void Load(std::string scene_name);

	static std::string GetCurrent();

	static void DontDestroy(Actor* a);
};