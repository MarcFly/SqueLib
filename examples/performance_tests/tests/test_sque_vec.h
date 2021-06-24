#ifndef _TEST_SQUE_VEC_
#define _TEST_SQUE_VEC_

#include <squelib.h>
#include <list>

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

#endif