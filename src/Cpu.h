#pragma once
#include "String.h"

class Cpu
{
private:
	unsigned int rp, rf, rs, rb, r0, r1, r2, r3;
	unsigned int *(regs[8]);
	unsigned char *vmem;
	StringManager *StrMan;
	unsigned int DataPtr;

	void CoreDump();
	bool GetFlag(unsigned char Flag);
	void PutFlag(unsigned char Flag);
	bool RunUnit();
	void CoreCrash(const char *fmt, ...);

	unsigned int GetReg(unsigned int Reg);
	void PutReg(unsigned int Reg, unsigned int Data, unsigned int Size);

	unsigned int GetMem(unsigned int Addr);
	void PutMem(unsigned int Addr, unsigned int Data, unsigned int Size);

	unsigned int GetData(unsigned char Addressing, unsigned int Argument);
	void PutData(unsigned char Addressing, unsigned int Argument, unsigned int Data, unsigned int DataType);

	void Push(unsigned int Data);
	unsigned int Pop();

	unsigned int InPort(unsigned int Port);
	void OutPort(unsigned int Port, unsigned int Argu);

public:
	Cpu();
	~Cpu();

	bool PowerOn;
	unsigned int StateBase;
	int Run(void *Args);
};
