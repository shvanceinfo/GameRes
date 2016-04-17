#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "stdint.h"
#include "vector"
#ifndef _WIN32
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif

#define MESSAGE_MAX_LENGTH	1024 * 4

class MessageBuffer
{
	typedef std::vector<uint8_t>::size_type size_type;
public:
	MessageBuffer() : _rpos(0), _wpos(0)
	{
		_buffer.resize(MESSAGE_MAX_LENGTH);
	}

	MessageBuffer(MessageBuffer const &right) : _rpos(right._rpos), _wpos(right._wpos), _buffer(right._buffer)
	{
	}

	MessageBuffer(MessageBuffer && right) :_rpos(right._rpos), _wpos(right._wpos), _buffer(right.Move())
	{
	}

	void Reset()
	{
		_wpos = 0;
		_rpos = 0;
	}

	void Resize()
	{
		_buffer.resize(MESSAGE_MAX_LENGTH);
	}

	uint8_t *GetBasePointer(){ return _buffer.data(); }
	uint8_t *GetWritePointer(){ return &_buffer[_wpos]; }
	uint8_t *GetReadPointer(){ return &_buffer[_rpos]; }
	size_t GetbufferSize() const { return _buffer.size(); }
	void WriteComplete(size_t bytes){ _wpos += bytes; }
	void ReadComplete(size_t bytes){ _rpos += bytes; }
	size_t GetActiveSize() const { return _wpos - _rpos; }
	size_t GetRemainingSpace() const { return _buffer.size() - _wpos; }


	void Write(void const* data, std::size_t size)
	{
		if ( size)
		{
			memcpy(GetWritePointer(), data, size);
			WriteComplete(size);
		}
	}

	void Normalize()
	{
		if ( _rpos )
		{
			if ( _rpos != _wpos )
			{
				memmove(GetBasePointer(), GetReadPointer(), GetbufferSize());
			}

			_wpos -= _rpos;
			_rpos = 0;
		}
	}

	std::vector<uint8_t> &&Move()
	{
		_wpos = 0;
		_rpos = 0;
		return std::move(_buffer);
	}

	MessageBuffer & operator=(MessageBuffer &right)
	{
		if ( this != &right)
		{
			_wpos = right._wpos;
			_rpos = right._rpos;
			_buffer = right._buffer;
		}

		return *this;
	}

	MessageBuffer &operator =(MessageBuffer &&right)
	{
		if ( this != &right)
		{
			_wpos = right._wpos;
			_rpos = right._rpos;
			_buffer = right.Move();
		}

		return *this;
	}


	///-
	template<typename T> void append(T value)
	{
		//static_assert(std::is_fundamental<T>::value, "append(compound)");
		append((uint8_t*)&value, sizeof(value));
	}

	void append(const uint8_t *src, size_t cnt)
	{
		if ( !cnt )
		{
			return;
		}

		if ( !src )
		{
			return;
		}

		if ( _buffer.size() < _wpos + cnt )
		{
			_buffer.resize(_wpos + cnt);
		}

		std::memcpy(&_buffer[_wpos], src, cnt);
		_wpos += cnt;
	}

	MessageBuffer &operator<<(uint8_t value)
	{
		append<uint8_t>(value);
		return *this;
	}

	MessageBuffer &operator<<(uint16_t value)
	{
		uint16_t _temp = htons(value);
		append<uint16_t>(_temp);
	}

	MessageBuffer &operator<<(int value)
	{
		int _temp = htonl(value);
		append<int>(_temp);
	}

	MessageBuffer &operator<<(uint64_t value)
	{
		uint64_t _temp = htonll(value);
		append<uint64_t>(_temp);
	}

	MessageBuffer &operator<<(int8_t value)
	{
		append<int8_t>(value);
		return *this;
	}

	MessageBuffer &operator<<(int16_t value)
	{
		int16_t _temp = htons(value);
		append<int16_t>(_temp);
		return *this;
	}

// 	MessageBuffer &operator<<(int value)
// 	{
// 		int _temp = htonl(value);
// 		append<int>(_temp);
// 		return *this;
// 	}

	MessageBuffer &operator<<(int64_t value)
	{
		int64_t _temp = htonll(value);
		append<int64_t>(_temp);
		return *this;
	}

	MessageBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}

	MessageBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}

	MessageBuffer &operator<<(const std::string &value)
	{
		if ( size_t len = value.length())
		{
			append((uint8_t const*)value.c_str(), len);
		}
		append((uint8_t)0);
		return *this;
	}

	MessageBuffer &operator=(const char *str)
	{
		if ( size_t len=(str?strlen(str):0))
		{
			append((uint8_t const*)str, len);
		}
		append((uint8_t *)0);
		return *this;
	}

	template<typename T> T read()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	}

	template<typename T> T read(size_t pos)const
	{
		if (pos + sizeof(T) > GetbufferSize())
		{
			T t;
			memset(&t, 0, sizeof(T));
			return t;
		}

		T val = *((T const*)&_buffer[pos]);
		return val;
	}

	void read(uint8_t* dest, size_t len)
	{
		if ( _rpos + len > GetbufferSize())
		{
			return;
		}
		std::memcpy(dest, &_buffer[_rpos], len);
		_rpos += len;
	}

	MessageBuffer &operator>>(uint8_t &value)
	{
		value = read<uint8_t>();
		return *this;
	}

	MessageBuffer &operator>>(uint16_t &value)
	{
		uint16_t _temp = read<uint16_t>();
		value = ntohs(_temp);
		return *this;
	}

	MessageBuffer &operator>>(int &value)
	{
		int _temp = read<int>();
		value = ntohl(_temp);
		return *this;
	}

	MessageBuffer &operator>>(uint64_t &value)
	{
		uint64_t _temp = read<uint64_t>();
		value = ntohll(_temp);
		return *this;
	}

	MessageBuffer &operator>>(int8_t &value)
	{
		value = read<int8_t>();
		return *this;
	}

	MessageBuffer &operator>>(int16_t &value)
	{
		int16_t _temp = read<int16_t>();
		value = ntohs(_temp);
		return *this;
	}

// 	MessageBuffer &operator>>(int &value)
// 	{
// 		int _temp = read<int>();
// 		value = ntohl(_temp);
// 		return *this;
// 	}

	MessageBuffer &operator>>(int64_t &value)
	{
		int64_t _temp = read<int64_t>();
		value = ntohll(_temp);
		return *this;
	}

	MessageBuffer &operator>>(float &value)
	{
		float _temp = read<float>();
		if ( !std::isfinite(_temp ))
		{
			//TODO会出异常
			return *this;
		}
		value = _temp;
		return *this;
	}

	MessageBuffer &operator>>(double &value)
	{
		double _temp = read<double>();
		if ( !std::isfinite(_temp))
		{
			//TODO会出异常
			return *this;
		}
		value = _temp;
		return *this;
	}

	MessageBuffer &operator>>(std::string &value)
	{
		value.clear();
		while (_rpos<GetbufferSize())
		{
			char c = read<char>();
			if ( c== 0)
			{
				break;
			}
			value += c;
		}
		return *this;
	}

private:
	std::vector<uint8_t> _buffer;
	size_type _rpos;
	size_type _wpos;
};

#endif