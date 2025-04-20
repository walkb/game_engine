#include "ParticleSystem.h"
#include "ImageDB.h"

// --- LIFECYCLE FUNCTIONS ---

void ParticleSystem::OnStart() {
	ImageDB::CreateDefaultParticleTextureWithName(DEFAULT_PARTICLE_NAME);

	emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
	emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
	scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);
	rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
	speed_distribution = RandomEngine(start_speed_min, start_speed_max, 498);
	rotation_speed_distribution = RandomEngine(rotation_speed_min, rotation_speed_max, 305);

	if (image == "") {
		image = DEFAULT_PARTICLE_NAME;
	}
}

void ParticleSystem::OnUpdate() {
	if (local_frame_number % frames_between_bursts == 0 && emission_allowed) {
		Burst();
	}
	// Render all new, existing, and active particles with pivot point 0.5f, 0.5f, sorting order 9999, scale 1.0f, and tint 255, 255, 255, 255
	Render();
	local_frame_number++;
}

// --- LUA FUNCTIONS ---

void ParticleSystem::Stop() {
	emission_allowed = false;
}

void ParticleSystem::Play() {
	emission_allowed = true;
}

void ParticleSystem::LuaBurst() {
	Burst();
}

// --- HELPER FUNCTIONS ---

void ParticleSystem::Burst() {
	// Use polar coordinates to calculate random values
	for (int i = 0; i < burst_quantity; i++) {
		float angle_radians = glm::radians(emit_angle_distribution.Sample());
		float radius = emit_radius_distribution.Sample();

		float cos_angle = glm::cos(angle_radians);
		float sin_angle = glm::sin(angle_radians);

		float start_x_pos = x + cos_angle * radius;
		float start_y_pos = y + sin_angle * radius;

		float rotation = glm::radians(rotation_distribution.Sample());

		float scale = scale_distribution.Sample();

		float speed = speed_distribution.Sample();
		float start_x_vel = cos_angle * speed;
		float start_y_vel = sin_angle * speed;

		float rotation_speed = rotation_speed_distribution.Sample();

		// Add to vectors
		Color c = Color(start_color_r, start_color_g, start_color_b, start_color_a);
		AddToVectors(start_x_pos, start_y_pos, rotation, scale, c, start_x_vel, start_y_vel, rotation_speed);
	}
}

void ParticleSystem::ProcessParticle(int index) {
	// Apply gravity to velocity
	x_vel_values[index] += gravity_scale_x;
	y_vel_values[index] += gravity_scale_y;

	// Apply drag to velocity and angular drag to angular velocity
	x_vel_values[index] *= drag_factor;
	y_vel_values[index] *= drag_factor;

	angular_vel_values[index] *= angular_drag_factor;

	// Apply velocities to position and rotation

	x_values[index] += x_vel_values[index];
	y_values[index] += y_vel_values[index];

	rotation_values[index] += angular_vel_values[index];

	// Process color and scale
	float lifetime_progress = static_cast<float>(static_cast<float>(local_frame_number - start_frames[index]) / duration_frames);
	if (has_end_scale) {
		scale_values[index] = glm::mix(init_scale_values[index], end_scale, lifetime_progress);
	}
	if (has_end_color) {
		uint8_t r = glm::mix(init_color_values[index].r, end_color_r, lifetime_progress);
		uint8_t g = glm::mix(init_color_values[index].g, end_color_g, lifetime_progress);
		uint8_t b = glm::mix(init_color_values[index].b, end_color_b, lifetime_progress);
		uint8_t a = glm::mix(init_color_values[index].a, end_color_a, lifetime_progress);
		color_values[index] = Color(r, g, b, a);
	}
}

void ParticleSystem::Render() {
	int num_particles = is_active.size();
	for (int i = 0; i < num_particles; i++) {
		// Process and render every particle
		if (is_active[i] == false) {
			continue;
		}
		if (local_frame_number - start_frames[i] >= duration_frames) {
			is_active[i] = false;
			freed_indices.push(i);
			continue;
		}
		ProcessParticle(i);
		Renderer::DrawEx(image, x_values[i], y_values[i], rotation_values[i], scale_values[i], scale_values[i], 0.5f, 0.5f, color_values[i].r, color_values[i].g, color_values[i].b, color_values[i].a, sorting_order);
	}
}

void ParticleSystem::AddToVectors(float x, float y, float rotation, float scale, Color& color, float x_vel, float y_vel, float ang_vel) {
	if (freed_indices.empty()) {
		is_active.push_back(true);
		start_frames.push_back(local_frame_number);
		x_values.push_back(x);
		y_values.push_back(y);
		rotation_values.push_back(rotation);
		scale_values.push_back(scale);
		init_scale_values.push_back(scale);
		init_color_values.emplace_back(color);
		color_values.push_back(color);
		x_vel_values.push_back(x_vel);
		y_vel_values.push_back(y_vel);
		angular_vel_values.push_back(ang_vel);
	}
	else {
		int index = freed_indices.front();
		freed_indices.pop();

		// overwrite old particle
		is_active[index] = true;
		start_frames[index] = local_frame_number;
		x_values[index] = x;
		y_values[index] = y;
		rotation_values[index] = rotation;
		init_scale_values[index] = scale;
		scale_values[index] = scale;
		init_color_values[index] = color;
		color_values[index] = color;
		x_vel_values[index] = x_vel;
		y_vel_values[index] = y_vel;
		angular_vel_values[index] = ang_vel;
	}
}

void ParticleSystem::AddParticleToVectors(Particle& p) {
	if (freed_indices.empty()) {
		is_active.push_back(true);
		start_frames.push_back(local_frame_number);
		x_values.push_back(p.start_x);
		y_values.push_back(p.start_y);
		rotation_values.push_back(p.rotation);
		scale_values.push_back(p.start_scale);
		init_scale_values.push_back(p.start_scale);
		init_color_values.push_back(p.color);
		color_values.push_back(p.color);
		x_vel_values.push_back(p.x_vel);
		y_vel_values.push_back(p.y_vel);
		angular_vel_values.push_back(p.angular_vel);
	}
	else {
		int index = freed_indices.front();
		freed_indices.pop();

		// overwrite old particle
		is_active[index] = true;
		start_frames[index] = local_frame_number;
		x_values[index] = p.start_x;
		y_values[index] = p.start_y;
		rotation_values[index] = p.rotation;
		init_scale_values[index] = p.start_scale;
		scale_values[index] = p.start_scale;
		init_color_values[index] = p.color;
		color_values[index] = p.color;
		x_vel_values[index] = p.x_vel;
		y_vel_values[index] = p.y_vel;
		angular_vel_values[index] = p.angular_vel;
	}
}