#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include "glm/glm.hpp"
#include "SceneDB.h"
#include "Actor.h"
#include "TemplateDB.h"
#include "Renderer.h"
#include "ImageDB.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "SDL2/SDL.h"
#include "Input.h"
#include "ComponentManager.h"
#include "ComponentDB.h"
//#include "Component.h"
#include <optional>
#include "box2d/box2d.h"


class Engine
{
public:
	Engine();

	void Run();

private:
	// Initializer functions
	void GetGameConfig();
	void GetRenderConfig();
	void StartRenderer();

	void LoadImages();
	void LoadText();
	void LoadAudio();
	void LoadInitialScene();

	void GameLoop();
	void Input(SDL_Event& next_event);
	void Update(SDL_Event& next_event);
	void Render();

	// ------------[CONFIG FILES]-------------
	rapidjson::Document game_config;
	rapidjson::Document render_config;

	// -----------[WINDOW CONFIGS]------------
	std::string game_title;

	// -----------[STATUS FLAGS]-----------
	bool running = true;
	bool quit = false;
	std::string next_scene = "";

	// -----------[INPUT]------------------
	std::string input = "";
};

