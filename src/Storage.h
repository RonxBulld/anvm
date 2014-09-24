#pragma once
#include <map>
using namespace std;

class Storage
{
private:
	map<int, FILE*> FilePool;
public:
	void OpenFile(const char *FileName, const int Handle);
	void CloseFile(const int Handle);
	int ReadFile_Int(const int Handle, const int Offset);
	float ReadFile_Float(const int Handle, const int Offset);
	unsigned char *ReadFile_String(const int Handle, const int Offset);
	void WriteFile_Int32(const int Handle, const int Offset, const int Data);
	void WriteFile_String(const int Handle, const int Offset, const char *Data);
	bool Eof(const int Handle);
	unsigned int GetFileSize(const int Handle);
	unsigned int GetPosition(const int Handle);
	void MovePosition(const int Handle, const int Position);
};
