#include <cstdlib>
#include <string.h>
#include "anvm.h"
#include "String.h"

#define INDEX (-(Handle + 1))

StringManager::StringManager()
{
	this->PoolSize = 0;
	this->StringPool = (string**)malloc(sizeof(string**) * this->PoolSize);
}

StringManager::~StringManager()
{
	for (unsigned int i = 0; i < this->PoolSize; i++)
	{
		if (this->StringPool[i] != NULL)
		{
			delete this->StringPool[i];
		}
	}
	free(this->StringPool);
}

inline bool StringManager::EffectiveHandle(unsigned int Handle)
{
	return (INDEX < this->PoolSize) && this->StringPool[INDEX] != NULL;
}

unsigned int StringManager::CreateString()
{
	unsigned int i = 0;
	while (i < this->PoolSize)
	{
		if (this->StringPool[i] == NULL)
		{
			this->StringPool[i] = new string();
			return -i - 1;
		}
		i++;
	}
	this->PoolSize = i + 1;
	this->StringPool = (string**)realloc(this->StringPool, sizeof(string**) * this->PoolSize);
	this->StringPool[i] = new string();
	return -i - 1;
}

unsigned int StringManager::ToInt(unsigned int Handle)
{
	return atoi((char *)GetString(Handle));
	if (EffectiveHandle((int)Handle) == true)
	{
		return atoi((char *)GetString(Handle));
	} else {
		return Handle;
	}
}

void StringManager::PutString(unsigned int Handle, const unsigned char *Str)
{
	if (EffectiveHandle((int)Handle) == true)
	{
		this->StringPool[INDEX]->assign((char *)Str);
	} else {

	}
}

unsigned char *StringManager::GetString(unsigned int Handle)
{
	if (EffectiveHandle((int)Handle) == true)
	{
		return (unsigned char *)(this->StringPool[INDEX]->c_str());
	} else {
		return (unsigned char *)vmem + Handle;
	}
}

string *StringManager::QuoteString(unsigned int Handle)
{
	if (EffectiveHandle((int)Handle) == true)
	{
		return this->StringPool[INDEX];
	} else {
		return NULL;
	}
}

void StringManager::FreeString(unsigned int Handle)
{
	if (EffectiveHandle((int)Handle) == true)
	{
		delete this->StringPool[INDEX];
		this->StringPool[INDEX] = NULL;
	} else {

	}
}

unsigned char *StringManager::StringLeft(unsigned int Handle, unsigned int Size)
{
	char *tmp = (char *)GetString(Handle);
	char *buf = (char *)malloc(sizeof(char) * Size + 1);
	strncpy(buf, tmp, Size);
	return (unsigned char *)buf;
}

unsigned char *StringManager::StringRight(unsigned int Handle, unsigned int Size)
{
	char *tmp = (char *)GetString(Handle);
	char *buf = (char *)malloc(sizeof(char) * Size + 1);
	strncpy(buf, tmp + strlen(tmp) - Size, Size);
	return (unsigned char *)buf;
}

unsigned char *StringManager::StringMid(unsigned int Handle, unsigned int Offset, unsigned int Size)
{
	char *tmp = (char *)GetString(Handle);
	char *buf = (char *)malloc(sizeof(char) * Size + 1);
	strncpy(buf, tmp + Offset, Size);
	return (unsigned char *)buf;
}

unsigned int StringManager::StringFind(unsigned int MasterHandle, unsigned int SubHandle, unsigned int Offset)
{
	char *MasterString = (char *)GetString(MasterHandle);
	char *SubString = (char *)GetString(SubHandle);
	char *Found = strstr(MasterString + Offset, SubString);
	return Found - MasterString;
}

bool StringManager::StringCompare(unsigned int MasterHandle, unsigned int SlaveHandle)
{
	char *MasterString = (char *)GetString(MasterHandle);
	char *SlaveString = (char *)GetString(SlaveHandle);
	return (strcmp(MasterString, SlaveString) == 0);
}

unsigned int StringManager::StringLength(unsigned int Handle)
{
	return strlen((char *)GetString(Handle));
}
