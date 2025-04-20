#include "TextDB.h"
#include <SDL2_ttf/SDL_ttf.h>
#include "EngineUtils.h"
#include <filesystem>
#include <string>
#include <iostream>

void TextDB::Initialize() {
	TTF_Init();
}

// Loads font
void TextDB::LoadFont(std::string font_name, int ptsize) {
	const std::string fonts_directory_path = "resources/fonts";
	bool font = false;
	if (EngineUtils::CheckPathExists(fonts_directory_path)) {
		// iterate and find font
		for (const auto& font_file : std::filesystem::directory_iterator(fonts_directory_path)) {
			std::string name = font_file.path().filename().stem().string();
			if (name == font_name) {
				std::string font_path = fonts_directory_path + "/" + name + ".ttf";
				// push font to map
				font = true;
				fonts[font_name][ptsize] = TTF_OpenFont(font_path.c_str(), ptsize);
			}
		}
	}
	if (font == false) {
		std::cout << "error: font " << font_name << " missing";
		exit(0);
	}
}


void TextDB::Draw(std::string str_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a) {
	TextRenderRequest req(x, y, str_content, font_name, font_size, r, g, b, a);
	Renderer::text_render_request_queue.emplace_back(req);
}