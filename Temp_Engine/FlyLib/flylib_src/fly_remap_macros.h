#ifndef FLY_REMAP_MACROS
#define FLY_REMAP_MACROS

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
#if defined(USE_OPENGL)
#include<glad/glad.h>
#else
#include<GLES/gl32.h>
#endif
// Data Types
#define FLY_BYTE    GL_BYTE
#define FLY_UBYTE   GL_UNSIGNED_BYTE
#define FLY_FLOAT   GL_FLOAT
#define FLY_DOUBLE  GL_DOUBLE
// Texture Types
#define FLY_RGBA    GL_RGBA
#define FLY_R8      GL_R8
#define FLY_R16     GL_R16
#define FLY_RGB     GL_RGB
// Texture Parameters
#define FLY_NEAREST         GL_NEAREST
#define FLY_LINEAR          GL_LINEAR
#define FLY_NEAREST_NEAREST GL_NEAREST_MIPMAP_NEAREST
#define FLY_LINEAR_NEAREST  GL_LINEAR_MIPMAP_NEAREST
#define FLY_NEAREST_LINEAR  GL_NEAREST_MIPMAP_LINEAR
#define FLY_LINEAR_LINEAR   GL_LINEAR_MIPMAP_LINEAR
#define FLY_CLAMP           GL_CLAMP_TO_EDGE
#define FLY_MIRROR          GL_MIRRORED_REPEAT
#define FLY_REPEAT          GL_REPEAT
// Shaders
#define FLY_VERTEX_SHADER   GL_VERTEX_SHADER
#define FLY_FRAGMENT_SHADER GL_FRAGMENT_SHADER
#define FLY_COMPUTE_SHADER
#define FLY_GEOMETRY_SHADER
#endif

#endif