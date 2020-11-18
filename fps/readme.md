
# TO DO

## timed
- add gravity to movement simulation (2) (10-11-2020)
- make light vector part of game_state  (1) (10-11-2020)
- make functions for map access that check for key validity (2) (18-11-2020)
- pass asset path to shader & texture manager (1) (10-11-2020)
- change render_system to render_manager (2) (10-11-2020)
- change shader extensions to canonical opengl (.vert, .frag, .tesc, tese, .comp) (1) (10-11-2020)
- normals shader (1) (10-11-2020)
- toon shader (2) (10-11-2020)

## untimed
- add draw_3d api, think about how to scope the buffers 
- add draw_2d api, think about how to scope the buffers
- make `Task_Queue` use futures? (10-11-2020)
- particle compute shader 
- near / far z-plane values?
- try freetype openGL 
- uniform naming prefix? `u_`?
- shader toy inspiration
- press vs hold input (switch two state vectors?)
- load obj files (tiny obj?)
- add Slab functionality
- test physics with bullet

### Slabs
 - interact / modify
 - vertex picking / object highlighting?
 

# To compile:
meson builddir
cd builddir
ninja

# DONE
- make `timed_function(function_name)` (2) (10-11-2020)
- pass dt factor instead of actual dt to `update` functions. (1) (10-11-2020)
- make `update_uniform` crash & refactor internal variable names (1) (10-11-2020)
- make `put_arrow` functionality

## Refactor
- `g_framebuffer -> fb_framebuffer` 
