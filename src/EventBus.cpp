#include "EventBus.h"


void EventBus::Publish(std::string event_type, luabridge::LuaRef event_object) {
	if (_eventbus.count(event_type) == 0) {
		return;
	}
	std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>& vec = _eventbus[event_type];
	for (std::pair<luabridge::LuaRef, luabridge::LuaRef>& pair : vec) {
		if (pair.second.isFunction()) {
			pair.second(pair.first, event_object);
		}
	}
}

void EventBus::Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
	SubEvent e(event_type, component, function);
	_pending_unsubs.push_back(e);
}

void EventBus::Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
	SubEvent e(event_type, component, function);
	_pending_subs.push_back(e);
}

void EventBus::UpdateSubs() {
	for (auto& event : _pending_subs) {
		// subscribe
		_eventbus[event.event_type].push_back(std::make_pair(event.component, event.function));
	}

	for (auto& event : _pending_unsubs) {
		// unsubscribe
		auto it = _eventbus.find(event.event_type);
		if (it != _eventbus.end()) {
			auto& sub_vec = it->second;
			auto vec_it = std::remove_if(sub_vec.begin(), sub_vec.end(),
				[&](const std::pair<luabridge::LuaRef, luabridge::LuaRef>& pair) {
					return pair.first == event.component && pair.second == event.function;
				});

			if (vec_it != sub_vec.end()) {
				sub_vec.erase(vec_it, sub_vec.end()); // Erase the listener(s)
			}

			// If no listeners remain, erase the event from the map
			if (sub_vec.empty()) {
				_eventbus.erase(it);
			}
		}

	}
	_pending_subs.clear();
	_pending_unsubs.clear();
}