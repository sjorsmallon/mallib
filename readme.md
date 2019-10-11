

# near future TODO
look at harfbuzz for text shaping.
text effects? (pulse?)

# The const curse
constness spreads throughout the application as a virus. What's mostly annoying is that constness should be more like 
a contract in terms of what we are trying to do with it, but it also forces us to write more operator overloads.
which is kind of annoying. I'm not sure what road I should go yet. I feel constness should actually be that 
by default: let everything be const instead of mutable. sigh.


# Long Term Goals
This repository should contain a set of classes and structures that allow any user
to easily construct a game engine. This is very work-in-progress; don't expect things to stay the same for very long.




## openGL extension loading
openGL on windows is convoluted. windows' `Gl\Gl.h` only includes the basic openGL stuff. In order for it to be usable, we perform some black magic. In order to set context flags for openGl > 3.3, we need an openGl context. We create a dummy window, through which we are able to init a proper openGl context, as the context needs to be set before opening a window, as we can only change the context through a window.

Furthermore, openGL extension loading is also tricky. Normally, libraries like GLEW handle the loading of extensions. The problem with those libraries is distribution: it is one more thing to deploy and link against, etc. The way we do it now is sort of manually: using `gl_lite.h`, we can define and include types and functions, specified in `gl_ext.h`(the official gl_extensions header). This will result in lower compile times & less code bloat overall. It feels archaic, but it works.

The way it works is we need to include "gl_lite.h" everywhere we would use openGL functions. In one of the source files, we need a `#define`. I would suggest putting this define in the entrypoint of your application.


## Writing as we go
I'll keep updating `main_test.cc` as we go along. I have a habit of writing code I may or may not use. Keeping the YAGNI principle in mind, I'll just write what I need. I do want to think about APIs for these tools that I will need (such as the renderer or the audio player), but superficial substitutions will do.


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


# TODO

## platform Task Queue
read about task queueing, work stealing and multithreading.

## `Vec3` 
Perform the `POD` test again with the union vec3. 

## `sound_device`
only allow valid wav files (not compressed!). See how HHH handles the file reading.
It's very similar.

typedef uint32_t to buffer / sound_source. But does that coerce `ALuint` to `uint32_t`?

Move the buffers to outside of the class. Should be in data storage. Play sound via string? or via enum name? <- this is annoying, as user needs to add enums. better to instantiate map based on filenames.

pool pattern. 
Maximum of (32) sound sources. 
at startup, create 32 ALsources.
ask pool for a source. (implement as ring buffer?)
if no free sources, stop longest playing (priority list?)
classify pool into groups:

1. 1-8 unit sfx
2. 8-12 explosions
3. you name it.

# HAVE DONE

## wav files
read / play wav file



