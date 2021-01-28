#ifndef SQUE_REMAP_MACROS
#define SQUE_REMAP_MACROS

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SUMMARY ///////////////////////////////////////////////////////////////////////////////////////////////
// Most apis have an easy way to set or get information through a int/bit system
// In order to maintain functionality and have an easy way to interact with these systems
// my idea is to have resettable macros that can be shared across apis
// Whenever a macro is not setup in another, a NULL value most probably will not throw errors OR
// I can intercept with a specfic value (INT_MAX for example) and not perform the call to the api
// That way a single initialization can benefit to all apis... contatn with Jesus about it
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SQUE_INVALID -1

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER MACROS /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
#include<glad/glad.h>
#if defined(USE_OPENGLES)
#include<GLES3/gl32.h>
#endif
// Render State
#define SQUE_BLEND				GL_BLEND
#define SQUE_FUNC_ADD			GL_FUNC_ADD
#define SQUE_SRC_ALPHA			GL_SRC_ALPHA
#define SQUE_ONE_MINUS_SRC_ALPHA	GL_ONE_MINUS_SRC_ALPHA
#define SQUE_ONE_MINUS_SRC_COLOR	GL_ONE_MINUS_SRC_COLOR
#define SQUE_SRC_COLOR			GL_SRC_COLOR
#define SQUE_ZERO				GL_ZERO
#define SQUE_ONE				GL_ONE
#define SQUE_CULL_FACE			GL_CULL_FACE
#define SQUE_DEPTH_TEST			GL_DEPTH_TEST
#define SQUE_SCISSOR_TEST		GL_SCISSOR_TEST
// OPENGL VERSION SPECIFIC RENDER STATES
#if defined(USE_OPENGL)
#define SQUE_FRONT_AND_BACK		GL_FRONT_AND_BACK
#define SQUE_FILL				GL_FILL
#else
#define SQUE_FRONT_AND_BACK		SQUE_INVALID
#define SQUE_FILL				SQUE_INVALID
#endif
// Texture Locators
#define SQUE_TEXTURE_2D	GL_TEXTURE_2D
#define SQUE_TEXTURE_3D	GL_TEXTURE_3D
#define SQUE_TEXTURE0	GL_TEXTURE0
#define SQUE_TEXTURE1	GL_TEXTURE1
#define SQUE_TEXTURE2	GL_TEXTURE2
#define SQUE_TEXTURE3	GL_TEXTURE3
#define SQUE_TEXTURE4	GL_TEXTURE4
#define SQUE_TEXTURE5	GL_TEXTURE5
#define SQUE_TEXTURE6	GL_TEXTURE6
#define SQUE_TEXTURE7	GL_TEXTURE7
#define SQUE_TEXTURE8	GL_TEXTURE8
#define SQUE_TEXTURE9	GL_TEXTURE9
#define SQUE_TEXTURE10	GL_TEXTURE10
#define SQUE_TEXTURE11	GL_TEXTURE11
#define SQUE_TEXTURE12	GL_TEXTURE12
#define SQUE_TEXTURE13	GL_TEXTURE13
#define SQUE_TEXTURE14	GL_TEXTURE14
#define SQUE_TEXTURE15	GL_TEXTURE15
#define SQUE_TEXTURE16	GL_TEXTURE16
#define SQUE_TEXTURE17	GL_TEXTURE17
#define SQUE_TEXTURE18	GL_TEXTURE18
#define SQUE_TEXTURE19	GL_TEXTURE19
#define SQUE_TEXTURE20	GL_TEXTURE20
#define SQUE_TEXTURE21	GL_TEXTURE21
#define SQUE_TEXTURE22	GL_TEXTURE22
#define SQUE_TEXTURE23	GL_TEXTURE23
#define SQUE_TEXTURE24	GL_TEXTURE24
#define SQUE_TEXTURE25	GL_TEXTURE25
#define SQUE_TEXTURE26	GL_TEXTURE26
#define SQUE_TEXTURE27	GL_TEXTURE27
#define SQUE_TEXTURE28	GL_TEXTURE28
#define SQUE_TEXTURE29	GL_TEXTURE29
#define SQUE_TEXTURE30	GL_TEXTURE30
#define SQUE_TEXTURE31	GL_TEXTURE31
// Draw Mode
#define SQUE_STATIC_DRAW		GL_STATIC_DRAW
#define SQUE_STREAM_DRAW		GL_STREAM_DRAW
#define SQUE_DYNAMIC_DRAW	GL_DYNAMIC_DRAW
#define SQUE_TRIANGLES		GL_TRIANGLES
#define SQUE_QUADS			GL_QUADS
#define SQUE_LINES			GL_LINES
#define SQUE_POINTS			GL_POINTS
// Data Types
#define SQUE_BYTE    GL_BYTE
#define SQUE_UBYTE   GL_UNSIGNED_BYTE
#define SQUE_USHORT  GL_UNSIGNED_SHORT
#define SQUE_UINT    GL_UNSIGNED_INT
#define SQUE_INT     GL_INT
#define SQUE_FLOAT   GL_FLOAT
#define SQUE_DOUBLE  GL_DOUBLE
// Texture Types
#define SQUE_RGBA    GL_RGBA
#define SQUE_R8      GL_R8
#define SQUE_R16     GL_R16
#define SQUE_RGB     GL_RGB
// Texture Parameters
#define SQUE_MIN_FILTER		    GL_TEXTURE_MIN_FILTER
#define SQUE_MAG_FILTER		    GL_TEXTURE_MAG_FILTER
#define SQUE_WRAP_S		        GL_TEXTURE_WRAP_S
#define SQUE_WRAP_T			    GL_TEXTURE_WRAP_T
#define SQUE_BORDER_COLOR       GL_TEXTURE_BORDER_COLOR
#define SQUE_NEAREST            GL_NEAREST
#define SQUE_LINEAR             GL_LINEAR
#define SQUE_NEAREST_NEAREST    GL_NEAREST_MIPMAP_NEAREST
#define SQUE_LINEAR_NEAREST     GL_LINEAR_MIPMAP_NEAREST
#define SQUE_NEAREST_LINEAR     GL_NEAREST_MIPMAP_LINEAR
#define SQUE_LINEAR_LINEAR      GL_LINEAR_MIPMAP_LINEAR
#define SQUE_CLAMP              GL_CLAMP_TO_EDGE
#define SQUE_MIRROR             GL_MIRRORED_REPEAT
#define SQUE_REPEAT             GL_REPEAT
// Shaders
#define SQUE_VERTEX_SHADER      GL_VERTEX_SHADER
#define SQUE_FRAGMENT_SHADER    GL_FRAGMENT_SHADER
#define SQUE_COMPUTE_SHADER
#define SQUE_GEOMETRY_SHADER
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY MACROS ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLFW //////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(USE_GLFW)
#include <GLFW/glfw3.h>
#define SQUE_DISPLAY_END			SQUE_INVALID
// WINDOW HINTS
#define SQUE_MINIMIZED				GLFW_ICONIFIED
#define SQUE_RESIZABLE				GLFW_RESIZABLE
#define SQUE_VISIBLE					GLFW_VISIBLE
#define SQUE_DECORATED				GLFW_DECORATED
#define SQUE_FLOATING				GLFW_FLOATING
#define SQUE_MAXIMIZED				GLFW_MAXIMIZED
#define SQUE_CENTER_CURSOR			GLFW_CENTER_CURSOR
// CONTEXT HINTS
#define SQUE_WINDOW_CONTEXT_MAJOR	    GLFW_CONTEXT_VERSION_MAJOR
#define SQUE_WINDOW_CONTEXT_MINOR	    GLFW_CONTEXT_VERSION_MINOR
#define SQUE_WINDOW_CONTEXT_REVISION	GLFW_CONTEXT_REVISION
#define SQUE_CONTEXT_MAJOR_MIN		    3
// BUFFER HINTS
#define SQUE_RED_BITS				GLFW_RED_BITS
#define SQUE_GREEN_BITS				GLFW_GREEN_BITS
#define SQUE_BLUE_BITS				GLFW_BLUE_BITS
#define SQUE_ALPHA_BITS				GLFW_ALPHA_BITS
#define SQUE_DEPTH_BITS				SQUE_INVALID
#define SQUE_STENCIL_BITS			GLFW_STENCIL_BITS
#define SQUE_BUFFER_SIZE			SQUE_INVALID
#define SQUE_STEREOSCOPIC			GLFW_STEREO
#define SQUE_MSAA_SAMPLES			GLFW_SAMPLES
#define SQUE_SRGB					GLFW_SRGB_CAPABLE
#define	SQUE_REFRESH_RATE			GLFW_REFRESH_RATE
#define SQUE_DOUBLE_BUFFER			GLFW_DOUBLEBUFFER
#define SQUE_RENDERABLE_TYPE         SQUE_INVALID
#define SQUE_MIN_RENDERABLE          SQUE_INVALID

// EGL //////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(USE_EGL)
#include<EGL/egl.h>
#define SQUE_DISPLAY_END				EGL_NONE
// WINDOW HINTS
#define SQUE_MINIMIZED				SQUE_INVALID
#define SQUE_RESIZABLE				SQUE_INVALID
#define SQUE_VISIBLE					SQUE_INVALID
#define SQUE_DECORATED				SQUE_INVALID
#define SQUE_FLOATING				SQUE_INVALID
#define SQUE_MAXIMIZED				SQUE_INVALID
#define SQUE_CENTER_CURSOR			SQUE_INVALID
// CONTEXT HINTS
#define SQUE_WINDOW_CONTEXT_MAJOR	    EGL_CONTEXT_CLIENT_VERSION
#define SQUE_WINDOW_CONTEXT_MINOR	    SQUE_INVALID
#define SQUE_WINDOW_CONTEXT_REVISION    SQUE_INVALID
#define SQUE_CONTEXT_MAJOR_MIN		    3
// BUFFER HINTS
#define SQUE_RED_BITS				EGL_RED_SIZE
#define SQUE_GREEN_BITS				EGL_GREEN_SIZE
#define SQUE_BLUE_BITS				EGL_BLUE_SIZE
#define SQUE_ALPHA_BITS				EGL_ALPHA_SIZE
#define SQUE_DEPTH_BITS				EGL_DEPTH_SIZE
#define SQUE_STENCIL_BITS			EGL_STENCIL_SIZE
#define SQUE_BUFFER_SIZE			EGL_BUFFER_SIZE
#define SQUE_STEREOSCOPIC			SQUE_INVALID
#define SQUE_MSAA_SAMPLES			SQUE_INVALID
#define SQUE_SRGB					SQUE_INVALID
#define	SQUE_REFRESH_RATE			SQUE_INVALID
#define SQUE_DOUBLE_BUFFER			SQUE_INVALID
#define SQUE_RENDERABLE_TYPE         EGL_RENDERABLE_TYPE
#define SQUE_MIN_RENDERABLE          EGL_OPENGL_ES3_BIT

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILESYSTEM MACROS /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// WINDOWS ///////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#include <Windows.h>
#include <libloaderapi.h>
#define SQUE_FS_HIDDEN				FILE_ATTRIBUTE_HIDDEN
#elif defined(__linux__)
#define SQUE_FS_HIDDEN				BITSET1
#elif defined(ANDROID)
#endif
#endif // SQUE_REMAP_MACROS

