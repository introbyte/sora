# project
**sora (そら)** is a codebase for creating 2d and 3d applications. this codebase takes heavy inspiration from the [raddebugger project](https://github.com/EpicGamesExt/raddebugger).

features:
- abstracted platform layers (currently only supports windows).
- simd math library.
- 2d single draw call rendering library.
- custom immediate mode ui library. 
- customizable render graph.

# build
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
the `build.bat` script expects the main file to be in `\src` and outputs all files to `\build`.
# directory
- `\res`: contains all resources and assets needed for each project.
- `\src`: contains all source code.
	- `\src\engine`: contains engine specific code.
		- `\src\engine\backends`: contains backend for layers.
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
- `draw` (`draw_`): implements a simple 2d shape renderer library.
- `font` (`font_`): implements a font renderer and cache for different backends.
- `gfx` (`gfx_`): implements an abstraction over different graphics apis.
- `os` (`os_`): implements an abstraction over operating system features such as events, windows, threads, etc.
- `ui` (`ui_`): implements an immediate mode ui system.