#include "renderer/OrthographicCamera.h"

OrthoCamera camera;

void createOrthoCamera(float left, float right, float bottom, float top)
{
	glm_ortho(left, right, bottom, top, -1.0f, 1.0f, camera.ProjectionMatrix);
	glm_vec3_make((float []){0.0f, 0.0f, 0.0f}, camera.position);
	camera.rotation = 0.0f;
	
	glm_mat4_identity(camera.ViewMatrix);
	glm_mat4_ucopy(camera.ProjectionMatrix, camera.PVMatrix); // view mat is ignored as it is set to the identity element
}

static void recalculate_view_mat()
{
	mat4 transform = GLM_MAT4_IDENTITY_INIT;
	glm_translate(transform, camera.position);
	glm_rotate(transform, camera.rotation, (float []){0.0f, 0.0f, 1.0f});
	
	glm_mat4_inv(transform, camera.ViewMatrix); //since moving camera right is actually just moving scene left
	glm_mat4_mul(camera.ProjectionMatrix, camera.ViewMatrix, camera.PVMatrix);
}

void setOrthoProjection(float left, float right, float bottom, float top) 
{  
	glm_ortho(left, right, bottom, top, -1.0f, 1.0f, camera.ProjectionMatrix);
	glm_mat4_mul(camera.ProjectionMatrix, camera.ViewMatrix, camera.PVMatrix);
}

void setOrthoPosition(float* position, bool recalculate) 
{  
	//is make or copy better/faster
	glm_vec3_make(position, camera.position);
	
	if(recalculate)
		recalculate_view_mat();
}

void setOrthoRotation(float rotation, bool recalculate) 
{  
	camera.rotation = rotation;
	
	if(recalculate)
		recalculate_view_mat();
}

//c does not like returning array types; in this case float (*)[4].
void* getOrthoPosition() { void* ptr = &camera.position[0]; return ptr; }
float getOrthoRotation() { return camera.rotation; }
void* getOrthoProjection() { void* ptr = &camera.ProjectionMatrix[0]; return ptr; }
void* getOrthoView() { void* ptr = &camera.ViewMatrix[0]; return ptr; }
void* getPVMat() { void* ptr = &camera.PVMatrix[0]; return ptr; }

