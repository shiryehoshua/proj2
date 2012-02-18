Project 2
=========

Usage
-----
Project 2 should be run just as `./proj2`; however, it is built so that it can be passed a default
shader to start in.

	$ ./proj2 -h
	usage: ./proj2 [<vertshader> <fragshader>]
	  Call `./proj2', optionally taking a default pair of vertex and fragment
	  shaders to render. Otherwise we just load our stack of shaders.

### Value Interaction
Our project two lets you view 4 scenes by pressing keys 1, 2, 3 and 4, respectively. In each scene,
the variables `Ka`, `Ks`, `Kd`, `shexp`, and `bkgr color` may be manipulated interactively, in
addition to the following scene-specific variables:
1. Gouraud and Phong shading on an untextured sphere
2. Per-vertex texturing versus "real" texturing, with or without seam correction on a sphere
   textured with the UChicago seal
3. Filtering modes when applying a repeated texture (`rgb-check.png`) to a plane
4. Bump-mapping modes (Disabled, Bump, or Parallax) on a sphere textured with the UChicago seal

### 3D Interaction
Our project implements all interactons from project 1 (though in a few cases they same glitchier).
- Light can be rotated
- Models can be zoomed in and out via translation or FOV changes
- etc.
