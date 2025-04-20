#pragma once
#include <string>
#include <queue>
#include <vector>
#include <optional>
#include "Actor.h"
#include "Helper.h"


struct Color {
	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;
	uint8_t a = 255;
	Color(uint8_t _r = 255, uint8_t _g = 255, uint8_t _b = 255, uint8_t _a = 255) : r(_r), g(_g), b(_b), a(_a) {};
};

struct Particle {
	float start_x = 0.0f;
	float start_y = 0.0f;
	float rotation = 0.0f;
	float start_scale = 1.0f;
	Color color = Color(255, 255, 255, 255);
	float x_vel = 0.0f;
	float y_vel = 0.0f;
	float angular_vel = 0.0f;

	Particle(float x_in = 0.0f, float y_in = 0.0f, float rotate_in = 0.0f, float start_scale_in = 1.0f, Color color_in = Color(255, 255, 255, 255), float x_vel_in = 0.0f, float y_vel_in = 0.0f, float r_speed_in = 0.0f) : start_x(x_in), start_y(y_in), rotation(rotate_in), start_scale(start_scale_in), color(color_in), x_vel(x_vel_in), y_vel(y_vel_in), angular_vel(r_speed_in) {}
};


class ParticleSystem
{
public:
	// Used to determine whether or not functions should fire in the frame
	bool enabled = true;

	// Used to determine whether the burst should occur
	bool emission_allowed = true;

	// Helper reference to owner
	Actor* actor = nullptr;

	// Component variables
	std::string key = "???";
	std::string type = "Rigidbody";

	const std::string DEFAULT_PARTICLE_NAME = "default_particle";

	// Initial ParticleSystem variables
	float x = 0.0f;
	float y = 0.0f;

	int local_frame_number = 0;

	int frames_between_bursts = 1;
	int burst_quantity = 1;
	int duration_frames = 300;

	std::string image = "";

	float start_scale_min = 1.0f;
	float start_scale_max = 1.0f;

	float rotation_min = 0.0f;
	float rotation_max = 0.0f;
	
	float emit_angle_min = 0.0f;
	float emit_angle_max = 360.0f;
	
	float emit_radius_min = 0.0f;
	float emit_radius_max = 0.5f;

	float start_speed_min = 0.0f;
	float start_speed_max = 0.0f;

	float rotation_speed_min = 0.0f;
	float rotation_speed_max = 0.0f;

	float gravity_scale_x = 0.0f;
	float gravity_scale_y = 0.0f;

	float drag_factor = 1.0f;
	float angular_drag_factor = 1.0f;

	float end_scale = 0.0f;
	bool has_end_scale = false;

	uint8_t start_color_r = 255;
	uint8_t start_color_g = 255;
	uint8_t start_color_b = 255;
	uint8_t start_color_a = 255;

	float has_end_color = false;
	uint8_t end_color_r = 255;
	uint8_t end_color_g = 255;
	uint8_t end_color_b = 255;
	uint8_t end_color_a = 255;

	// Helper engine objects for seeded randomness (Helper.h)
	RandomEngine emit_angle_distribution; // 298
	RandomEngine emit_radius_distribution; // 404

	RandomEngine rotation_distribution; // 440
	RandomEngine scale_distribution; // 494

	RandomEngine speed_distribution; // 498
	RandomEngine rotation_speed_distribution; // 305

	// Number of particles spawned per burst
	uint32_t burst_particles = 1;

	int sorting_order = 9999;

	// Memory queue
	std::queue<int> freed_indices;

	// Data-oriented vectors
	std::vector<bool> is_active;
	std::vector<int> start_frames;
	std::vector<float> x_values;
	std::vector<float> y_values;
	std::vector<float> rotation_values;
	std::vector<float> init_scale_values;
	std::vector<float> scale_values;
	std::vector<Color> color_values;
	std::vector<Color> init_color_values;
	std::vector<float> x_vel_values;
	std::vector<float> y_vel_values;
	std::vector<float> angular_vel_values;


	// --- [ LIFE CYCLE FUNCTIONS ] ---

	// Runs on component creation
	void OnStart();

	// Runs every update that this component is enabled
	void OnUpdate();

	// Runs when component is removed or parent actor is destroyed
	void OnDestroy();

	// --- [ LUA FUNCTIONS ] ---

	// Stops generating particles in burst stage
	void Stop();

	// Resumes generating particles in burst stage
	void Play();

	// Immediately bursts
	void LuaBurst();


private:
	// --- [ HELPER FUNCTIONS ] ---

	// Emits a burst of particles based on number of particles per burst
	void Burst();

	// Renders all new, current, and active particles.
	void Render();

	void AddToVectors(float x, float y, float rotation, float scale, Color& color, float x_vel, float y_vel, float ang_vel);

	// Destructs a particle struct into all necessary particle vectors
	void AddParticleToVectors(Particle& p);

    // Deprecated function as it felt unnecessary to construct a particle just to deconstruct it.
	void ProcessParticle(int index);
};

