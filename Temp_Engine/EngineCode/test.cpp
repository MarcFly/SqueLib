extern struct android_app * gapp;
void AndroidMakeFullscreen();
int AndroidHasPermissions(const char* perm_name);
void AndroidRequestAppPermissions(const char * perm);
void AndroidDisplayKeyboard(int pShow);
int AndroidGetUnicodeChar( int keyCode, int metaState );
void AndroidSendToBack( int param );

extern int android_sdk_version; //Derived at start from property ro.build.version.sdk
extern int android_width, android_height;
extern int UpdateScreenWithBitmapOffsetX;
extern int UpdateScreenWithBitmapOffsetY;

volatile int suspended;

void HandleSuspend()
{
	suspended = 1;
}

void HandleResume()
{
	suspended = 0;
}

#include <android_native_app_glue.h>
//#include <android/sensor.h>

extern struct android_app * gapp;

int main()
{
    if(gapp == nullptr || gapp != nullptr)
    {
        int a = 0;
        a+=2;
        a*=2;
        a/=2;
    }

    return 0;
}

