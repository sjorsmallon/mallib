#include <fmt/core.h>
#include <fmt/printf.h> // should fmt be in shared? like logging?

int main(int argc, char* argv[])
{ 
    // do the window setup.

    // enter the main game loop.
	game::init_everything();
    while (true)
    {
      game::main_loop();
    }

}
