#ifndef _TEST_SORT_
#define _TEST_SORT_

static sque_vec<uint32_t> values_100;
static sque_vec<uint32_t> values_1000;
static sque_vec<uint32_t> values_10000;

static SQUE_Timer sort_timer;
#include <sque_sort.h>
#include <ctime>

void Test10Sorts()
{
    sque_vec<uint32_t> values_10;

    for (uint32_t i = 0; i < 10; ++i)
        values_10.push_back(SQUE_RNG(10));
    BrutePrintVec<uint32_t>(values_10);

    {
        sque_vec<uint32_t> values = values_10;
        SQUE_SORT_Bubble<uint32_t>(values.begin(), values.size());
        BrutePrintVec<uint32_t>(values);

        values = values_10;
        SQUE_SORT_Selection<uint32_t>(values.begin(), values.size());
        BrutePrintVec<uint32_t>(values);

        values = values_10;
        SQUE_SORT_Insertion<uint32_t>(values.begin(), values.size());
        BrutePrintVec<uint32_t>(values);

        values = values_10;
        SQUE_SORT_InsertionRecursive<uint32_t>(values.begin(), values.size());
        BrutePrintVec<uint32_t>(values);

        values = values_10;
        SQUE_SORT_Merge<uint32_t>(values.begin(), values.size());
        BrutePrintVec<uint32_t>(values);

        values = values_10;
        SQUE_SORT_Quick<uint32_t>(values.begin(), values.size());
        BrutePrintVec<uint32_t>(values);
    }

}

void TestSorts()
{
    

    for (uint32_t i = 0; i < TIMES_S / 10; ++i)
    {
        values_100.push_back(SQUE_RNG(100));
        printf("%d ", values_100[i]);
    }

    printf("\n\n");

    for (uint32_t i = 0; i < TIMES_S; ++i)
    {
        values_1000.push_back(SQUE_RNG(1000));
        printf("%d ", values_1000[i]);
    }

    printf("\n\n");

    for (uint32_t i = 0; i < TIMES_S * 10; ++i)
    {
        values_10000.push_back(SQUE_RNG(10000));
        printf("%d ", values_10000[i]);
    }

    printf("\n\n");
    
    { // Selection Sort
        sque_vec<uint32_t> selection_values = values_100;
        sort_timer.Start();
        SQUE_SORT_Selection<uint32_t>(selection_values.begin(), selection_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Selection 100: %f us", sort_timer.ReadMicroSec());

        selection_values = values_1000;
        sort_timer.Start();
        SQUE_SORT_Selection<uint32_t>(selection_values.begin(), selection_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Selection 1000: %f us", sort_timer.ReadMicroSec());

        selection_values = values_10000;
        sort_timer.Start();
        SQUE_SORT_Selection<uint32_t>(selection_values.begin(), selection_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Selection 10000: %f us", sort_timer.ReadMicroSec());
    }

    { // Bubble Sort
        sque_vec<uint32_t> Bubble_values = values_100;
        sort_timer.Start();
        SQUE_SORT_Bubble<uint32_t>(Bubble_values.begin(), Bubble_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Bubble 100: %f us", sort_timer.ReadMicroSec());

        Bubble_values = values_1000;
        sort_timer.Start();
        SQUE_SORT_Bubble<uint32_t>(Bubble_values.begin(), Bubble_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Bubble 1000: %f us", sort_timer.ReadMicroSec());

        Bubble_values = values_10000;
        sort_timer.Start();
        SQUE_SORT_Bubble<uint32_t>(Bubble_values.begin(), Bubble_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Bubble 10000: %f us", sort_timer.ReadMicroSec());
    }

    { // Insertion Sort
        sque_vec<uint32_t> Insertion_values = values_100;
        sort_timer.Start();
        SQUE_SORT_Insertion<uint32_t>(Insertion_values.begin(), Insertion_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Insertion 100: %f us", sort_timer.ReadMicroSec());

        Insertion_values = values_1000;
        sort_timer.Start();
        SQUE_SORT_Insertion<uint32_t>(Insertion_values.begin(), Insertion_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Insertion 1000: %f us", sort_timer.ReadMicroSec());

        Insertion_values = values_10000;
        sort_timer.Start();
        SQUE_SORT_Insertion<uint32_t>(Insertion_values.begin(), Insertion_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Insertion 10000: %f us", sort_timer.ReadMicroSec());
    }

    { // Merge Sort
        sque_vec<uint32_t> Merge_values = values_100;
        sort_timer.Start();
        SQUE_SORT_Merge<uint32_t>(Merge_values.begin(), Merge_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Merge 100: %f us", sort_timer.ReadMicroSec());

        Merge_values = values_1000;
        sort_timer.Start();
        SQUE_SORT_Merge<uint32_t>(Merge_values.begin(), Merge_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Merge 1000: %f us", sort_timer.ReadMicroSec());

        Merge_values = values_10000;
        sort_timer.Start();
        SQUE_SORT_Merge<uint32_t>(Merge_values.begin(), Merge_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Merge 10000: %f us", sort_timer.ReadMicroSec());
    }

    { // Quick Sort
        sque_vec<uint32_t> Quick_values = values_100;
        sort_timer.Start();
        SQUE_SORT_Quick<uint32_t>(Quick_values.begin(), Quick_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Quick 100: %f us", sort_timer.ReadMicroSec());

        Quick_values = values_1000;
        sort_timer.Start();
        SQUE_SORT_Quick<uint32_t>(Quick_values.begin(), Quick_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Quick 1000: %f us", sort_timer.ReadMicroSec());

        Quick_values = values_10000;
        sort_timer.Start();
        SQUE_SORT_Quick<uint32_t>(Quick_values.begin(), Quick_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Quick 10000: %f us", sort_timer.ReadMicroSec());
    }
}

#endif