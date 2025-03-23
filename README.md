# project
**sora (そら)** is a codebase for creating 2d and 3d applications. 
this project was created as a way for me to learn about writing 
software from scratch and handling every aspect myself.

features:
- abstracted platform layers:
	- os platforms (win32, linux, etc.)
	- graphics api (d3d11, opengl, vulkan, etc.)
	- audio (wasapi, alsa, etc.)
	- font rasterizers (dwrite, freetype, etc. )
- math library:
	- vectors, matrices, quaternions.
	- simd support.
	- colors.
- 2d drawing library:
	- variety of shapes (rects, circles, lines, etc.)
	- blended textures and colors.
	- batched into single draw call.
- custom imgui library:
	- panels and draggable tabs.
	- build in widgets (label, button, etc.)
	- support for custom widgets.
	- popups and context menus. 

# screenshots
ui showcase:
![ui showcase](/images/ui_showcase.png?raw=true "ui showcase")

atmosphere showcase:
![atmosphere showcase](/images/atmosphere_showcase.png?raw=true "atmosphere showcase")

shadows showcase:
![shadow showcase](/images/shadow_showcase.png?raw=true "shadow showcase")

# build

### windows:
use the `build.bat` script:
```
build [options] <file>

args:
  <file> - the main file to build. defaults to main.cpp.
  [options] - options: (build mode: 'debug', 'release'; compiler: 'msvc', 'clang'). defaults to 'msvc' and 'debug'.
  
examples:
  build.bat debug main.cpp
  build.bat release clang console.cpp
```
`build.bat` expects the main file to be in `\src` and outputs generated files to `\build`.

### linux:

not supported yet.

# directory
- `\res`: contains all resources and assets needed for each project.
- `\src`: contains all source code.
	- `\src\engine`: contains engine specific code.
		- `\src\engine\core`: contains core layers.
		- `\src\engine\utils`: contains utility layers.
		- `\src\engine\vendor`: contains third party code.
	- `\src\projects`: contains project specific code.
	- `\src\templates`: contains template main files.
- `\build`: all build artifacts and generated binaries.

# structure
the codebase is set up in *layers*. some layers depend on other layers.
here is the current list of layers:

core layers:
- `audio` (`audio_`): implements audio abstraction for different backends.
- `base` (none): includes basic types, strings, math, and helper macros.
- `font` (`font_`): implements a font renderer and cache for different backends.
- `gfx` (`gfx_`): implements an abstraction over different graphics apis.
- `os` (`os_`): implements an abstraction over operating system features such as events, windows, threads, etc.

utility layers:
- `draw` (`draw_`): implements a simple 2d shape renderer.
- `draw_3d` (`draw_3d_`): implements a simple 3d renderer.
- `log` (`log_`): implements a logging library.
- `profile` (`profile_`): implements a profiling library.
- `task` (`task_`): implements a fiber-based job system.
- `ui` (`ui_`): implements an immediate mode ui system.

# templates

this codebase contains several templates to start off from.

- `template_console.cpp`: simple console program.
- `template_window.cpp`: opens a single window set up for rendering.
- `template_multi_window.cpp`: can open any number of windows set up for rendering.
- `template_custom_border.cpp`: opens a clean custom borderless window.