#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Component.h"


struct SubEvent {
	SubEvent(std::string event, luabridge::LuaRef comp, luabridge::LuaRef func) : event_type(event), component(comp), function(func) {};
	std::string event_type;
	luabridge::LuaRef component;
	luabridge::LuaRef function;
};


class EventBus
{
public:
	// Calls all functions in the EventBus in the event_type vector
	static void Publish(std::string event_type, luabridge::LuaRef event_object);

	static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);

	static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);

	static void UpdateSubs();





private:
	// _eventbus with key event_type, value a vector of pair of component luaref, function luaref.
	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> _eventbus;

	static inline std::vector<SubEvent> _pending_subs;
	static inline std::vector<SubEvent> _pending_unsubs;

};

