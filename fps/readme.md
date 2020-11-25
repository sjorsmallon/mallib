
# TO DO

## timed
- apply friction only on ground (1)
- decipher quake air movement (2)
- optional unitization of meshes (1, default false)
- check for gl context in all gl-related managers (2)
- make functions instead of direct map access to counteract implicit creation (sigh..) (2) (18-11-2020)
- pass asset path to file-based managers (1) (10-11-2020)
- make light vector part of some system or manager (1) (10-11-2020)
- normals shader (1) (10-11-2020)
- toon shader (2) (10-11-2020)

## untimed
- add pbr
- make logr console a ring buffer
- add draw_3d api, think about how to scope the buffers 
- add draw_2d api, think about how to scope the buffers
- test physics with bullet
- entities with meshes should contain xform_state
- near / far z-plane values?
- particle compute shader 
- uniform naming prefix? `u_`?
- press vs hold input (switch two state vectors? look at wish_to_jump from quake)
- load obj files (tiny obj?)
- add Slab functionality
- make `Task_Queue` use futures? (10-11-2020)
- shader toy inspiration
- change render_system to render_manager (2) (10-11-2020)

### Slabs
 - interact / modify
 - vertex picking / object highlighting?
 
### Commands

All movement must
- feel good
- be intuitive
- be fun
- be fast
- be controlled

All images must 
- reflect reality (but NOT be realistic)
- have solidity
- be original
- be cool
- evoke emotion
- look alive

All sounds must 
- be diegetic
- be distinct
- be relatable
- be logical

All menus must
- be diegetic
- use the SAME input as the game
- minimize language use

# To compile:
meson builddir
cd builddir
ninja

# DONE
- added 3d sound
- added attenuation
- added sound (SoLoud)
- create crosshair texture
- create GUI view buffer
- add transparent PNG export
- crash if gl_texture_frame high watermark is reached
- create texture in blender
- create viewmodel weapon in blender
- add viewmodel weapon
- change shader extensions to canonical glsl names (.vert, .frag, .tesc, tese, .comp) (1) (10-11-2020)
- add gravity to movement simulation (2) (10-11-2020)
- make `timed_function(function_name)` (2) (10-11-2020)
- pass dt factor instead of actual dt to `update` functions. (1) (10-11-2020)
- make `update_uniform` crash & refactor internal variable names (1) (10-11-2020)
- make `put_arrow` functionality

## Refactor
- `g_framebuffer -> fb_framebuffer` 
