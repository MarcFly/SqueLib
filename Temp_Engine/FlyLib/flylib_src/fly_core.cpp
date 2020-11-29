#include "fly_lib.h"

bool FLYLIB_INIT(/* flags */)
{
    bool ret = true;

    // Call Init for all loaded modules and with required flags
    FLYLOGGER_INIT(/*pass flag true or false*/ true);

    return ret;
}

bool FLYLIB_CLOSE(/* flags */)
{
    bool ret = true;

    // Call close for all required modules for FlyLib
    FLYLOGGER_CLOSE();

    return ret;
}
unsigned int FLGetVersion(void)
{
    return FL_VERSION;
}

int FLIsCompatibleDLL(void)
{
    uint32_t major = FLGetVersion() >> 16;
    return major == FL_VERSION_MAJOR;
}