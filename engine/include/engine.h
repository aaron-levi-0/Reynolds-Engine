#ifndef ENGINE_H
#define ENGINE_H

#include "logging.h"
#include "OrthographicCamera.h"
#include "OrthoCameraController.h"
#include "entry.h"

#include "layers.h"
#include "renderer.h"
#include "shader.h"
#include "input.h"
#include "timestep.h"
#include "window.h"

#include "defines.h"
#include "texture.h"
#include "subTexture.h"

REN_API void EngineDependencies();

#endif // ENGINE_H