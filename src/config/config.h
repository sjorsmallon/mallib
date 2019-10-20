#ifndef INCLUDED_CONFIG_
#define INCLUDED_CONFIG_
#include "../file/file.h"
#include <string>

/*
The config file format works as follows:

# denotes a header. This is used to denote the catalog
of settings that is about to come.

A special type of header is # Developer. These contain
settings that can be used specifically for us.
Expect these to be both in local.config and global.config. 
As is to be expected, the local.config will not be placed under
Version control.

After it, we can either have three values:
floats, ints, and strings.

floats are denoted by numerical notation, i.e. 0.0. 
ints are unterminated numbers.
strings are indicated by " and end on " (the literal).

A problem that is becoming apparent now is that we need
to poke these values into place, but this will quickly become tedious
to add new values to the config. We then need to add 
both to the struct and the the loading function the same duplicate code.
Additionally, this sucks for serialization.
or we create a mapping of string -> variant type, but that sucks too.
sigh. I don't know how to do this properly.
if possible, I want to generate this at compile time.

*/

namespace config
{
    bool load_config(std::string filename);
    bool load_local_config();

};


#endif