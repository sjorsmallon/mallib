//     // unsigned int g_debug_geometry_vao;
//     // unsigned int g_debug_geometry_vbo;
//     // std::vector<float> g_debug_draw_data;

//     // unsigned int g_dodecahedron_vao;
//     // unsigned int g_dodecahedron_vbo;
//     // unsigned int g_dodecahedron_model_vbo;
//     // unsigned int g_dodecahedron_mvp_vbo;
//     // std::vector<glm::mat4> g_dodecahedron_model_matrices(2048);
//     // std::vector<glm::mat4> g_dodecahedron_mvp_matrices(2048);
//     // unsigned int g_parallax_cube_vao;
//     // unsigned int g_parallax_cube_vbo;

//     void init_parallax_cube()
//     {
//         logr::report_warning("Loading cube.obj instead of dodecahedron!\n");
//         // const auto& mesh = get_obj(*asset_manager, "sphere");
//         const auto& mesh = get_obj(*asset_manager, "cube");


//         glGenVertexArrays(1, &g_parallax_cube_vao);
//         glGenBuffers(1, &g_parallax_cube_vbo);
        
//         glBindVertexArray(g_parallax_cube_vao);

//         // fill buffer with vertices.
//         glBindBuffer(GL_ARRAY_BUFFER, g_parallax_cube_vbo);
//         glBufferData(GL_ARRAY_BUFFER, mesh.interleaved_XNTBU.size() * sizeof(float), mesh.interleaved_XNTBU.data(), GL_STATIC_DRAW);

//         // init vertex attributes (0: position, 1: normals, 2: binormals, 3:bitangent. 2: texture coordinates);
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);

//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));

//         glEnableVertexAttribArray(2);
//         glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));

//         glEnableVertexAttribArray(3);
//         glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(9 * sizeof(float)));

//         glEnableVertexAttribArray(4);
//         glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(12 * sizeof(float)));

//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);
//     }

//     void init_dodecahedron()
//     {
//         logr::report_warning("Loading cube.obj instead of dodecahedron!\n");
//         const auto& mesh = get_obj(*asset_manager, "cube");
//         // const auto& mesh = get_obj(*asset_manager, "dodecahedron");


//         glGenVertexArrays(1, &g_dodecahedron_vao);
//         glGenBuffers(1, &g_dodecahedron_vbo);

//         glBindVertexArray(g_dodecahedron_vao);
        
//         // fill buffer with vertices.
//         glBindBuffer(GL_ARRAY_BUFFER, g_dodecahedron_vbo);
//         glBufferData(GL_ARRAY_BUFFER, mesh.interleaved_XNU.size() * sizeof(float), mesh.interleaved_XNU.data(), GL_STATIC_DRAW);
        
//         // enable vertex attributes (0: position (xyz), 1: normals (xyz), 2: texture coordinates (uv));
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

//         glEnableVertexAttribArray(2);
//         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

//         // initialize two buffers for model matrices and mvp matrices for instanced objects.
//         glGenBuffers(1, &g_dodecahedron_model_vbo);
//         glGenBuffers(1, &g_dodecahedron_mvp_vbo);

//         //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
//         glBindBuffer(GL_ARRAY_BUFFER, g_dodecahedron_model_vbo);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_dodecahedron_model_matrices.size(), g_dodecahedron_model_matrices.data(), GL_DYNAMIC_DRAW);

//         // since location 0,1,2 are occupied by position, normal, texture coords, 
//         const uint32_t model_location = 3;
//         const uint32_t mat4_row_count = 4;
//         for (unsigned int location_offset = 0; location_offset != mat4_row_count; ++location_offset)
//         {
//             glEnableVertexAttribArray(model_location + location_offset);
//             glVertexAttribPointer(model_location + location_offset, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * location_offset * 4));
//             glVertexAttribDivisor(model_location + location_offset, 1);
//         }
        
//         //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
//         glBindBuffer(GL_ARRAY_BUFFER, g_dodecahedron_mvp_vbo);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_dodecahedron_mvp_matrices.size(), g_dodecahedron_mvp_matrices.data(), GL_DYNAMIC_DRAW);

//         const uint32_t mvp_location = 7;
//         for (unsigned int location_offset = 0; location_offset != mat4_row_count; ++location_offset)
//         {

//             glEnableVertexAttribArray(mvp_location + location_offset);
//             glVertexAttribPointer(mvp_location + location_offset, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * location_offset * 4));
//             glVertexAttribDivisor(mvp_location + location_offset, 1);
//         }
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);
//     }


// void init_debug_geometry()
// {
//     glGenVertexArrays(1, &g_debug_geometry_vao);
//     glBindVertexArray(g_debug_geometry_vao);

//     glGenBuffers(1, &g_debug_geometry_vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, g_debug_geometry_vbo);

//      // link vertex attributes (position, normals, color);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
//     glEnableVertexAttribArray(2);
//     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));


//     // add initial buffer to data.
//     glBufferData(GL_ARRAY_BUFFER, sizeof(float) *g_debug_draw_data.size(), g_debug_draw_data.data(), GL_STATIC_DRAW);

//     glBindVertexArray(0);
// }

//  void init_unit_cube()
//     {
//         float vertices[] = {
//             // back face
//             -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
//              1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
//              1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
//              1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
//             -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
//             -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
//             // front face
//             -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
//              1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
//              1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
//              1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
//             -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
//             -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
//             // left face
//             -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
//             -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
//             -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
//             -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
//             -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
//             -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
//             // right face
//              1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
//              1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
//              1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
//              1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
//              1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
//              1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
//             // bottom face
//             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
//              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
//              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
//              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
//             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
//             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
//             // top face
//             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
//              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
//              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
//              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
//             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
//             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
//         };
//         glGenVertexArrays(1, &g_cube_vao);
//         glGenBuffers(1, &g_cube_vbo);
//         // fill buffer
//         glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbo);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
//         glBindVertexArray(g_cube_vao);

//         // init vertex attributes (0: position, 1: normals, 2: texture coordinates);
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

//         glEnableVertexAttribArray(2);
//         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);
//     }




// // void render_parallax_cube()
// // {
// //     // render Cube
// //     glBindVertexArray(g_parallax_cube_vao);

// //     // size_t float_count = 36;
// //     // auto& obj = get_obj(*asset_manager, "sphere");
// //     auto& obj = get_obj(*asset_manager, "cube");

// //     size_t float_count = obj.interleaved_XNTBU.size();

// //     glDrawArrays(GL_TRIANGLES, 0, float_count);
// //     glBindVertexArray(0);
// // }



// // void render_cube()
// // {
// //     // render Cube
// //     glBindVertexArray(g_cube_vao);
// //     glDrawArrays(GL_TRIANGLES, 0, 36);
// //     glBindVertexArray(0);
// // }


//             // // render spheres
//             // {
//             //     timed_function("render_spheres");

//             //     set_shader(*shader_manager, "deferred_instanced_pbr");
//             //     // set_uniform(*shader_manager, "view", view);
//             //     // set_uniform(*shader_manager, "projection", projection);

//             //     const auto& pavingstones_albedo_texture = get_texture(*texture_manager, "pavingstones_4K_color");
//             //     const auto& pavingstones_normal_texture = get_texture(*texture_manager, "pavingstones_4K_normal");
//             //     const auto& pavingstones_roughness_texture = get_texture(*texture_manager, "pavingstones_4K_roughness");
//             //     // const auto& pavingstones_metallic_texture does not exist :^)
//             //     const auto& pavingstones_ambient_occlusion_texture = get_texture(*texture_manager, "pavingstones_4K_ambient_occlusion");
//             //     const auto& pavingstones_displacement_texture = get_texture(*texture_manager, "pavingstones_4K_displacement");

//             //     set_uniform(*shader_manager, "texture_albedo", pavingstones_albedo_texture.gl_texture_frame);
//             //     set_uniform(*shader_manager, "texture_normal", pavingstones_normal_texture.gl_texture_frame);
//             //     set_uniform(*shader_manager, "texture_roughness", pavingstones_roughness_texture.gl_texture_frame);
//             //     // set_uniform(*shader_manager, "texture_metallic", pavingstones_roughness_texture.gl_texture_frame); // this does not exist.
//             //     set_uniform(*shader_manager, "texture_ambient_occlusion", pavingstones_ambient_occlusion_texture.gl_texture_frame);
//             //     set_uniform(*shader_manager, "texture_displacement", pavingstones_displacement_texture.gl_texture_frame);


//             //     size_t draw_count = 0;
//             //     // this is not actually part of rendering: we are just updating the buffers.
//             //     {
    
//             //         glm::mat4& model_matrix_0 = g_dodecahedron_model_matrices[0];
//             //         glm::mat4& mvp_matrix_0 = g_dodecahedron_mvp_matrices[0];
//             //         glm::mat4& model_matrix_1 = g_dodecahedron_model_matrices[1];
//             //         glm::mat4& mvp_matrix_1 = g_dodecahedron_mvp_matrices[1];
//             //         glm::mat4& model_matrix_2 = g_dodecahedron_model_matrices[2];
//             //         glm::mat4& mvp_matrix_2 = g_dodecahedron_mvp_matrices[2];
//             //         glm::mat4& model_matrix_3 = g_dodecahedron_model_matrices[2];
//             //         glm::mat4& mvp_matrix_3 = g_dodecahedron_mvp_matrices[2];

//             //         glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(20.f));
//             //         glm::mat4 translate_0 = glm::translate(glm::mat4(1.f), glm::vec3(300.f, -5.f, 0.f));
//             //         glm::mat4 translate_1 = glm::translate(glm::mat4(1.f), glm::vec3(300.f, -5.f, 300.f));
//             //         glm::mat4 translate_2 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -5.f, 300.f));
//             //         glm::mat4 translate_3 = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -5.f, 0.0f));
//             //         glm::mat4 rotate = glm::mat4(1.0f);

//             //         model_matrix_0 = translate_0 * rotate * scale;
//             //         model_matrix_1 = translate_1 * rotate * scale;
//             //         model_matrix_2 = translate_2 * rotate * scale;
//             //         model_matrix_3 = translate_3 * rotate * scale;
//             //         mvp_matrix_0 = projection * view * model_matrix_0;
//             //         mvp_matrix_1 = projection * view * model_matrix_1;
//             //         mvp_matrix_2 = projection * view * model_matrix_2;
//             //         mvp_matrix_3 = projection * view * model_matrix_3;

//             //         draw_count = 4;


//             //         glNamedBufferData(g_dodecahedron_model_vbo, sizeof(glm::mat4) * g_dodecahedron_model_matrices.size(), g_dodecahedron_model_matrices.data(), GL_DYNAMIC_DRAW);
//             //         glNamedBufferData(g_dodecahedron_mvp_vbo,   sizeof(glm::mat4) * g_dodecahedron_mvp_matrices.size(),   g_dodecahedron_mvp_matrices.data(), GL_DYNAMIC_DRAW);
//             //     }
                
//             //     const size_t mesh_size = get_obj(*asset_manager, "cube").interleaved_XNU.size() / 8;

//             //     glBindVertexArray(g_dodecahedron_vao);
//             //     glDrawArraysInstanced(
//             //         GL_TRIANGLES,
//             //         0,
//             //         mesh_size,
//             //         draw_count);


//             //     glBindVertexArray(0);
//             // }


//             // // bind shader and update all relevant uniforms.
//         // set_shader(*shader_manager, "deferred_geometry");
//         // set_uniform(*shader_manager, "projection", projection);

//         // // 1.a:  render static geometry in the scene.
//         // // ---------------------------------------
//         // {
//         //     // render weapon(viewmodel). These values are arbitrary as of now.
//         //     {
//         //         const auto& spear_texture = get_texture(*texture_manager, "metal");
//         //         set_uniform(*shader_manager,  "texture_diffuse", spear_texture.gl_texture_frame);
//         //         set_uniform(*shader_manager, "view", glm::mat4(1.0f));

//         //         glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
//         //         glm::quat quaternion = glm::quat(0.70f, -0.70f, 0.0f, 0.0f);
//         //         glm::mat4 rotate = glm::toMat4(quaternion);
//         //         glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f,-1.0f, -0.7f));
//         //         glm::mat4 model = translate * rotate * scale;

//         //         set_uniform(*shader_manager, "model", model);
//         //         render_viewmodel();
//         //     }

//         //     set_uniform(*shader_manager, "view", view);
//         //     // for all cubes in the scene..)
//         //     {
//         //         const auto& metal_texture = get_texture(*texture_manager, "metal");
//         //         set_uniform(*shader_manager,  "texture_diffuse", metal_texture.gl_texture_frame);

//         //         glm::mat4 model = glm::mat4(1.0f);
//         //         set_uniform(*shader_manager, "model", model);
//         //         render_cube();

//         //     }
        
//         //     //   render floor
//         //     {
//         //         const auto& floor_texture = get_texture(*texture_manager, "floor_64");
//         //         set_uniform(*shader_manager, "texture_diffuse",  floor_texture.gl_texture_frame);

//         //         glm::mat4 model = glm::mat4(1.0f);
//         //         set_uniform(*shader_manager, "model", model);
//         //         render_floor();
//         //     }

//         //     // render walls
//         //     {
//         //         timed_function("render_walls");

//         //         set_shader(*shader_manager, "deferred_instanced");
//         //         set_uniform(*shader_manager, "view", view);

//         //         // update matrix of the walls.
//         //         {
//         //             auto& model_matrix_0 = g_wall_model_matrices[0];
//         //             auto& mvp_matrix_0 = g_wall_mvp_matrices[0];
//         //             glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
//         //             glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(1024.0f, 0.0f, 0.0f));
//         //             glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), -0.5f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
//         //             model_matrix_0 = translate * rotate * scale;
//         //             mvp_matrix_0 = projection * view * model_matrix_0;  

//         //             auto& model_matrix_1 = g_wall_model_matrices[1];
//         //             auto& mvp_matrix_1 = g_wall_mvp_matrices[1];
//         //             scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
//         //             translate = glm::translate(glm::mat4(1.0f), glm::vec3(-1024.0f, 0.0f, 0.0f));
//         //             rotate = glm::rotate(glm::mat4(1.0f), 0.5f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
//         //             model_matrix_1 = translate * rotate * scale;
//         //             mvp_matrix_1 = projection * view * model_matrix_1;  

//         //             auto& model_matrix_2 = g_wall_model_matrices[2];
//         //             auto& mvp_matrix_2 = g_wall_mvp_matrices[2];
//         //             scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
//         //             translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1024.0f));
//         //             rotate = glm::rotate(glm::mat4(1.0f), 0.0f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
//         //             model_matrix_2 = translate * rotate * scale;
//         //             mvp_matrix_2 = projection * view * model_matrix_2;  

//         //             auto& model_matrix_3 = g_wall_model_matrices[3];
//         //             auto& mvp_matrix_3 = g_wall_mvp_matrices[3];
//         //             scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
//         //             translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1024.0f));
//         //             rotate = glm::rotate(glm::mat4(1.0f), -1.0f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
//         //             model_matrix_3 = translate * rotate * scale;
//         //             mvp_matrix_3 = projection * view * model_matrix_3;  
//         //         }

//         //         glNamedBufferData(g_wall_model_vbo, sizeof(glm::mat4) * g_wall_model_matrices.size(), g_wall_model_matrices.data(), GL_DYNAMIC_DRAW);
//         //         glNamedBufferData(g_wall_mvp_vbo,   sizeof(glm::mat4) * g_wall_mvp_matrices.size(),   g_wall_mvp_matrices.data(), GL_DYNAMIC_DRAW);

//         //         const auto& wall_stone_diffuse_texture  = get_texture(*texture_manager, "wall_stone_diffuse");
//         //         const auto& wall_stone_specular_texture = get_texture(*texture_manager, "wall_stone_specular");
//         //         const auto& wall_stone_normal_texture   = get_texture(*texture_manager, "wall_stone_normal");

//         //         set_uniform(*shader_manager, "texture_diffuse",  wall_stone_diffuse_texture.gl_texture_frame);
//         //         set_uniform(*shader_manager, "texture_specular", wall_stone_specular_texture.gl_texture_frame);

//         //         glBindVertexArray(g_wall_vao);
//         //         glDrawArraysInstanced(
//         //             GL_TRIANGLES,
//         //             0,
//         //             6,
//         //             g_wall_model_matrices.size());
//         //         glBindVertexArray(0);
//         //     }

//         //     // render dodecahedrons
//         //     {
//         //         timed_function("render_dodecahedron");

//         //         set_shader(*shader_manager, "deferred_instanced");
//         //         set_uniform(*shader_manager, "view", view);
                

//         //         size_t draw_count = 0;
//         //         // this is not actually part of rendering: we are just updating the buffers.
//         //         {
               
//         //             //@Note(Sjors): we now use draw count in order to not overdraw.
//         //             // draw_count is used in gldrawarraysinstanced.
//         //             // however, this means that there is garbage at the end of the buffer (past draw count).
//         //             // we may want to zero that.
//         //             Entity_Manager& manager = *entity_manager;
//         //             for (auto&& entity: by_type(manager, Entity_Type::Cube))
//         //             {

//         //                 if (entity.scheduled_for_destruction) continue;

//         //                 glm::mat4& model_matrix = g_dodecahedron_model_matrices[draw_count];
//         //                 glm::mat4& mvp_matrix = g_dodecahedron_mvp_matrices[draw_count];
//         //                 draw_count += 1;

//         //                 glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(20.f));
                        
//         //                 // turn angry face towards player
//         //                 glm::mat4 rotate = glm::mat4(1.0f);
//         //                 glm::vec3 target_ray = glm::normalize(camera.position - entity.position);
//         //                 glm::vec3 object_ray = glm::vec3(0.f,1.f, 0.f);
//         //                 float angle_dif = acos(glm::dot(target_ray, object_ray));
//         //                 if (angle_dif != 0)
//         //                 {
//         //                     glm::vec3 ortho_ray = glm::cross(object_ray, target_ray);
//         //                     ortho_ray = glm::normalize(ortho_ray);
//         //                     glm::quat delta_quaternion= glm::angleAxis(angle_dif, ortho_ray);
//         //                     rotate = glm::toMat4(delta_quaternion);
//         //                 }

//         //                 glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);
//         //                 model_matrix = translate * rotate * scale;
//         //                 mvp_matrix = projection * view * model_matrix;  
//         //             }

//         //             glNamedBufferData(g_dodecahedron_model_vbo, sizeof(glm::mat4) * g_dodecahedron_model_matrices.size(), g_dodecahedron_model_matrices.data(), GL_DYNAMIC_DRAW);
//         //             glNamedBufferData(g_dodecahedron_mvp_vbo,   sizeof(glm::mat4) * g_dodecahedron_mvp_matrices.size(),   g_dodecahedron_mvp_matrices.data(), GL_DYNAMIC_DRAW);
//         //         }
            
//         //         const auto& dodecahedron_diffuse_texture  = get_texture(*texture_manager, "angry_face");
//         //         const auto& wall_stone_specular_texture = get_texture(*texture_manager, "wall_stone_specular");

//         //         set_uniform(*shader_manager, "texture_diffuse",  dodecahedron_diffuse_texture.gl_texture_frame);
//         //         set_uniform(*shader_manager, "texture_specular", wall_stone_specular_texture.gl_texture_frame);
                
//         //         const size_t mesh_size = get_obj(*asset_manager, "dodecahedron").interleaved_XNU.size() / 8;

//         //         glBindVertexArray(g_dodecahedron_vao);
//         //         glDrawArraysInstanced(
//         //             GL_TRIANGLES,
//         //             0,
//         //             mesh_size,
//         //             draw_count);


//         //         glBindVertexArray(0);
//         //     }

//         // }



//     // // 2. lighting pass:
//     // // -----------------------------------------------------------------
//     // {
//     //     // get texture handles for the frame buffers.
//     //     const auto& position_tfbo_texture = get_texture(*texture_manager, "position_tfbo");
//     //     const auto& normal_tfbo_texture = get_texture(*texture_manager, "normal_tfbo");
//     //     const auto& albedo_specular_tfbo_texture = get_texture(*texture_manager, "albedo_specular_tfbo");

//     //     set_shader(*shader_manager, "post_deferred_lighting");

//     //     set_uniform(*shader_manager, "fb_position",    position_tfbo_texture.gl_texture_frame);
//     //     set_uniform(*shader_manager, "fb_normal",      normal_tfbo_texture.gl_texture_frame);    
//     //     set_uniform(*shader_manager, "fb_albedo_spec", albedo_specular_tfbo_texture.gl_texture_frame);    

//     //     glm::vec4 camera_position = glm::vec4(camera.position, 1.0f); 
//     //     set_uniform(*shader_manager, "view_position", glm::vec4(camera_position));

//     //     // for (auto& light: lights)
//     //     {
//     //         static float time = 0.0f;
//     //         static float y_position = 0.0f;
//     //         static float x_position = 0.0f;
//     //         time += 2.0f;
//     //         time = fmod(time, 6280.0f);
//     //         y_position = sin(time/ 1000.0f);
//     //         x_position = cos(time/ 1000.0f);

//     //         // player light.
//     //         lights[0].position.x = camera.position.x;
//     //         lights[0].position.y = camera.position.y;
//     //         lights[0].position.z = camera.position.z;
//     //         lights[0].color = glm::vec4(1.0f,1.0f,1.0f,0.0f);
//     //         lights[0].on = true;
//     //         lights[0].linear = 0.001f;
//     //         lights[0].quadratic = 0.002f;

//     //         lights[1].position = glm::vec4(0.0f, 3.0f, 0.0f, 0.0f);
//     //         lights[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
//     //         lights[1].on = true;
//     //         lights[1].linear = 0.1;
//     //         lights[1].quadratic = 0.2;

//     //     }
        
//     //     //@Note(Sjors): while not necessary to bind the buffer
//     //     // when using glnamedbufferdata to update the buffer. it IS important to still bind it before rendering the
//     //     // NDC quad, since any glDraw* call will take the currently bound array / index buffer.
//     //     glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
//     //     glNamedBufferData(light_ubo, lights.size() * sizeof(Light),  lights.data(), GL_DYNAMIC_DRAW);

//     //     render_NDC_quad();
//     // }


// //     void render_shadows(const Camera camera, Particle_Cache& particle_cache)
// // {  

// //     // render
// //     // ------
// //     Camera player_camera = camera;
// //     glm::mat4 view       = create_view_matrix_from_camera(player_camera);
// //     glm::mat4 projection = glm::perspective(glm::radians(g_fov), g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane); 

// //         // STEP 0: SHADOW MAPPING
// //     {
// //         // 0.1: first render to depth map
// //         // ------------------------------------------
// //         {
// //             set_shader(*shader_manager, "simple_depth");
// //             glViewport(0, 0, SHADOW_FB_WIDTH, SHADOW_FB_HEIGHT);
// //             glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
// //             glClear(GL_DEPTH_BUFFER_BIT);

// //             const float near_plane = g_projection_z_near_plane;
// //             const float far_plane = g_projection_z_far_plane;

// //             static float time = 0.0f;
// //             static float z_position = 0.0f;
// //             static float x_position = 0.0f;
// //             time += 2.0f;
// //             time = fmod(time, 6280.0f);
// //             z_position = sin(time/ 1000.0f);
// //             x_position = cos(time/ 1000.0f);

// //             // frustum?
// //             glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
// //             glm::mat4 light_view =       glm::lookAt(glm::vec3(x_position, 3.0f, z_position), // eye
// //                                                      glm::vec3(0.0f, 0.0f,  0.0f),  // target
// //                                                      glm::vec3(0.0f, 1.0f,  0.0f));  // up

// //             glm::mat4 light_space_matrix = light_projection * light_view;

// //             set_uniform(*shader_manager, "light_space_matrix", light_space_matrix);
// //             // for all cubes in the scene..
// //             {
// //                 glm::mat4 model  = glm::mat4(1.0f);
// //                 set_uniform(*shader_manager, "model", model);
// //                 render_cube();
// //             }
// //             // unbind framebuffer
// //             glBindFramebuffer(GL_FRAMEBUFFER, 0);

// //             // 0.2. then render scene as normal with shadow mapping (using depth map)
// //             glViewport(0, 0, g_window_width, g_window_height);
// //             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// //             set_shader(*shader_manager, "simple_depth_shadow_mapping");

// //             set_uniform(*shader_manager, "projection", projection);
// //             set_uniform(*shader_manager, "view", view);
// //             // set light uniforms
// //             set_uniform(*shader_manager, "light_pos", glm::vec3(x_position, 3.0f, z_position));
// //             set_uniform(*shader_manager, "view_pos", glm::vec3(camera.position.x, camera.position.y, camera.position.z));
// //             set_uniform(*shader_manager, "light_space_matrix", light_space_matrix);

// //             const auto& diffuse_texture = get_texture(*texture_manager, "metal");
// //             const auto& shadow_map = get_texture(*texture_manager, "depth_map_tfbo");
// //             set_uniform(*shader_manager, "diffuse_texture", diffuse_texture.gl_texture_frame);
// //             set_uniform(*shader_manager, "shadow_map", shadow_map.gl_texture_frame);
            
// //             // for all cubes in the scene..
// //             {
// //                 glm::mat4 model  = glm::mat4(1.0f);
// //                 set_uniform(*shader_manager, "model", model);
// //                 render_cube();
// //                 render_floor();
// //             }
// //             set_shader(*shader_manager, "lightbox");
// //             set_uniform(*shader_manager, "view", view);
// //             set_uniform(*shader_manager, "projection", projection);
// //             // for all cubes in the scene..
// //             {
// //                 glm::mat4 model  = glm::mat4(1.0f);
// //                 model = glm::translate(model, glm::vec3(x_position, 3.0f, z_position));
// //                 model = glm::scale(model, glm::vec3(0.125f));
// //                 set_uniform(*shader_manager, "model", model);
// //                 set_uniform(*shader_manager, "light_color", glm::vec4(1.0f,1.0f,1.0f,0.0f));

// //                 render_cube();
// //             }

// //         }
// //     }
   
// // }


//     // // // 3. render geometry on top of scene that does not need to be affected by lighting.
//     // // --------------------------------
//     // {
//     //     // only render the active lights.normally, for light: active_lights)
//     //     {
//     //         set_shader( *shader_manager, "lightbox");
//     //         set_uniform(*shader_manager, "projection", projection);
//     //         set_uniform(*shader_manager, "view", view);

//     //         glm::mat4 model = glm::mat4(1.0f);
//     //         model = glm::translate(model, glm::vec3(lights[0].position));
//     //         model = glm::scale(model, glm::vec3(0.01f));
//     //         set_uniform(*shader_manager, "model", model);
//     //         set_uniform(*shader_manager, "light_color", lights[0].color);
//     //         render_cube();

//     //         model = glm::mat4(1.0f);
//     //         model = glm::translate(model, glm::vec3(lights[1].position));
//     //         model = glm::scale(model, glm::vec3(0.125f));
//     //         set_uniform(*shader_manager, "model", model);
//     //         set_uniform(*shader_manager, "light_color", lights[1].color);
//     //         render_cube();
//     //     }

//     //     // render debug data
//     //     {
//     //         set_shader(*shader_manager, "debug_geometry");

//     //         set_uniform(*shader_manager, "view", view);
//     //         set_uniform(*shader_manager, "projection", projection);
//     //         set_uniform(*shader_manager, "model", glm::mat4(1.0f));

//     //         glBindVertexArray(g_debug_geometry_vao);
//     //         glBindBuffer(GL_ARRAY_BUFFER, g_debug_geometry_vbo);
//     //         glBufferData(GL_ARRAY_BUFFER, sizeof(float) *g_debug_draw_data.size(), g_debug_draw_data.data(), GL_STATIC_DRAW);

//     //         // debug draw data = position(3) + normal 3+ color(3)
//     //         glDrawArrays(GL_TRIANGLES, 0, g_debug_draw_data.size() / 9);
//     //     }
//     // }


// // Shadow frame buffer
// // // Shadow Map
// // //------------------------------------------------------
// // glGenFramebuffers(1, &depth_map_fbo);
// // g_depth_map_tfbo = register_framebuffer_texture(*texture_manager, "depth_map_tfbo");
// // const auto& depth_map_texture = get_texture(*texture_manager, "depth_map_tfbo");
// // glActiveTexture(GL_TEXTURE0 + depth_map_texture.gl_texture_frame);
// // glBindTexture(GL_TEXTURE_2D, g_depth_map_tfbo);
// // glTexImage2D(
// //     GL_TEXTURE_2D, 
// //     0, 
// //     GL_DEPTH_COMPONENT, 
// //     SHADOW_FB_WIDTH, 
// //     SHADOW_FB_HEIGHT, 
// //     0, 
// //     GL_DEPTH_COMPONENT, 
// //     GL_FLOAT, 
// //     NULL);

// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

// // glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
// // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depth_map_tfbo, 0);
// // glDrawBuffer(GL_NONE);
// // glReadBuffer(GL_NONE);
// // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logr::report_error("[OpenGL] Shadow framebuffer is incomplete.\n");

// // glBindFramebuffer(GL_FRAMEBUFFER, 0);  