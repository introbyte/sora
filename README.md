# project
**sora (そら)** is a codebase for creating 2d and 3d applications. 
it takes heavy inspiration from the [raddebugger project](https://github.com/EpicGamesExt/raddebugger) and is used for my personal projects.

todo:
- [ ] abstracted platform layers.
	- [ ] os platforms.
		- [x] win32.
		- [ ] linux.
		- [ ] macos? (unsure).
	- [ ] graphics apis.
		- [x] d3d11.
		- [ ] opengl.
		- [ ] metal? (unsure).
		- [ ] vulkan (unsure).
		- [ ] d3d12 (unsure).
	- [ ] audio.
		- [x] wasapi.
		- [ ] alsa.
		- [ ] coreaudio? (unsure).
	- [ ] fonts.
		- [x] directwrite.
		- [ ] freetype.
		- [ ] coretext (unsure).
		- [ ] custom (unsure).
- [x] math library.
	- [x] vector functions.
	- [x] matrix functions.
	- [x] simd support.
	- [x] color functions.
- [x] 2d shape drawing library.
	- [x] different shapes.
	- [x] single draw call.
	- [x] support for clipping.
	- [x] support for textures.
	- [x] stack property modifiers (push_color, pop_thickness, etc.).
- [ ] custom immediate mode ui library. 
	- [ ] widgets.
		- [x] basic (labels, buttons, sliders, etc).
		- [x] color pickers.
		- [x] tooltips.
		- [ ] popup contexts.
	- [ ] panels.
		- [x] panel tree edits (split, close, etc).
		- [ ] tabs.

# screenshots

![ui showcase](/images/ui_showcase.png?raw=true "ui showcase")

# build

### windows:
use the `build.bat` script:
```
build <file> [mode]

args:
  <file> - the main file to build. defaults to main.cpp.
  [mode] - the build mode. options: (debug, release). defaults to debug.
  
examples:
  build.bat main.cpp debug
  build.bat ui_test.cpp release
```
the `build.bat` script expects the main file to be in `\src` and outputs generated files to `\build`.


### linux:

not supported yet.

# directory
- `\res`: contains all resources and assets needed for each project.
- `\src`: contains all source code.
	- `\src\engine`: contains engine specific code.
		- `\src\engine\backends`: contains backends for layers.
		- `\src\engines\vendor`: contains third party code.
	- `\src\templates`: contains template main files.
	- `\src\projects`: contains project specific code.
	- `\src\utils`: contains common utility code.
- `\build`: all build files and generate binaries.

# structure
the codebase is set up in *layers*. some layers depend on other layers.
here is the current list of layers:
- `audio` (`audio_`): implements audio abstraction for different backends.
- `base` (none): includes basic types, strings, math, and helper macros.
- `draw` (`draw_`): implements a simple 2d shape renderer.
- `draw_3d` (`draw_3d_`): implements a simple 3d renderer.
- `font` (`font_`): implements a font renderer and cache for different backends.
- `log` (`log_`): implements a logging library.
- `gfx` (`gfx_`): implements an abstraction over different graphics apis.
- `os` (`os_`): implements an abstraction over operating system features such as events, windows, threads, etc.
- `ui` (`ui_`): implements an immediate mode ui system.


# templates

this codebase contains several templates to start off from.

- `template_console.cpp`: simple console program.
- `template_window.cpp`: opens a single window set up for rendering.
- `template_multi_window.cpp`: can open any number of windows set up for rendering.
- `template_custom_border.cpp`: opens a clean custom borderless window.