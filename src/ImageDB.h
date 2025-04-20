#pragma once
#include "SDL2/SDL.h"
#include <unordered_map>
#include <string>
#include "rapidjson/document.h"
#include "glm/glm.hpp"
#include "Renderer.h"
#include <vector>


class ImageDB
{
public:
	// ImageDB loads images, retrieves images, and stores them for later
	static std::vector<std::string> introImages;
	static inline SDL_Texture* game_loss = nullptr;
	static inline SDL_Texture* game_win = nullptr;
	static inline SDL_Texture* hp = nullptr;
	static inline glm::vec2 hp_size = glm::vec2(0, 0);

	static inline std::unordered_map<std::string, SDL_Texture*> images;

	// Reads in all textures in the resources folder.
	static void Initialize();

	// Returns a pointer to the texture or a nullptr if not found in the database.
	static SDL_Texture* GetImage(std::string name);

	// A deprecated function from earlier versions of the engine
	static void SetIntroImages(rapidjson::GenericArray<false, rapidjson::Value>& images);

	// A deprecated function from earlier versions of the engine
	static void SetWinImage(std::string img_name);

	// A deprecated function from earlier versions of the engine
	static void SetLossImage(std::string img_name);

	// A deprecated function from earlier versions of the engine
	static void SetHPImage(std::string img_name);

	// Creates a default texture for the particle system and loads it into the database.
	static void CreateDefaultParticleTextureWithName(const std::string& name);

	//static std::pair<bool, SDL_Texture*> NextIntroImage(int index);
};

