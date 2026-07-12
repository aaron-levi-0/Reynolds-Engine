#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include <cglm/cglm.h>

#include "defines.h"
#include "layers.h"
#include "shader.h"
#include "text.h"

struct Renderer;
struct Statistics
{
	uint32_t DrawCalls;
	uint32_t QuadCount;
};

REN_API struct Renderer* renderer_create();

REN_API void SetClearColour(vec3 );
REN_API void render_clear();
REN_API Layer create_render_layer(struct Renderer* );

REN_API void SetShader(struct Renderer* r, struct Shader* shader);

REN_API void BeginBatch(struct Renderer* );
REN_API void EndBatch(struct Renderer* );
REN_API void FlushBatch(struct Renderer* );

/* The matrix used by the NEXT flush. EngineRun sets the camera's PV each frame;
   a UI layer may flush mid-frame and switch to a screen-space ortho matrix. */
REN_API void setViewProjection(struct Renderer* renderer, mat4 view_projection);

/* Scissor clipping in window pixels (origin bottom-left, like glScissor).
   Each call flushes the current batch — clip state is per-draw-call. */
REN_API void SetClipRect(struct Renderer* r, const vec2 position, const vec2 size);
REN_API void ClearClipRect(struct Renderer* r);

REN_API void DrawQuad(struct Renderer* renderer, const vec2 position, const vec2 size, const uint32_t textureID, const vec4 tex_coords);
REN_API void DrawColour(struct Renderer* renderer, const vec2 position, const vec2 size, const vec4 colour);
REN_API void DrawText(struct Renderer* renderer, struct Font* font, const char* text, const vec2 position, float size, const vec4 colour);

REN_API uint32_t getDrawCalls(const struct Renderer* r);
REN_API uint32_t getQuadCount(const struct Renderer* r);
REN_API void resetStats(struct Renderer* r);

#endif
