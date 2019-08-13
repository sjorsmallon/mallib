
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



# TODO

## platform Task Queue
read about task queueing, work stealing and multithreading.

##`Vec3` 
Perform the `POD` test again with the union vec3. 

##`sound_device`
only allow valid wav files (not compressed!). See how HHH handles the file reading.
It's very similar.

typedef uint32_t to buffer / sound_source. But does that coerce ALuint to uint32_t?

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



