# Reynolds-Engine - v 0.2.5

 ## Current Features

* 2D batch renderer in C over OpenGL, using GLFW (windowing/input) and GLEW (GL loading)
* Texture loading and sprite-sheet sub-textures (stb_image)
* Orthographic camera with pan/zoom
* Layer stack with a synchronous event/input system
* Levelled, coloured logging and a delta-time clock
* Windows build via the included `.bat` scripts (see `build_files/`)
* Vendored, not yet wired in: stb_truetype (text) and miniaudio (audio)

## Documentation

* [`ARCHITECTURE.md`](ARCHITECTURE.md) — how the engine fits together (with diagrams)
* [`progress.md`](progress.md) — live status of each system
* [`docs/adr/`](docs/adr/) — architecture decision records (the *why*)
* [`CHANGELOG.md`](CHANGELOG.md) — notable changes per version
 
## Future Additions

* Physics
* Removing GLFW dependence
* Adding Linux and MacOS support
* Adding Vulkan support
* Replacing vendor libraries such as CJSON and stb\_image with my own implementations
* Entity Component System
* Separate game related files from graphics process files. Don't want a game engine, want a multi-purpose graphics engine

