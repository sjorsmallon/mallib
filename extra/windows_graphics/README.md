
# README
win32 is sort of self-sufficient. it needs the IO header in order to provide the keyboard / mouse input things.
The same can be said for the globals.


`
cl /EHsc win32_entrypoint.cc io.cc /I include/ /link opengl32.lib user32.lib Gdi32.lib
` 



`cl.exe /MP4 /EHsc /Fe:test.exe /DEBUG /Zi /std:c++17 win32_entrypoint.cc ../game/game.cc ../sound/sound.cc ../file/file.cc ../graphics/graphics.cc ../font/font.cc ../menu/menu.cc ../input/input.cc ../asset/asset.cc /I ../../include/ /link ../../libs/fmt.lib ../../libs/soloud_x64.lib ../../libs/freetype.lib Opengl32.lib user32.lib Gdi32.lib ` 


` cl.exe /MP4 /EHsc /Fe:test.exe /DEBUG /Zi /std:c++17 src/win32/win32_entrypoint.cc src/game/game.cc src/sound/sound.cc src/file/file.cc src/graphics/graphics.cc src/font/font.cc src/menu/menu.cc src/input/input.cc src/asset/asset.cc /I include/ /link libs/fmt.lib libs/soloud_x64.lib libs/freetype.lib Opengl32.lib user32.lib Gdi32.lib ` 


// user32 for some random error?
#pragma comment(lib, "Gdi32.lib") // for blit.

