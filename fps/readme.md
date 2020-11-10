
# TO DO
## timed
- make `timed_function(function_name)` (2) (10-11-2020)
- make light vector part of game_state  (1) (10-11-2020)
- make update_uniform crash & refactor internal variable names (1) (10-11-2020)
- pass dt factor instead of actual dt to update functions. (1) (10-11-2020)
- pass asset path to shader & texture manager (1) (10-11-2020)
- change render_system to render_manager (2) (10-11-2020)

## untimed
- make Task_Queue use futures that return bool?
- particle compute shader 
- near / far z-plane values?
- try freetype openGL 
- vertex picking / object highlighting?
- uniform naming prefix? `u_`?


## bugs
- lighting is incorrect with negative z viewing


# To compile:
meson builddir
cd builddir
ninja

# DONE

## Refactor
- `g_framebuffer -> fb_framebuffer` 
` 
