#pragma once
#include <memory>
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component
{
public:
	Component();
	explicit Component(std::string type);

	// Checks componentRef table for "enabled" boolean
	bool isEnabled();

	std::string key;
	std::shared_ptr<luabridge::LuaRef> componentRef;
	std::string type;

	bool hasStart;
	bool hasUpdate;
	bool hasLateUpdate;
	bool hasDestroy;
};

