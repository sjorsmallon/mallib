#include "asset.h"
#include "../file/file.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../image/stb_image.h"


#include "fmt/core.h"
#include "../on_leaving_scope/on_leaving_scope.h" // for the file pointer.
#include <sstream>
#include <fstream>

// hmm. For now, asset manages all assets.
std::map<std::string, asset::Raw_Obj_Data>& asset::obj_data()
{
    static std::map<std::string, asset::Raw_Obj_Data> obj_data;
    return obj_data;
}

std::map<std::string, asset::Texture>& asset::texture_data()
{
    static std::map<std::string, asset::Texture> texture_data;
    return texture_data;
}
std::map<std::string, asset::Material>& asset::mtl_data()
{
    static std::map<std::string, asset::Material> mtl_data;
    return mtl_data;
}

std::map<std::string, scene::Scene>& asset::scenes()
{
    static std::map<std::string, scene::Scene> scenes;
    return scenes;
}


//@Incomplete: these are now hardcoded. Where do we define these folders?
//@Refactor: does it even make sense to separate these assets?
//use from_to? do we supply folder names here? does this function assume a certain folder structure?
void asset::load_assets_from_file(const Asset_Folders& asset_folders)
{

    const auto obj_files     = file::list_files_in_dir(asset_folders.obj_folder);
    const auto mtl_files     = file::list_files_in_dir(asset_folders.mtl_folder);
    const auto texture_files = file::list_files_in_dir(asset_folders.texture_folder);
    const auto scene_files   = file::list_files_in_dir(asset_folders.scene_folder);

    //@TODO: preallocate space in the vector based on the number of entries here?
    auto& objects = asset::obj_data(); 
    auto& materials = asset::mtl_data();
    auto& textures = asset::texture_data();
    auto& scenes = asset::scenes();

    for (auto& obj_file: obj_files)
    {                
        // implicit creation.
        auto& new_object = objects[obj_file];
        load_obj_from_file(new_object, asset_folders.obj_folder + obj_file);
    }

    // one MTL file can contain many mtls. We should thus pass the map itself to this function,
    // and not a handle to a single mtl.
    for (auto& mtl_file: mtl_files)
    {
        // we pass the entire array here.
        // load_mtl_from_file(materials, asset_folders.mtl_folder + mtl_file);
    }

    for (auto& texture_file: texture_files)
    {
        // implicit creation.
        auto& new_texture = textures[texture_file];
        load_texture_from_file(new_texture, asset_folders.texture_folder + texture_file);
    }

    for (auto& scene_file: scene_files)
    {
        auto& new_scene = scenes[scene_file];
        load_scene_from_file(new_scene, asset_folders.scene_folder + scene_file);
    }

}

// internal.
static void generate_vertices_from_raw_data(asset::Raw_Obj_Data& raw_data)
{
    //@Memory: preallocate space.   
    raw_data.vertices.reserve(raw_data.faces.size());
    for (auto &face : raw_data.faces)
    {
        //@Refactor: maybe rewrite face to use the index set?
            asset::Vertex v0 = {
                raw_data.positions[face.v0_indices.data[0]],
                raw_data.tex_coords[face.v0_indices.data[1]],
                raw_data.normals[face.v0_indices.data[2]]
            };

            asset::Vertex v1 = {
                raw_data.positions[face.v1_indices.data[0]],
                raw_data.tex_coords[face.v1_indices.data[1]],
                raw_data.normals[face.v1_indices.data[2]]
            };

            asset::Vertex v2 = {
                raw_data.positions[face.v2_indices.data[0]],
                raw_data.tex_coords[face.v2_indices.data[1]],
                raw_data.normals[face.v2_indices.data[2]]
            };

            raw_data.vertices.emplace_back(v0);
            raw_data.vertices.emplace_back(v1);
            raw_data.vertices.emplace_back(v2);
    }
}

// @Refactor: this also warrants a string split function.
// Furthermore, we don't account for there not being a space in the scene file.
//@Note: assume an empty scene here.
void asset::load_scene_from_file(scene::Scene& scene, const std::string& filename)
{
    auto file_content = std::string();
    file::file_to_string(filename, file_content);
    auto data_stream = std::stringstream(file_content);

    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[max_string_length] = {0}; // used for the garbage at the start of each line. 

    // @Memory: how to predict how to preallocate?
    scene.set_pieces.reserve(10);
    // we use a pointer here so the logic for creating new set pieces 
    // is more concise.
    scene::Set_Piece* set_piece = nullptr;

    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        line_number++;
        if (line.empty())
        {
            continue;
        }
        if (line[0] == '#') // comment
        {
            continue;
        }       
        if (line[0] == ':' && line[1] == '/') // name of the object in the scene.
        {           
            // create a new set piece & set the pointer.
            scene.set_pieces.emplace_back();
            set_piece = &scene.set_pieces.back();

            line = line.substr(line.find_last_of(" ") + 1);
            fmt::print("[asset] set piece name: {}\n", line);
            set_piece->name = line;
        }
        else if (line[0] == 'm' && line[1] == 'o') // model
        {
            line = line.substr(line.find_last_of(" ") + 1);
            set_piece->model_name = line;
        }
        else if (line[0] == 'm' && line[1] == 'a') // material
        {
            line = line.substr(line.find_last_of(" ") + 1);
            set_piece->material_name = line;
        }
        else if (line[0] == 't' && line[1] == 'e') // texture
        {
            line = line.substr(line.find_last_of(" ") + 1);
            set_piece->texture_name = line;
        }
        else if (line[0] == 'p' && line[1] == 'o') // position
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
            fmt::print("[asset] Warning: file {}: No label recognized on line {}. content: {}\n",filename, line_number, line);
        }
    }
    fmt::print("[asset] Scene: succesfully loaded {}\n", filename);
}

//@Refactor:    
// we need a null-terminated bytestring in order to use sscanf.
// we can point to an address in memory, but then it doesn't null terminate.
// read until either linebreak or EOF. replace linebreak by \0.
// hand ptr over to user.
// in essence, we want to make our own getline for a char array.
// read line by line 
// size_t file_size = file::get_file_size(filename);
// if (file_size == 0)
//     fmt::print("load_obj: could not read file {}", filename);
// hand me a pointer, and I will alloc for you.
// we can mitigate the unsafety of sscanf by limiting the number of characters that are 
// read by %s by affixing a number, i.e. %1s for 1 character only.  
void asset::load_obj_from_file(asset::Raw_Obj_Data& raw_data, const std::string& filename)
{
    std::string data ={};
    file::file_to_string(filename, data);
    auto data_stream = std::stringstream(data);
    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[20] = {}; // used for the garbage in each line. 

    // @Memory: how to predict how to preallocate?
    fmt::print("[asset] Warning: load_obj does not efficiently preallocate.\n");
    raw_data.positions.reserve(4000);
    raw_data.normals.reserve(4000);
    raw_data.tex_coords.reserve(4000);
    raw_data.faces.reserve(4000);

    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        ++line_number;

        if (line.empty())
        {
            continue;
        }
        if (line[0] == '#') // comment 
        {
            continue;
        }
        else if (line[0] == 's')
        {
            // what does the 's' mean?
            continue;
        }
        else if (line[0] ==  'v' && line[1] == ' ') // vertex
        {
            Vec3 pos = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &pos.x, &pos.y, &pos.z);
            raw_data.positions.emplace_back(pos);
        } 
        else if (line[0] == 'v' && line[1] == 't') // texture coordinates
        {
            Vec2 tex_coords = {};
            sscanf(line.c_str(), "%9s %f %f", garbage_buffer, &tex_coords.x, &tex_coords.y);
            raw_data.tex_coords.emplace_back(tex_coords);
        }
        else if (line[0] == 'v' && line[1] == 'n') // vertex normals
        {
            Vec3 normal = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &normal.x, &normal.y, &normal.z);
            raw_data.normals.emplace_back(normal);
        }
        else if (line[0] == 'f') // face indices
        {
            Face face = {};
            //@incomplete: This will barf on unstructured obj files. for now, we assume everything's present.
            sscanf(line.c_str(), "%9s %u / %u / %u %u /%u /%u %u /%u /%u",
                   garbage_buffer,
                   &face.v0_indices.data[0], &face.v0_indices.data[1], &face.v0_indices.data[2],
                   &face.v1_indices.data[0], &face.v1_indices.data[1], &face.v1_indices.data[2],
                   &face.v2_indices.data[0], &face.v2_indices.data[1], &face.v2_indices.data[2]
                   );

            // The indices in the wavefront obj start at 1. we offset them to use them correctly
            // with the arrays in raw_data.
            //@Note: be careful with this subtraction. made a dumb mistake.
            face.v0_indices -= 1;
            face.v1_indices -= 1;
            face.v2_indices -= 1;
            
            raw_data.faces.emplace_back(face);
        }
        else
            fmt::print("[asset] ERROR: load_obj: no matching indicator. line number: {}. content: {}\n", line_number, line);
    }
    generate_vertices_from_raw_data(raw_data);

    fmt::print("[asset] Obj: succesfully loaded {}. num_faces: {}\n", filename, raw_data.faces.size());
}


void asset::load_mtl_from_file(std::map<std::string, asset::Material>& materials, const std::string& filename)
{
    std::string data ={};
    file::file_to_string(filename, data);
    std::stringstream data_stream(data);
    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[20] = {}; // used for the garbage at the start of    each line. 

    // there is at least one mtl in a mtl file, I think.
    //@Refactor: raw pointer here. not very nice.
    Material *material_ptr = nullptr;

    //@Refactor: we need to account for empty lines now as well. Better to have a better file handler
    // which only returns valid strings  and increments some line number.
    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        ++line_number;
        //@Refactor: can't we just rewrite the format to something more useful?
        //trim leading whitespace (tabs or spaces)
        if (line.empty()) // nothing on this line.
            continue;
        line = line.substr(line.find_first_not_of("\t "));
        if (line[0] == '#') // comment or empty.
        {
            continue;
        }
        //@TODO: substring lookup instead of this?
        else if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w')
        {
            // Either the current mtl is done, or we haven't seen one yet.
            //@refactor: I'm just splitting the string by space and taking the last part of it.
            std::string new_material_name = line.substr(line.find_first_of(" "));
            fmt::print("[asset] newmtl. mtl name: {}\n", new_material_name);
            material_ptr = &materials[new_material_name];

        }
        else if (line[0] == 'K' && line[1] == 'a') // ambient color
        {
            Vec3 color = {};
            sscanf(line.c_str(), "%2s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
            material_ptr->Ka = color;
        }
        else if (line[0] == 'K' && line[1] == 'd') // diffuse color
        {
            fmt::print("{}\n",line);
            Vec3 color = {};
            sscanf(line.c_str(), "%2s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
            material_ptr->Kd = color;
        }
        else if (line[0] == 'K' && line[1] == 's') // specular color
        {
            Vec3 color = {};
            sscanf(line.c_str(), "%2s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
            material_ptr->Ks = color;
        }
        else if (line[0] == 'd') // non_transparency
        {
            float alpha = 0;
            sscanf(line.c_str(), "%9s %f", garbage_buffer, &alpha);
            material_ptr->alpha = alpha;
        }
        else if (line[0] == 'T' && line[1] == 'r') // transparency
        {
            float inv_alpha = 0;
            sscanf(line.c_str(), "%9s %f", garbage_buffer, &inv_alpha);

            material_ptr->alpha = 1.0f / inv_alpha;
        }
        else if (line[0] == 'i' && line[1] == 'l') // illumination
        {
            int illum = 0;
            sscanf(line.c_str(), "%9s %d",garbage_buffer, &illum);
            // if illum == 1: we can skip Ks.
            // if illum == 2: requires ks to be defined.
            material_ptr->illum = illum;
        }
        else if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p') // map_Ka
        {
            //@Incomplete:
        }
    }
    fmt::print("[asset] Mtl: loaded {} materials.\n", materials.size());
    for (const auto& [key,material]: materials)
    {
        fmt::print("material {}:  {}\n",key, material);
    }
}

//@Incomplete: we create pointers to the texture data here. What if they're null? how do we prevent
// the rest of the program from suffering because of it?
void asset::load_texture_from_file(Texture& new_texture, const std::string& filename)
{
    new_texture.data = stbi_load(filename.c_str(), &new_texture.dimensions.x, &new_texture.dimensions.y, &new_texture.channels, STBI_rgb);
    if (new_texture.data == NULL)
    {
        fmt::print("[asset] failed loading texture. stbi_error: {}\n", stbi_failure_reason());
        return;
    }
    else 
    {
        new_texture.data_size = new_texture.dimensions.x * new_texture.dimensions.y * new_texture.channels;
        fmt::print("[asset] loaded texture {}. description: {}\n",filename, new_texture);
    }

}