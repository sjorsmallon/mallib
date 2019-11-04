#ifndef INCLUDED_ASSET_
#define INCLUDED_ASSET_
#include "../scene/scene.h"

namespace asset
{
    enum Asset_Type
    {
        MODEL,
        MUSIC,
        SOUND_FX
    }


    void load_model_from_obj(std::string& filename);
    void load_scene_from_file(const std::string& filename, scene::Scene& scene);

    void load_texture_from_file();
    void load_obj_from_file();
    void load_material_from_file();


}

#endif
