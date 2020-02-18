

# shortlist TODO

## move math functionality into their own library. (move all .mat functions to source file?)

## re-enable textures

## remove IO from windows folder in some way. Look at how Casey does platform specific functionality.

## implement Uniform Buffer object for matrices (no need to rebind these between shaders!)
Look at the learnopenGL implementation how to do this.


## move towards MVP mode of operation. Rethink scene stuff. 

## create simple ECS

## switch to tinySTL (with custom allocator).


## Rotating the Scene Objects Correctly
currently, the ordering w.r.t translation/ rotation is borked because of the row major decision.
Maybe we should keep it fire & forget for now.. or just abstract it as a whole.

refactor the namespaces to some sort of init -> alloc pattern. (alloc can be called in init, but just that that is the regular pattern).


switch to the custom allocator for trying new stuff out.

implement font waving up and down.

## include IMGUI
We want to use IMGUI as some sort of debugging UI menu. IMGUI extension: allow IMGUI to run as detached window.


# longlist TODO



## Graphics manager
Look at how Ogre3D does it. Try something similar. https://ogrecave.github.io/ogre/api/latest/setup.html

## VAO / VBO structure
How are relevant `entity` meshes combined in the VBO? Do we have one? What do other people use? 
according to  https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices , The best way to do it is to create
as few VBOs as possible. I'm wondering whether it's possible to keep data on the gpu and re-render that data (instead of needing to supply
a model multiple times in the same VBO.) (this is possible.)

### Ideas of how to handle the VAO/VBO situation
use a VAO for generic drawings (for now, just update the VBO everytime. We could look at concatenating it. or switching to an IBO.)
secondly, use a VAO/VBO combo for entity placement (i.e. light locations.)

## Rotating the Scene Objects with Input
we want to manipulate the scene using the mouse / keyboard. 



## look at input polling thread.


## Unify normalize & transpose syntax.

## Fix how we set Active Shader
we want some `active_shader` pointer or reference in graphics, since the way we do it now is convoluted.

##  Texture descriptions
Devise some sort of construct / order / layout which specifies the texture resolution.

## Better errors when things are not specified in the scene format
We need to know when textures etc are not specified properly, so we can save some debugging time.

## Shader reloading && Refactor adding shaders.
Shader reloading is almost there. Let's finish it.

## Animation
I think I know how to do animation, but I am not sure. We'll try with the cat.

# Particle system
Create a simple particle system.

## Expand the Scene description
Look at what sounds are played when? which animations to load? starting pose? scene graph?

## Unify graphics / Font / Asset
All of these use some form of the `Vertex` struct. Where should it be defined? The problem is that they all need to agree on the common form in order for them to be able to communicate, but that also introduces some interdependence.

## Font rendering
look at harfbuzz for text shaping.
text effects? (pulse? Wavy stretchy?)

## `sound_device`
only allow valid wav files (not compressed!). See how HHH handles the file reading.
It's very similar.

pool pattern. 
Maximum of (32) sound sources. 
at startup, create 32 ALsources.
ask pool for a source. (implement as ring buffer?)
if no free sources, stop longest playing (priority list?)
classify pool into groups:

1. 1-8 unit sfx
2. 8-12 explosions
3. you name it.

## platform Task Queue
read about task queueing, work stealing and multithreading.

# Concepts
look at the tprint concept, as well as the on_leaving_scope thing.

# Globals, Singletons, and modularity
For now, I'm using the Static singleton pattern for some global state management. Some of these can move into some sort of global struct,
which every gameplay module gets access to. The problem for now is I want to properly compartmentalize all of the
several substructures in the program (for example, `font` or `graphics`). This allows me to have a global object that is accessible
in the namespace. An additional benefit for now is that I can work with some global object without thinking about where it lives, while
still allowing me to change where it lives live later on without a huge amount of refactoring. I want to take a look at the `stb` 
files, as their concept of ownership and memory management are very useful: either they do it for you (via malloc etc), or they
request the user to do so, and provide a different set of functions for this.

# using the STL
It saves me some pain dealing with heap allocations / memory management. I do not have a 
solid concept on handling memory, or at least, I just don't know how to do it properly. So instead of doing a very bad job,
I'm not doing a job at all. to be fair, I mostly use the containers of the STL, like `vector`, `map` and `string`. `string` is 
especially difficult, since I don't know what I want to do. most C APIs expect some sort of NTBS, but I don't like that. 
superficially, a string class should be a `size`and a`capacity`. Furthermore, I don't know much about `utf8`, but that will
be a problem for a later date.


# The const curse
constness spreads throughout the application. What's mostly annoying is that constness should be more like 
a contract in terms of what we are trying to do with it, but it also forces us to write more operator overloads and stricter function definitions, which is kind of annoying.


## openGL extension loading
openGL on windows is convoluted. windows' `Gl\Gl.h` only includes the basic openGL stuff. In order for it to be usable, we perform some black magic. In order to set context flags for openGl > 3.3, we need an openGl context. We create a dummy window, through which we are able to init a proper openGl context, as the context needs to be set before opening a window, as we can only change the context through a window.

Furthermore, openGL extension loading is also tricky. Normally, libraries like GLEW handle the loading of extensions. The problem with those libraries is distribution: it is one more thing to deploy and link against, etc. The way we do it now is sort of manually: using `gl_lite.h`, we can define and include types and functions, specified in `gl_ext.h`(the official gl_extensions header). This will result in lower compile times & less code bloat overall. It feels archaic, but it works.

The way it works is we need to include `"gl_lite.h"` everywhere we would use openGL functions. In one of the source files, we need a `#define`. I would suggest putting this define in the entrypoint of your application.


## sound API Design
the sound_player is a global namespace. It contains a map with string names and sound files. we can register a sound through the filename (no paths, just the filename), and play it by invoking the filename as well. Maybe I want to change this to some sort of sound ID (enum?) but I would want to generate the enum at compile time. We will see how this works out.

## renderer API design
I have no idea how to properly write a renderer API. I'll try to look at how Ogre does it.

# Entrypoints


# CMake
CMake is not a build tool, but cmake is a build generator. It can generate platform specific build files (i.e. make or a VS solution).

the build directory is used to generate all the necessary files in order to build the application. it is not as complex as it sounds.
We can use cmake to select 32 or 64 bit file generation (which is kind of weird, but ok). It selects a different compiler on the VS side.
unsure what happens on the linux side.

usage: 'cmake  -DCMAKE_GENERATOR_PLATFORM=x64 ..'

using other code is weird though.

# Visual studio settings
Cmake generates a 64 bit solution (????), but we need to change the build settings to build a lib instead of a DLL. furthermore, we need to change
the `code generation -> runtime library` field to either Multithreaded or multithreaded DLL (we're building libs, so that does not apply here).

# use of FMT 
FMT is only used in the glue parts of the engine, not in the classes / other files.
any error reporting there is done using `cerr` (for now).

# use of SOLOUD
I need to document how to build soloud, as it uses this weird build system. It's a nice library though.




# HAVE DONE


## Contracts for namespaces
What do namespaces need in order to function? i.e. some sort of mandatory "constructor". I am leaning to a `Settings` struct.

Rename shader uniforms (projection_model -> projection_matrix)

`glLinkProgram` can fail. check the status of the program after linking (i.e. glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);



## See how to fix font / texture rendering
The way font renders textures does something funky with the `glPixelParameteri` that we need to undo before trying to render our own textures.

## Render Textures
exactly what it sounds like. They render, but they are reversed atm.

## Scaffolding for Asset loading
We need to load the assets. i.e. the cat model. The structure I have in my mind for storing the model is 
a string -> vector<Vertex> map.

## Scene
create a scene (level?) So we know what to load when.
The Scene will be read from a file. For now, the file will be described by
``` 
// these are comments.
# entity_name
model name
material name
texture name
position
q_orientation
scale.
``` 

## SYNTAX
unify all from -> to syntax in the program. All the loading functions (load_obj_from_file) 
and the file functions(file_to_string). This is inconsistent.

the `load_x` functions use a from -> to pattern.


## `Vec3` 
Perform the `POD` test again with the union vec3. 

typedef uint32_t to buffer / sound_source. But does that coerce `ALuint` to `uint32_t`?

Move the buffers to outside of the class. Should be in data storage. Play sound via string? or via enum name? <- this is annoying, as user needs to add enums. better to instantiate map based on filenames.

## wav files
read / play wav file



