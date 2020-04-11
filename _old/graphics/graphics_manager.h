#ifndef INCLUDED_GRAPHICS_MANAGER_
#define INCLUDED_GRAPHICS_MANAGER_



namespace gm
{
	// either a VAO needs to rebind its VBO, 
	// or each VAO has a VBO.
	// let's stick to the last thing for now.

	struct Graphics_Settings
	{
		// opengl context already exists.
	}
	class Graphics_Manager
	{
		// Graphics_Settings settings;
		// std::map<std::string, uint32_t>* shaders;
		// std::map<std::string, Buffers>*  buffers;
		// std::map<std::string, Texture>* textures;

		public:
			Graphics_Manager() = delete;
			Graphics_Manager(Graphics_Settings& new_settings)
			:
				settings(new_settings)
			{
			}
 			Graphics_Manager(const Graphics_Manager&) = delete;
 		    Graphics_Manager& operator=(const Graphics_Manager&) = delete; 
 		    Graphics_Manager(Graphics_Manager&&) = delete;
	};

	void load_compile_attach_shader()
	{

	}
};



// if it's only functions, how do we deal with state?
// graphics can use a list of shaders
// graphics can take a pointer to a list of shaders?

// void game::graphics_test()
// {

//     // all of this means that graphics is aware of some type of manager.
//     // which I think makes sense.
//     // problem is still the contract that the manager enforces.
//     // How do we make people aware?

//     // sound_manager
//     // auto & camera =  scene_manager->create_camera()
//     // camera.position = {};
//     // camera.orientation = lookat()
//     // canera.near_clip_distance
//     // camera.far_clip_distance 
//     // 
//     Scene_Manager* scene_manager = new Scene_Manager();
//     Camera* camera =  create_camera(scene_manager);
//     camera->position = {};
//     camera->orientation = {};
//     //@note: these should be in Z.
//     camera->near_clip_distance = 1.0; 
//     camera->far_clip_distance = 100.0;
//     camera->aspect_ratio  = globals->window_width / globals->window_height;
//     camera->field_of_view = 90.0f;
//     // communicate the use of more than one camera?

//     // new idea: the construction of the graphics manager preps the graphics environment.
//     // (i.e. what is not called init_opengl.)
//     // but what does graphics offer?
//     // we can register shaders via the graphics manager
//     // and .....? request VBOs and VAOs?
//     // manages the texture state?
//     // keep the manager a compositional type?
//     Graphics_Manager* graphics_manager = new Graphics_Manager();
//     create_shader();
//     load_compile_attach_shader();
//     create_shader();
//     load_compile_attach_shader();
//     create_shader();
//     load_compile_attach_shader();
//     load_compile_attach_shader();
//     load_compile_attach_shader();
//     load_compile_attach_shader();
//     create_shader();


//     // scene_manager->add_light()
//     // 


//     Shader_Program;
//     load_compile_attach_shader
//     load_compile_attach_shader
//     bind
//     detach_shader
//     detach_shader
//     graphics::shaders.push_back()
//     // implicit connection between font and graphics.
//     // can we use font without graphics?
//     font VAO
//     font VBO
//     graphics::left_handed_3d();

// }
// more the bookkeeper than the manager, but whatever.


#endif