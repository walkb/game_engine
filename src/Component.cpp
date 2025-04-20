#include "Component.h"

Component::Component() {
	type = "";
}

Component::Component(std::string type) {
	type = type;
}

bool Component::isEnabled() {
	return (*componentRef)["enabled"];
}