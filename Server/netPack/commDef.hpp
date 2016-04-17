#ifndef __COMMDEF_H__
#define __COMMDEF_H__

#include "iostream"
#include "vector"
#include "string"

#include <stdio.h>
#include <algorithm>

using namespace std;

enum {
	TEXT_STREAM_PROTOCOL = 0x01,
	BINARY_STREAM_PROTOCOL = 0x02,
	TEXT_STREAM_PROTOCOL_2 = 0x03,
};

enum {
	COMPOSITE_FLAG = 0x10000000,
	COMPOSITE_TEXT_STREAM_PROTOCOL = TEXT_STREAM_PROTOCOL | COMPOSITE_FLAG,
	COMPOSITE_BINARY_STREAM_PROTOCOL = BINARY_STREAM_PROTOCOL | COMPOSITE_FLAG,
	COMPOSITE_TEXT_STREAM_PROTOCOL_2 = TEXT_STREAM_PROTOCOL_2 | COMPOSITE_FLAG,
};

struct ReadStream
{
	virtual ~ReadStream(){}

	virtual size_t GetSize() const = 0;

	virtual int IsEmpty() const = 0;

	virtual int Read(char* str, size_t strlen, /* out */ size_t & len) = 0;

	virtual int Read(/* out */ int & i) = 0;

	virtual int Read(/* out */ short & i) = 0;

	virtual int Read(/* out */ char & c) = 0;

	virtual size_t ReadAll(char * szBuffer, size_t iLen) const = 0;

	virtual int IsEnd() const = 0;
};

struct WriteStream
{
	virtual ~WriteStream(){}

	virtual int Write(const char* str, size_t len) = 0;

	virtual int Write(int i) = 0;

	virtual int Write(short i) = 0;

	virtual int Write(char c) = 0;

	virtual void Clear() = 0;

	virtual void Flush() = 0;

	virtual char* GetCurrent() const = 0;
};

enum {
	TEXT_PACKLEN_LEN = 4,
	TEXT_PACKAGE_MAXLEN = 0xffff,
	BINARY_PACKLEN_LEN = 4,
	BINARY_PACKET_CMD_LEN = 4,
	BINARY_PACKLEN_CHECK_LEN = 1,
	BINARY_PACKAGE_MAXLEN = 0xffff,
	BINARY_PACKLEN_LEN2 = 2,
	TEXT_PACKLEN_LEN_2 = 6,
	TEXT_PACKAGE_MAXLEN_2 = 0xffffff,
};

struct ReadStreamImpl : public ReadStream
{
	virtual const char* GetData() const = 0;

	virtual size_t GetSize() const = 0;
};

struct WriteStreamImpl : public WriteStream
{
	virtual const char* GetData() const = 0;

	virtual size_t GetSize() const = 0;

	virtual int IsValid() const = 0;
};


class TextReadStream : public ReadStreamImpl
{
private:
	const char* const ptr;
	const size_t len;
	const char* cur;

	TextReadStream(const TextReadStream&);
	TextReadStream& operator=(const TextReadStream&);

public:
	TextReadStream(const char* ptr, size_t len);

	virtual size_t GetSize() const;

	virtual int IsEmpty() const;

	virtual int Read(char* str, size_t strlen, /* out */ size_t & len);

	virtual int Read(/* out */ int & i);

	virtual int Read(/* out */ short & i);

	virtual int Read(/* out */ char & c);

	virtual const char* GetData() const;

	virtual size_t ReadAll(char * szBuffer, size_t iLen) const;

	virtual int IsEnd() const;

public:

	int ReadLength(size_t & len);

	int ReadContent(char* buf, size_t buflen);

	int ReadLengthWithoutOffset(size_t & len);
};


class TextWriteStream : public WriteStreamImpl
{
private:
	char* const ptr;
	const size_t len;
	char*  cur;

	TextWriteStream(const TextWriteStream&);
	TextWriteStream& operator=(const TextWriteStream&);

public:
	TextWriteStream(char* ptr, size_t len);

	virtual int Write(const char* str, size_t len);

	virtual int Write(int i);

	virtual int Write(short i);

	virtual int Write(char c);

	virtual size_t GetSize() const;

	virtual int IsValid() const;

	virtual void Flush();

	virtual void Clear();

	virtual const char* GetData() const;

	virtual char* GetCurrent() const
	{
		return cur;
	}

public:
	int WriteLength(size_t len);

	int WriteContent(const char* buf, size_t buflen);
};


class BinaryReadStream : public ReadStreamImpl
{
private:
	const char* const ptr;
	const size_t len;
	const char* cur;



public:
	//BinaryReadStream(const BinaryReadStream&);
	BinaryReadStream& operator=(const BinaryReadStream&);
	BinaryReadStream(const char* ptr, size_t len);

	virtual size_t GetSize() const;

	virtual int IsEmpty() const;

	virtual int Read(string *str, /* out */ size_t& outlen);

	virtual int Read(char* str, size_t strlen, /* out */ size_t& len);

	virtual int Read(/* out */ int & i);

	virtual int Read(/* out */ short & i);

	virtual int Read(/* out */ char & c);

	BinaryReadStream& operator >> (int& i);

	BinaryReadStream& operator >> (long long & i);

	BinaryReadStream& operator >> (float& f);

	BinaryReadStream& operator >> (short& i);

	BinaryReadStream& operator >> (char& c);

	BinaryReadStream& operator >> (string& str);

	template<typename T>
	BinaryReadStream& operator>> (std::vector<T>& val)
	{
		short vallen;
		Read(vallen);
		val.clear();
		if (vallen <0 || vallen > 1000)
		{
			return *this;
		}

		T temp;
		for (size_t i = 0; i<(size_t)vallen; ++i)
		{
			*this >> temp;
			val.push_back(temp);
		}
		return *this;
	}

	template<typename T>
	BinaryReadStream& operator >> (T& t)
	{
		t.UnPack(*this);
		return *this;
	}

	virtual const char* GetData() const;

	virtual size_t ReadAll(char * szBuffer, size_t iLen) const;

	virtual int IsEnd() const;

public:

	int ReadLength(size_t & len);

	int ReadLengthWithoutOffset(size_t & outlen);
};

class BinaryWriteStream : public WriteStreamImpl
{
private:
	char* const ptr;
	const size_t len;
	char*  cur;

public:
	BinaryWriteStream();
	~BinaryWriteStream();
	BinaryWriteStream& operator=(const BinaryWriteStream&);

	virtual int Write(const char* str, size_t len);

	virtual int Write(int i);

	virtual int Write(float f);

	virtual int Write(short i);

	virtual int Write(char c);

	virtual int Write(const std::string& str);

	int Write(const unsigned char c);

	BinaryWriteStream& operator << (long long i);

	BinaryWriteStream& operator << (float f);

	BinaryWriteStream& operator << (int i);

	BinaryWriteStream& operator << (short i);

	BinaryWriteStream& operator << (char c);

	BinaryWriteStream& operator << (const unsigned char c);

	BinaryWriteStream& operator << (const std::string& str);

	template<typename T>
	BinaryWriteStream& operator<< (const std::vector<T>& val)
	{
		short vallen = val.size();
		int result = false;
		result = Write(vallen);
		/*if(!result)
		{
		return result;
		}*/
		typename std::vector<T>::const_iterator itend = val.end();
		for (typename std::vector<T>::const_iterator it = val.begin(); it != itend; ++it)
		{
			//(*it).Sn(*this);
			*this << (*it);
		}
		Flush();
		return *this;
	}

	template<typename T>
	BinaryWriteStream& operator<<(T& t)
	{
		return t.Pack(*this);
	}

	virtual size_t GetSize() const;

	virtual int IsValid() const;

	virtual void Flush();

	virtual void Clear();

	virtual const char* GetData() const;

	virtual char* GetCurrent() const
	{
		return cur;
	}

public:
	int WriteLength(size_t len);
	int WriteNoLength(const char* str, size_t length);

public:
	int Seek(size_t length);
};

class TextReadStream2 : public ReadStreamImpl
{
private:
	const char* const ptr;
	const size_t      len;
	const char* cur;

	TextReadStream2(const TextReadStream2&);
	TextReadStream2& operator=(const TextReadStream2&);

public:
	TextReadStream2(const char* ptr, size_t len);

	virtual size_t GetSize() const;

	virtual int IsEmpty() const;

	virtual int Read(char* str, size_t strlen, /* out */ size_t & len);

	virtual int Read(/* out */ int & i);

	virtual int Read(/* out */ short & i);

	virtual int Read(/* out */ char & c);

	virtual const char* GetData() const;

	virtual size_t ReadAll(char * szBuffer, size_t iLen) const;

	virtual int IsEnd() const;

public:

	int ReadLength(size_t & len);

	int ReadContent(char* buf, size_t buflen);

	int ReadLengthWithoutOffset(size_t & len);
};


class TextWriteStream2 : public WriteStreamImpl
{
private:
	char* const ptr;
	const size_t len;
	char*  cur;

	TextWriteStream2(const TextWriteStream2&);
	TextWriteStream2& operator=(const TextWriteStream2&);

public:
	TextWriteStream2(char* ptr, size_t len);

	virtual int Write(const char* str, size_t len);

	virtual int Write(int i);

	virtual int Write(short i);

	virtual int Write(char c);

	virtual size_t GetSize() const;

	virtual int IsValid() const;

	virtual void Flush();

	virtual void Clear();

	virtual const char* GetData() const;

	virtual char* GetCurrent() const
	{
		return cur;
	}

public:
	int WriteLength(size_t len);

	int WriteContent(const char* buf, size_t buflen);
};

template<typename T>
int WriteVector(BinaryWriteStream& usn, std::vector<T> val)
{
	short vallen = val.size();
	int result = false;
	result = usn.Write(vallen);
	if (!result)
	{
		return result;
	}
	typename std::vector<T>::const_iterator itend = val.end();
	for (typename std::vector<T>::const_iterator it = val.begin(); it != itend; ++it)
	{
		//result = 
		(*it).Pack(usn);
		/*if(!result)
		{
		return result;
		}*/
	}
	return result;
}
template<typename T>
int ReadVector(BinaryReadStream& sn, std::vector<T>& val)
{
	short  vallen;
	sn.Read(vallen);
	val.clear();
	T temp;
	for (short i = 0; i<vallen; ++i)
	{
		temp.UnPack(sn);
		val.push_back(temp);
	}
	return true;
}



#define DEFINE_STRUCT_1(STRUCTNAME, T1, V1) \
struct STRUCTNAME \
{ \
	T1 V1; \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1; }\
}; \

#define DEFINE_STRUCT_2(STRUCTNAME, T1, V1, T2, V2) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2; } \
}; \

#define DEFINE_STRUCT_3(STRUCTNAME, T1, V1, T2, V2, T3, V3) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3; } \
};

#define DEFINE_STRUCT_4(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4; } \
};

#define DEFINE_STRUCT_5(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5; } \
};

#define DEFINE_STRUCT_6(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6; } \
};

#define DEFINE_STRUCT_7(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7; } \
};

#define DEFINE_STRUCT_8(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8; } \
};

#define DEFINE_STRUCT_9(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9; } \
};

#define DEFINE_STRUCT_10(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10; } \
};

#define DEFINE_STRUCT_11(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11; } \
};

#define DEFINE_STRUCT_12(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12; } \
};

#define DEFINE_STRUCT_13(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13; } \
};

#define DEFINE_STRUCT_14(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14; } \
};

#define DEFINE_STRUCT_15(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15; } \
};

#define DEFINE_STRUCT_16(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16; } \
};

#define DEFINE_STRUCT_17(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17; } \
};

#define DEFINE_STRUCT_19(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19; } \
};

#define DEFINE_STRUCT_20(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19, T20, V20) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20; } \
};

#define DEFINE_STRUCT_29(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19, T20, V20, T21, V21, T22, V22, T23, V23, T24, V24, T25, V25, T26, V26,T27, V27, T28, V28, T29, V29) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	T21 V21; \
	T22 V22; \
	T23 V23; \
	T24 V24; \
	T25 V25; \
	T26 V26; \
	T27 V27; \
	T28 V28; \
	T29 V29; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20 << V21 << V22 << V23 << V24 << V25 << V26 << V27 << V28 << V29; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20 >> V21 >> V22 >> V23 >> V24 >> V25 >> V26 >> V27 >> V28 >> V29; } \
};

#define DEFINE_STRUCT_30(STRUCTNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19, T20, V20, T21, V21, T22, V22, T23, V23, T24, V24, T25, V25, T26, V26,T27, V27, T28, V28, T29, V29, T30, V30) \
struct STRUCTNAME \
{ \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	T21 V21; \
	T22 V22; \
	T23 V23; \
	T24 V24; \
	T25 V25; \
	T26 V26; \
	T27 V27; \
	T28 V28; \
	T29 V29; \
	T30 V30; \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20 << V21 << V22 << V23 << V24 << V25 << V26 << V27 << V28 << V29 << V30; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20 >> V21 >> V22 >> V23 >> V24 >> V25 >> V26 >> V27 >> V28 >> V29 >> V30; } \
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define DEFINE_MSG_0(STRUCTNAME, MSGIDNAME) \
struct STRUCTNAME \
{ \
	int msgID; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn; } \
}; \

#define DEFINE_MSG_1(STRUCTNAME, MSGIDNAME, T1, V1) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1; } \
}; \

#define DEFINE_MSG_2(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2) \
struct STRUCTNAME  \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2; } \
}; \

#define DEFINE_MSG_3(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3; } \
}; \

#define DEFINE_MSG_4(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4; } \
}; \

#define DEFINE_MSG_5(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5) \
struct STRUCTNAME  \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5; } \
}; \

#define DEFINE_MSG_6(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6; } \
}; \

#define DEFINE_MSG_7(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7; } \
}; \

#define DEFINE_MSG_8(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8) \
struct STRUCTNAME  \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8; } \
}; \

#define DEFINE_MSG_9(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9; } \
}; \

#define DEFINE_MSG_10(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10; } \
}; \

#define DEFINE_MSG_11(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11; } \
}; \

#define DEFINE_MSG_12(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12; } \
}; \

#define DEFINE_MSG_13(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13; } \
}; \

#define DEFINE_MSG_14(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14; } \
}; \

#define DEFINE_MSG_15(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15; } \
}; \


#define DEFINE_MSG_16(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16; } \
}; \


#define DEFINE_MSG_17(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17; } \
}; \

#define DEFINE_MSG_18(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18; } \
}; \

#define DEFINE_MSG_19(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19; } \
}; \

#define DEFINE_MSG_20(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19,T20, V20) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20; } \
}; \

#define DEFINE_MSG_21(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19, T20, V20, T21, V21) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	T21 V21; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20 << V21; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20 >> V21; } \
}; \

#define DEFINE_MSG_22(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19, T20, V20, T21, V21, T22, V22) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	T21 V21; \
	T22 V22; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20 << V21 << V22; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20 >> V21 >> V22; } \
}; \

#define DEFINE_MSG_25(STRUCTNAME, MSGIDNAME, T1, V1, T2, V2, T3, V3, T4, V4, T5, V5, T6, V6, T7, V7, T8, V8, T9, V9, T10, V10, T11, V11, T12, V12, T13, V13, T14, V14, T15, V15, T16, V16, T17, V17, T18, V18, T19, V19, T20, V20, T21, V21, T22, V22, T23, V23, T24, V24, T25, V25) \
struct STRUCTNAME \
{ \
	int msgID; \
	T1 V1; \
	T2 V2; \
	T3 V3; \
	T4 V4; \
	T5 V5; \
	T6 V6; \
	T7 V7; \
	T8 V8; \
	T9 V9; \
	T10 V10; \
	T11 V11; \
	T12 V12; \
	T13 V13; \
	T14 V14; \
	T15 V15; \
	T16 V16; \
	T17 V17; \
	T18 V18; \
	T19 V19; \
	T20 V20; \
	T21 V21; \
	T22 V22; \
	T23 V23; \
	T24 V24; \
	T25 V25; \
	STRUCTNAME() { msgID = MSGIDNAME; } \
	BinaryWriteStream& Pack(BinaryWriteStream& sn) const { return sn << msgID << V1 << V2 << V3 << V4 << V5 << V6 << V7 << V8 << V9 << V10 << V11 << V12 << V13 << V14 << V15 << V16 << V17 << V18 << V19 << V20 << V21 << V22 << V23 << V24 << V25; } \
	BinaryReadStream& UnPack(BinaryReadStream& usn) { return usn >> V1 >> V2 >> V3 >> V4 >> V5 >> V6 >> V7 >> V8 >> V9 >> V10 >> V11 >> V12 >> V13 >> V14 >> V15 >> V16 >> V17 >> V18 >> V19 >> V20 >> V21 >> V22 >> V23 >> V24 >> V25; } \
}; \

#endif