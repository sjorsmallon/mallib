#ifndef INCLUDED_CVAR_
#define INCLUDED_CVAR_
#include <variant>
#include <vector>
#include <string>
#include <map>
#include "logr.h"
#include <iostream>


//@Note(Sjors): The way this is structured is strange, but hear me out:
// In order to deal with the static initialization 'fiasco' stuff,
// it is necessary to make sure that that the cvar_system is initialized (the map of cvar pointers)
// and valid. Because the cvars are initialized statically too, this becomes a "race" condition.
// Exposing a cvar_system variable from an anonymous namespace in the cvar_system.cc file
// is not good enough: cvars can apparently be initialized before the cvar_system is initialized.
// In order to prevent this, the map for all cvars is accessed through the get_global_cvar()
// function, which contains a static cvar_system. This ensures that the map is always valid, even if the cvars are initialized statically.
// Even though this is not a very elegant solution, I currently don't know a way of properly resolving this. - SJM, 23-01-2020

///-------------------------------------------------
/// Cvar System that permeates mallib. users can register different types
/// of console variables.
/// Due to the way static cvar registration works,
/// there is exactly one Cvar_System, which is defined in cvar_system.cpp.
///
///
/// some categories are :
/// r_*   -  renderer
//  pm_*  -  player_movement
//  g_*   -  global / nondescript.
//  sys_* -  system settings / variables.
//  cm_*  -  collision model.
//    

using cvar_data_t = std::variant<float, bool, int32_t, std::string>;

enum Cvar_Flags : int64_t
{
    CVAR_NOFLAGS     = 0,       // 0
    CVAR_SERIALIZE   = 1 << 0,  // 1
    CVAR_NOCHEAT     = 1 << 1,  // 2
    CVAR_CHEAT       = 1 << 2,  // 4
    CVAR_SOUND       = 1 << 3,  // 8
    CVAR_RENDERER    = 1 << 4,  // 16
    CVAR_SETTINGS    = 1 << 5,  // 32
    CVAR_SYSTEM      = 1 << 6   // 64
};

class Cvar
{
public:
    const char* name;
    const char* description;
    int64_t registry_flags;
    cvar_data_t data;
    cvar_data_t min;
    cvar_data_t max;
    std::vector<cvar_data_t> value_set;

    Cvar() = delete;

    explicit Cvar(
        const char* name,
        bool default_data,
        int64_t cvar_registry_flags,
        const char* description);

    explicit Cvar(
        const char* name,
        std::string default_data,
        int64_t cvar_registry_flags,
        const char* description,
        std::vector<std::string> value_set = std::vector<std::string>());

    explicit Cvar(
        const char* name,
        float default_data,
        int64_t cvar_registry_flags,
        const char* description,
        float min_value,
        float max_value,
        std::vector<float> value_set = std::vector<float>());

    explicit Cvar(
        const char* name,
        int32_t default_data,
        int64_t cvar_registry_flags,
        const char* description,
        int32_t min_value,
        int32_t max_value,
        std::vector<int> value_set = std::vector<int>());

    Cvar(const Cvar&) = delete;

    Cvar& operator=(const Cvar&) = delete;

};

float       get_float(const Cvar&);
std::string get_string(const Cvar&);
bool        get_bool(const Cvar&);
int32_t     get_int(const Cvar&);

void set_float(Cvar& cvar, float value);
void set_string(Cvar& cvar, std::string value);
void set_bool(Cvar& cvar, bool value);
void set_int(Cvar& cvar, int32_t value);

bool        get_bool(const char* cvar_name);
float       get_float(const char* cvar_name);
int32_t     get_int(const char* cvar_name);
std::string get_string(const char* cvar_name);

void set_float(const char* cvar_name, float value);
void set_bool(const char* cvar_name, bool value);
void set_int(const char* cvar_name, int32_t value);
void set_string(const char* cvar_name, std::string value);

class Cvar_System
{
public:
    std::map<std::string, Cvar*> cvars;
    Cvar_System() = default;
    // Cvar_System(const Cvar_System&) = delete;

    // Cvar_System& operator=(const Cvar_System&) = delete;
};

void register_cvar(Cvar_System& cvar_system, Cvar* cvar);

Cvar_System& get_global_cvar_system();




#endif