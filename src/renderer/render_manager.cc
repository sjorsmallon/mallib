#include "render_manager.h"

//--- OpenGL
#include <gl_lite/gl_lite.h> 

//--- Imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_opengl3.h>

#include "../common/globals.h" // win32 context.
#include "camera.h"


Render_Manager& rm::render_manager()
{
    static Render_Manager& render_manager;

    return render_manager;
}


void rm::init_render_manager()
{
     auto& render_manager = rm::render_manager();
     render_manager.active_camera = cam::default_camera();
     render_manager.win32_context.gl_context = win32_globals.gl_context;
     render_manager.win32_context.device_context = win32_globals.device_context;
     render_manager.win32_context.window_handle = win32_globals.window_handle;
}

void rm::init()
{ 
    rm::init_render_manager();
    auto& render_manager = rm::render_manager();

    rm::init_opengl();
    rm::init_imgui();
    rm::init_cvars();
    rm::clear_buffer_bits();
}


void rm::init_opengl()
{
    //@NOte::init gl_lite only after the gl_context has been created
    // (which is done in the win32 section of the program, since that is OS related.)
    gl_lite_init();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(40.0f/255.0f, 45.0f/255.0f, 54.0f/ 255.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

};


void rm::init_imgui()
{
  	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(graphics::global_Win32_context().window_handle);
    std::string glsl_version = "#version 430";
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

void rm::init_cvars()
{

}


// void rm::update_active_camera(const io::Mouse_State& mouse_state, const io::Keyboard_State& keyboard_state)
// {
//     static float camera_z_accumulator = 5.0f;
//     static float x_rotation_accumulator = 0.0f;
//     static float y_rotation_accumulator = 0.0f;

//     static float yaw = 0.0f; // left - right
//     static float pitch = 0.0f; // up - down

//     using namespace cam;

//     auto& camera = graphics::active_camera();
//     switch (camera.control_mode)
//     {
//         case Control_Mode::CAM_ORBIT:
//         {
//             if (mouse_state.lmb_pressed)
//             {
//                 x_rotation_accumulator += mouse_state.pos_delta_x * 0.005f;
//                 x_rotation_accumulator =  fmod(x_rotation_accumulator, 360.0f);
//                 y_rotation_accumulator += mouse_state.pos_delta_y * 0.005f;
//                 y_rotation_accumulator =  fmod(y_rotation_accumulator, 360.0f);
//             }
   
//             if (mouse_state.scroll_delta_y != 0.0f)
//             {
//                 camera_z_accumulator = (mouse_state.scroll_delta_y > 0.0f) ? camera_z_accumulator - 0.05f : camera_z_accumulator + 0.05f;                 
//             }

//             mgl::vec3 camera_position{0.0f,0.0f, camera_z_accumulator};
//             mgl::vec3 target_position{0.0f, 0.0f, 0.0f}; 
//             mgl::vec3 up_vector{0.0f, 1.0f, 0.0f}; 

//             mgl::mat4 x_rotation_matrix = mgl::rotate(mgl::mat4_identity(), y_rotation_accumulator, mgl::vec3{1.0f, 0.0f, 0.0f});
//             mgl::mat4 y_rotation_matrix = mgl::rotate(mgl::mat4_identity(), x_rotation_accumulator, mgl::vec3{0.0f, 1.0f, 0.0f});                   

//             mgl::mat4 rotation_matrix = y_rotation_matrix * x_rotation_matrix;

//             up_vector       = mgl::make_vec3(rotation_matrix * mgl::make_vec4(up_vector, 1.0f));
//             camera_position = mgl::make_vec3(rotation_matrix * mgl::make_vec4(camera_position, 1.0f));

//             mgl::mat4 view_matrix = mgl::look_at(camera_position, target_position, up_vector);

//             // @Note: near_z and far_z should be positive in this context (DISTANCE to camera).
//             const float fov_in_degrees     = cvar::get_float("r_fov");
//             const float perspective_near_z = 0.1f;
//             const float perspective_far_z  = 200.0f;
//             const float aspect_ratio = static_cast<float>(globals.window_width) / static_cast<float>(globals.window_height);
//             mgl::mat4 projection_matrix = mgl::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);

//             update_uniform("projection_matrix", projection_matrix);
//             update_uniform("view_matrix", view_matrix);

//             break;
//         }
//         case Control_Mode::CAM_ISOMETRIC:
//         {
//             break;
//         }
//         case Control_Mode::CAM_FPS:
//         {
//             yaw   += mouse_state.pos_delta_x * 0.005f;
//             pitch += mouse_state.pos_delta_y * 0.005f;  

//             if(pitch >  89.0f) pitch =  89.0f;
//             if(pitch < -89.0f) pitch = -89.0f;

//             mgl::vec3 direction{};
//             direction.x = cos(mgl::radians(yaw)) * cos(mgl::radians(pitch));
//             direction.y = sin(mgl::radians(pitch));
//             direction.z = sin(mgl::radians(yaw)) * cos(mgl::radians(pitch));
//             mgl::vec3 camera_front = mgl::normalize(direction);

//             if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//                 cameraPos += cameraSpeed * cameraFront;
//             if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//                 cameraPos -= cameraSpeed * cameraFront;
//             if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//                 cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//             if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//                 cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            
            
//             break;
//         }

//         default:
//         {
//             break;
//         }
//     }
// }

// void graphics::render_game_3d()
// {

//     uint32_t active_shader_id = graphics::active_shader_id();
//     // auto defer_shader_state = On_Leaving_Scope([]{graphics::set_shader("gouraud");});


//     //@IC: keep this disabled for now. camera is updated.
//     //render_3d_left_handed_perspective(active_shader_id); // perspective matrix.

//     // if (active_shader_id == graphics::shaders()["gouraud"])
//     // {
//     //     update_uniform("light_position", mgl::vec3{50.0f, 100.0f, 100.0f});
//     //     update_uniform("light_color", mgl::vec3{0.0f, 0.0f, 0.1f});
//     //     update_uniform("material", mgl::vec4{0.9f, 0.9f, 0.9f, 32.0f});
//     // }

//     if (active_shader_id == graphics::shaders()["cel"])
//     {
//         update_uniform("light_position", mgl::vec3{50.0f, 100.0f, 100.0f});
//         update_uniform("light_color", mgl::vec3{0.0f, 0.0f, 0.1f});
//         update_uniform("material", mgl::vec4{0.9f, 0.9f, 0.9f, 32.0f});
//     }


//     glBindVertexArray(graphics::buffers()["cat.obj"].VAO);

//     mgl::mat4 view_matrix = std::get<mgl::mat4>(graphics::shader_info()[graphics::active_shader_name()].uniforms["view_matrix"].data);

//     for (auto &set_piece: graphics::active_scene().set_pieces)
//     {
//         mgl::mat4 model_matrix = mgl::model_from_xform_state(set_piece.xform_state);
//         mgl::mat4 model_view_matrix =  view_matrix * model_matrix; // reverse order. 
//         mgl::mat3 model_normal_matrix = mgl::normal_transform(model_matrix);

//         update_uniform("model_matrix", model_matrix);
//         update_uniform("model_normal_matrix", model_normal_matrix);

//         if (active_shader_id == graphics::shaders()["gouraud"])
//         {
//             asset::Texture& texture = asset::texture_data()[set_piece.texture_name];
//             glActiveTexture(GL_TEXTURE0 + texture.gl_texture_frame);
//             glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
//             update_uniform("texture_uniform",  texture.gl_texture_frame);
//         }

//         //@FIXME: for now, we invoke draw after every object. 
//         const auto& object_data = asset::obj_data()[set_piece.model_name];
//         glDrawArrays(GL_TRIANGLES,0, object_data.vertices.size());
//     }


//     render_centroid_axes();


//     glBindVertexArray(0);
// }

// // render axes for the 0,0,0 point.
// void graphics::render_centroid_axes()
// {

//     mgl::vec4 x_axis_end{0.5f,0.0f,0.0f, 1.0f};
//     mgl::vec4 y_axis_end{0.0f,0.5f,0.0f, 1.0f};
//     mgl::vec4 z_axis_end{0.0f,0.0f,0.5f, 1.0f};
//     mgl::vec4 zero_start{0.0f,0.0f,0.0f,1.0f};

//     graphics::set_shader("cel");
//     mgl::mat4 view_matrix       = std::get<mgl::mat4>(graphics::shader_info()["cel"].uniforms["view_matrix"].data);
//     mgl::mat4 projection_matrix = std::get<mgl::mat4>(graphics::shader_info()["cel"].uniforms["projection_matrix"].data);

//     graphics::set_shader("line_position_color");

//     x_axis_end =  projection_matrix * view_matrix * x_axis_end;
//     y_axis_end =  projection_matrix * view_matrix * y_axis_end;
//     z_axis_end =  projection_matrix * view_matrix * z_axis_end;
//     zero_start =  projection_matrix * view_matrix * zero_start;

//     mgl::vec4 x_axis_color{1.0f, 0.0f, 0.0f, 1.0f};
//     mgl::vec4 y_axis_color{0.0f, 1.0f, 0.0f, 1.0f};
//     mgl::vec4 z_axis_color{0.0f, 0.0f, 1.0f, 1.0f};

//     glBindVertexArray(graphics::buffers()["line_position_color"].VAO);

//     std::vector<mgl::vec4> data{
//         zero_start,
//         x_axis_color,
//         x_axis_end,
//         x_axis_color,
//         zero_start,
//         y_axis_color,
//         y_axis_end,
//         y_axis_color,
//         zero_start,
//         z_axis_color,
//         z_axis_end,
//         z_axis_color
//     };
//     glBufferData(GL_ARRAY_BUFFER,
//         static_cast<int32_t>(data.size() * sizeof(mgl::vec4)),
//         data.data(),
//         GL_STATIC_DRAW);

//     glDrawArrays(GL_LINES,0, data.size() / 2); 

// }

// void graphics::render_ui()
// {
//       // Start the Dear ImGui frame
//     ImGui_ImplOpenGL3_NewFrame();
//     ImGui_ImplWin32_NewFrame();
//     ImGui::NewFrame();

//     bool show_demo_window = false;
//     if (show_demo_window)
//             ImGui::ShowDemoWindow(&show_demo_window);

//     std::vector<const char *> shader_names;
//     for (auto& [shader_name, _]: graphics::shader_info())
//     {
//         shader_names.push_back(shader_name.c_str());
//     }
//     // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//     {
//         ImGui::Begin("Menu");                        
//         ImGui::Text("Render Settings");

//         float fov_value = cvar::get_float("r_fov");
//         if (ImGui::SliderFloat("FOV", &fov_value, 45.0f, 90.0f, "%.3f", 1.0f))
//         {
//             cvar::set_float("r_fov", fov_value);
//         }
//         // List box
//         static int selected_shader = 1;
//         ImGui::ListBox("Shader Mode", &selected_shader, shader_names.data(), shader_names.size(), 4);
//         graphics::set_shader(shader_names[selected_shader]);
//         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//         ImGui::End();
//     }
    
//     ImGui::Render();

//     glViewport(0, 0, globals.window_width, globals.window_height);
//     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
// }




// ///--- Render Modes -------------------------------------------------------

// void graphics::render_2d_left_handed_dc(const uint32_t active_shader_id)
// {
//     //@Note: do NOT forget this.
//     const bool row_major = true;

//     const uint32_t window_height = globals.window_height;
//     const uint32_t window_width = globals.window_width;
//     const float top   = window_height; // viewport 
//     const float bot   = 0.0f;
//     const float left  = 0.0f;
//     const float right = window_width; // viewport
//     const float z_near  = 0.0f;
//     const float z_far   = 1.0f; // near and far are reserved by windows???


//     mgl::mat4 projection_matrix = mgl::ortho(left, right, top, bot, z_near, z_far); 
//     update_uniform("text", projection_matrix);
// }

// void graphics::render_3d_left_handed_perspective(uint32_t active_shader_id)
// {
//     //@Note: do NOT forget this.
//     const bool row_major = true;

//     //@Note: view matrix is identity (i.e. opengl_left_handed, mgl::vec3(0.0,0.0,0.)
//     mgl::vec3 camera_position{0.0f,0.0f, 2.0f};
//     mgl::vec3 target_position{0.0f, 0.0f, 0.0f};
//     mgl::vec3 up_vector{0.0f, 1.0f, 0.0f};
//     mgl::mat4 view_matrix = mgl::look_at(camera_position, target_position, up_vector);
   
//     // @Note: near_z and far_z should be positive in this context (DISTANCE to camera).
//     const float fov_in_degrees = cvar::get_float("r_fov");
//     const float perspective_near_z = 0.1f;
//     const float perspective_far_z  = 1000.0f;
//     const float aspect_ratio = static_cast<float>(globals.window_width) / static_cast<float>(globals.window_height);
//     mgl::mat4 projection_matrix = mgl::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);

//     update_uniform("projection_matrix", projection_matrix);
//     update_uniform("view_matrix", view_matrix);

//     glm::mat4 actual_projection_matrix = glm::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);

//     glm::vec3 actual_camera_position(0.0f, 0.0f, 2.0f);
//     glm::vec3 actual_target_position(0.0f, 0.0f, 0.0f);
//     glm::vec3 actual_up_vector(0.0f, 1.0f, 0.0);

//     // glm::mat4 actual_view_matrix = glm::lookAt(actual_camera_position, actual_target_position, actual_up_vector);

//     // fmt::print("my view matrix: {}\n", view_matrix);
//     // fmt::print("actual view matrix: {}\n", glm::to_string(actual_view_matrix));
//     // fmt::print("my projection matrix: {}\n", projection_matrix);
//     // fmt::print("actual projection matrix: {}\n", glm::to_string(actual_projection_matrix));

// }



//--- frame buffer

void rm::swap_buffers()
{
    SwapBuffers(graphics::global_Win32_context().device_context);
}


// static
void rm::clear_buffer_bits()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

