#ifndef INCLUDED_PARTICLE_
#define INCLUDED_PARTICLE_

constexpr const int MAX_PARTICLE_COUNT = 1024;
constexpr const int MAX_PARTICLE_COUNT_4 = MAX_PARTICLE_COUNT / 4;

//@TODO(Sjors): we use vec4 here for GPU alignment. Should we introduce a GPU_vec3?
// or use something like vec3_4x?
struct particle
{
	glm::vec4 position;
	glm::vec4 delta_position;
	glm::vec4 delta_delta_position;
	glm::vec4 color;
	glm::vec4 delta_color;
	glm::vec4 size;
}

struct Particle_System
{
	particle particles[MAX_PARTICLE_COUNT_4];
	// u32 next_particle_4;
	// bitmap_id BitmapID;

}

struct Particle_Cache
{
	// random_series particle_entropy;
	particle_system gun_system;
		// Particle_Cache(Texture_Manager &texture_manager) :  { gun_system.bitmap_id = ....}
	/
};





#endif