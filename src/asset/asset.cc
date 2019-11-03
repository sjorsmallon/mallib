#include "asset.h"
#include "../file/file.h"

//@Incomplete
void asset::load_model_from_obj(std::string& filename)
{
}

//@Refactor: this naming convention is backwards with the parameter order. can we fix that?
void asset:load_scene_from_file(const std::string& filename, const Scene& scene)
{
	auto file_content = std::string();
	file::file_to_string()()
}