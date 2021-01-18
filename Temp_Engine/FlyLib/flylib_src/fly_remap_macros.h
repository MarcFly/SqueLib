#ifndef FLY_REMAP_MACROS
#define FLY_REMAP_MACROS

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SUMMARY ///////////////////////////////////////////////////////////////////////////////////////////////
// Most apis have an easy way to set or get information through a int/bit system
// In order to maintain functionality and have an easy way to interact with these systems
// my idea is to have resettable macros that can be shared across apis
// Whenever a macro is not setup in another, a NULL value most probably will not throw errors OR
// I can intercept with a specfic value (INT_MAX for example) and not perform the call to the api
// That way a single initialization can benefit to all apis... contatn with Jesus about it
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FLY_INVALID -1

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER MACROS /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
#include<glad/glad.h>
#if defined(USE_OPENGLES)
#include<GLES3/gl32.h>
#endif
// Render State
#define FLY_BLEND				GL_BLEND
#define FLY_FUNC_ADD			GL_FUNC_ADD
#define FLY_SRC_ALPHA			GL_SRC_ALPHA
#define FLY_ONE_MINUS_SRC_ALPHA	GL_ONE_MINUS_SRC_ALPHA
#define FLY_ONE_MINUS_SRC_COLOR	GL_ONE_MINUS_SRC_COLOR
#define FLY_SRC_COLOR			GL_SRC_COLOR
#define FLY_ZERO				GL_ZERO
#define FLY_ONE					GL_ONE
#define FLY_CULL_FACE			GL_CULL_FACE
#define FLY_DEPTH_TEST			GL_DEPTH_TEST
#define FLY_SCISSOR_TEST		GL_SCISSOR_TEST
// OPENGL VERSION SPECIFIC RENDER STATES
#if defined(USE_OPENGL)
#define FLY_FRONT_AND_BACK		GL_FRONT_AND_BACK
#define FLY_FILL				GL_FILL
#else
#define FLY_FRONT_AND_BACK		FLY_INVALID
#define FLY_FILL				FLY_INVALID
#endif
// Texture Locators
#define FLY_TEXTURE_2D	GL_TEXTURE_2D
#define FLY_TEXTURE_3D	GL_TEXTURE_3D
#define FLY_TEXTURE0	GL_TEXTURE0
#define FLY_TEXTURE1	GL_TEXTURE1
#define FLY_TEXTURE2	GL_TEXTURE2
#define FLY_TEXTURE3	GL_TEXTURE3
#define FLY_TEXTURE4	GL_TEXTURE4
#define FLY_TEXTURE5	GL_TEXTURE5
#define FLY_TEXTURE6	GL_TEXTURE6
#define FLY_TEXTURE7	GL_TEXTURE7
#define FLY_TEXTURE8	GL_TEXTURE8
#define FLY_TEXTURE9	GL_TEXTURE9
#define FLY_TEXTURE10	GL_TEXTURE10
#define FLY_TEXTURE11	GL_TEXTURE11
#define FLY_TEXTURE12	GL_TEXTURE12
#define FLY_TEXTURE13	GL_TEXTURE13
#define FLY_TEXTURE14	GL_TEXTURE14
#define FLY_TEXTURE15	GL_TEXTURE15
#define FLY_TEXTURE16	GL_TEXTURE16
#define FLY_TEXTURE17	GL_TEXTURE17
#define FLY_TEXTURE18	GL_TEXTURE18
#define FLY_TEXTURE19	GL_TEXTURE19
#define FLY_TEXTURE20	GL_TEXTURE20
#define FLY_TEXTURE21	GL_TEXTURE21
#define FLY_TEXTURE22	GL_TEXTURE22
#define FLY_TEXTURE23	GL_TEXTURE23
#define FLY_TEXTURE24	GL_TEXTURE24
#define FLY_TEXTURE25	GL_TEXTURE25
#define FLY_TEXTURE26	GL_TEXTURE26
#define FLY_TEXTURE27	GL_TEXTURE27
#define FLY_TEXTURE28	GL_TEXTURE28
#define FLY_TEXTURE29	GL_TEXTURE29
#define FLY_TEXTURE30	GL_TEXTURE30
#define FLY_TEXTURE31	GL_TEXTURE31
// Draw Mode
#define FLY_STATIC_DRAW		GL_STATIC_DRAW
#define FLY_STREAM_DRAW		GL_STREAM_DRAW
#define FLY_DYNAMIC_DRAW	GL_DYNAMIC_DRAW
#define FLY_TRIANGLES		GL_TRIANGLES
#define FLY_QUADS			GL_QUADS
#define FLY_LINES			GL_LINES
#define FLY_POINTS			GL_POINTS
// Data Types
#define FLY_BYTE    GL_BYTE
#define FLY_UBYTE   GL_UNSIGNED_BYTE
#define FLY_USHORT  GL_UNSIGNED_SHORT
#define FLY_UINT    GL_UNSIGNED_INT
#define FLY_INT     GL_INT
#define FLY_FLOAT   GL_FLOAT
#define FLY_DOUBLE  GL_DOUBLE
// Texture Types
#define FLY_RGBA    GL_RGBA
#define FLY_R8      GL_R8
#define FLY_R16     GL_R16
#define FLY_RGB     GL_RGB
// Texture Parameters
#define FLY_MIN_FILTER		GL_TEXTURE_MIN_FILTER
#define FLY_MAG_FILTER		GL_TEXTURE_MAG_FILTER
#define FLY_WRAP_S			GL_TEXTURE_WRAP_S
#define FLY_WRAP_T			GL_TEXTURE_WRAP_T
#define FLY_BORDER_COLOR	GL_TEXTURE_BORDER_COLOR
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY MACROS ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLFW //////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(USE_GLFW)
#include <GLFW/glfw3.h>
#define FLY_DISPLAY_END				FLY_INVALID
// WINDOW HINTS
#define FLY_MINIMIZED				GLFW_ICONIFIED
#define FLY_RESIZABLE				GLFW_RESIZABLE
#define FLY_VISIBLE					GLFW_VISIBLE
#define FLY_DECORATED				GLFW_DECORATED
#define FLY_FLOATING				GLFW_FLOATING
#define FLY_MAXIMIZED				GLFW_MAXIMIZED
#define FLY_CENTER_CURSOR			GLFW_CENTER_CURSOR
// CONTEXT HINTS
#define FLY_WINDOW_CONTEXT_MAJOR	GLFW_CONTEXT_VERSION_MAJOR
#define FLY_WINDOW_CONTEXT_MINOR	GLFW_CONTEXT_VERSION_MINOR
#define FLY_WINDOW_CONTEXT_REVISION	GLFW_CONTEXT_REVISION
#define FLY_CONTEXT_MAJOR_MIN		3
// BUFFER HINTS
#define FLY_RED_BITS				GLFW_RED_BITS
#define FLY_GREEN_BITS				GLFW_GREEN_BITS
#define FLY_BLUE_BITS				GLFW_BLUE_BITS
#define FLY_ALPHA_BITS				GLFW_ALPHA_BITS
#define FLY_DEPTH_BITS				FLY_INVALID
#define FLY_STENCIL_BITS			GLFW_STENCIL_BITS
#define FLY_BUFFER_SIZE				FLY_INVALID
#define FLY_STEREOSCOPIC			GLFW_STEREO
#define FLY_MSAA_SAMPLES			GLFW_SAMPLES
#define FLY_SRGB					GLFW_SRGB_CAPABLE
#define	FLY_REFRESH_RATE			GLFW_REFRESH_RATE
#define FLY_DOUBLE_BUFFER			GLFW_DOUBLEBUFFER
#define FLY_RENDERABLE_TYPE         FLY_INVALID
#define FLY_MIN_RENDERABLE          FLY_INVALID

// EGL //////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(USE_EGL)
#include<EGL/egl.h>
#define FLY_DISPLAY_END				EGL_NONE
// WINDOW HINTS
#define FLY_MINIMIZED				FLY_INVALID
#define FLY_RESIZABLE				FLY_INVALID
#define FLY_VISIBLE					FLY_INVALID
#define FLY_DECORATED				FLY_INVALID
#define FLY_FLOATING				FLY_INVALID
#define FLY_MAXIMIZED				FLY_INVALID
#define FLY_CENTER_CURSOR			FLY_INVALID
// CONTEXT HINTS
#define FLY_WINDOW_CONTEXT_MAJOR	EGL_CONTEXT_CLIENT_VERSION
#define FLY_WINDOW_CONTEXT_MINOR	FLY_INVALID
#define FLY_WINDOW_CONTEXT_REVISION FLY_INVALID
#define FLY_CONTEXT_MAJOR_MIN		3
// BUFFER HINTS
#define FLY_RED_BITS				EGL_RED_SIZE
#define FLY_GREEN_BITS				EGL_GREEN_SIZE
#define FLY_BLUE_BITS				EGL_BLUE_SIZE
#define FLY_ALPHA_BITS				EGL_ALPHA_SIZE
#define FLY_DEPTH_BITS				EGL_DEPTH_SIZE
#define FLY_STENCIL_BITS			EGL_STENCIL_SIZE
#define FLY_BUFFER_SIZE				EGL_BUFFER_SIZE
#define FLY_STEREOSCOPIC			FLY_INVALID
#define FLY_MSAA_SAMPLES			FLY_INVALID
#define FLY_SRGB					FLY_INVALID
#define	FLY_REFRESH_RATE			FLY_INVALID
#define FLY_DOUBLE_BUFFER			FLY_INVALID
#define FLY_RENDERABLE_TYPE         EGL_RENDERABLE_TYPE
#define FLY_MIN_RENDERABLE          EGL_OPENGL_ES3_BIT

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILESYSTEM MACROS /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// WINDOWS ///////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#include <Windows.h>
#include <libloaderapi.h>
#define FLYFS_HIDDEN				FILE_ATTRIBUTE_HIDDEN
#elif defined(__linux__)
#define FLYFS_HIDDEN				BITSET1
#elif defined(ANDROID)
#endif
#endif // FLY_REMAP_MACROS

