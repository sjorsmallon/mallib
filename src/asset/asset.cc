#include "asset.h"
#include "../file/file.h"
#include "fmt/core.h"

//@Incomplete
void asset::load_model_from_obj(std::string& filename)
{
}

//@Refactor: this naming convention is backwards with the parameter order. can we fix that?
//@Note: assume an empty scene here.
void asset:load_scene_from_file(const std::string& filename, const Scene& scene)
{
	auto file_content = std::string();
    file::file_to_string(filename, file_content);
    std::stringstream data_stream(file_content);

    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[max_string_length] = {0}; // used for the garbage at the start of each line. 


    // @Memory: how to predict how to preallocate?
    fmt::print("[asset] Warning: load_scene_from_file does not efficiently preallocate.\n");
    scene.set_pieces.reserve(10);
    // we use a pointer here so the logic for creating new set pieces 
    // is more concise.
    auto* set_piece = nullptr;
    size_t line_number = 0;

    for (std::string line; std::getline(data_stream, line);)
    {
    	line_number++;
    	if (line[0] == '#') // comment
    	{
    		continue;
    	}    	
    	if (line[0] == ':' && line[1] == '/') // name of the object in the scene.
    	{    		
    		scene.set_pieces.emplace_back();
    		set_piece = &scene.set_pieces.back();
    		auto name = std::string(100);
    		sscanf(line.c_str(), "%2s %50s", garbage_buffer, name);
    		set_piece->name = name;
    	}

    	else if (line[0] == 'm' && line[1] == 'o') // model
    	{
    		auto model_name = std::string(100);
    		sscanf(line.c_str(), "%15s %50s", garbage_buffer, model_name);
    		set_piece->model_name = model_name;
    	}
    	else if (line[0] == 'm' && line[1] == 'a') // material
    	{
    		auto material_name = std::string(100);
    		sscanf(line.c_str(), "%15s %50s", garbage_buffer, material_name);
    		set_piece->material_name = material_name;
    	}
    	else if (line[0] == 't' && line[1] == 'e') // texture
    	{
    		auto texture_name = std::string(100);
    		sscanf(line.c_str(), "%15s %50s", garbage_buffer, texture_name);
    		set_piece->texture_name = texture_name;
    	}
    	else if (line[0] == 'p' && line[1] == '0') // position
    	{
    		Vec3 position = {};
    		sscanf(line.c_str(), "%15s %f %f %f", garbage_buffer, &position.x, &position.y, &position.z);
    		set_piece->xform_state.position = position;
    	}
    	else if (line[0] == 'q' && line[1] == '_') // q_orientation
    	{
    		Vec4 q_orientation = {};
    		sscanf(line.c_str(), "%15s %f %f %f %f", garbage_buffer, &q_orientation.x, &q_orientation.y, &q_orientation.z, &q_orientation.w);
    		set_piece->xform_state.q_orientation = q_orientation;
    	}
    	else if (line[0] == 's' && line[1] == 'c') // scale
    	{
    		float scale = 0.0f;
    		sscanf(line.c_str(), "%15s %f", garbage_buffer, &scale);
    		set_piece->xform_state.scale = scale;
    	}
    	else
    	{
    		fmt::print("[asset] Warning. file {}: No label recognized on line {}. content: {}\n",filename, line_number, line);
    	}
    }
}