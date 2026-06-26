

|**System**|**Status**|**Notes**|
|-|-|-|
|Window + GL context|Done|GLFW, clean callback wiring (window.c)|
|Batch quad renderer|Done (Strong)|Texture slots, sub-textures, white-texture trick (renderer.c)|
|Sprite sheets (sub-textures)|Done|UV math in subTexture.c|
|Orthographic camera + pan/zoom|Done|OrthoCameraController.c|
|Layer stack|Done|layers.c|
|Event system + input|Partial|Works, but single shared event object|
|Texture loading|Done|stb\_image|
|Shaders + hot path|Done|shader.c|
|Time|Done|timestep.c|
|Text/font rendering|||
|Audio|||
|Physics|||
|Scene/entity management||Each game wires layers by hand|
|Sprite animation||Have sub-textures but no frame timeline on top|
|Input action mapping||Uses raw GLFW\_KEY\_\*|
|Cross-platform build||CMakeLists.txt is incomplete, real build is Windows .bat|



