#include <string.h>
#include "anvm.h"
#include "Cpu.h"

static union _conv
{
	unsigned int i;
	float f;
} conv;


void Cpu::OutPort(unsigned int Port, unsigned int Argu)
{
    switch(Port)
    {
	case 0:		// disp integer + \n
		scn->Display((int)Argu);
		scn->NewLine();
		break;
	case 1:		// disp string + \n
		scn->Display((char *)this->StrMan->GetString(Argu));
		scn->NewLine();
		break;
	case 2:		// disp string
		scn->Display((char *)this->StrMan->GetString(Argu));
		break;
	case 3:		// disp integer
		scn->Display((int)Argu);
		break;
	case 4:		// disp charactor
		{
			char tmp[2] = {Argu, '\0'};
			scn->Display(tmp);
		}
		break;
	case 5:		// disp float
		conv.i = Argu;
		scn->Display(conv.f);
		break;
	case 10:		// input int
		this->r3 = keybd->GetInteger();
		break;
	case 11:		// input string
		this->StrMan->PutString(this->r3, keybd->GetString());
		break;
	case 12:		// input float
		conv.f = keybd->GetFloat();
		this->r3 = conv.i;
		break;
	case 13:		// get data as integer
		this->r3 = GetMem(this->DataPtr);
		this->DataPtr += 4;
		break;
	case 14:		// get data as string
		{
			unsigned char *tmp = this->StrMan->GetString(this->DataPtr);
			this->DataPtr += strlen((char *)tmp) + 1;
			this->StrMan->PutString(this->r3, tmp);
		}
		break;
	case 15:		// get data as float
		this->r3 = GetMem(this->DataPtr);
		this->DataPtr += 4;
		break;
	case 16:		// set simulator screen size
		scn->SetScreenSize(this->r2, this->r3);
		break;
	case 17:		// allocate a surface
		this->r3 = gpu->AllocSurface();
		break;
	case 18:		// free a surface
		gpu->FreeSurface(this->r3);
		break;
	case 19:		// load picture
		this->r3 = (unsigned int)gpu->LoadPicture((char *)this->StrMan->GetString(this->r3), this->r2);
		break;
	case 20:		// copy rectange
		gpu->DrawPicture(
						GetMem(this->r3 + 32), GetMem(this->r3 + 28), GetMem(this->r3 + 24),
						GetMem(this->r3 + 20), GetMem(this->r3 + 16), GetMem(this->r3 + 12),
						GetMem(this->r3 + 8), GetMem(this->r3 + 4), GetMem(this->r3));
		break;
	case 21:		// show surface
		gpu->CloneSurface(this->r3, -1);
		break;
	case 22:		// clone surface
		gpu->CloneSurface(this->r3, this->r2);
		break;
	case 23:		// fill surface
		gpu->FillSurface(
						GetMem(this->r3 + 20), GetMem(this->r3 + 16), GetMem(this->r3 + 12),
						GetMem(this->r3 + 8), GetMem(this->r3 + 4), GetMem(this->r3));
		break;
	case 24:		// write pixel
		gpu->WritePixel(GetMem(this->r3 + 12), GetMem(this->r3 + 8),
						GetMem(this->r3 + 4), GetMem(this->r3));
		break;
	case 25:		// read pixel
		this->r3 = gpu->ReadPixel(GetMem(this->r3 + 8), GetMem(this->r3 + 4), GetMem(this->r3));
		break;
	case 26:		// free picture
		gpu->FreePicture(this->r3);
		break;
	case 27:		// delay
		scn->Delay(this->r3);
		break;
	case 32:		// set random seed
		srand(this->r3);
		break;
	case 33:		// get random
		this->r3 = rand() % this->r3;
		break;
	case 34:		// keypress
		this->r3 = (SDL2Input::KeyPress() == this->r3)? 1 : 0;
		break;
	case 35:		// clean screen
		scn->CleanScreen();
		break;
	case 36:		// set locate as text
		scn->SetTextLocation(r2, r3);
		break;
	case 37:		// set font color
		scn->SetTextColor(GetMem(this->r3 + 8), GetMem(this->r3 + 4));
		break;
	case 38:		// set font size
		scn->SetFont(this->r3);
		break;
	case 39:		// wait key
		this->r3 = SDL2Input::WaitKey();
		break;
	case 40:		// get picture width
		this->r3 = gpu->GetPictureWidth(this->r3);
		break;
	case 41:		// get picture heigth
		this->r3 = gpu->GetPictureHeigth(this->r3);
		break;
	case 42:		// set locate as pixel
		scn->SetDispPosition(this->r2, this->r3);
		break;
	case 43:		// clone a part of surface
		gpu->CloneSurface(
							GetMem(this->r3), GetMem(this->r3 + 4),
							GetMem(this->r3 + 8), GetMem(this->r3 + 12));
		break;
	case 44:		// set background mode
		scn->SetTransparent(this->r3 == 1);
		break;
	case 45:		// get key charactor string
		{
			unsigned char tmp[2];
			tmp[0] = SDL2Input::WaitKey();
			tmp[1] = '\0';
			this->StrMan->PutString(this->r3, tmp);
		}
		break;
	case 46:		// get key code
		this->r3 = SDL2Input::WaitKey();
		break;
	case 48:		// open file
		storage->OpenFile((char *)this->StrMan->GetString(this->r3), this->r1);
		break;
	case 49:		// close file
		storage->CloseFile(Argu);
		break;
	case 50:		// read from file
		switch(Argu)
		{
		case 16:	// Int
			this->r3 = storage->ReadFile_Int(this->r1, this->r2);
			break;
		case 17:	// Float
			conv.f = storage->ReadFile_Float(this->r1, this->r2);
			this->r3 = conv.i;
			break;
		case 18:	// String
			unsigned char *tmp = storage->ReadFile_String(this->r1, this->r2);
			this->StrMan->PutString(this->r3, tmp);
			break;
		}
		break;
	case 51:		// write to file
		switch(Argu)
		{
		case 16:
			storage->WriteFile_Int32(this->r1, this->r2, this->r3);
			break;
		case 17:
			storage->WriteFile_Int32(this->r1, this->r2, this->r3);
			break;
		case 18:
			storage->WriteFile_String(this->r1, this->r2, (char *)this->StrMan->GetString(this->r3));
			break;
		}
		break;
	case 52:		// feof
		this->r3 = storage->Eof(this->r3);
		break;
	case 53:		// get file size
		this->r3 = storage->GetFileSize(this->r3);
		break;
	case 54:		// get position
		this->r3 = storage->GetPosition(this->r3);
		break;
	case 55:		// fseek
		storage->MovePosition(this->r2, this->r3);
		break;
	case 64:		// set pen
		gpu->SetPenColor(GetMem(this->r3 + 12), GetMem(this->r3));
		break;
	case 65:		// set brush
		break;
	case 66:		// move pen
		gpu->MovePen(this->r1, this->r2, this->r3);
		break;
	case 67:		// draw line
		gpu->DrawLine(this->r1, this->r2, this->r3);
		break;
	case 68:		// draw rectange
		gpu->DrawRectangle(GetMem(this->r3 + 16),
							GetMem(this->r3 + 12), GetMem(this->r3 + 8),
							GetMem(this->r3 + 4), GetMem(this->r3));
		break;
	case 69:		// draw circle
		gpu->DrawCircle(
						GetMem(this->r3 + 12), GetMem(this->r3 + 8),
							GetMem(this->r3 + 4), GetMem(this->r3));
		break;
	case 80:		// vm test - useless
		break;
    }
}
