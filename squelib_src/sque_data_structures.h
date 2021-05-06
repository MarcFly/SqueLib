#ifndef _SQUE_VEC_
#define _SQUE_VEC_

// Do a list, but memory stacked
// Compact the Vector without the functions of empty data and such, just take care of the memory

#ifdef ANDROID
#include <malloc.h>
#endif

// Do a Vec - Assures Positions, erase will not swap positions
// Indexable Unordered Vec - FreeAble Vec + Index by a uint32_t
// - Uses uint32_t as ids that are assumed unique but not checked
// Freeable Vec - Pop from any position, try_insert and free_index


// COMMON
template<class T>
static T* allocate(uint32_t size)
{
    return (T*) malloc(sizeof(T)*size);
}

template<class T>
static void copyRange(T* begin, T* end, T* dest)
{
    while (begin != end)
    {
        new((void*)dest) T(*begin); // new ((convert to void*)at pointer dest) Value<T>(copy from *begin)
        ++dest;
        ++begin;
    } // Why this in loop (guess it can be vectorized instead of single operation memcpy and then delete
}

template<class T>
static void constructRange(T* begin, T* end)
    {
        while (begin != end)
        {
            new((void*)begin) T();
            ++begin;
        }
}

template<class T>
static void constructRange(T* begin, T* end, const T& fillWith)
{
    while (begin != end)
    {
        new((void*)begin) T(fillWith);
        ++begin;
    }
}

template<class T>
static void deleteRange(T* begin, T* end)
{
    while (begin != end)
    {
        begin->~T();
        ++begin;
    }
}

template<class T>
static void reallocate(T** _data, uint32_t _size, uint32_t& _capacity, uint32_t new_capacity)
{
    T* new_data = allocate<T>(new_capacity);
    copyRange(*_data, *_data + _size, new_data);
    deleteRange(*_data, *_data + _size);
    free(*_data);
    *_data = new_data;
    _capacity = new_capacity;
}

// VECTOR WITH FREEABLE INDICES... it is bad right now should be 2 separate
// VECTOR LIST - Contiguous memory, list type iteration and search
template<class T>
class vec_node
{
public:
    bool free = false;
    T _data;

    vec_node() {};
    vec_node(const T& cpy) { _data = cpy; }
    ~vec_node() { free = true; _data.~T(); }
};


template<class T>
class sque_free_vec
{
    vec_node<T>* _nodes = NULL;
    uint32_t _size = 0;
    uint32_t _capacity = 0;

    uint32_t* _empty = NULL;
    uint32_t _empty_size = 0;
    uint32_t _empty_capacity = 0;

    vec_node<T>* GetFromIndex(const uint32_t index) const
    {
        if (_nodes[index].free == true)
        {
            uint32_t ind = index;
            for (uint32_t i = index + 1; i < _size; ++i)
                if (_nodes[i].free == false)
                    return &_nodes[i];
        }
        else
            return &_nodes[index];

        return &_nodes[_size]; // If you fucked up you fucked up
    }

public:
    typedef T* iterator;
    typedef T value_type;

    sque_free_vec() {};
    sque_free_vec(const sque_free_vec<T>& cpy)
    {
        _size = cpy._size;
        _capacity = cpy._capacity;
        _empty_size = cpy._empty_size;
        _empty_capacity = cpy._empty_capacity;

        _nodes = allocate<vec_node<T>>(_capacity);
        copyRange(cpy._nodes, cpy._nodes+_size, _nodes);

        _empty = allocate<uint32_t>(_empty_capacity);
        copyRange<uint32_t>(cpy._empty, cpy._empty + _empty_size, _empty);
    }
    ~sque_free_vec()
    {
        if (_capacity > 0)
        {
            deleteRange(_nodes, _nodes + _size);
            free(_nodes);
        }
        if (_empty_capacity > 0)
        {
            deleteRange<uint32_t>(_empty, _empty+_empty_size);
            free(_empty);
        }

        _capacity = 0;
        _size = 0;
        _nodes = NULL;
        _empty_capacity = 0;
        _empty_size = 0;
        _empty = NULL;
    }

    sque_free_vec<T>& operator=(const sque_free_vec<T>& cpy)
    {
        if (this == &cpy)
            return *this;
        this->~sque_free_vec();

        _size = cpy._size;
        _capacity = cpy._capacity;
        _empty_size = cpy._empty_size;
        _empty_capacity = cpy._empty_capacity;

        _nodes = allocate<vec_node<T>>(_capacity);
        copyRange(cpy._nodes, cpy._nodes + _size, _nodes);

        _empty = allocate<uint32_t>(_empty_capacity);
        copyRange<uint32_t>(cpy._empty, cpy._empty + _empty_size, _empty);
    }

    T& operator[] (const uint32_t index) { return (GetFromIndex(index)->_data); }
    const T& operator[] (const uint32_t index) const { return (GetFromIndex(index)->_data); }
    T* begin() const { return &GetFromIndex(0)->_data; }
    T* end() const { return &(_nodes[_size]._data); }
    uint32_t size() const { return _size; }
    
    uint32_t push(const T& value)
    {
        if (_empty_size > 0)
        {
            _nodes[_empty[--_empty_size]]._data = value;
            _nodes[_empty[_empty_size]].free = false;
            return _empty[_empty_size];
        }

        if (_size != _capacity)
        {
            new((void*)(_nodes + _size)) vec_node<T>(value);
            return _size++;
        }

        _capacity = _capacity * 2 + 8;
        vec_node<T>* new_nodes = allocate<vec_node<T>>(_capacity);
        copyRange(_nodes, _nodes + _size, new_nodes);
        new((void*)(new_nodes + _size)) vec_node<T>(value);
        deleteRange(_nodes, _nodes + _size);
        free(_nodes);
        _nodes = new_nodes;
        return _size++;
    }

    void pop_back()
    {
        if (_size == 0) return;
        _nodes[_size - 1].~vec_node();
        --_size;
    }

    void free_index(const uint32_t index)
    {
        if (index > _size - 1) return;
        if (index == _size - 1) pop_back();
        _nodes[index].free = true;
        if (_empty_size != _empty_capacity)
        {
            _empty[_empty_size-1] = index;
            ++_empty_size;
            return;
        }
        _empty_capacity = _empty_capacity * 2 + 8;
        uint32_t* new_nodes = allocate<uint32_t>(_empty_capacity);
        copyRange<uint32_t>(_empty, _empty + _empty_size, new_nodes);
        new_nodes[_empty_size] = index;
        deleteRange<uint32_t>(_empty, _empty + _empty_size);
        _empty = nullptr;
        free(_empty);
        _empty = new_nodes;
        ++_empty_size;
    }

    void clear()
    {
        deleteRange(_nodes, _nodes + _size);
        deleteRange<uint32_t>(_empty, _empty + _empty_size);
        _size = 0;
        _empty_size = 0;
    }

    void reserve(uint32_t new_capacity)
    {
        if (new_capacity > _capacity)
            reallocate(_nodes, _size, _capacity, new_capacity);
    }

    void resize(uint32_t new_size)
    {
        if (new_size < _size)
        {
            deleteRange(_nodes + new_size, _nodes + _size);
            _size = new_size;
            uint32_t* iter = _empty;
            uint32_t* new_empty = allocate<uint32_t>(_empty_capacity);
            uint32_t* dest_iter = new_empty;
            uint32_t new_empty_size;
            while (iter != (_empty + _empty_size))
            {
                if (*iter < new_size)
                {
                    dest_iter = new uint32_t(*iter);
                    ++dest_iter;
                    ++iter;
                    ++new_empty_size;
                }
            }
            deleteRange<uint32_t>(_empty, _empty + _empty_size);
            free(_empty);
            _empty_size = new_empty_size;
            _empty = new_empty;
        }

        if (new_size <= _capacity)
        {
            constructRange(_nodes + _size, _nodes + new_size);
            _size = new_size;
            return;
        }
        
        uint32_t new_capacity = new_size;
        if (new_capacity < _size * 2)
            new_capacity = _size * 2;
        
        reallocate(&_nodes, _size, _capacity, new_capacity);
        constructRange(_nodes + _size, _nodes + new_size);
        _size = new_size;
                
    }
};

template<class T>
class sque_vec
{
    T* _data = NULL;
    uint32_t _size = 0;
    uint32_t _capacity = 0;    
public:
    typedef T* iterator;
    typedef T value_type;

    sque_vec()
    {
        _data = NULL;
        _size = 0;
        _capacity = 0;
    }

    sque_vec(const sque_vec<T>& cpy_vec)
    {
        _size = cpy_vec._size;
        _capacity = cpy_vec._capacity;

        _data = allocate<T>(_capacity);
        copyRange(cpy_vec._data, cpy_vec._data + _size, _data);
    }

    ~sque_vec()
    {
        if (_capacity > 0 && _size > 0)
        {
            deleteRange(_data, _data + _size);
            free(_data);
        }

        _capacity = 0;
        _data = NULL;
    }

    sque_vec<T>& operator=(const sque_vec<T>& cpy_vec)
    {
        if (this == &cpy_vec)
            return *this;

        _size = cpy_vec._size;
        _capacity = cpy_vec._capacity;

        _data = allocate<T>(_capacity);
        copyRange(cpy_vec._data, cpy_vec._data + _size, _data);
    }

    T& operator[] (const uint32_t index) { return _data[index]; };
    const T& operator[] (uint32_t index) const { return _data[index]; };
    T* begin() const { return _data; }
    T* end() const { return _data + _size; }
    T* last() { return _data + (_size - 1); }
    uint32_t size() const { return _size; }

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
        T* new_data = allocate<T>(_capacity);
        copyRange(_data, _data + _size, new_data);
        new((void*)(new_data + _size)) T(value);
        deleteRange(_data, _data + _size);
        //_data = nullptr;
        free(_data);
        _data = new_data;
        ++_size;
        
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
        _size = 0;
    }

    void reserve(uint32_t new_capacity)
    {
        if (new_capacity < _capacity)
            reallocate(_data, _size, _capacity, new_capacity);        
    }

    void resize(uint32_t new_size)
    {
        if (new_size < _size)
        {
            deleteRange(_data + new_size, _data + _size);
            _size = new_size;
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
        reallocate(&_data, _size, _capacity, new_capacity);
        constructRange(_data + _size, _data + new_size);
        _size = new_size;
    }   
};


template<class T>
void BrutePrintVec(sque_vec<T>& vec)
{
    for (uint32_t i = 0; i < vec.size(); ++i)
        printf("%d ", vec[i]);

    printf("\n\n");

}


template<class T>
class list_node
{
public:
    list_node* prev = NULL;
    list_node* next = NULL;
    T* _data = NULL;
    list_node() { _data = new T(); }
    list_node(const list_node<T>& node) 
    {
        prev = node.prev;
        next = node.next;
        _data = new T(*node._data); 
    }
    list_node(const T& value)
    {
        _data = new T(value);
    }
    ~list_node() { delete _data; }

    list_node<T>& operator++()
    {
        
        return *this;
    }
    list_node<T> operator++(int)
    {
        list_node<T> old = *this;
        operator++();
        return old;
    }
    list_node<T>& operator--()
    {

        return *this;
    }
    list_node<T> operator--(int)
    {
        list_node<T> old = *this;
        operator--();
        return old;
    }
};

template<class T>
class sq_list
{
    list_node<T>* first = NULL;
    list_node<T>* last = NULL;
    uint32_t _size = 0;
    
    // Something like checkpoints? Every a certain amount, add a pointer to checkpoints per size variable...
public:
    sq_list() {};
    sq_list(const sq_list<T>& cpy) 
    {
        if (cpy._size == 0) return;
        _size = cpy._size;
        first = new list_node<T>(*cpy.first);
        last = new list_node<T>(*cpy.last);
    };
    ~sq_list()
    {
        list_node<T>* iter = first;
        while (iter != last)
        {
            iter = iter->next;
            delete iter->prev;
        }
        if(iter != NULL)
            delete iter->_data;
        _size = 0;
    }

    list_node<T>* begin() { return first; }
    list_node<T>* end() { return last; }

    list_node<T>* at(const uint32_t pos)
    {
        if (first == NULL || pos >= _size) return NULL;
        list_node<T>* iter = first;
        uint32_t i = 0;
        while (i != pos)
        {
            ++iter;
            ++i;
        }
        return iter;
    }

    void pop(list_node<T>* iterator)
    {
        iterator->prev->next = iterator->next;
        iterator->next->prev = iterator->prev;
        delete iterator;
        --_size;
    }

    void pop(const uint32_t pos)
    {
        pop(at(pos));
    }

    void pop_front()
    {
        if (first == NULL) return;
        list_node<T>* temp = first;
        first = first->next;
        first->prev = NULL;
        delete temp;
        --_size;
    }

    void pop_back()
    {
        if (last == NULL) return;
        list_node<T>* temp = last;
        last = last->prev;
        last->next = NULL;
        delete temp;
        --_size;
    }

    void insert(list_node<T>* iterator, const T& value)
    {
        list_node<T>* new_node = new list_node<T>(value);
        new_node->next = iterator->next;
        iterator->next->prev = new_node;
        iterator->next = new_node;
        new_node->prev = iterator;
        ++_size;
    }

    void insert(const uint32_t pos, const T& value)
    {
        insert(at(pos), value);
    }

    void push_front(const T& value)
    {
        list_node<T>* new_node = new list_node<T>(value);
        if(first != NULL) first->prev = new_node;
        new_node->next = first;
        first = new_node;
        if (last == NULL) last = first;
        ++_size;
    }

    void push_back(const T& value)
    {
        list_node<T>* new_node = new list_node<T>(value);   
        if (last != NULL) last->next = new_node;
        new_node->prev = last;
        last = new_node;
        if (first == NULL) first = last;
        ++_size;
    }
};

#endif