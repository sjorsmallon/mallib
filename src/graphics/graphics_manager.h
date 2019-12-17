#ifndef INCLUDED_GRAPHICS_MANAGER_
#define INCLUDED_GRAPHICS_MANAGER_

// more the bookkeeper than the manager, but whatever.

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


#endif