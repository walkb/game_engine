#pragma once
#include <unordered_map>
#include <string>
#include "Component.h"
class ComponentDB
{
public:
	inline static std::unordered_map<std::string, Component> globalComponents;

	static void Initialize(lua_State* lua_state);

	// Checks if a component exists in the globalComponents map.
	static bool CheckComponentExists(std::string componentName);

	static void EstablishInheritance(Component& instance, Component& parent);
};

