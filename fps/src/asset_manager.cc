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
        exit(1);
    }
    else
    {

        for (auto& shape: object.shapes)
        {
            size_t index_offset = 0;

            for (size_t face_idx = 0; face_idx != shape.mesh.num_face_vertices.size(); ++face_idx)
            {
                size_t vertex_count = shape.mesh.num_face_vertices[face_idx];
                // Loop over vertices in the face.
                for (size_t vertex_idx = 0; vertex_idx != vertex_count; ++vertex_idx)
                {
                    // access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + vertex_idx];
                    float vx = object.attributes.vertices[3 * idx.vertex_index + 0];
                    float vy = object.attributes.vertices[3 * idx.vertex_index + 1];
                    float vz = object.attributes.vertices[3 * idx.vertex_index + 2];
                    float nx = object.attributes.normals[3 * idx.normal_index + 0];
                    float ny = object.attributes.normals[3 * idx.normal_index + 1];
                    float nz = object.attributes.normals[3 * idx.normal_index + 2];
                    float tx = object.attributes.texcoords[2 * idx.texcoord_index + 0];
                    float ty = object.attributes.texcoords[2 * idx.texcoord_index + 1];

                    object.interleaved_XNU.push_back(vx);
                    object.interleaved_XNU.push_back(vy);
                    object.interleaved_XNU.push_back(vz);
                    object.interleaved_XNU.push_back(nx);
                    object.interleaved_XNU.push_back(ny);
                    object.interleaved_XNU.push_back(nz);
                    object.interleaved_XNU.push_back(tx);
                    object.interleaved_XNU.push_back(ty);
                    // Optional: vertex colors
                    // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                    // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                    // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                }
                
                index_offset += vertex_count;
                // per-face material
                // object.shapes[shape_idx].mesh.material_ids[face_idx];
            }
        }
    }
}