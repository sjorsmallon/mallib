project('testt', 'cpp',default_options : ['cpp_std=c++17'])

libfmt_project = subproject('fmt')
libfmt_dep = libfmt_project.get_variable('fmt_dep')


libimgui_project = subproject('imgui')
libimgui_dep = libimgui_project.get_variable('imgui_dep')

libglfw_project = subproject('glfw-3.3.2')
libglfw_dep = libglfw_project.get_variable('glfw_dep')

inc_dir = include_directories('include')

source_files = [
	'src/main.cc',
	'src/entity_system.cc',
	'src/input.cc',
	'src/player.cc', 
	'src/render_system.cc',
	'src/shader_manager.cc',
	'src/texture_manager.cc',
	'src/window_manager.cc',
	'src/asset_manager.cc',
	'src/game_simulate.cc',
	'src/game_render.cc',
	'src/game.cc',
	'src/task_system.cc',
	'src/timed_function.cc',
	'src/logr.cc'

]

executable('testt',
  source_files,
  include_directories: inc_dir,
  dependencies : [libimgui_dep, libglfw_dep, libfmt_dep]
  )