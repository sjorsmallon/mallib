#version 450

uniform sampler2D input_depth_buffer;
uniform restrict writeonly image2DArray output_downsampled_min_depth_buffer_image;
// The dimension in normalized texture coordinate space of a single texel in
// input_depth_buffer.
uniform vec2 texel_dimensions;

// Resulting image is 1/16th x 1/16th resolution, but we fetch 4x4 texels per thread, hence
// the divisions by 4 here.
layout(local_size_x = 16/4, local_size_y = 16/4, local_size_z = 1) in;

// This is stored as uint because atomicMin only works on integer types. Luckily
// (non-negative) floats maintain their order when their bits are interpreted as uint (using
// floatBitsToUint).
shared uint work_group_min_depth_encoded_as_uint;

void main() {
	if (gl_LocalInvocationIndex == 0) {
        // Initialize to 1.0 (max depth) before performing atomicMin's.
		work_group_min_depth_encoded_as_uint = floatBitsToUint(1.0);
	}

	memoryBarrierShared();
	barrier();

	// Fetch a 4x4 texel region per thread with 4 calls to textureGather. 'gather_coords_'
    // are set up to be equidistant from the centers of the 4 texels being gathered (which
    // puts them on integer values). In my tests textureGather was not faster than 
    // individually fetching each texel - I use it here only for conciseness.
    //
    // Note that in the case of the full-res depth buffer's dimensions not being evenly 
    // divisible by the downscaling factor (16), these textureGather's may try to fetch 
    // out-of-bounds coordinates - that's fine as long as the texture sampler is set to 
    // clamp-to-edge, as redundant values don't affect the resulting min.

	uvec2 base_texel_coords = 4 * gl_GlobalInvocationID.xy;
	vec2 gather_coords_1 = (base_texel_coords + uvec2(1, 1)) * texel_dimensions;
	vec2 gather_coords_2 = (base_texel_coords + uvec2(3, 1)) * texel_dimensions;
	vec2 gather_coords_3 = (base_texel_coords + uvec2(1, 3)) * texel_dimensions;
	vec2 gather_coords_4 = (base_texel_coords + uvec2(3, 3)) * texel_dimensions;

	vec4 gathered_texel_values1 = textureGather(input_depth_buffer, gather_coords_1);
	vec4 gathered_texel_values2 = textureGather(input_depth_buffer, gather_coords_2);
	vec4 gathered_texel_values3 = textureGather(input_depth_buffer, gather_coords_3);
	vec4 gathered_texel_values4 = textureGather(input_depth_buffer, gather_coords_4);

	// Now find the min across the 4x4 region fetched, and apply that to the workgroup min
    // using atomicMin.
	vec4 gathered_texel_mins = min(min(gathered_texel_values1, gathered_texel_values2),
                                 min(gathered_texel_values3, gathered_texel_values4));
	float final_min = min(min(gathered_texel_mins.x, gathered_texel_mins.y),
                         min(gathered_texel_mins.z, gathered_texel_mins.w));
	atomicMin(work_group_min_depth_encoded_as_uint, floatBitsToUint(final_min));

	memoryBarrierShared();
	barrier();

    // Thread 0 writes workgroup-wide min to image.
	if (gl_LocalInvocationIndex == 0) {
		float work_group_min_depth = uintBitsToFloat(work_group_min_depth_encoded_as_uint);

		imageStore(output_downsampled_min_depth_buffer_image,
		           ivec2(gl_WorkGroupID.xy),
                   // imageStore can only be passed vec4, but only a float is stored.
				   vec4(work_group_min_depth));
	}
}