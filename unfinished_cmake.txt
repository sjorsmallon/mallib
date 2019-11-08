cmake_minimum_required (VERSION 3.1...3.3)


project (Test
		VERSION 0.1
		Description "test"
		LANGUAGES CXX)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

add_library(test
	
	)
	
target_include_directories(
	
)


include_directories(include/)

add_library(my_lib 
			    src/menu/menu.cc
			    src/font/font.cc
			    src/graphics/graphics.cc
			    src/sound/sound.cc)

target_link_libraries(my_lib
					  libs/fmt.lib
					  libs/freetype.lib
					  libs/soloud_x64.lib
					  Opengl32.lib
					  user32.lib
					  Gdi32.lib 
					  )
add_executable(Test src/win32/win32_entrypoint.cc)
target_link_libraries(Test my_lib)

