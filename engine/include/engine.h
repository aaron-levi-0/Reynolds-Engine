#ifndef ENGINE_H
#define ENGINE_H

#include "defines.h"
#include "logging.h"
#include "entry.h"

#include "layers.h"
#include "renderer.h"
#include "shader.h"
#include "input.h"
#include "timestep.h"
#include "window.h"
#include "keycodes.h"

#include "texture.h"
#include "subTexture.h"

#include "audio.h"

REN_API void EngineDependencies();

#endif // ENGINE_H