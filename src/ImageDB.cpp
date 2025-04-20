#include "ImageDB.h"
#include <filesystem>
#include "SDL2_image/SDL_image.h"
#include <iostream>
#include "EngineUtils.h"
#include "Helper.h"

std::vector<std::string> ImageDB::introImages;

void ImageDB::Initialize() {
	// iterate through all loadable assets and load them
	const std::string images_directory_path = "resources/images";
	if (std::filesystem::exists(images_directory_path)) {
		for (const auto& image : std::filesystem::directory_iterator(images_directory_path)) {
			// load images
			std::string name = image.path().filename().stem().string();
			std::string image_location = images_directory_path + "/" + name + ".png";
			SDL_Texture* img = IMG_LoadTexture(Renderer::renderer, image_location.c_str());
			if (img == nullptr) {
				std::cout << "Unable to load images from resources/images, " << IMG_GetError();
				exit(0);
			}
			images[name] = img;
		}
	}
}

SDL_Texture* ImageDB::GetImage(std::string name) {
	auto itr = images.find(name);
	if (itr == images.end()) {
		return nullptr;
	}
	return itr->second;
}

void ImageDB::SetHPImage(std::string img_name) {
	SDL_Texture* img = GetImage(img_name);
	if (img == nullptr) {
		std::cout << "Could not set HP image";
	}
	else {
		hp = img;
		float width = 0;
		float height = 0;
		Helper::SDL_QueryTexture(hp, &width, &height);
		hp_size = glm::vec2(width, height);
	}
}


void ImageDB::SetIntroImages(rapidjson::GenericArray<false, rapidjson::Value>& images) {
	for (auto& image : images) {
		std::string name = image.GetString();
		std::string url = "resources/images/" + name + ".png";
		if (!EngineUtils::CheckPathExists(url)) {
			std::cout << "error: missing image " << name;
			exit(0);
		}
		introImages.push_back(name);
	}
}

void ImageDB::SetWinImage(std::string img_name) {
	SDL_Texture* img = GetImage(img_name);
	if (img == nullptr) {
		std::cout << "Could not set win image";
	}
	else {
		game_win = img;
	}
}

void ImageDB::SetLossImage(std::string img_name) {
	SDL_Texture* img = GetImage(img_name);
	if (img == nullptr) {
		std::cout << "Could not set loss image";
	}
	else {
		game_loss = img;
	}
}

void ImageDB::CreateDefaultParticleTextureWithName(const std::string& name) {
	// Check if this texture has been created already
	if (images.find(name) != images.end()) {
		return;
	}

	// Create a new SDL_Surface with no flags, 8x8 width and height, 32 bits of color depth (RGBA) and no masking.
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);

	// Set the color to white
	Uint32 white_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
	SDL_FillRect(surface, NULL, white_color);

	// Create a gpu-side texture from the surface
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer::renderer, surface);

	// Clean up the surface and cache the texture for future default particle spawning
	SDL_FreeSurface(surface);
	images[name] = texture;
}