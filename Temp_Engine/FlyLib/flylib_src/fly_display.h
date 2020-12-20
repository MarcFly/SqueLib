#ifndef _FLY_DISPLAY_
#define _FLY_DISPLAY_

typedef unsigned short uint16;

// Flag setting and unsettign using |=FLAG|FLAG and &=(~FLAG | ~FLAG | ~FLAG |...)
typedef uint16 FLY_WindowFlags; // -> enum FLY_WindowFlags_
enum FLY_WindowFlags_
{
	FLYWINDOW_RESIZABLE = 1 << 0,
	FLYWINDOW_DECORATED = 1 << 1,
	FLYWINDOW_ALWAYS_TOP = 1 << 2,
	FLYWINDOW_MAXIMIZED = 1 << 3
};


typedef struct FLY_Window{
	const char* title = "";
	uint16 width=0, height=0;
	FLY_WindowFlags flags;
} FLY_Window;

// Initialization / Global State
bool FLYDISPLAY_Init(uint16 flags, const char* title = "", uint16 w = 0, uint16 h = 0);
bool FLYDISPLAY_Close();
void FLYDISPLAY_SetVSYNC(uint16 vsync_val);

// Control Specific Windows
void FLYDISPLAY_Resize(uint16 window, uint16 w, uint16 h);
void FLYDISPLAY_GetSize(uint16 window, uint16* w, uint16* h);
void FLYDISPLAY_GetAmountWindows(uint16* windows);
bool FLYDISPLAY_ShouldWindowClose(uint16 window);
void FLYDISPLAY_CloseWindow(uint16 window);
void FLYDISPLAY_DestroyWindow(uint16 window);
bool FLYDISPLAY_OpenWindow(FLY_Window* window = NULL, uint16 monitor = 0);
// Controlling Contexts
void FLYDISPLAY_Clean();
void FLYDISPLAY_SwapBuffers();
void FLYDISPLAY_MakeContextMain(uint16 window);
#endif //_FLY_DISPLAY_H_