#ifndef INCLUDED_ASSET_MANAGER_
#define INCLUDED_ASSET_MANAGER_

#include <tinyobj/tiny_obj_loader.h>
#include <string>

struct obj_t
{
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	std::vector<float> Vertex_XNU;
};

struct Asset_Manager
{	
	std::map<std::string, obj_t> meshes;
};


void load_obj(Asset_Manager& asset_manager, const std::string& obj_name);


#endif