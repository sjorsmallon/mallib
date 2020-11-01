#version 450

layout(local_size_x = 1, local_size_y = 1) in;

// color_per_particle as vec3
// position_per_particle

layout(int64_t, binding = 0) uniform image2D img_output;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 render_texture_dimensions; // pixel x,y of framebuffer.

void main()
{

	// for each particle:
	vec4 clip_space_position = projection * view  * model * vec4(world_position, 1.0);
	// Frustum cull before perspective divide by checking of any of xyz are outside [-w, w].
	if (any(greaterThan(abs(clip_space_position.xyz), vec3(abs(clip_space_position.w))))) {
		return;
	}

	vec3 ndc_position = clip_space_position.xyz / clip_space_position.w;
	vec3 unscaled_window_coords = 0.5f * ndc_position + vec3(0.5f);

	// Technically this calculation should use the values set for glDepthRange and glViewport.
	vec3 windowCoords = vec3(render_texture_dimensions * unscaled_window_coords.xy,
		                     unscaled_window_coords.z);

	// Account for pixel centers being halfway between integers.
	ivec2 pixel_coords = clamp(ivec2(round(windowCoords.xy - vec2(0.5f))),
	                         ivec2(0), render_texture_dimensions - ivec2(1));

	// int64 destination = width * row_count + col_count; 
	// this assumes HALF float values.
	int64_t rgb_value_to_add = ((color.r << 43) | (color.g << 21) | color.b);
	atomicAdd(per_pixel_RGB_atomic_ints[], rgb_value_to_add);



}



// So in summary, the basic idea of the approach I took is to 
//   allocate a new buffer with 2 uints per pixel,
//   clear that buffer to 0’s every frame, 
//   and then run the compute shader described above to:
//   	- determine the pixel location of each particle (as well as its color), 
//   	- convert that color to uint, 
//  	-  and do atomicAdd’s on the 2 uints corresponding to that pixel.
//   	- Then do a full-screen triangle pass that reads the final uint values,
//   converts them back to float, and additively blends them onto the normal framebuffer.