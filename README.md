# Reynolds-Engine
A 2D Game Engine written in C

## Current Features
- A game library using OpenGL and helper libraries GLFW and GLAD.
- Windows only build

## Future Additions
- Physics
- Audio
- Removing GLFW dependence
- Adding Linux and MacOS support
- Adding Vulkan support
- Replacing vendor libraries such as CJSON and stb_image with my own implementations

## Known Bugs
- Minesweeper game in testbed does not handle zoom and translation well, culprit is "getGridIndicies" func
- "Layerstack is NULL" error before window instance closes 
