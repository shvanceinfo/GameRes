#include "commDef.hpp"
#ifdef WIN32
#include <WinSock2.h>
#include <assert.h>
#else
#include <cassert>
#include <netinet/in.h>
#include <sys/types.h>
#endif

TextReadStream::TextReadStream(const char* ptr_, size_t len_)
	: ptr(ptr_), len(len_), cur(ptr_)
{
	cur += TEXT_PACKLEN_LEN;
}

int TextReadStream::IsEmpty() const
{
	return len <= TEXT_PACKLEN_LEN;
}

size_t TextReadStream::GetSize() const
{
	return len;
}

int TextReadStream::Read(char* str, size_t strlen, /* out */ size_t & len)
{
	size_t fieldlen;
	if (!ReadLengthWithoutOffset(fieldlen)) {
		return false;
	}

	// user buffer is not enough
	if (strlen < fieldlen) {
		return false;
	}

	// 偏移到数据的位置
	cur += TEXT_PACKLEN_LEN;

	if (!ReadContent(str, fieldlen)) {
		cur -= TEXT_PACKLEN_LEN;
		return false;	// 正确的包绝对不会出现这种情况
	}

	len = fieldlen;
	return true;
}

int TextReadStream::Read(/* out */ int & i)
{
	char intstr[32];
	size_t len = 0;

	if (!Read(intstr, sizeof(intstr)-1, len)) {
		return false;
	}

	intstr[len] = '\0';
	i = atoi(intstr);
	return true;
}

int TextReadStream::Read(/* out */ short & i)
{
	int i_;
	if (!Read(i_))
		return false;
	i = i_;
	return true;
}

int TextReadStream::Read(/* out */ char & c)
{
	size_t len;
	return Read(&c, 1, len);
}

int TextReadStream::ReadLength(size_t & outlen)
{
	if (!ReadLengthWithoutOffset(outlen)) {
		return false;
	}

	cur += TEXT_PACKLEN_LEN;
	return true;
}

int TextReadStream::ReadLengthWithoutOffset(size_t & outlen)
{
	if (cur + TEXT_PACKLEN_LEN > ptr + len) {
		return false;
	}

	char lenstr[TEXT_PACKLEN_LEN + 1];
	memcpy(lenstr, cur, TEXT_PACKLEN_LEN);
	lenstr[TEXT_PACKLEN_LEN] = '\0';
	outlen = strtol(lenstr, NULL, 16);
	return true;
}

int TextReadStream::ReadContent(char* str, size_t maxlen)
{
	if (cur + maxlen > ptr + len) {
		return false;
	}

	memcpy(str, cur, maxlen);
	cur += maxlen;
	return true;
}

int TextReadStream::IsEnd() const
{
	assert(cur <= ptr + len);
	return cur == ptr + len;
}

const char* TextReadStream::GetData() const
{
	return ptr;
}

size_t TextReadStream::ReadAll(char * szBuffer, size_t iLen) const
{
	size_t iRealLen = min(iLen, len);
	memcpy(szBuffer, ptr, iRealLen);
	return iRealLen;
}

TextWriteStream::TextWriteStream(char* ptr_, size_t len_)
	: ptr(ptr_), len(len_), cur(ptr_)
{
	cur += TEXT_PACKLEN_LEN;
}

int TextWriteStream::Write(const char* str, size_t len)
{
	if (cur + TEXT_PACKLEN_LEN + len > ptr + this->len)
		return false;

	if (!WriteLength(len))
		return false;

	if (!WriteContent(str, len))
	{
		cur -= TEXT_PACKLEN_LEN;
		return false;
	}

	return true;
}

int TextWriteStream::Write(int i)
{
	char intstr[32];
#ifdef WIN32
	_snprintf_s(intstr, sizeof(intstr), "%d", i);
#else
	snprintf(intstr, sizeof(intstr), "%d", i);
#endif
	return Write(intstr, strlen(intstr));
}

int TextWriteStream::Write(short i)
{
	char intstr[32];
#ifdef WIN32
	_snprintf_s(intstr, sizeof(intstr), "%d", i);
#else
	snprintf(intstr, sizeof(intstr), "%d", i);
#endif
	return Write(intstr, strlen(intstr));
}

int TextWriteStream::Write(char c)
{
	return Write(&c, 1);
}

int TextWriteStream::WriteLength(size_t length)
{
	char finalstr[TEXT_PACKLEN_LEN + 1];
	memset(finalstr, '0', TEXT_PACKLEN_LEN);
	finalstr[TEXT_PACKLEN_LEN] = '\0';

	char lengthstr[32] = { 0 };
#ifdef WIN32
	_snprintf_s(lengthstr, sizeof(lengthstr), "%zx", length);
#else
	snprintf(lengthstr, sizeof(lengthstr), "%zx", length);
#endif
	size_t len = strlen(lengthstr);

	if (len > TEXT_PACKLEN_LEN) {
		return false;
	}

	size_t offset = TEXT_PACKLEN_LEN - len;
	memcpy(finalstr + offset, lengthstr, len);

	return WriteContent(finalstr, TEXT_PACKLEN_LEN);
}

int TextWriteStream::WriteContent(const char* str, size_t length)
{
	memcpy(cur, str, length);
	cur += length;
	return true;
}

size_t TextWriteStream::GetSize() const
{
	return cur - ptr;
}

int TextWriteStream::IsValid() const
{
	size_t datalen = GetSize() - TEXT_PACKLEN_LEN;
	return datalen > 0 && datalen <= TEXT_PACKAGE_MAXLEN;
}

void TextWriteStream::Flush()
{
	size_t datalen = GetSize() - TEXT_PACKLEN_LEN;
	char lenstr[TEXT_PACKLEN_LEN + 1];
#ifdef WIN32
	_snprintf_s(lenstr, sizeof(lenstr), "%04zx", datalen);
#else
	snprintf(lenstr, sizeof(lenstr), "%04zx", datalen);
#endif
	memcpy(ptr, lenstr, TEXT_PACKLEN_LEN);
}

void TextWriteStream::Clear()
{
	cur = ptr + TEXT_PACKLEN_LEN;
}

const char* TextWriteStream::GetData() const
{
	return ptr;
}

// Binary stream 

BinaryReadStream::BinaryReadStream(const char* ptr_, size_t len_)
	: ptr(ptr_), len(len_), cur(ptr_)
{
	cur += BINARY_PACKLEN_LEN;
}

int BinaryReadStream::IsEmpty() const
{
	return len <= BINARY_PACKLEN_LEN;
}

size_t BinaryReadStream::GetSize() const
{
	return len;
}

int BinaryReadStream::Read(char* str, size_t strlen, /* out */ size_t& outlen)
{
	size_t fieldlen;
	if (!ReadLengthWithoutOffset(fieldlen)) {
		return false;
	}

	// user buffer is not enough
	if (strlen < fieldlen) {
		return false;
	}

	// 偏移到数据的位置
	//zhangyongjun edit	
	cur += BINARY_PACKLEN_LEN2;

	memcpy(str, cur, fieldlen);
	outlen = fieldlen;

	cur += outlen;

	return true;
}

int BinaryReadStream::Read(string *str, /* out */ size_t& outlen)
{
	size_t fieldlen;
	if (!ReadLengthWithoutOffset(fieldlen)) {
		return false;
	}

	// 偏移到数据的位置
	cur += BINARY_PACKLEN_LEN2;

	str->assign(cur, fieldlen);
	outlen = fieldlen;

	cur += outlen;

	return true;
}

int BinaryReadStream::Read(/* out */ int & i)
{
	const int VALUE_SIZE = sizeof(int);

	if (cur + VALUE_SIZE > ptr + len) {
		return false;
	}
	//GT_INFO("cur = %d, %d, %d, %d, %d, %d, %d, %d", 
	//	cur[0], cur[1], cur[2], cur[3], cur[4], cur[5], cur[6], cur[7]);
	memcpy(&i, cur, VALUE_SIZE);


#if (0x1234&0xFF)==0x12

#else
	i = ntohl(i);
#endif	

	cur += VALUE_SIZE;

	return true;
}

int BinaryReadStream::Read(/* out */ short & i)
{
	const int VALUE_SIZE = sizeof(short);

	if (cur + VALUE_SIZE > ptr + len) {
		return false;
	}

	memcpy(&i, cur, VALUE_SIZE);

#if (0x1234&0xFF)==0x12

#else
	i = ntohs(i);
#endif

	cur += VALUE_SIZE;

	return true;
}

int BinaryReadStream::Read(/* out */ char & c)
{
	const int VALUE_SIZE = sizeof(char);

	if (cur + VALUE_SIZE > ptr + len) {
		return false;
	}

	memcpy(&c, cur, VALUE_SIZE);
	cur += VALUE_SIZE;

	return true;
}

BinaryReadStream&  BinaryReadStream::operator >> (int& i)
{
	const int VALUE_SIZE = sizeof(int);

	if (cur + VALUE_SIZE > ptr + len) {
		return *this;
	}
	//GT_INFO("cur = %d, %d, %d, %d, %d, %d, %d, %d", 
	//	cur[0], cur[1], cur[2], cur[3], cur[4], cur[5], cur[6], cur[7]);
	memcpy(&i, cur, VALUE_SIZE);

#if (0x1234&0xFF)==0x12

#else
	i = ntohl(i);
#endif

	cur += VALUE_SIZE;

	return *this;
}

BinaryReadStream&  BinaryReadStream::operator >> (long long & i)
{
	//	const int VALUE_SIZE = sizeof(long long);
	//
	//	if ( cur + VALUE_SIZE > ptr + len ) {
	//		return *this;
	//	}
	//	//GT_INFO("cur = %d, %d, %d, %d, %d, %d, %d, %d", 
	//	//	cur[0], cur[1], cur[2], cur[3], cur[4], cur[5], cur[6], cur[7]);
	//	memcpy(&i, cur, VALUE_SIZE);
	//
	//#if (0x1234&0xFF)==0x12
	//
	//#else
	//	//i = ntohl(i);
	//#endif
	//
	//	cur += VALUE_SIZE;
	int highValue;
	if (!Read(highValue))
	{
		return *this;
	}
	int lowValue;
	if (!Read(lowValue))
	{
		return *this;
	}
	int uiLowValue = (int)lowValue;
	i = (long long)highValue << 32 | (int)uiLowValue;
	return *this;
}

BinaryReadStream& BinaryReadStream::operator >> (float& f)
{
	const int VALUE_SIZE = sizeof(int);

	if (cur + VALUE_SIZE > ptr + len) {
		return *this;
	}
	//GT_INFO("cur = %d, %d, %d, %d, %d, %d, %d, %d", 
	//	cur[0], cur[1], cur[2], cur[3], cur[4], cur[5], cur[6], cur[7]);

	int iTmp = 0;
	memcpy(&iTmp, cur, VALUE_SIZE);

#if (0x1234&0xFF)==0x12

#else
	iTmp = ntohl(iTmp);
#endif

	memcpy(&f, &iTmp, VALUE_SIZE);
	cur += VALUE_SIZE;

	return *this;
}

BinaryReadStream&  BinaryReadStream::operator >> (short& i)
{
	const int VALUE_SIZE = sizeof(short);

	if (cur + VALUE_SIZE > ptr + len) {
		return *this;
	}

	memcpy(&i, cur, VALUE_SIZE);

#if (0x1234&0xFF)==0x12

#else
	i = ntohs(i);
#endif

	cur += VALUE_SIZE;
	return *this;
}

BinaryReadStream&  BinaryReadStream::operator >> (char& c)
{
	const int VALUE_SIZE = sizeof(char);

	if (cur + VALUE_SIZE > ptr + len) {
		return *this;
	}

	memcpy(&c, cur, VALUE_SIZE);
	cur += VALUE_SIZE;
	return *this;
}

BinaryReadStream&  BinaryReadStream::operator >> (string& str)
{
	size_t fieldlen;
	if (!ReadLengthWithoutOffset(fieldlen)) {
		return *this;
	}

	// 偏移到数据的位置
	cur += BINARY_PACKLEN_LEN2;

	str.assign(cur, fieldlen);

	cur += fieldlen;

	return *this;
}

int BinaryReadStream::ReadLength(size_t & outlen)
{
	if (!ReadLengthWithoutOffset(outlen)) {
		return false;
	}

	cur += BINARY_PACKLEN_LEN;
	return true;
}

int BinaryReadStream::ReadLengthWithoutOffset(size_t & outlen)
{
	if (cur + BINARY_PACKLEN_LEN2 > ptr + len) {
		return false;
	}

	//int tmp;
	//memcpy(&tmp, cur, sizeof(tmp));
	//outlen = ntohl(tmp);
	unsigned short tmp;
	memcpy(&tmp, cur, sizeof(tmp));


#if (0x1234&0xFF)==0x12
	outlen = tmp;
#else
	outlen = ntohs(tmp);
#endif

	return true;
}

int BinaryReadStream::IsEnd() const
{
	assert(cur <= ptr + len);
	return cur == ptr + len;
}

const char* BinaryReadStream::GetData() const
{
	return ptr;
}

size_t BinaryReadStream::ReadAll(char * szBuffer, size_t iLen) const
{
	size_t iRealLen = min(iLen, len);
	memcpy(szBuffer, ptr, iRealLen);
	return iRealLen;
}

BinaryWriteStream::BinaryWriteStream()
//TODO
:ptr(new char[10]),
len(10),
cur(ptr)
{
	cur += BINARY_PACKLEN_LEN;
}

BinaryWriteStream::~BinaryWriteStream()
{
	//TODO
	//ProtocolBuffer::Instance()->ReleasePBuffer(ptr);
}

int BinaryWriteStream::Write(const char* str, size_t length)
{
	//zhangyongjun edit
	if ((cur + length + BINARY_PACKLEN_LEN2) > (ptr + len)) {
		return false;
	}
	if (!WriteLength(length)) {
		cur -= BINARY_PACKLEN_LEN2;
		return false;
	}
	memcpy(cur, str, length);
	cur += length;
	return true;
}

//add by zhang 2010.01.05
int BinaryWriteStream::WriteNoLength(const char* str, size_t length)
{
	if (cur + length + BINARY_PACKLEN_LEN > ptr + len) {
		return false;
	}
	memcpy(cur, str, length);
	cur += length;

	return true;
}
//end by zhang

int BinaryWriteStream::Write(float f)
{
	if (cur + sizeof(f) > ptr + len) {
		return false;
	}

	int iTmp = 0;
	memcpy(&iTmp, &f, sizeof(float));
#if (0x1234&0xFF)==0x12
	iTmp = i;
#else
	iTmp = htonl(iTmp);
#endif	

	memcpy(cur, &iTmp, sizeof(iTmp));
	cur += sizeof(iTmp);

	return true;
}

int BinaryWriteStream::Write(int i)
{
	if (cur + sizeof(i) > ptr + len) {
		return false;
	}

	int iTmp = 0;
#if (0x1234&0xFF)==0x12
	iTmp = i;
#else
	iTmp = htonl(i);
#endif	

	memcpy(cur, &iTmp, sizeof(iTmp));
	cur += sizeof(iTmp);

	return true;
}

int BinaryWriteStream::Write(short i)
{
	if (cur + sizeof(i) > ptr + len) {
		return false;
	}
	short iTmp = 0;
#if (0x1234&0xFF)==0x12
	iTmp = i;
#else
	iTmp = htons(i);
#endif	

	memcpy(cur, &iTmp, sizeof(iTmp));
	cur += sizeof(iTmp);

	return true;
}

int BinaryWriteStream::Write(char c)
{
	if (cur + sizeof(c) > ptr + len) {
		return false;
	}

	*cur = c;
	++cur;
	return true;
}

int BinaryWriteStream::Write(const unsigned char c)
{
	if (cur + sizeof(c) > ptr + len) {
		return false;
	}

	*cur = c;
	++cur;
	return true;
}

int BinaryWriteStream::Write(const std::string& str)
{
	short length = str.size();
	if ((cur + length + BINARY_PACKLEN_LEN2) > (ptr + len)) {
		return false;
	}
	if (!WriteLength(length)) {
		cur -= BINARY_PACKLEN_LEN2;
		return false;
	}
	memcpy(cur, str.c_str(), length);
	cur += length;
	return true;
}
//////////////////////////////
BinaryWriteStream& BinaryWriteStream::operator << (int i)
{
	if (cur + sizeof(i) > ptr + len) {
		//GT_ERROR("msg buffer overflow");
		return *this;
	}

	int iTmp = 0;
#if (0x1234&0xFF)==0x12
	iTmp = i;
#else
	iTmp = htonl(i);
#endif	

	memcpy(cur, &iTmp, sizeof(iTmp));
	cur += sizeof(iTmp);
	Flush();
	return *this;
}

BinaryWriteStream& BinaryWriteStream::operator << (long long i)
{
	//	if (cur + sizeof(i) > ptr + len) {
	//		GT_ERROR("msg buffer overflow");
	//return *this;
	//	}
	//
	//	long long iTmp = 0;
	//#if (0x1234&0xFF)==0x12
	//	iTmp = i;
	//#else
	//	//iTmp = htonl(i);
	//	iTmp = i;
	//#endif	
	//
	//	memcpy(cur, &iTmp, sizeof(iTmp));
	//	cur += sizeof(iTmp);
	//	Flush();
	Write((int)(i >> 32));
	Write((int)i);
	Flush();
	return *this;
}

BinaryWriteStream& BinaryWriteStream::operator << (float f)
{
	if (cur + sizeof(f) > ptr + len) {
		//GT_ERROR("msg buffer overflow");
		return *this;
	}

	int iTmp = 0;
	memcpy(&iTmp, &f, sizeof(float));
#if (0x1234&0xFF)==0x12
	iTmp = i;
#else
	iTmp = htonl(iTmp);
#endif	

	memcpy(cur, &iTmp, sizeof(iTmp));
	cur += sizeof(iTmp);
	Flush();
	return *this;
}

BinaryWriteStream& BinaryWriteStream::operator << (short i)
{
	if (cur + sizeof(i) > ptr + len) {
		//GT_ERROR("msg buffer overflow");
		return *this;
	}

	short iTmp = 0;
#if (0x1234&0xFF)==0x12
	iTmp = i;
#else
	iTmp = htons(i);
#endif

	memcpy(cur, &iTmp, sizeof(iTmp));
	cur += sizeof(iTmp);
	Flush();
	return *this;
}

BinaryWriteStream& BinaryWriteStream::operator << (char c)
{
	if (cur + sizeof(c) > ptr + len) {
		//return false;
		//GT_ERROR("msg buffer overflow");
		return *this;
	}

	*cur = c;
	++cur;
	Flush();
	return *this;
}

BinaryWriteStream& BinaryWriteStream::operator << (const unsigned char c)
{
	if (cur + sizeof(c) > ptr + len) {
		//GT_ERROR("msg buffer overflow");
		return *this;
	}

	*cur = c;
	++cur;
	Flush();
	return *this;
}

BinaryWriteStream& BinaryWriteStream::operator << (const std::string& str)
{
	short length = str.size();
	if ((cur + length + BINARY_PACKLEN_LEN2) > (ptr + len)) {
		//GT_ERROR("msg buffer overflow");
		return *this;
	}
	if (!WriteLength(length)) {
		cur -= BINARY_PACKLEN_LEN2;
		//GT_ERROR("WriteLength error");
		return *this;
	}
	memcpy(cur, str.c_str(), length);
	cur += length;
	Flush();
	return *this;
}




/////////////////////////////


int BinaryWriteStream::WriteLength(size_t length)
{
	//zhangyongjun edit
	//int ulen = length;
	//ulen = htonl(ulen);
	//memcpy(cur, &ulen, sizeof(ulen));
	//cur += sizeof(ulen);

	unsigned short ulen = length;
#if (0x1234&0xFF)==0x12
	//ulen = i;
#else
	ulen = htons(ulen);
#endif

	memcpy(cur, &ulen, sizeof(ulen));
	cur += sizeof(ulen);
	return true;
}

// add by wangpf 2010/1/5
int BinaryWriteStream::Seek(size_t length)
{
	if ((cur + length > ptr + len) || (cur + length < ptr + BINARY_PACKLEN_LEN)) {
		return false;
	}

	cur += length;
	return true;
}
// end

size_t BinaryWriteStream::GetSize() const
{
	return cur - ptr;
}

int BinaryWriteStream::IsValid() const
{
	size_t datalen = GetSize();
	return datalen > BINARY_PACKLEN_LEN && datalen <= BINARY_PACKAGE_MAXLEN;
}

void BinaryWriteStream::Flush()
{
	int datalen = GetSize();

#if (0x1234&0xFF)==0x12
	//ulen = i;
#else
	datalen = htonl(datalen);
#endif

	int* ps = reinterpret_cast<int*>(ptr);
	*ps = datalen;
	//unsigned short datalen = GetSize();
	//datalen = htons(datalen);
	//unsigned short* ps = reinterpret_cast<unsigned short*>(ptr);
	//*ps = datalen;
}

void BinaryWriteStream::Clear()
{
	cur = ptr + BINARY_PACKLEN_LEN;
}

const char* BinaryWriteStream::GetData() const
{
	return ptr;
}

// version 2


TextReadStream2::TextReadStream2(const char* ptr_, size_t len_)
	: ptr(ptr_), len(len_), cur(ptr_)
{
	cur += TEXT_PACKLEN_LEN_2;
}

int TextReadStream2::IsEmpty() const
{
	return len <= TEXT_PACKLEN_LEN_2;
}

size_t TextReadStream2::GetSize() const
{
	return len;
}

int TextReadStream2::Read(char* str, size_t strlen, /* out */ size_t & len)
{
	size_t fieldlen;
	if (!ReadLengthWithoutOffset(fieldlen)) {
		return false;
	}

	// user buffer is not enough
	if (strlen < fieldlen) {
		return false;
	}

	// 偏移到数据的位置
	cur += TEXT_PACKLEN_LEN_2;

	if (!ReadContent(str, fieldlen)) {
		return false;	// 正确的包绝对不会出现这种情况
	}

	len = fieldlen;
	return true;
}

int TextReadStream2::Read(/* out */ int & i)
{
	char intstr[32];
	size_t len = 0;

	if (!Read(intstr, sizeof(intstr)-1, len)) {
		return false;
	}

	intstr[len] = '\0';
	i = atoi(intstr);
	return true;
}

int TextReadStream2::Read(/* out */ short & i)
{
	char intstr[32];
	size_t len = 0;

	if (!Read(intstr, sizeof(intstr)-1, len)) {
		return false;
	}

	intstr[len] = '\0';
	i = atoi(intstr);
	return true;
}

int TextReadStream2::Read(/* out */ char & c)
{
	size_t len;
	return Read(&c, 1, len);
}

int TextReadStream2::ReadLength(size_t & outlen)
{
	if (!ReadLengthWithoutOffset(outlen)) {
		return false;
	}

	cur += TEXT_PACKLEN_LEN_2;
	return true;
}

int TextReadStream2::ReadLengthWithoutOffset(size_t & outlen)
{
	if (cur + TEXT_PACKLEN_LEN_2 > ptr + len) {
		return false;
	}

	char lenstr[TEXT_PACKLEN_LEN_2 + 1];
	memcpy(lenstr, cur, TEXT_PACKLEN_LEN_2);
	lenstr[TEXT_PACKLEN_LEN_2] = '\0';
	outlen = strtol(lenstr, NULL, 16);
	return true;
}

int TextReadStream2::ReadContent(char* str, size_t maxlen)
{
	if (cur + maxlen > ptr + len) {
		return false;
	}

	memcpy(str, cur, maxlen);
	cur += maxlen;
	return true;
}

int TextReadStream2::IsEnd() const
{
	assert(cur <= ptr + len);
	return cur == ptr + len;
}

const char* TextReadStream2::GetData() const
{
	return ptr;
}

size_t TextReadStream2::ReadAll(char * szBuffer, size_t iLen) const
{
	size_t iRealLen = min(iLen, len);
	memcpy(szBuffer, ptr, iRealLen);
	return iRealLen;
}



TextWriteStream2::TextWriteStream2(char* ptr_, size_t len_)
	: ptr(ptr_), len(len_), cur(ptr_)
{
	cur += TEXT_PACKLEN_LEN_2;
}

int TextWriteStream2::Write(const char* str, size_t len)
{
	if (cur + TEXT_PACKLEN_LEN_2 + len > ptr + this->len)
	{
		//GT_INFO("Not enough space\n");
		return false;
	}

	if (!WriteLength(len))
	{
		//GT_INFO("Write string length failed\n");
		return false;
	}

	if (!WriteContent(str, len))
	{
		//GT_INFO("Write string content failed\n");
		return false;
	}

	return true;
}

int TextWriteStream2::Write(int i)
{
	char intstr[32];
#ifdef WIN32
	_snprintf_s(intstr, sizeof(intstr), "%d", i);
#else
	snprintf(intstr, sizeof(intstr), "%d", i);
#endif
	return Write(intstr, strlen(intstr));
}

int TextWriteStream2::Write(short i)
{
	char intstr[32];
#ifdef WIN32
	_snprintf_s(intstr, sizeof(intstr), "%d", i);
#else
	snprintf(intstr, sizeof(intstr), "%d", i);
#endif
	return Write(intstr, strlen(intstr));
}

int TextWriteStream2::Write(char c)
{
	return Write(&c, 1);
}

int TextWriteStream2::WriteLength(size_t length)
{
	char finalstr[TEXT_PACKLEN_LEN_2 + 1];
	memset(finalstr, '0', TEXT_PACKLEN_LEN_2);
	finalstr[TEXT_PACKLEN_LEN_2] = '\0';

	char lengthstr[32] = { 0 };
#ifdef WIN32
	_snprintf_s(lengthstr, sizeof(lengthstr), "%06zx", length);
#else
	snprintf(lengthstr, sizeof(lengthstr), "%06zx", length);
#endif
	size_t len = strlen(lengthstr);

	if (len > TEXT_PACKLEN_LEN_2) {
		return false;
	}

	size_t offset = TEXT_PACKLEN_LEN_2 - len;
	memcpy(finalstr + offset, lengthstr, len);

	return WriteContent(finalstr, TEXT_PACKLEN_LEN_2);
}

int TextWriteStream2::WriteContent(const char* str, size_t length)
{
	memcpy(cur, str, length);
	cur += length;
	return true;
}

size_t TextWriteStream2::GetSize() const
{
	return cur - ptr;
}

int TextWriteStream2::IsValid() const
{
	size_t datalen = GetSize() - TEXT_PACKLEN_LEN_2;
	return datalen > 0 && datalen <= TEXT_PACKAGE_MAXLEN_2;
}

void TextWriteStream2::Flush()
{
	size_t datalen = GetSize() - TEXT_PACKLEN_LEN_2;
	char lenstr[TEXT_PACKLEN_LEN_2 + 1];
	//snprintf(lenstr, sizeof(lenstr), "%06zx", datalen);
#ifdef WIN32
	_snprintf_s(lenstr, sizeof(lenstr), "%06zx", datalen + TEXT_PACKLEN_LEN_2);
#else
	snprintf(lenstr, sizeof(lenstr), "%06zx", datalen + TEXT_PACKLEN_LEN_2);
#endif

	memcpy(ptr, lenstr, TEXT_PACKLEN_LEN_2);
}

void TextWriteStream2::Clear()
{
	cur = ptr + TEXT_PACKLEN_LEN_2;
}

const char* TextWriteStream2::GetData() const
{
	return ptr;
}