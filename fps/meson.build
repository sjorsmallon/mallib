project('testt', 'cpp',default_options : ['cpp_std=c++latest', 'warning_level=3'])

libfmt_project = subproject('fmt')
libfmt_dep = libfmt_project.get_variable('fmt_dep')

# only for clang
#add_project_arguments('-ftime-trace', language:'cpp')

inc_dir = include_directories('include')

libimgui_project = subproject('imgui')
libimgui_dep = libimgui_project.get_variable('imgui_dep')

libglfw_project = subproject('glfw-3.3.2')
libglfw_dep = libglfw_project.get_variable('glfw_dep')

project_dir  = meson.current_source_dir() # MESON_SOURCE_ROOT
build_dir    = meson.current_build_dir() # MESON_BUILD_ROOT

soloud_path = join_paths(project_dir, 'static_libraries/soloud')
soloud_include_path = join_paths(project_dir, 'static_libraries/soloud/include')

compiler = meson.get_compiler('cpp')

libsoloud_dep = compiler.find_library('soloud', dirs: soloud_path)
soloud_inc_dir = include_directories(soloud_include_path)

inc_dir = [inc_dir, soloud_inc_dir]

source_files = [
	'src/main.cc',
	'src/input.cc',
	'src/player.cc', 
	'src/render_system.cc',
	'src/shader_manager.cc',
	'src/texture_manager.cc',
	'src/window_manager.cc',
	'src/asset_manager.cc',
	'src/game_simulate.cc',
	'src/game_render.cc',
	'src/game_end_frame.cc',
	'src/game.cc',
	'src/task_system.cc',
	'src/timed_function.cc',
	'src/logr.cc',
	'src/sound_system.cc',
	'src/game_combat.cc',
	'src/game_weapons.cc',
	'src/mallet.cc',
	'src/plane.cc',
	'src/bsp.cc',
]

executable('run',
  source_files,
  include_directories: inc_dir,
  dependencies : [libimgui_dep, libglfw_dep, libfmt_dep,libsoloud_dep]
  )

# test_source_files = [
# 	'src/logr.cc',
# 	'tests/bsp/bsp_test.cc',
# 	'src/plane.cc',
# 	'src/bsp.cc',
# 	'src/mallet.cc',
# 	'src/render_system.cc',
# ]

# executable('testt',
# 	test_source_files,
# 	include_directories: inc_dir,
# 	dependencies: [libfmt_dep, libimgui_dep]
# 	)
