#include "asset_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

#include "log.h"

namespace
{
	const std::string g_obj_extension_suffix{".obj"};

	struct obj_t
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;
	};
}



void load_obj(Asset_Manager& asset_manager, const std::string& obj_name)
{
	const std::string obj_folder_prefix = "../assets/obj/";
	const std::string model_path = obj_folder_prefix + obj_name + g_obj_extension_suffix;

	obj_t object{};

    if (!tinyobj::LoadObj(&object.attrib, &object.shapes, &object.materials, &object.warn, &object.err, model_path.c_str()))
    {
        logr::report_error("[load_obj] {}",object.warn + object.err);
    }
    
}