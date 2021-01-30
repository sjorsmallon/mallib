#include "asset_manager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

#include "logr.h"

namespace
{
	std::string g_obj_extension_suffix{".obj"};
    std::string g_obj_folder_prefix{"../assets/obj/"};
}

void set_asset_path(Asset_Manager& asset_manager, const char* asset_folder_prefix)
{
    g_obj_folder_prefix = asset_folder_prefix;
}

void load_obj(Asset_Manager& asset_manager, const std::string& obj_name, bool should_unitize)
{

	const std::string model_path = g_obj_folder_prefix + obj_name + g_obj_extension_suffix;
	//@FIXME(Sjors): implicit creation.
	auto& object = asset_manager.meshes[obj_name];
    object.unitized = should_unitize;

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

                std::vector<glm::vec3> temporary_positions{};
                std::vector<glm::vec3> temporary_normals{};
                std::vector<glm::vec2> temporary_texture_coords{}; 


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

                    if (vx > object.bounds.max.x) object.bounds.max.x = vx;
                    if (vy > object.bounds.max.y) object.bounds.max.y = vy;
                    if (vz > object.bounds.max.z) object.bounds.max.z = vz;
                    if (vx < object.bounds.min.x) object.bounds.min.x = vx;
                    if (vx < object.bounds.min.y) object.bounds.min.y = vy;
                    if (vx < object.bounds.min.z) object.bounds.min.z = vz;
                    // Optional: vertex colors
                    // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                    // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                    // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                    temporary_positions.push_back(glm::vec3(vx,vy,vz));
                    temporary_normals.push_back(glm::vec3(nx,ny,nz));
                    temporary_texture_coords.push_back(glm::vec2(tx,ty));
                }

                // calculate tangent/bitangent
                {
                    auto& v0 = temporary_positions[0];
                    auto& v1 = temporary_positions[1];
                    auto& v2 = temporary_positions[2];

                    auto& n0 = temporary_normals[0];
                    auto& n1 = temporary_normals[1];
                    auto& n2 = temporary_normals[2];

                    auto& uv0 = temporary_texture_coords[0];
                    auto& uv1 = temporary_texture_coords[1];
                    auto& uv2 = temporary_texture_coords[2];

                    // Calculate position difference
                    glm::vec3 deltaPos1 = v1 - v0;
                    glm::vec3 deltaPos2 = v2 - v0;

                    // Calculate texture coordinate difference
                    glm::vec2 deltaUV1 = uv1 - uv0;
                    glm::vec2 deltaUV2 = uv2 - uv0;

                    // Calculate tangent and bitangent
                    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                    // Orthogonalize using Gram–Schmidt process, to make tangents and bitangents smooth based on normal
                    glm::vec3 tangent_0 = glm::normalize(tangent - n0 * glm::dot(n0, tangent));
                    glm::vec3 tangent_1 = glm::normalize(tangent - n1 * glm::dot(n1, tangent));
                    glm::vec3 tangent_2 = glm::normalize(tangent - n2 * glm::dot(n2, tangent));

                    glm::vec3 bitangent_0 = glm::normalize(bitangent - n0 * glm::dot(n0, bitangent));
                    glm::vec3 bitangent_1 = glm::normalize(bitangent - n1 * glm::dot(n1, bitangent));
                    glm::vec3 bitangent_2 = glm::normalize(bitangent - n2 * glm::dot(n2, bitangent));

                    object.interleaved_XNTBU.push_back(v0.x);
                    object.interleaved_XNTBU.push_back(v0.y);
                    object.interleaved_XNTBU.push_back(v0.z);
                    object.interleaved_XNTBU.push_back(n0.x);
                    object.interleaved_XNTBU.push_back(n0.y);
                    object.interleaved_XNTBU.push_back(n0.z);
                    object.interleaved_XNTBU.push_back(tangent_0.x);
                    object.interleaved_XNTBU.push_back(tangent_0.y);
                    object.interleaved_XNTBU.push_back(tangent_0.z);
                    object.interleaved_XNTBU.push_back(bitangent_0.x);
                    object.interleaved_XNTBU.push_back(bitangent_0.y);
                    object.interleaved_XNTBU.push_back(bitangent_0.z);
                    object.interleaved_XNTBU.push_back(uv0.x);
                    object.interleaved_XNTBU.push_back(uv0.y);

                    object.interleaved_XNTBU.push_back(v1.x);
                    object.interleaved_XNTBU.push_back(v1.y);
                    object.interleaved_XNTBU.push_back(v1.z);
                    object.interleaved_XNTBU.push_back(n1.x);
                    object.interleaved_XNTBU.push_back(n1.y);
                    object.interleaved_XNTBU.push_back(n1.z);
                    object.interleaved_XNTBU.push_back(tangent_1.x);
                    object.interleaved_XNTBU.push_back(tangent_1.y);
                    object.interleaved_XNTBU.push_back(tangent_1.z);
                    object.interleaved_XNTBU.push_back(bitangent_1.x);
                    object.interleaved_XNTBU.push_back(bitangent_1.y);
                    object.interleaved_XNTBU.push_back(bitangent_1.z);
                    object.interleaved_XNTBU.push_back(uv1.x);
                    object.interleaved_XNTBU.push_back(uv1.y);

                    object.interleaved_XNTBU.push_back(v2.x);
                    object.interleaved_XNTBU.push_back(v2.y);
                    object.interleaved_XNTBU.push_back(v2.z);
                    object.interleaved_XNTBU.push_back(n2.x);
                    object.interleaved_XNTBU.push_back(n2.y);
                    object.interleaved_XNTBU.push_back(n2.z);
                    object.interleaved_XNTBU.push_back(tangent_2.x);
                    object.interleaved_XNTBU.push_back(tangent_2.y);
                    object.interleaved_XNTBU.push_back(tangent_2.z);
                    object.interleaved_XNTBU.push_back(bitangent_2.x);
                    object.interleaved_XNTBU.push_back(bitangent_2.y);
                    object.interleaved_XNTBU.push_back(bitangent_2.z);
                    object.interleaved_XNTBU.push_back(uv2.x);
                    object.interleaved_XNTBU.push_back(uv2.y);
                }   

                
                index_offset += vertex_count;
                // per-face material
                // object.shapes[shape_idx].mesh.material_ids[face_idx];
            }
        }

        if (object.unitized)
        {
            for (size_t idx = 0; idx != object.interleaved_XNU.size(); idx += 8)
            {
                const size_t x_idx = idx;
                const size_t y_idx = idx + 1;
                const size_t z_idx = idx + 2;

                object.interleaved_XNU[x_idx] = (object.interleaved_XNU[x_idx] - object.bounds.min.x) / (object.bounds.max.x - object.bounds.min.x);
                object.interleaved_XNU[y_idx] = (object.interleaved_XNU[y_idx] - object.bounds.min.y) / (object.bounds.max.y - object.bounds.min.y);
                object.interleaved_XNU[z_idx] = (object.interleaved_XNU[z_idx] - object.bounds.min.z) / (object.bounds.max.z - object.bounds.min.z);
            }

            object.bounds.min = glm::vec3(0.0f);
            object.bounds.max = glm::vec3(1.0f);
        }

        logr::report("[load_obj] succesfully loaded obj: {}\n", obj_name);
    }
}


const obj_t& get_obj(Asset_Manager& asset_manager, const std::string& obj_name)
{
    assert(asset_manager.meshes.find(obj_name) != asset_manager.meshes.end());

    return asset_manager.meshes[obj_name];
}
