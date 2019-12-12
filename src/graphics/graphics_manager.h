#ifndef INCLUDED_GRAPHICS_MANAGER_
#define INCLUDED_GRAPHICS_MANAGER_

namespace gm
{

	struct Buffers
	{
		uint32_t VAO;
		uint32_t VBO;
		uint32_t IBO;
	}

	struct Graphics_Settings
	{

	}
	class Graphics_Manager
	{
		Graphics_Settings settings;
		std::map<std::string, uint32_t>* shaders;
		std::map<std::string, Buffers>* buffers;



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

};


#endif