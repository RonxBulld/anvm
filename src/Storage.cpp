#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include "Storage.h"
using namespace std;

#define FILEPTR (this->FilePool[Handle])

void Storage::OpenFile(const char *FileName, const int Handle)
{
	CloseFile(Handle);
	if (access(FileName, F_OK) == 0)
		FILEPTR = fopen(FileName, "rb+");
	else
		FILEPTR = fopen(FileName, "wb+");
}

void Storage::CloseFile(const int Handle)
{
	if (FILEPTR != NULL)
	{
		fclose(FILEPTR);
		FILEPTR = NULL;
	}
}

int Storage::ReadFile_Int(const int Handle, const int Offset)
{
	int tmp = 0;
	if (FILEPTR != NULL)
	{
		fseek(FILEPTR, Offset - 0x7FFFFFFF, SEEK_CUR);
		fread(&tmp, 4, 1, FILEPTR);
	}
	return tmp;
}

float Storage::ReadFile_Float(const int Handle, const int Offset)
{
	float tmp = 0.0f;
	if (FILEPTR != NULL)
	{
		fseek(FILEPTR, Offset - 0x7FFFFFFF, SEEK_CUR);
		fread(&tmp, 4, 1, FILEPTR);
	}
	return tmp;
}

unsigned char *Storage::ReadFile_String(const int Handle, const int Offset)
{
	string tmp = "";
	if (FILEPTR != NULL)
	{
		fseek(FILEPTR, Offset - 0x7FFFFFFF, SEEK_CUR);
		unsigned char ch = '\0';
		do {
			ch = fgetc(FILEPTR);
			if (ch != 0xFF)
				tmp.append(1, ch);
		} while (ch != '\0' && feof(FILEPTR) == 0);
	}
	return (unsigned char *)strdup(tmp.c_str());
}

void Storage::WriteFile_Int32(const int Handle, const int Offset, const int Data)
{
	if (FILEPTR != NULL)
	{
		fseek(FILEPTR, Offset - 0x7FFFFFFF, SEEK_CUR);
		fwrite(&Data, 4, 1, FILEPTR);
	}
}

void Storage::WriteFile_String(const int Handle, const int Offset, const char *Data)
{
	if (FILEPTR != NULL)
	{
		fseek(FILEPTR, Offset - 0x7FFFFFFF, SEEK_CUR);
		fwrite(Data, 1, strlen(Data) + 1, FILEPTR);
	}
}

bool Storage::Eof(const int Handle)
{
	if (FILEPTR != NULL)
	{
		return (feof(FILEPTR) != 0)? true : false;
	} else {
		return true;
	}
}

unsigned int Storage::GetFileSize(const int Handle)
{
	if (FILEPTR != NULL)
	{
		int _cur = ftell(FILEPTR);
		fseek(FILEPTR, 0, SEEK_END);
		int _size = ftell(FILEPTR);
		fseek(FILEPTR, _cur, SEEK_SET);
		return _size;
	} else {
		return 0;
	}
}

unsigned int Storage::GetPosition(const int Handle)
{
	if (FILEPTR != NULL)
	{
		return ftell(FILEPTR);
	} else {
		return 0;
	}
}

void Storage::MovePosition(const int Handle, const int Position)
{
	if (FILEPTR != NULL)
	{
		fseek(FILEPTR, Position, SEEK_SET);
	}
}
