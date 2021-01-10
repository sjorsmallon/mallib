#ifndef INCLUDED_ASSET_MANAGER_
#define INCLUDED_ASSET_MANAGER_

#include <tinyobj/tiny_obj_loader.h>
#include <string>
#include <glm/vec3.hpp>

//@FIXME(Sjors): proper float min/ max
#include <limits>
constexpr const float MIN_FLOAT = std::numeric_limits<float>::min();
constexpr const float MAX_FLOAT = std::numeric_limits<float>::max();

// use bounds for broad sweep?

struct Hitbox
{

};

struct Box_3D
{
	glm::vec3 min{MAX_FLOAT, MAX_FLOAT, MAX_FLOAT};
	glm::vec3 max{MIN_FLOAT, MIN_FLOAT, MIN_FLOAT};
};

struct obj_t
{
	// tiny obj things
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	// 
	std::vector<float> interleaved_XNU;
	std::vector<float> interleaved_XNTBU;
	// for meshes
	bool unitized = false;
	Box_3D bounds;
	Hitbox hitbox;
};

struct Asset_Manager
{	
	std::map<std::string, obj_t> meshes;
};


void set_asset_path(Asset_Manager& asset_manager, const char* asset_folder_prefix);

void load_obj(Asset_Manager& asset_manager, const std::string& obj_name, bool should_unitize = false);
const obj_t& get_obj(Asset_Manager& asset_manager, const std::string& obj_name);



#endif