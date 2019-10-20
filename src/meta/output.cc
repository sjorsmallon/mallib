
#include <type_traits>
#include <iostream>

//camera\camera.h
struct Camera

{

    Vec3 offset;

    Vec3 shift;



    float pitch;

    float orbit;

    float dolly;



    float focal_length;

    float near_clip_plane;

    float far_clip_plane;

};

//entity\entity.h
struct Entity

{

	bool marked_for_destruction;

	

};

//file_watcher\file_watcher.h
struct File_Watcher

{

	std::vector<>

};

//font\font.h
	struct Character

	{

		GLuint textureID;

		Vec2i size;

		Vec2u bearing;

		int32_t advance;

	};

//font\font.h
	struct Font

	{

		std::vector<Character> characters;

		// std::array<Character, 255?>& characters;

		Font() = default;

		~Font()

		{

			for(const auto& character: characters)

				glDeleteTextures(1, &character.textureID);

		}

		Font(const Font&) = delete;

		Font& operator=(const Font&) = delete;

		

	};

//font\font.h
	struct gl_Objects

	{

		GLuint VAO;

		GLuint VBO;

	};

//globals\globals.h
	struct Window_Settings

	{

		Vec2i window_dimensions;

	};

//globals\globals.h
	struct Global_Input

	{

		Vec2i mouse_coords;

		std::vector<Key_Input> input_queue;

	};

//globals\globals.h
	struct Campaign_State

	{

		uint32_t current_level;

		uint32_t master_volume;

		uint32_t music_volume;

	};

//graphics\graphics.h
	struct Window_Settings

	{

		float width;

		float height;

		bool v_sync;

	};

	struct Shaders //@Note: shader_programs?

	{

		uint32_t bomb;

		uint32_t text;

		uint32_t normals;

		uint32_t default;

	};

//graphics\graphics.h
	struct Win32_Context

	{

		HGLRC gl_context;

		HDC	device_context;

	};

//mat4\mat4.h
struct Mat4

{

    // Vec4 d_matrix[4];

    float data[4][4]; // we can always change this. for now, let's do this.

};

//memory_pool\memory_pool.h
	struct Pool

	{

	  byte* data; 

	  size_t size;

	  size_t occupied;

	  byte* occupied_mark;

	  // unsigned char* high_water_mark;

	  std::vector<byte*> indices;

	  Memory_Pool* Next_Memory_Pool  = nullptr;

	  bool has_next = false;

	  bool is_dirty = true;

	};

//quaternion\quaternion.h
struct Quaternion

{

    float x;

    float y;

    float z;

    float w;

};

//ring_buffer\ring_buffer.h
struct Ring_Buffer

{

	size_t first_free_spot;

	size_t size;

	std::vector<Type> data;



	public:



		Ring_Buffer(size_t size)

		:

			data(size),

			size(data.size()),

			first_free_spot(0)

		{

		}



	void insert(Type T)

	{

		//@Speed:

		vector.at(first_free_spot) = T; // this overrides... hmm. This will only work for ints or such.

		++first_free_spot;

		first_free_spot %= size;

	}

};

//short_allocator\short_alloc.h
    template <class _Up> struct rebind {using other = short_alloc<_Up, size, alignment>;};

//sound\sound.h
	struct Music

	{

		int handle;

		//@Memory: for now, this is a unique ptr

		// we want to not have ownership actually,

		// but just let the allocator know that the memory can be used again.

		// however, let's first make this work.

		// In retrospect, this is a terrible idea,

		// since it's not the whole file, but a stream.

		// anyway. 

		std::unique_ptr<wav_stream> stream_ptr;

	};

//sound\sound.h
	struct Sound

	{

		int handle;

		wav_file* wav_ptr;

	};

//string\string.h
struct SString

{

    char data[128];

};

//vec2\vec2.h
struct Vec2

{

    float x;

    float y;

};

//vec2\vec2.h
struct Vec2i

{

	int32_t x;

	int32_t y;

};

//vec2\vec2.h
struct Vec2u

{

	uint32_t x;

	uint32_t y;

};

//vec3\vec3.h
        struct 

        {

            float r;

            float g;

            float b;

        };

//vec4\vec4.h
    struct 

    {

        float r;

        float g;

        float b;

        float a;

    };

//xform_state\xform_state.h
struct Xform_State

{

    Quaternion orientation;

    Vec3 position;

    float scale;

};


int main()
{
    std::cout << std::boolalpha
    // for all types, 

std::cout << std::boolalpha << "Camera is trivial: "std::is_trivial<Camera>::value<< std::endl;
std::cout << std::boolalpha << "Camera is standard layout: "std::is_standard_layout<Camera>::value<< std::endl;
 
std::cout << std::boolalpha << "Entity is trivial: "std::is_trivial<Entity>::value<< std::endl;
std::cout << std::boolalpha << "Entity is standard layout: "std::is_standard_layout<Entity>::value<< std::endl;
 
std::cout << std::boolalpha << "File_Watcher is trivial: "std::is_trivial<File_Watcher>::value<< std::endl;
std::cout << std::boolalpha << "File_Watcher is standard layout: "std::is_standard_layout<File_Watcher>::value<< std::endl;
 
std::cout << std::boolalpha << "Character is trivial: "std::is_trivial<Character>::value<< std::endl;
std::cout << std::boolalpha << "Character is standard layout: "std::is_standard_layout<Character>::value<< std::endl;
 
std::cout << std::boolalpha << "Font is trivial: "std::is_trivial<Font>::value<< std::endl;
std::cout << std::boolalpha << "Font is standard layout: "std::is_standard_layout<Font>::value<< std::endl;
 
std::cout << std::boolalpha << "gl_Objects is trivial: "std::is_trivial<gl_Objects>::value<< std::endl;
std::cout << std::boolalpha << "gl_Objects is standard layout: "std::is_standard_layout<gl_Objects>::value<< std::endl;
 
std::cout << std::boolalpha << "Window_Settings is trivial: "std::is_trivial<Window_Settings>::value<< std::endl;
std::cout << std::boolalpha << "Window_Settings is standard layout: "std::is_standard_layout<Window_Settings>::value<< std::endl;
 
std::cout << std::boolalpha << "Global_Input is trivial: "std::is_trivial<Global_Input>::value<< std::endl;
std::cout << std::boolalpha << "Global_Input is standard layout: "std::is_standard_layout<Global_Input>::value<< std::endl;
 
std::cout << std::boolalpha << "Campaign_State is trivial: "std::is_trivial<Campaign_State>::value<< std::endl;
std::cout << std::boolalpha << "Campaign_State is standard layout: "std::is_standard_layout<Campaign_State>::value<< std::endl;
 
std::cout << std::boolalpha << "Window_Settings is trivial: "std::is_trivial<Window_Settings>::value<< std::endl;
std::cout << std::boolalpha << "Window_Settings is standard layout: "std::is_standard_layout<Window_Settings>::value<< std::endl;
 
std::cout << std::boolalpha << "Win32_Context is trivial: "std::is_trivial<Win32_Context>::value<< std::endl;
std::cout << std::boolalpha << "Win32_Context is standard layout: "std::is_standard_layout<Win32_Context>::value<< std::endl;
 
std::cout << std::boolalpha << "Mat4 is trivial: "std::is_trivial<Mat4>::value<< std::endl;
std::cout << std::boolalpha << "Mat4 is standard layout: "std::is_standard_layout<Mat4>::value<< std::endl;
 
std::cout << std::boolalpha << "Pool is trivial: "std::is_trivial<Pool>::value<< std::endl;
std::cout << std::boolalpha << "Pool is standard layout: "std::is_standard_layout<Pool>::value<< std::endl;
 
std::cout << std::boolalpha << "Quaternion is trivial: "std::is_trivial<Quaternion>::value<< std::endl;
std::cout << std::boolalpha << "Quaternion is standard layout: "std::is_standard_layout<Quaternion>::value<< std::endl;
 
std::cout << std::boolalpha << "Ring_Buffer is trivial: "std::is_trivial<Ring_Buffer>::value<< std::endl;
std::cout << std::boolalpha << "Ring_Buffer is standard layout: "std::is_standard_layout<Ring_Buffer>::value<< std::endl;
 
std::cout << std::boolalpha << "rebind is trivial: "std::is_trivial<rebind>::value<< std::endl;
std::cout << std::boolalpha << "rebind is standard layout: "std::is_standard_layout<rebind>::value<< std::endl;
 
std::cout << std::boolalpha << "Music is trivial: "std::is_trivial<Music>::value<< std::endl;
std::cout << std::boolalpha << "Music is standard layout: "std::is_standard_layout<Music>::value<< std::endl;
 
std::cout << std::boolalpha << "Sound is trivial: "std::is_trivial<Sound>::value<< std::endl;
std::cout << std::boolalpha << "Sound is standard layout: "std::is_standard_layout<Sound>::value<< std::endl;
 
std::cout << std::boolalpha << "SString is trivial: "std::is_trivial<SString>::value<< std::endl;
std::cout << std::boolalpha << "SString is standard layout: "std::is_standard_layout<SString>::value<< std::endl;
 
std::cout << std::boolalpha << "Vec2 is trivial: "std::is_trivial<Vec2>::value<< std::endl;
std::cout << std::boolalpha << "Vec2 is standard layout: "std::is_standard_layout<Vec2>::value<< std::endl;
 
std::cout << std::boolalpha << "Vec2i is trivial: "std::is_trivial<Vec2i>::value<< std::endl;
std::cout << std::boolalpha << "Vec2i is standard layout: "std::is_standard_layout<Vec2i>::value<< std::endl;
 
std::cout << std::boolalpha << "Vec2u is trivial: "std::is_trivial<Vec2u>::value<< std::endl;
std::cout << std::boolalpha << "Vec2u is standard layout: "std::is_standard_layout<Vec2u>::value<< std::endl;
 
std::cout << std::boolalpha << "struct is trivial: "std::is_trivial<struct>::value<< std::endl;
std::cout << std::boolalpha << "struct is standard layout: "std::is_standard_layout<struct>::value<< std::endl;
 
std::cout << std::boolalpha << "struct is trivial: "std::is_trivial<struct>::value<< std::endl;
std::cout << std::boolalpha << "struct is standard layout: "std::is_standard_layout<struct>::value<< std::endl;
 
std::cout << std::boolalpha << "Xform_State is trivial: "std::is_trivial<Xform_State>::value<< std::endl;
std::cout << std::boolalpha << "Xform_State is standard layout: "std::is_standard_layout<Xform_State>::value<< std::endl;
 
 

}

['Camera', 'Entity', 'File_Watcher', 'Character', 'Font', 'gl_Objects', 'Window_Settings', 'Global_Input', 'Campaign_State', 'Window_Settings', 'Win32_Context', 'Mat4', 'Pool', 'Quaternion', 'Ring_Buffer', 'rebind', 'Music', 'Sound', 'SString', 'Vec2', 'Vec2i', 'Vec2u', 'struct', 'struct', 'Xform_State']
