#include "asset_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

#include "log.h"

namespace
{
	const std::string g_obj_extension_suffix{".obj"};

}



void load_obj(Asset_Manager& asset_manager, const std::string& obj_name)
{


	const std::string obj_folder_prefix = "../assets/obj/";
	const std::string model_path = obj_folder_prefix + obj_name + g_obj_extension_suffix;

	//@FIXME(Sjors): implicit creation.
	auto& object = asset_manager.meshes[obj_name];

    if (!tinyobj::LoadObj(&object.attributes, &object.shapes, &object.materials, &object.warn, &object.err, model_path.c_str()))
    {
        logr::report_error("[load_obj] could not load {}. errors: {}", obj_name, object.warn + object.err);
    }
    else
    {
    	logr::report("[load_obj] loaded {}.\n", obj_name);
        logr::report("vertices: {}\n vertex normals: {}\n", object.attributes.vertices.size(), object.attributes.normals.size());
        // logr::report_warning("[load_obj] interleaving data\n", obj_name);
        // for (size_t idx = 0; idx != object.attributes.vertices.size() / 3; ++idx)
        // {
        //     size_t x_idx = idx;
        //     size_t y_idx = idx + 1;
        //     size_t z_idx = idx + 2;
        //     std::vector<float> XNU;

        //     object.Vertex_XNU.push_back(object.attributes.vertices[x_idx]);
        //     object.Vertex_XNU.push_back(object.attributes.vertices[y_idx]);
        //     object.Vertex_XNU.push_back(object.attributes.vertices[z_idx]);
            
        //     object.Vertex_XNU.push_back(object.attributes.normals[x_idx]);
        //     object.Vertex_XNU.push_back(object.attributes.normals[y_idx]);
        //     object.Vertex_XNU.push_back(object.attributes.normals[z_idx]);

        //     object.Vertex_XNU.push_back(object.attributes.texcoords[x_idx]);
        //     object.Vertex_XNU.push_back(object.attributes.texcoords[y_idx]);

        // }
    }

    
}