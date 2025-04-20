#include "ComponentDB.h"
#include "ComponentManager.h"
#include "EngineUtils.h"
#include <memory>


bool ComponentDB::CheckComponentExists(std::string componentName) {
	if (globalComponents.count(componentName) == 0) {
		return false;
	}
	return true;
}

// Reads all .lua files in resources/component_types for components and stores them in map with type as key.
void ComponentDB::Initialize(lua_State* lua_state) {
	lua_state = lua_state;
	const std::string component_dir_path = "resources/component_types/";
	if (EngineUtils::CheckPathExists(component_dir_path)) {
		for (const auto& component : std::filesystem::directory_iterator(component_dir_path)) {
			std::string name = component.path().filename().stem().string();
			std::string extension = component.path().filename().extension().string();

			// attempt to make the component table
			if (luaL_dofile(lua_state, component.path().string().c_str()) != LUA_OK) {
				std::cout << "problem with lua file " << name;
				exit(0);
			}
			Component c = Component(name);
			// link component to LuaRef
			c.componentRef = std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(lua_state, name.c_str()));
			// todo: check if this is worth moving instead of copying via make_pair
			globalComponents.insert(std::make_pair(name, c));

			//std::cout << "Created component " << name << std::endl;
		}
	}
}

void ComponentDB::EstablishInheritance(Component& instance, Component& parent) {
	if (!parent.componentRef) {
		std::cout << "Error: parent.componentRef is null" << std::endl;
		exit(0);
	}
	luabridge::LuaRef new_metatable = luabridge::newTable(ComponentManager::lua_state);
	new_metatable["__index"] = *parent.componentRef;

	instance.componentRef.get()->push(ComponentManager::lua_state);
	new_metatable.push(ComponentManager::lua_state);
	lua_setmetatable(ComponentManager::lua_state, -2);
	lua_pop(ComponentManager::lua_state, 1);
}