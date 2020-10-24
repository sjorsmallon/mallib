project('libimgui', 'cpp', default_options : ['cpp_std=c++17'])
incdir = include_directories('include')


libimgui = static_library('imgui', ['src/glad.cc', 'src/imgui.cc', 'src/imgui_demo.cc', 'src/imgui_draw.cc', 'src/imgui_impl_glfw.cc', 'src/imgui_impl_opengl3.cc', 'src/imgui_widgets.cc'], include_directories : incdir, name_prefix: '', name_suffix: 'lib')


imgui_dep = declare_dependency(include_directories : incdir,
  link_with : libimgui)

