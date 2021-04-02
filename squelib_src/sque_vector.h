#ifndef _SQUE_VEC_
#define _SQUE_VEC_

#ifdef ANDROID
#include <malloc.h>
#endif

template<class T>
class sque_vec
{
    T* _data;
    uint32_t _size;
    uint32_t _capacity;

    uint32_t _populated;
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
            dest = new uint32_t(*begin);
            //new((void*)dest) uint32_t(*begin); // new ((convert to void*)at pointer dest) Value<T>(copy from *begin)
            // This syntax does not work on unix... ?
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

    sque_vec()
    {
        _size = 0;
        _capacity = 0;
        _populated = 0;
        _empty_size = 0;
        _empty_capacity = 0;
        _data = NULL;
        _empty_data = NULL;
    }

    sque_vec(const sque_vec<T>& cpy_vec)
    {
        _size = cpy_vec._size;
        _capacity = cpy_vec._capacity;
        _populated = cpy_vec._populated;
        _empty_size = cpy_vec._empty_size;
        _empty_capacity = cpy_vec._empty_capacity;

        _data = allocate(_capacity);
        copyRange(cpy_vec._data, cpy_vec._data + _size, _data);

        _empty_data = allocateU(_empty_size);
        copyRangeU(cpy_vec._empty_data, cpy_vec._empty_data + _empty_size, _empty_data);
    }

    ~sque_vec()
    {
        if (_capacity > 0) deleteRange(_data, _data + _size);
        free(_data);

        if (_empty_capacity > 0) deleteRangeU(_empty_data, _empty_data + _empty_size);
        free(_empty_data);

        _capacity = 0;
        _data = NULL;

        _empty_capacity = 0;
        _empty_data = NULL;
    }

    T& operator[] (uint32_t index) { return _data[index]; };
    T* begin() const { return _data; }
    T* end() const { return _data + _size; }
    uint32_t size() const { return _size; }
    uint32_t populated() const { return _populated; }

    void push_back(const T& value)
    {
        ++_populated;

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
        //_data = nullptr;
        free(_data);
        _data = new_data;
        ++_size;
        
    }

    uint32_t try_insert(const T& value)
    {
        uint32_t ret;
        if (_empty_size == 0)
        {
            push_back(value);
            return (_size - 1);
        }

        ret = _empty_data[_empty_size-- - 1];
        new((void*)(_data + ret)) T(value);
        ++_populated;
        return ret;
        
    }

    void free_index(uint32_t index)
    {
        if(index > (_size-2)) return;
        if (_empty_size != _empty_capacity)
        {
            _empty_data[_empty_size] = index;
            //new((uint32_t*)(_empty_data + _empty_size)) uint32_t(index);
            ++_empty_size;
            --_populated;
            return;
        }
        _empty_capacity = _empty_capacity * 2 + 1;
        uint32_t* new_data = allocateU(_empty_capacity);
        copyRangeU(_empty_data, _empty_data + _empty_size, new_data);
        new_data[_empty_size] = index;
        //new((void*)(new_data + _empty_size)) uint32_t(index);
        deleteRangeU(_empty_data, _empty_data + _empty_size);
        _empty_data = nullptr;
        free(_empty_data);
        _empty_data = new_data;
        ++_empty_size;
    }

    void pop_back()
    {
        if(_size == 0) return;
        _data[_size-1].~T();
        --_size;
    }

    void clear()
    {
        deleteRange(_data, _data+_size);
        deleteRangeU(_empty_data, _empty_data+_empty_size);
        _size = 0;
        _empty_size = 0;
    }

    void resize(uint32_t new_size)
    {
        if (new_size < _size)
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
                    dest_iter = new uint32_t(*iter);
                    //new((void*)dest_iter) uint32_t(*iter);
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

#endif