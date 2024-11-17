# project
**sora (そら)** is a codebase for creating 2d and 3d applications.

features:
- simd math library.
- 2d shape drawing library.
- custom immediate mode ui library. 

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

# directory
- `\res`: contains all resources and assets needed for each project.
- `\src`: contains all source code.
	- `\src\engine`: contains engine specific code.
		- `\src\engine\backends`: contains backend for layers.
		- `\src\engines\vendor`: contains third party code.
	- `\src\projects`: contains project specific code.
	- `\src\utils`: contains common utility code.
- `\build`: all build files and generate binaries.

# structure
the codebase is set up in *layers*. some layers depend on other layers.
here is the current list of layers:
- `audio` (`audio_`): implements audio abstraction for different backends. depends on `os`.
- `base` (none): includes strings, math, basic types, helper macros. no dependencies.
- `draw` (`draw_`): implements a simple 2d shape renderer library as well a general 3d renderer. depends on `gfx`.
- `font` (`font_`): implements a font renderer and cache for different backends. depends on `gfx`.
- `gfx` (`gfx_`): implements a low level abstraction over different graphics apis. depends on `os`.
- `os` (`os_`): implements an abstraction over operating system features such as events, windows, threads, etc.
- `ui` (`ui_`): implements an immediate mode ui system. depends on `os`, `gfx`, `draw`.