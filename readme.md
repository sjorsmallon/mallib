
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



