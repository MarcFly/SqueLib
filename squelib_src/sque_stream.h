#ifndef _SQUE_STREAM_
#define _SQUE_STREAM_

#ifdef ANDROID
#include <malloc.h>
#endif

// BruteForce Serialization
class SQUE_OutStream
{
	uint64_t _capacity = 0;
	uint64_t _size = 0;
	char* _data = NULL;	

	static char* allocate(uint32_t size)
	{
		return  new char[size];// (char*)malloc(size);
	}

	static void copyRange(char* begin, char* end, char* dest)
	{
		while (begin != end)
		{
			new((char*)dest) char(*begin); // new ((convert to void*)at pointer dest) Value<T>(copy from *begin)
			++dest;
			++begin;
		} // Why this in loop (guess it can be vectorized instead of single operation memcpy and then delete
	}

	static void deleteRange(char* begin, char* end)
	{
		// non templated type delete deallocate the whole block?
		// but why does it work before?
		delete begin; //?
		//while (begin != end)
		//{
		//	delete begin;
		//	++begin;
		//}
	}

	void reallocate(uint32_t new_capacity)
	{
		char* new_data = allocate(new_capacity);
		copyRange(_data, _data + _size, new_data);
		delete _data;
		//deleteRange(_data, _data + _size);
		//free(_data);
		_data = new_data;
		_capacity = new_capacity;
	}

public:
	uint64_t GetSize() const { return _size; }

	char* GetData() const { return _data; };

	template<class T>
	void WriteBytesAt(const T* from, uint64_t at, uint32_t num_items = 1)
	{
		uint64_t end_write = at + sizeof(T) * num_items;
		if (_capacity < end_write) reallocate(end_write * 2);
		memcpy(&_data[at], from, sizeof(T) * num_items);
		if (at + sizeof(T) * num_items > _size) _size = end_write;
	}

	template<class T>
	void WriteBytes(const T* from, uint32_t num_items = 1)
	{
		uint64_t end_write = _size + sizeof(T) * num_items;
		if (_capacity < end_write) reallocate(end_write * 2);
		memcpy(&_data[_size], from, sizeof(T) * num_items);
		_size = end_write;
	}
};

class SQUE_InStream
{
	SQUE_Asset* _data;

	char* _readpos;
	uint64_t _readpos_n;


	SQUE_InStream(SQUE_Asset* asset) : _data(asset), _readpos(asset->raw_data), _readpos_n(0) {};
	~SQUE_InStream() { delete _data; _readpos = NULL; }
public:
	uint64_t GetSize() const { return _data->size; };
	uint64_t GetReadPos() const { return _readpos_n; }

	void AttachAsset(SQUE_Asset* asset) { _data = asset; _readpos = _data->raw_data; _readpos_n = 0; }

	template<class T>
	void ReadBytesAt(T* to, uint32_t num_items, uint64_t at)
	{
		SQ_ASSERT((num_items * sizeof(T) + at) < _data->size);
		memcpy(to, &_data->raw_data[at], num_items * sizeof(T));
	}

	template<class T>
	void ReadBytes(T* to, uint32_t num_items)
	{
		memcpy(to, &_data->raw_data[_readpos_n], num_items * sizeof(T));
		_readpos_n += num_items * sizeof(T);
	}
};

#endif