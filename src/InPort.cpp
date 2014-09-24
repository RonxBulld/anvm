#include <time.h>
#include <math.h>
#include <string.h>

#include "anvm.h"
#include "Cpu.h"
#include "CoreConst.h"

static union _conv
{
	unsigned int i;
	float f;
} conv;

unsigned int Cpu::InPort(unsigned int Port)
{
	switch(Port)
	{
	case 0:		// float -> int
		conv.i = this->r3;
		return (unsigned int)conv.f;
	case 1:		// int -> float
		conv.f = (float)this->r3;
		return conv.i;
	case 2:		// request a string handle
		return this->StrMan->CreateString();
	case 3:		// string -> int
		return this->StrMan->ToInt(this->r3);
	case 4:		// int -> string
		{
			char tmp[16];
			sprintf(tmp, "%d", (int)this->r3);
			this->StrMan->PutString(this->r2, (unsigned char *)tmp);
		}
		return this->r3;
	case 5:		// copy string
		this->StrMan->PutString(this->r3, this->StrMan->GetString(this->r2));
		return this->r3;
	case 6:		// append string
		this->StrMan->QuoteString(this->r3)->append((char *)this->StrMan->GetString(this->r2));
		return this->r3;
	case 7:		// get length
		return this->StrMan->StringLength(this->r3);
	case 8:		// free handle
        this->StrMan->FreeString(this->r3);
        return this->r3;
	case 9:		// compare string
		return this->StrMan->StringCompare(this->r2, this->r3) == 0 ? 0 : 1;
	case 10:		// int -> float -> string
		{
			char buf[16];
			sprintf(buf, "%f", (float)this->r3);
			this->StrMan->PutString(this->r2, (unsigned char*)&buf);
		}
		return this->r3;
	case 11:		// string -> float
		conv.f = (float)atof((char *)this->StrMan->GetString(this->r3));
		return conv.i;
	case 12:		// string[index]
		return this->StrMan->GetString(this->r3)[this->r2];
	case 13:		// string[index] = ch
		{
			unsigned char *buf = this->StrMan->GetString(this->r3);
			if (this->r2 < strlen((char *)buf))
			{
				buf[this->r2] = this->r1 & 0xFF;
			}
			this->StrMan->PutString(this->r3, buf);
		}
		return this->r3;
	case 14:		// 65536
		return 65536;
	case 15:		// get ticks
		return (unsigned int)(clock() / CLOCKS_PER_SEC * 1000);
	case 16:		// sin
		conv.i = this->r3;
		conv.f = sin(conv.f);
		return conv.i;
	case 17:		// cos
		conv.i = this->r3;
		conv.f = cos(conv.f);
		return conv.i;
	case 18:		// tan
		conv.i = this->r3;
		conv.f = tan(conv.f);
		return conv.i;
	case 19:		// sqrt
		conv.i = this->r3;
		conv.f = sqrt(conv.f);
		return conv.i;
	case 20:		// abs by integer
		return abs(this->r3);
	case 21:		// abs
		conv.i = this->r3;
		conv.f = fabs(conv.f);
		return conv.i;
	case 22:		// relocation data field
		this->DataPtr = this->r2;
		return this->r3;
	case 23:		// read memory
		return GetMem(this->r3);
	case 24:		// write memory
		PutMem(this->r3, this->r2, DWORD);
		return this->r3;
	case 25:		// environment value
		return ENV_SIM;
	case 32:		// int -> string
		{
			unsigned char buf[16];
			sprintf((char *)buf, "%d", (int)this->r1);
			this->StrMan->PutString(this->r3, buf);
		}
		return this->r3;
	case 33:		// string -> int
		return atoi((char *)this->StrMan->GetString(this->r3));
	case 34:		// string[0]
		return this->StrMan->GetString(this->r3)[0];
	case 35:		// left
		this->StrMan->PutString(this->r3, this->StrMan->StringLeft(this->r2, this->r1));
		return this->r3;
	case 36:		// right
		this->StrMan->PutString(this->r3, this->StrMan->StringRight(this->r2, this->r1));
		return this->r3;
	case 37:		// mid
		this->StrMan->PutString(this->r3, this->StrMan->StringMid(this->r2, this->r1, this->r0));
		return this->r3;
	case 38:		// find
		return this->StrMan->StringFind(this->r3, this->r2, this->r1);
	case 39:		// length
		return this->StrMan->StringLength(this->r3);
	default:
		CoreCrash("Unknow I/O operate: I.%d", Port);
		return 0;
	}
}
