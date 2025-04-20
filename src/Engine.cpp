#include "Engine.h"
#include "EngineUtils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "rapidjson/document.h"
#include "Helper.h"
#include "Input.h"
#include "World.h"
#include "EventBus.h"



void Engine::GetGameConfig() {
	// Check for resources directory
	if (!EngineUtils::CheckPathExists("./resources")) {
		std::cout << "error: resources/ missing";
		exit(0);
	}
	if (!EngineUtils::CheckPathExists("./resources/game.config")) {
		std::cout << "error: resources/game.config missing";
		exit(0);
	}
	EngineUtils::ReadJsonFile("./resources/game.config", game_config);
}

void Engine::GetRenderConfig() {
	if (EngineUtils::CheckPathExists("./resources/rendering.config")) {
		EngineUtils::ReadJsonFile("./resources/rendering.config", render_config);
		if (render_config.HasMember("y_resolution")) {
			Renderer::window_size.x = static_cast<float>(render_config["x_resolution"].GetInt());
		}
		if (render_config.HasMember("x_resolution")) {
			Renderer::window_size.y = static_cast<float>(render_config["y_resolution"].GetInt());
		}
		if (render_config.HasMember("clear_color_r")) {
			Renderer::clear_color_r = render_config["clear_color_r"].GetInt();
		}
		if (render_config.HasMember("clear_color_g")) {
			Renderer::clear_color_g = render_config["clear_color_g"].GetInt();
		}
		if (render_config.HasMember("clear_color_b")) {
			Renderer::clear_color_b = render_config["clear_color_b"].GetInt();
		}
		if (render_config.HasMember("zoom_factor")) {
			Renderer::zoom_factor = render_config["zoom_factor"].GetFloat();
		}
	}
}

void Engine::StartRenderer() {
	if (game_config.HasMember("game_title")) {
		Renderer::game_title = game_config["game_title"].GetString();
	}
	Renderer::makeWindow();
	Renderer::makeRenderer();
	Renderer::clear();
}

void Engine::LoadImages() {
	ImageDB::Initialize();
}

void Engine::LoadText() {
	TextDB::Initialize();
}

void Engine::LoadAudio() {
	AudioDB::Initialize();
	if (game_config.HasMember("gameplay_audio")) {
		AudioDB::gameplay_audio = game_config["gameplay_audio"].GetString();
	}
}

void Engine::LoadInitialScene() {
	// Load resources and initial scene
	if (game_config.HasMember("initial_scene")) {
		SceneDB::LoadScene(game_config["initial_scene"].GetString());
	}
	else {
		std::cout << "error: initial_scene unspecified";
		exit(0);
	}
}


void Engine::Input(SDL_Event& next_event)
{
	while (Helper::SDL_PollEvent(&next_event)) {
		if (next_event.type == SDL_QUIT) {
			// Finish the current frame?
			quit = true;
			running = false;
			break;
		}
		Input::ProcessEvent(next_event);
	}
}

void Engine::Update(SDL_Event& next_event)
{
	// onStart Components
	for (Actor* actor : SceneDB::OnStartActorQueue) {
		actor->Start();
	}
	SceneDB::OnStartActorQueue.clear();

	for (Actor* actor : SceneDB::OnUpdateActorQueue) {
		actor->Update();
	}

	for (Actor* actor : SceneDB::OnLateUpdateActorQueue) {
		actor->LateUpdate();
	}

	// Removals and updates
	SceneDB::ProcessActorUpdates();
}

void Engine::Render()
{
	Renderer::clear();
	// Render scene_space images
	Renderer::FlushImageAndRender();

	// Render UI images
	Renderer::FlushUIAndRender();

	// Render text
	Renderer::FlushTextAndRender();

	// Render pixels
	Renderer::FlushPixelAndRender();

	// Flip buffer
	Helper::SDL_RenderPresent(Renderer::renderer);
}



void Engine::GameLoop()
{
	while (running) {
		// Event handling loop
		if (SceneDB::change) {
			// Scene has to be changed
			SceneDB::LoadScene(SceneDB::next_scene_name);
		}
		SDL_Event next_event;
		Input(next_event);
		Update(next_event);
		Input::LateUpdate();
		EventBus::UpdateSubs();
		World::Step();
		Render();
	}
	if (quit) {
		exit(0);
	}
}


Engine::Engine() {
	// Setup Lua
	ComponentManager::Initialize();
	// Load templates
	TemplateDB::LoadTemplates();
	GetGameConfig();
	GetRenderConfig();
	StartRenderer();
	LoadImages();
	LoadText();
	LoadAudio();
	LoadInitialScene();
	Input::Init();
}

void Engine::Run() {
	GameLoop();
}