#pragma once
#include <vector>
#include <list>
#include <string>
using namespace std;

class StringManager
{
private:
	string **StringPool;
	unsigned int PoolSize;

	inline bool EffectiveHandle(unsigned int Handle);

public:
	StringManager();
	~StringManager();

	unsigned int CreateString();
	unsigned int ToInt(unsigned int Handle);
	void PutString(unsigned int Handle, const unsigned char *Str);
	unsigned char *GetString(unsigned int Handle);
	string *QuoteString(unsigned int Handle);
	void FreeString(unsigned int Handle);
	unsigned char *StringLeft(unsigned int Handle, unsigned int Size);
	unsigned char *StringRight(unsigned int Handle, unsigned int Size);
	unsigned char *StringMid(unsigned int Handle, unsigned int Offset, unsigned int Size);
	unsigned int StringFind(unsigned int MasterHandle, unsigned int SubHandle, unsigned int Offset);
	bool StringCompare(unsigned int MasterHandle, unsigned int SlaveHandle);
	unsigned int StringLength(unsigned int Handle);
};
