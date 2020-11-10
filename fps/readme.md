
# TO DO
## timed
- make `timed_function(function_name)` (2) (10-11-2020)
- make `Task_Queue` use futures that return bool (2) (10-11-2020)
- make light vector part of game_state  (1) (10-11-2020)
- make update_uniform crash & refactor internal variable names (1) (10-11-2020)
- pass dt factor instead of actual dt to update functions. (1) (10-11-2020)
- pass asset path to shader & texture manager (1) (10-11-2020)
- change render_system to render_manager (2) (10-11-2020)
- change shader extensions to canonical opengl (.vert, .frag, .tesc, tese, .comp) (1) (10-11-2020)
- add gravity to movement simulation (2) (10-11-2020)
- normals shader (1) (10-11-2020)
- toon shader (2) (10-11-2020)

## untimed
- particle compute shader 
- near / far z-plane values?
- try freetype openGL 
- uniform naming prefix? `u_`?
- shader toy inspiration
- press vs hold input (switch two state vectors?)
- load obj files (tiny obj?)
- add Slab functionality
- test physics with bullet
- test freetype_gl

### Slabs
 - interact / modify
 - vertex picking / object highlighting?
 


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
