#ifndef INCLUDED_ASSET_
#define INCLUDED_ASSET_
#include <scene.h>
// #include <mglmgl::vec3.h>
// #include <mgl::vec2.h>
#include <vector>
#include <map>
#include <string>

// asset loads all the asset from the Asset_Folder struct into memory.
// for now, these are accessible through the references provided in this namespace.

namespace asset
{
    enum Asset_Type
    {
        MODEL,
        MUSIC,
        SOUND_FX
    };

    struct Asset_Folders
    {
        std::string obj_folder;
        std::string mtl_folder;
        std::string texture_folder;
        std::string scene_folder;
        //std::string sound_folder?
    };

    struct Face
    {
        mgl::uivec3 v0_indices;
        mgl::uivec3 v1_indices;
        mgl::uivec3 v2_indices;
    };

    //@Refactor: put this in a shared header.
    struct Vertex
    {
        mgl::vec3 position;
        mgl::vec2 tex_coords;
        mgl::vec3 normals;
    };

    struct Raw_Obj_Data
    {
        std::vector<mgl::vec3> positions;
        std::vector<mgl::vec3> normals;
        std::vector<mgl::vec2> tex_coords;
        std::vector<Face> faces; // these indices are offset by 1.
        std::vector<Vertex> vertices;
    };

    struct Material
    {
        mgl::vec3 Ka; // ambient color
        mgl::vec3 Kd; // diffuse color
        mgl::vec3 Ks; // specular color
        float Ns; // specular exponent?
        uint16_t illum; // if illum == 1, skip Ks.
        float alpha; // either d or inverse transparency.
    };

    //@Incomplete: load into array, return index?
    struct Texture
    {
        unsigned char* data; // hmm. this is malloc'd by stb. do we just call free on delete?
        mgl::ivec2  dimensions;
        int    channels;
        size_t data_size;
        uint32_t gl_texture_id;
        uint32_t gl_texture_frame;
        ~Texture()
        {
            free(data);
        }
    };


    void load_assets_from_file(const asset::Asset_Folders& asset_folders);
    void load_scene_from_file(scene::Scene& scene,            const std::string& filename);
    void load_obj_from_file(asset::Raw_Obj_Data& raw_data,    const std::string& filename);
    void load_texture_from_file(asset::Texture& texture,      const std::string& filename);
    void load_mtl_from_file(std::map<std::string, Material>& materials, const std::string& filename);


    // hmm. For now, asset manages all assets.
    // of course this is data. What is the naming convention to use here?
    std::map<std::string, Raw_Obj_Data>& obj_data();
    std::map<std::string, Texture>&  texture_data();
    std::map<std::string, Material>& mtl_data();
    std::map<std::string, scene::Scene>& scenes();


}

// for formatting
namespace fmt {
    template <>
    struct formatter<asset::Material> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const asset::Material &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "\nKa (ambient): {}\nKd (diffuse): {}\nKs (specular): {}\nNs (spec-ex): {}\nillum: {}\nalpha: {}",
            lhs.Ka,
            lhs.Kd,
            lhs.Ks,
            lhs.Ns,
            lhs.illum,
            lhs.alpha);
      }
    };

       template <>
    struct formatter<asset::Texture> {
      template <typename ParseContext>
      constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

      template <typename FormatContext>
      auto format(const asset::Texture &lhs, FormatContext &ctx) {
        return format_to(ctx.out(), "\nx: {}, y: {}\nchannels: {}\ndata_size: {}.\n",
            lhs.dimensions.x,
            lhs.dimensions.y,
            lhs.channels,
            lhs.data_size);
      }
    };
}





#endif
