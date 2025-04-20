#include "Renderer.h"
#include "Helper.h"
#include "Actor.h"
#include "ImageDB.h"


ImageDrawRequest::ImageDrawRequest(std::string _img, float _x, float _y) :
	image_name(_img),
	x(_x),
	y(_y),
	rotation_degrees(0),
	scale_x(1.0f),
	scale_y(1.0f),
	pivot_x(0.5f),
	pivot_y(0.5f),
	r(255),
	g(255),
	b(255),
	a(255),
	sorting_order(0)
{
	tex = ImageDB::GetImage(_img);
	SDL_FRect tex_rect;
	Helper::SDL_QueryTexture(tex, &tex_rect.w, &tex_rect.h);
	width = tex_rect.w;
	height = tex_rect.h;
}

ImageDrawRequest::ImageDrawRequest(std::string _img, float _x, float _y, float _rotation_degrees,
	float _scale_x, float _scale_y, float _pivot_x, float _pivot_y,
	float _r, float _g, float _b, float _a, float _sorting_order) :
	image_name(_img),
	x(_x),
	y(_y),
	rotation_degrees(_rotation_degrees),
	scale_x(_scale_x),
	scale_y(_scale_y),
	pivot_x(_pivot_x),
	pivot_y(_pivot_y),
	r(_r),
	g(_g),
	b(_b),
	a(_a),
	sorting_order(_sorting_order)
{
	tex = ImageDB::GetImage(_img);
	SDL_FRect tex_rect;
	Helper::SDL_QueryTexture(tex, &tex_rect.w, &tex_rect.h);
	width = tex_rect.w;
	height = tex_rect.h;
}

void Renderer::makeWindow() {
	window = Helper::SDL_CreateWindow(game_title.c_str(), 10, 10, window_size.x, window_size.y, SDL_WINDOW_SHOWN);
}

void Renderer::makeRenderer() {
	renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
}

void Renderer::clear() {
	SDL_SetRenderDrawColor(renderer, clear_color_r, clear_color_g, clear_color_b, clear_color_a);
	SDL_RenderClear(renderer);
}

void Renderer::drawImage(SDL_Texture* texture, int x, int y, int width, int height) {
	//SDL_FRect* rect;
	//rect->x = x;
	//rect->y = y;
	//rect->w = width;
	//rect->h = height;

	Helper::SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}

void Renderer::drawImage(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst) {
	Helper::SDL_RenderCopy(renderer, texture, &src, &dst);
}

void Renderer::drawImage(SDL_Texture* texture, SDL_FRect dst) {
	Helper::SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

void Renderer::SetScale(float zoom) {
	SDL_RenderSetScale(renderer, zoom, zoom);
}

void Renderer::RenderActors(std::vector<Actor*>& actors) {
}

// Flushes all text in the text requests and renders
void Renderer::FlushTextAndRender() {
	for (TextRenderRequest& req : text_render_request_queue) {
		if (TextDB::fonts.count(req.font_name) == 0 || TextDB::fonts[req.font_name].count(req.font_size) == 0) {
			// we need to load the font first
			TextDB::LoadFont(req.font_name, req.font_size);
		}
		SDL_Surface* surface = TTF_RenderText_Solid(TextDB::fonts[req.font_name][req.font_size], req.str_content.c_str(), {req.r, req.g, req.b, req.a});
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FRect textRect = { static_cast<float>(req.x), static_cast<float>(req.y), static_cast<float>(surface->w), static_cast<float>(surface->h) };

		drawImage(textTexture, textRect);
	}
	text_render_request_queue.clear();
	//SDL_FreeSurface(surface);
}

void Renderer::FlushUIAndRender() {
	// Sort requests by order, break ties via order calls occur
	std::stable_sort(ui_render_request_queue.begin(), ui_render_request_queue.end(), [](const UIRenderRequest& a, const UIRenderRequest& b) {
		return a.sorting_order < b.sorting_order; });
	for (UIRenderRequest& req : ui_render_request_queue) {
		SDL_Texture* tex = ImageDB::GetImage(req.image_name);

		// Set modifications
		SDL_SetTextureColorMod(tex, req.r, req.g, req.b);
		SDL_SetTextureAlphaMod(tex, req.a);

		float w, h;
		Helper::SDL_QueryTexture(tex, &w, &h);
		//req.width = static_cast<int>(w);
		//req.height = static_cast<int>(h);

		SDL_FRect dst = { static_cast<float>(req.x), static_cast<float>(req.y), w, h };

		Helper::SDL_RenderCopy(renderer, tex, nullptr, &dst);

		// Reset modifications
		ResetMods(tex);
	}
	ui_render_request_queue.clear();
}

void Renderer::FlushImageAndRender() {
	SDL_FRect viewport = { 0, 0, Renderer::window_size.x, Renderer::window_size.y };
	image_draw_request_queue.erase(std::remove_if(image_draw_request_queue.begin(), image_draw_request_queue.end(), [&viewport](const ImageDrawRequest& req) {
		// Compute adjusted bounding box based on pivot
		float xMin = req.x - (req.width * req.pivot_x);
		float xMax = req.x + (req.width * (1.0f - req.pivot_x));
		float yMin = req.y - (req.height * req.pivot_y);
		float yMax = req.y + (req.height * (1.0f - req.pivot_y));

		return xMax < viewport.x || xMin > viewport.x + viewport.w ||
			yMax < viewport.y || yMin > viewport.y + viewport.h;
		}), image_draw_request_queue.end());

	std::stable_sort(image_draw_request_queue.begin(), image_draw_request_queue.end(), [](const ImageDrawRequest& a, const ImageDrawRequest& b) {
		return a.sorting_order < b.sorting_order; });

	float zoom_factor = Renderer::zoom_factor;
	SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);

	// using instructor solution
	for (ImageDrawRequest& req : image_draw_request_queue) {
		const float pixels_per_meter = 100;
		glm::vec2 final_rendering_position = glm::vec2(req.x, req.y) - current_cam_pos;

		SDL_FRect tex_rect;
		tex_rect.w = req.width;
		tex_rect.h = req.height;

		// Apply scale
		int flip_mode = SDL_FLIP_NONE;
		if (req.scale_x < 0) flip_mode |= SDL_FLIP_HORIZONTAL;
		if (req.scale_y < 0) flip_mode |= SDL_FLIP_VERTICAL;

		float x_scale = glm::abs(req.scale_x);
		float y_scale = glm::abs(req.scale_y);

		tex_rect.w *= x_scale;
		tex_rect.h *= y_scale;

		SDL_FPoint pivot_point = { (req.pivot_x * tex_rect.w), (req.pivot_y * tex_rect.h) };

		// TODO: check if this is correct
		glm::ivec2 cam_dimensions = Renderer::window_size;

		// TODO: check if we have to put back the static cast
		tex_rect.x = (final_rendering_position.x * pixels_per_meter + cam_dimensions.x * 0.5f * (1.0f / zoom_factor) - pivot_point.x);
		tex_rect.y = (final_rendering_position.y * pixels_per_meter + cam_dimensions.y * 0.5f * (1.0f / zoom_factor) - pivot_point.y);

		// Apply tint and alpha to texture
		SDL_SetTextureColorMod(req.tex, req.r, req.g, req.b);
		SDL_SetTextureAlphaMod(req.tex, req.a);


		// Draw image
		Helper::SDL_RenderCopyEx(0, "", renderer, req.tex, NULL, &tex_rect, req.rotation_degrees, &pivot_point, static_cast<SDL_RendererFlip>(flip_mode));
		
		SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);

		// Remove tint and alpha
		ResetMods(req.tex);
	}

	SDL_RenderSetScale(renderer, 1, 1);

	image_draw_request_queue.clear();
}

void Renderer::FlushPixelAndRender() {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for (PixelDrawRequest& req : pixel_draw_request_queue) {
		SDL_SetRenderDrawColor(renderer, req.r, req.g, req.b, req.a);
		SDL_RenderDrawPoint(renderer, req.x, req.y);
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	pixel_draw_request_queue.clear();
}


void Renderer::ResetMods(SDL_Texture* texture) {
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

void Renderer::DrawUI(std::string image_name, float x, float y) {
	UIRenderRequest req(image_name, x, y);
	ui_render_request_queue.emplace_back(req);
 }


void Renderer::DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order) {
	UIRenderRequest req(image_name, x, y, r, g, b, a, sorting_order);
	ui_render_request_queue.emplace_back(req);
 }

void Renderer::Draw(std::string image_name, float x, float y) {
	ImageDrawRequest req(image_name, x, y);
	image_draw_request_queue.emplace_back(req);
}

void Renderer::DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order) {
	ImageDrawRequest req(image_name, x, y, rotation_degrees, scale_x, scale_y, pivot_x, pivot_y, r, g, b, a, sorting_order);
	image_draw_request_queue.emplace_back(req);
}

void Renderer::DrawPixel(float x, float y, float r, float g, float b, float a) {
	PixelDrawRequest req(x, y, r, g, b, a);
	pixel_draw_request_queue.emplace_back(req);
}

// ---------[ LUA CAM ] -------
	// Sets position of camera
void Renderer::SetPosition(float x, float y) {
	current_cam_pos.x = x;
	current_cam_pos.y = y;
}

// Returns float of cam pos x
float Renderer::GetPositionX() {
	return current_cam_pos.x;
}

// Returns float of cam pos y
float Renderer::GetPositionY() {
	return current_cam_pos.y;
}

// Sets zoom factor
void Renderer::SetZoom(float zoom_factor_in) {
	zoom_factor = zoom_factor_in;
}

// Returns float of zoom factor
float Renderer::GetZoom() {
	return zoom_factor;
}