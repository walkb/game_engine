#pragma once
#include <string>
#include "Component.h"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"


class ComponentManager
{
public:
	// --- [ GLOBAL VARIABLES ] ---
	
	// The number of times AddComponent has been called
	inline static uint32_t n = 0;

	static void Initialize();
	inline static lua_State* lua_state;
	static Component MakeInstance(std::string type, std::string name);
	static bool CompareComponents(const std::shared_ptr<Component> a, const std::shared_ptr<Component> b);
	static void InitializeState();
	static void InitializeFunctions();
	static void InitializeComponents();

	static Component CreateRigidbody();

	// --- [ LUA LOG FUNCTIONS ] ---
	static void CppLog(const std::string& message);
	static void CppLogError(const std::string& message);

	// --- [ LUA REFERENCE FUNCTIONS ] ---
	
	// todo: change void
	// Returns a LuaRef to the first actor with [name]
	static luabridge::LuaRef Find(std::string name);

	// Returns an indexed table (or empty table) of all actors with provided name sorted by their key?
	static luabridge::LuaRef FindAll(std::string name);


};

