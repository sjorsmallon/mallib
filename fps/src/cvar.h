// #ifndef INCLUDED_CVAR_
// #define INCLUDED_CVAR_

// ///-------------------------------------------------
// /// Cvar System that permeates mallib. users can register different types
// /// of console variables.
// ///
// /// some categories are :
// /// r_*   -  renderer
// //  pm_*  -  player_movement
// //  g_*   -  global / nondescript.
// //  sys_* -  system settings / variables.
// using cvar_data_t = std::variant<float, bool, int32_t, std::string>;

//@Note(Sjors): Should we use constructor in order to register the *this ptr to the system?
// that way, we can initialize the values using the static.... whatever trick, 


// enum Cvar_Flags : int64_t
// 	{
// 		CVAR_NOFLAGS	 = 0,       // 0
// 		CVAR_SERIALIZE   = 1 << 0,  // 1
// 		CVAR_NOCHEAT     = 1 << 1,  // 2
// 		CVAR_CHEAT       = 1 << 2,  // 4
// 	    CVAR_SOUND       = 1 << 3,  // 8
// 	    CVAR_RENDERER    = 1 << 4,  // 16
// 	    CVAR_SETTINGS    = 1 << 5,  // 32
// 	    CVAR_SYSTEM      = 1 << 6   // 64
// };

// struct cvar
// {
// 	const char* name;
// 	const char* description;
// 	int64_t registry_flags;
// 	cvar_data_t data;
// 	cvar_data_t min;
// 	cvar_data_t max;
// 	std::vector<cvar_data_t> value_set;
// };







// void register_cvar_bool(
// 		const char* name, 
// 		cvar_data_type default_data, 
// 		int64_t cvar_registry_flags, 
// 		const char* description
// );

// void register_cvar_string(
// 	const char* name,
// 	cvar_data_type default_data,
// 	int64_t cvar_registry_flags,
// 	const char* description,
// 	std::vector<cvar_data_type> value_set
// );


// void register_cvar_float(
// 	const char* name,
// 	cvar_data_type default_data,
// 	int64_t cvar_registry_flags,
// 	const char* description,
// 	cvar_data_type min_value,
// 	cvar_data_type max_value,
// 	std::vector<cvar_data_type> value_set
// );

// void register_cvar_int(
// 	const char* name,
// 	cvar_data_type default_data,
// 	int64_t cvar_registry_flags,
// 	const char* description,
// 	cvar_data_type min_value,
// 	cvar_data_type max_value,
// 	std::vector<cvar_data_type> value_set 
// );

// bool get_bool(const char* cvar_name);

// float get_float(const char* cvar_name);

// int32_t get_int(const char* cvar_name);

// std::string get_string(const char* cvar_name);

// void set_float(const char* cvar_name, float value);

// void set_bool(const char* cvar_name, bool value);

// void set_int(const char* cvar_name, int32_t value);

// void set_string(const char* cvar_name, std::string value);





// #endif