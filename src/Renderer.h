#pragma once
#include <string>
#include <iostream>
#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include <vector>
#include "SDL2_ttf/SDL_ttf.h"
#include "TextDB.h"
#include <deque>
#include "Helper.h"


struct DrawRequest {
	int x;
	int y;
	int width;
	int height;
};

//// Forward declaration of Actor class
class Actor;

struct ImageDrawRequest : public DrawRequest {
	std::string image_name;
	float x;
	float y;
	int rotation_degrees;
	float scale_x;
	float scale_y;
	float pivot_x;
	float pivot_y;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	int sorting_order;
	SDL_Texture* tex;

	ImageDrawRequest(std::string _img, float _x, float _y);

	ImageDrawRequest(std::string _img, float _x, float _y, float _rotation_degrees,
		float _scale_x, float _scale_y, float _pivot_x, float _pivot_y,
		float _r, float _g, float _b, float _a, float _sorting_order);
};

struct TextRenderRequest : public DrawRequest {
	std::string str_content;
	std::string font_name;
	uint32_t font_size;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	TextRenderRequest(float _x, float _y, std::string _str_content, std::string _font_name, float _font_size, float _r, float _g, float _b, float _a) :
		DrawRequest{ static_cast<int>(_x), static_cast<int>(_y), 0, 0 },
		str_content(_str_content), 
		font_name(_font_name), 
		font_size(static_cast<uint32_t>(_font_size)),
		r(static_cast<uint8_t>(_r)), 
		g(static_cast<uint8_t>(_g)), 
		b(static_cast<uint8_t>(_b)),
		a(static_cast<uint8_t>(_a)) {}
};

struct UIRenderRequest : public DrawRequest {
	std::string image_name;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	int sorting_order;
	UIRenderRequest(std::string _img, float _x, float _y) : image_name(_img), r(255), g(255), b(255), a(255), sorting_order(0), DrawRequest{ static_cast<int>(_x), static_cast<int>(_y) } {}
	UIRenderRequest(std::string _img, float _x, float _y, float _r, float _g, float _b, float _a, float _order) : image_name(_img), r(static_cast<int>(_r)), g(static_cast<int>(_g)), b(static_cast<int>(_b)), a(static_cast<int>(_a)), sorting_order(static_cast<int>(_order)), DrawRequest{ static_cast<int>(_x), static_cast<int>(_y) } {}
};

struct PixelDrawRequest : public DrawRequest {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	PixelDrawRequest(float _x, float _y, float _r, float _g, float _b, float _a) :
		DrawRequest{ static_cast<int>(_x), static_cast<int>(_y) },
		r(static_cast<int>(_r)),
		g(static_cast<int>(_g)),
		b(static_cast<int>(_b)),
		a(static_cast<int>(_a)) {};
};

//struct LineDrawRequest : public DrawRequest {
//
//};

class Renderer
{
public:
	static void RenderActors(std::vector<Actor*>& actors);
	static void makeWindow();
	static void makeRenderer();

	// Resets render draw color and clears renderer to specified color in config
	static void clear();


	static void SetScale(float zoom);
	static void drawImage(SDL_Texture* image, int x, int y, int width, int height);
	static void drawImage(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst);
	static void drawImage(SDL_Texture* texture, SDL_FRect dst);


	// Flushes all text in the text_render_request_queue
	static void FlushTextAndRender();

	// Flushes all UI requests in queue
	static void FlushUIAndRender();

	// Flushes all Image requests in queue
	static void FlushImageAndRender();

	// Flushes all Pixel requests in queue
	static void FlushPixelAndRender();

	// Reset modifications to color and transparency on a texture
	static void ResetMods(SDL_Texture* texture);

	// --- [ LUA DRAW FUNCTIONS ] ----

	// Draws an image to UI via screen coordinates, rendering in sorting order, breaking ties via order calls occurred.
	static void DrawUI(std::string image_name, float x, float y);

	// Draws an image to UI via screen coordinates, rendering in sorting order, breaking ties via order calls occurred.
	static void DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order);

	// Draws an image via scene coordinates (affected by cam, zoom)
	static void Draw(std::string image_name, float x, float y);

	// Extended version of Draw
	static void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);

	// Draws a single pixel
	static void DrawPixel(float x, float y, float r, float g, float b, float a);

	// -------[ LUA CAMERA FUNCTIONS ] ---------


	// Sets position of camera
	static void SetPosition(float x, float y);

	// Returns float of cam pos x
	static float GetPositionX();

	// Returns float of cam pos y
	static float GetPositionY();

	// Sets zoom factor
	static void SetZoom(float zoom_factor);

	// Returns float of zoom factor
	static float GetZoom();

	// -------[ TODO: SORT CUZ IDK YET ] -----------

	inline static SDL_Window* window = nullptr;
	inline static SDL_Renderer* renderer = nullptr;

	inline static glm::vec2 window_position = glm::vec2(0, 30);
	inline static glm::vec2 window_size = glm::ivec2(640, 360);

	inline static glm::vec2 current_cam_pos = glm::vec2(0.0f, 0.0f);

	inline static float zoom_factor = 1.0f;

	inline static Uint8 clear_color_r = 255;
	inline static Uint8 clear_color_g = 255;
	inline static Uint8 clear_color_b = 255;
	inline static Uint8 clear_color_a = 255;

	inline static std::string game_title = "";

	// ---- [ DRAW REQUEST QUEUES ] ----

	inline static std::deque<ImageDrawRequest> image_draw_request_queue;
	inline static std::deque<TextRenderRequest> text_render_request_queue;
	inline static std::deque<UIRenderRequest> ui_render_request_queue;
	inline static std::deque<PixelDrawRequest> pixel_draw_request_queue;
	//inline static std::deque<LineDrawRequest> line_draw_request_queue;


};

