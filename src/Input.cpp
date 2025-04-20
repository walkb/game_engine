#include "Input.h"
#include <iostream>

void Input::Init() {
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
		keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
	}

	// left - 1, middle - 2, right - 3
	for (int button = 0; button < 3; button++) {
		mouse_button_states[button + 1] = INPUT_STATE_UP;
	}

}

void Input::ProcessEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && keyboard_states[e.key.keysym.scancode] == INPUT_STATE_UP) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_scancodes.push_back(e.key.keysym.scancode);
		//std::cout << e.key.keysym.scancode << " down" << std::endl;
	}
	else if (e.type == SDL_KEYUP) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_scancodes.push_back(e.key.keysym.scancode);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN && mouse_button_states[e.button.button] == INPUT_STATE_UP) {
		mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_buttons.push_back(e.button.button);
	}
	else if (e.type == SDL_MOUSEBUTTONUP && mouse_button_states[e.button.button] == INPUT_STATE_DOWN) {
		mouse_button_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_buttons.push_back(e.button.button);
	}
	else if (e.type == SDL_MOUSEMOTION) {
		mouse_position.x = e.motion.x;
		mouse_position.y = e.motion.y;
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		mouse_scroll_this_frame = e.wheel.preciseY;

		// Change direction if scroll direction is flipped on user's settings
		if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
			mouse_scroll_this_frame = -mouse_scroll_this_frame;
		}
	}
}

void Input::LateUpdate() {
	for (const SDL_Scancode& code : just_became_down_scancodes) {
		keyboard_states[code] = INPUT_STATE_DOWN;
	}
	just_became_down_scancodes.clear();
	for (const SDL_Scancode& code : just_became_up_scancodes) {
		keyboard_states[code] = INPUT_STATE_UP;
	}
	just_became_up_scancodes.clear();

	for (const int& button : just_became_down_buttons) {
		mouse_button_states[button] = INPUT_STATE_DOWN;
	}
	just_became_down_buttons.clear();
	for (const int& button : just_became_up_buttons) {
		mouse_button_states[button] = INPUT_STATE_UP;
	}
	just_became_up_buttons.clear();

	mouse_scroll_this_frame = 0;
}

bool Input::GetKey(std::string keycode) {
	if (__keycode_to_scancode.count(keycode) == 0) {
		return false;
	}
	return (keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_DOWN || keyboard_states[__keycode_to_scancode.at(keycode)
	] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyDown(std::string keycode) {
	if (__keycode_to_scancode.count(keycode) == 0) {
		return false;
	}
	return keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(std::string keycode) {
	if (__keycode_to_scancode.count(keycode) == 0) {
		return false;
	}
	return keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_JUST_BECAME_UP;
}

glm::vec2 Input::GetMousePosition() {
	return mouse_position;
}

bool Input::GetMouseButton(int button) {
	if (button < 1 || button > 3) {
		return false;
	}
	return (mouse_button_states[button] == INPUT_STATE_DOWN || mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN);
}
bool Input::GetMouseButtonDown(int button) {
	if (button < 1 || button > 3) {
		return false;
	}
	return (mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN);
}
bool Input::GetMouseButtonUp(int button) {
	if (button < 1 || button > 3) {
		return false;
	}
	return (mouse_button_states[button] == INPUT_STATE_JUST_BECAME_UP);
}
float Input::GetMouseScrollDelta() {
	return mouse_scroll_this_frame;
}

void Input::HideCursor() {
	SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor() {
	SDL_ShowCursor(SDL_ENABLE);
}