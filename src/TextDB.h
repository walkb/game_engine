#pragma once
#include "SDL2/SDL.h"
#include <unordered_map>
#include <string>
#include "rapidjson/document.h"
#include "Renderer.h"
#include <vector>
#include "SDL2_ttf/SDL_ttf.h"
#include "glm/glm.hpp"

class TextDB
{
public:
	inline static SDL_Texture* current;
	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;
	static void Initialize();
	static void LoadFont(std::string font_name, int ptsize);

	static void FlushTextAndRender();

	// ---[LUA FUNC]---

	// Makes a DrawRequest, must cast all floats to ints immediately
	static void Draw(std::string str_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a);
};

