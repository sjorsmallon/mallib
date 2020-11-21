#ifndef INCLUDED_PARTICLE_
#define INCLUDED_PARTICLE_

constexpr const int MAX_PARTICLE_COUNT = 1024;
constexpr const int MAX_PARTICLE_COUNT_4 = MAX_PARTICLE_COUNT / 4;

using vec3_4x = glm::vec4;

//@IC(Sjors) we use vec4 here for GPU alignment. However, the fields are meant to be used as vec3.
struct particle
{
	vec3_4x position;
	vec3_4x delta_position;
	vec3_4x delta_delta_position;
	vec3_4x color;
	vec3_4x delta_color;
	vec3_4x size;
};

struct Particle_System
{
	particle particles[MAX_PARTICLE_COUNT_4];
	// u32 next_particle_4;
	// bitmap_id BitmapID;
};

struct Particle_Cache
{
	// random_series particle_entropy;
	Particle_System gun_system;
		// Particle_Cache(Texture_Manager &texture_manager) :  { gun_system.bitmap_id = ....}

};





#endif