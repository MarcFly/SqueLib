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

template<class T>
class own_vec
{
    T* _data;
    uint32_t _size;
    uint32_t _capacity;

    uint32_t* _empty_data;
    uint32_t _empty_size;
    uint32_t _empty_capacity;
    
    static T* allocate(uint32_t size)
    {
        return (T*)malloc(sizeof(T) * size);
    }

    static uint32_t* allocateU(uint32_t size)
    {
        return (uint32_t*)malloc(sizeof(uint32_t) * size);
    }

    static void copyRange(T* begin, T* end, T* dest)
    {
        while (begin != end)
        {
            new((void*)dest) T(*begin); // new ((convert to void*)at pointer dest) Value<T>(copy from *begin)
            ++dest;
            ++begin;
        } // Why this in loop (guess it can be vectorized instead of single operation memcpy and then delete
    }

    static void copyRangeU(uint32_t* begin, uint32_t* end, uint32_t* dest)
    {
        while (begin != end)
        {
            new((void*)dest) T(*begin); // new ((convert to void*)at pointer dest) Value<T>(copy from *begin)
            ++dest;
            ++begin;
        } // Why this in loop (guess it can be vectorized instead of single operation memcpy and then delete
    }

    void reallocate(uint32_t new_capacity)
    {
        T* new_data = allocate(new_capacity);
        copyRange(_data, _data + _size, new_data);
        deleteRange(_data, _data + _size);
        free(_data);
        _data = new_data;
        _capacity = new_capacity;
    }
    static void constructRange(T* begin, T* end)
    {
        while (begin != end)
        {
            new((void*)begin) T();
            ++begin;
        }
    }
    static void constructRange(T* begin, T* end, const T& fillWith)
    {
        while (begin != end)
        {
            new((void*)begin) T(fillWith);
            ++begin;
        }
    }

    static void deleteRange(T* begin, T* end)
    {
        while (begin != end)
        {
            begin->~T();
            ++begin;
        }
    }
    static void deleteRangeU(uint32_t* begin, uint32_t* end)
    {
        while (begin != end)
        {
            begin->~uint32_t();
            ++begin;
        }
    }
public:
    typedef T* iterator;
    typedef T value_type;

    own_vec()
    {
        _size = 0;
        _capacity = 0;
        _empty_size = 0;
        _empty_capacity = 0;
        _data = nullptr;
        _empty_data = nullptr;
    }
    ~own_vec()
    {
        deleteRange(_data, _data + _size);
        free(_data);
        deleteRangeU(_empty_data, _empty_data + _empty_size);
        free(_empty_data);
    }
    
    void push_back(const T& value)
    {
        if (_size != _capacity) // Capacity does nto need increase, fastes possible
        {
            new((void*)(_data + _size)) T(value);
            ++_size;
            return;
        } 
        // Need Increase Capacity
        _capacity = _capacity * 2 + 8;
        T* new_data = allocate(_capacity);
        copyRange(_data, _data + _size, new_data);
        new((void*)(new_data + _size)) T(value);
        deleteRange(_data, _data + _size);
        _data = nullptr;
        free(_data);
        _data = new_data;
        ++_size;
    }

    bool try_insert(const T& value)
    {
        if (_empty_size > 0)
        {
            new((void*)(_data + _empty_data[_empty_size-- - 1])) T(value);
            return true;
        }
        push_back(value);
        return false;
    }

    void free_index(uint32_t index)
    {
        if (_empty_size != _empty_capacity)
        {
            new((uint32_t*)(_empty_data + _empty_size)) uint32_t(index);
            ++_empty_size;
            return;
        }
        _empty_capacity = _empty_capacity * 2 + 1;
        uint32_t* new_data = allocateU(_empty_capacity);
        copyRangeU(_empty_data, _empty_data + _empty_size, new_data);
        new((void*)(new_data + _empty_size)) uint32_t(index);
        deleteRangeU(_empty_data, _empty_data + _empty_size);
        _empty_data = nullptr;
        free(_empty_data);
        _empty_data = new_data;
        ++_empty_size;
    }

    T& operator[] (uint32_t index) { return _data[index]; };
    //const T& operator[](uint32_t index) { return _data[index]; } const;
    T* first() const { return _data; }
    T* last() const { return _data + _size; }
    uint32_t size() { return _size; }

    void resize(uint32_t new_size)
    {
        if (new_size <= _size)
        {
            deleteRange(_data + new_size, _data + _size);
            _size = new_size;
            // Here it should be checked to clean empty_spaces over new_size
            uint32_t* iter = _empty_data;
            uint32_t* new_empty_data = allocateU(_empty_capacity);
            uint32_t* dest_iter = new_empty_data;
            uint32_t new_empty_size;
            while (iter != (_empty_data + _empty_size))
            {
                if (*iter < new_size)
                {
                    new((void*)dest_iter) T(*iter);
                    ++dest_iter;
                    ++iter;
                    ++new_empty_size;
                }
            }
            deleteRangeU(_empty_data, _empty_data + _empty_size);
            free(_empty_data);
            _empty_size = new_empty_size;
            _empty_data = new_empty_data;
            return;
        }
        if (new_size <= _capacity)
        {
            constructRange(_data + _size, _data + new_size);
            _size = new_size;
            return;
        }
        uint32_t new_capacity = new_size;
        if (new_capacity < _size * 2)
        {
            new_capacity = _size * 2;
        }
        reallocate(new_capacity);
        constructRange(_data + _size, _data + new_size);
        _size = new_size;
    }

};

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
        own_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.push_back(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "own_vec push_back no resize: %f us", t.ReadMicroSec());
    }

    {
        own_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.try_insert(i);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "own_vec try_insert no free_index: %f us", t.ReadMicroSec());
    }

    {
        own_vec<int> data1;
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
        SQUE_LOG(LT_INFO, "own_vec try_insert free_index 50 every 500: %f us", t.ReadMicroSec());
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
        own_vec<int> data1;
        t.Start();
        for (int i = 1; i < TIMES; ++i)
        {
            data1.resize(data1.size()+50);
        }
        t.Stop();
        SQUE_LOG(LT_INFO, "own_vec resize +50: %f ms", t.ReadMilliSec());
    }

    {
        own_vec<int> data1;
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
        SQUE_LOG(LT_INFO, "own_vec resize +50 free_index 50 every 500: %f ms", t.ReadMilliSec());
    }
    
}

int main(int argc, char** argv)
{
    LoggingPerformance();
    TimerPerformance();
    
    Resize_VS_Reserve();
    TestVecWrap();
    //SQUE_LIB_Close();

    return 0;
}