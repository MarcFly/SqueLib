#ifndef _FLY_DISPLAY_
#define _FLY_DISPLAY_

#ifdef USE_EGL
    typedef void* FLY_window;
#elif USE_GLFW
    typedef GLFWwindow FLY_window;
#endif

bool FLYDISPLAY_Init(const char* title = "", int w = 0, int h = 0 /*,flags*/);
bool FLYDISPLAY_Close();
void FLYDISPLAY_SetVSYN(int vsync_val);
void FLYDISPLAY_Clean();
void FLYDISPLAY_SwapBuffers();
void FLYDISPLAY_Resize(int w, int h);
void FLYDISPLAY_GetSize(int* w, int* h);
void FLYDISPLAY_CloseWindow(int window)
#endif //_FLY_DISPLAY_H_