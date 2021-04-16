#include <squelib.h>

#include "tests/test_sque_basics.h"
#include "tests/test_sque_vec.h"
#include "tests/test_sorts.h"
#include "tests/template_class_vs_data_structs.h"

int main(int argc, char** argv)
{
    SQUE_LIB_InitRNG(time(NULL)); // For sort, should be self contained prob

    //LoggingPerformance();
    //TimerPerformance();
    //
    //Resize_VS_Reserve();
    //TestVecWrap();
    //SQUE_LIB_Close();
    
    //Test10Sorts();
    //TestSorts();
    
    for(uint16_t i = 0; i < 100; ++i)
        Test_Template_VS_Struct();

    TestAverageTimes_Template_Struct();

    return 0;
}