#include <squelib.h>

#define TIMES 1000000
#define TIMES_S 1000
// Timer Performance
void TimerPerformance()
{
    SQUE_Timer t;
    SQUE_Timer t2;

    t.Start();
    for (int i = 0; i < TIMES; ++i)
        double test = t2.ReadMilliSec();
    t.Stop();
    SQUE_LOG(LT_INFO, "SQUE_Timer::ReadMilliSec(): %f ms", t.ReadMilliSec());

    t.Start();
    for (int i = 0; i < TIMES; ++i)
        double test = t2.ReadMicroSec();
    t.Stop();
    SQUE_LOG(LT_INFO, "SQUE_Timer::ReadMicroSec(): %f ms", t.ReadMilliSec());

    t.Start();
    for (int i = 0; i < TIMES; ++i)
        double test = t2.ReadNanoSec();
    t.Stop();
    SQUE_LOG(LT_INFO, "SQUE_Timer::ReadNanoSec(): %f ms", t.ReadMilliSec());
}
// Logging Performance

void LoggingPerformance()
{
    SQUE_Timer t;
    // Logs generate a ton of memory each one, so stick to 1000
    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_LOG(LT_INFO, "Simple Log");
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_LOG Small Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_LOG(LT_INFO, "This is a long log that will put %s to think about the decisions in life and wonder if it was really worth the %d years it took %s %f.2 %b", "Marc Torres Jimenez", 1234, "iasuhfosaiugfh", 123124.124, true);
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_LOG Long Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_PRINT(LT_INFO, "Simple Log");
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_PRINT Small Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_PRINT(LT_INFO, "This is a long log that will put %s to think about the decisions in life and wonder if it was really worth the %d years it took %s %f.2 %b", "Marc Torres Jimenez", 1234, "iasuhfosaiugfh", 123124.124, true);
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_PRINT Long Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

}

#include <vector>
// RESIZE vs RESERVE
void Resize_VS_Reserve()
{
    SQUE_Timer t;
    
    {
        std::vector<int> data1;

        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            int cap = data1.size() - 1;
            data1.resize((cap < i) ? i : cap + 1);
            data1[i - 1] = i;
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Resize [] + 1: %f ms", t.ReadMilliSec());
    }

    {
        std::vector<int> data1;

        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.push_back(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Reserve push_back: %f ms", t.ReadMilliSec());
    }

    // Reserve More Space or Each time
    {
        std::vector<int> data1;

        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            int cap = data1.size() - 1;
            data1.resize((cap < i) ? cap + 100 : cap + 1);
            data1[i - 1] = i;
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Resize [] + 100: %f ms", t.ReadMilliSec());
    }
    {
        std::vector<int> data1;

        t.Start();
        data1.reserve(TIMES);
        for (int i = 1; i < TIMES; ++i)
        {
            data1.push_back(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Reserve push_back reserve 1000000: %f ms", t.ReadMilliSec());
    }

    // If or Branching Statement reserve
    {
        std::vector<int> data1;

        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            int cap = data1.size() - 1;
            data1.resize((cap < i) ? cap + 100 : cap + 1);
            data1[i - 1] = i;
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Resize [] branch +100/+0: %f ms", t.ReadMilliSec());
    }

    {
        std::vector<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            int cap = data1.size() - 1;
            if (cap < i) data1.resize(cap + 1 + 100);
            data1[i - 1] = i;
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Resize [] if +100: %f ms", t.ReadMilliSec());
    }
}



#include <list>

void TestVecWrap()
{
    SQUE_Timer t;
    {
        std::vector<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            int cap = data1.size() - 1;
            if (cap < i) data1.resize(cap + 1 + 100);
            data1[i - 1] = i;
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "Resize [] if +100: %f ms", t.ReadMilliSec());
    }

    {
        sque_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.push_back(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "sque_vec push_back no resize: %f us", t.ReadMicroSec());
    }

    {
        sque_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.try_insert(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "sque_vec try_insert no free_index: %f us", t.ReadMicroSec());
    }

    {
        sque_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            if (!(i % 500))
            {
                for (int i = 0; i < 50; ++i)
                    data1.free_index(i * 9);
            }
            data1.try_insert(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "sque_vec try_insert free_index 50 every 500: %f us", t.ReadMicroSec());
    } // Insert is about 50% slower and add another 16% slow if you are performing the free operations and inserting
    // Consistently 50% slower in release, memory uplift is quite good, could compare to std::list

    {
        std::list<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            if(!(i%500))
                for (int i = 0; i < 50; ++i)
                {
                    auto ind = data1.insert(data1.end(), i * 9);
                    data1.erase(ind);
                }
            data1.push_front(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "std::list push_front free_index 50 every 500 with iterator insert: %f us", t.ReadMicroSec());
    } // std::list is about 200% slower than try_insert and freeing

    {
        sque_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.resize(data1.size()+50);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "sque_vec resize +50: %f ms", t.ReadMilliSec());
    }

    {
        sque_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            if (!(i % 500))
            {
                for (int i = 0; i < 50; ++i)
                    data1.free_index(i * 9);
            }
            data1.resize(data1.size() + 50);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "sque_vec resize +50 free_index 50 every 500: %f ms", t.ReadMilliSec());
    }
    
}

static sque_vec<uint32_t> values_100;
static sque_vec<uint32_t> values_1000;
static sque_vec<uint32_t> values_10000;

static SQUE_Timer sort_timer;
#include <sque_sort.h>
#include <ctime>

template<class T>
void BrutePrintVec(sque_vec<T>& vec)
{
    for (uint32_t i = 0; i < vec.size(); ++i)
        printf("%d ", vec[i]);

    printf("\n\n");

}

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

        BrutePrintVec(selection_values);

        selection_values = values_1000;
        sort_timer.Start();
        SQUE_SORT_Selection<uint32_t>(selection_values.begin(), selection_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Selection 1000: %f us", sort_timer.ReadMicroSec());

        BrutePrintVec(selection_values);

        selection_values = values_10000;
        sort_timer.Start();
        SQUE_SORT_Selection<uint32_t>(selection_values.begin(), selection_values.size());
        sort_timer.Stop();
        SQUE_LOG(LT_INFO, "SQUE_SORT_Selection 10000: %f us", sort_timer.ReadMicroSec());

        BrutePrintVec(selection_values);
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
}

int main(int argc, char** argv)
{
    SQUE_LIB_InitRNG(time(NULL));

    //LoggingPerformance();
    //TimerPerformance();
    //
    //Resize_VS_Reserve();
    //TestVecWrap();
    //SQUE_LIB_Close();
    
    Test10Sorts();
    //TestSorts();
    return 0;
}