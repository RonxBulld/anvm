#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "anvm.h"
#include "Gpu.h"

#define TRANS_COLOR_END(C) ((((C) & 0xFF) << 16) | ((C) & 0xFF00) | (((C) & 0xFF0000) >> 16))
#define WILL_REFRESH(_x,_y,_w,_h) \
	{ if (SurfaceHandle == -1) { \
		Rect _rr; _rr.x = (_x); _rr.y = (_y); _rr.w = (_w); _rr.h = (_h); \
		scn->Refresh(&_rr); \
	} }
#define MIN(A,B) (((A)<=(B))?(A):(B))

Gpu::Gpu()
{
	for (int i = 0; i < 9; i++)
	{
		this->SurfacePool[i] = NULL;
	}
	for (int i = 0; i < 10; i++)
	{
		this->DrawRenderer[i] = NULL;
	}
	SetPenColor(-1, 0xFFFFFF);
}

Gpu::~Gpu()
{
	for (int i = 0; i < 10; i++)
		if (this->DrawRenderer[i] != NULL)
			SDL2Function::DestroyRenderer(this->DrawRenderer[i]);
}

SDL_Surface *Gpu::CreateSurface(int width, int height)
{
	return SDL2Function::CreateRGBSurface(0, width, height);
}

int Gpu::AllocSurface()
{
	for (int i = 0; i < 9; i++)
	{
		if (this->SurfacePool[i] == NULL)
		{
			int width, height;
			scn->GetScreenSize(&width, &height);
			this->SurfacePool[i] = CreateSurface(width, height);
			SDL2Draw::FillRect(this->SurfacePool[i], NULL, SDL2Function::MapRGB(SDL2Function::GetFormat(this->SurfacePool[i]),0x00, 0x00, 0x00));
			return i;
		}
	}
	return -1;
}

void Gpu::FreeSurface(int index)
{
	if (index >= 0 && index < 9)
	{
		SDL2Function::FreeSurface(this->SurfacePool[index]);
		this->SurfacePool[index] = NULL;
	}
}

int Gpu::RequestResourceHandle()
{
	int i = 0;
	for(map<int, SDL_Surface*>::iterator it = this->ResPool.begin(); it != this->ResPool.end(); it++, i++)
	{
		if (it->second == NULL)
		{
			return it->first;
		}
	}
	this->ResPool[i] = NULL;
	return i;
}

int Gpu::LoadPicture(const char *FileName, unsigned int index)
{
	FILE *fp = fopen(FileName, "rb");
	if (fp == NULL)
	{
		return -1;
	}

	unsigned int ResOffset = 4 * (index - 1), Size, ResCount;
	fread(&ResCount, 4, 1, fp);
	fseek(fp, ResOffset, SEEK_CUR);
	fread(&ResOffset, 4, 1, fp);
	fseek(fp, ResOffset, SEEK_SET);
	fread(&Size, 4, 1, fp);
	void *data = malloc(Size);
	fread(data, 1, Size, fp);

	SDL_Surface *res = SDL2Function::LoadBMP_RW(SDL2Function::RWFromConstMem(data, Size), 1);
	int i = RequestResourceHandle();
	this->ResPool[i] = res;
	return i;
}

void Gpu::FreePicture(int Handle)
{
	SDL_Surface *ptr = this->ResPool[Handle];
	if (ptr != NULL)
	{
		SDL2Function::FreeSurface(ptr);
		this->ResPool[Handle] = NULL;
	}
}

SDL_Surface *Gpu::GetSurface(int handle)
{
	return (handle == -1)? scn->GetScreenSurface() : this->SurfacePool[handle];
}

void Gpu::DrawPicture(int page, int pic, int dx, int dy, int w, int h, int x, int y, int mode)
{
	if (this->ResPool.find(pic)->second == NULL || (page < -1 || page > 8))
	{
		return ;
	}
	SDL_Surface *DestSurface = GetSurface(page);
	SDL_Surface *SrcSurface = this->ResPool[pic];
	Rect SrcRect, DestRect;
	SrcRect.x = x; SrcRect.y = y; SrcRect.h = h; SrcRect.w = w;
	DestRect.x = dx; DestRect.y = dy; DestRect.h = h; DestRect.w = w;

	SDL2Draw::BlitSurfaceWithColorKey(SrcSurface, &SrcRect, DestSurface, &DestRect, SDL2Function::MapRGB(SDL2Function::GetFormat(SrcSurface), 0xFF, 0x00, 0xFF));
	int SurfaceHandle = page;
	WILL_REFRESH(dx, dy, w, h);
}

void Gpu::CloneSurface(int SrcSurfaceHandle, int DestSurfaceHandle, int x, int y)
{
	SDL_Surface *SrcSurface = GetSurface(SrcSurfaceHandle), *DestSurface = GetSurface(DestSurfaceHandle);
	Rect CloneRect;
	CloneRect.x = 0; CloneRect.y = 0; CloneRect.w = SDL2Function::GetSurfaceWidth(SrcSurface) - x; CloneRect.h = SDL2Function::GetSurfaceHeight(SrcSurface) - y;

	SDL2Draw::BlitSurfaceWithColorKey(SrcSurface, NULL, DestSurface, &CloneRect, SDL2Function::MapRGB(SDL2Function::GetFormat(SrcSurface), 0xFF, 0x00, 0xFF));

	if (DestSurfaceHandle == -1)
		scn->Refresh();
}

void Gpu::FillSurface(int SurfaceHandle, int x, int y, int w, int h, int color)
{
	Rect DrawRect;
	DrawRect.x = x; DrawRect.y = y; DrawRect.w = w; DrawRect.h = h;

	SDL2Draw::FillRect(GetSurface(SurfaceHandle), &DrawRect, color);
	WILL_REFRESH(x, y, w, h);
}

Uint32 Gpu::__get_pixel(SDL_Surface *Surface, int x, int y)
{
	Uint32 pixel = *((Uint32*)SDL2Function::GetSurfacePixels(Surface) + y * SDL2Function::GetSurfaceWidth(Surface) + x);
	return TRANS_COLOR_END(pixel);
}

void Gpu::__put_pixel(SDL_Surface *Surface, int x, int y, Uint32 pixel)
{
	*((Uint32*)SDL2Function::GetSurfacePixels(Surface) + y * SDL2Function::GetSurfaceWidth(Surface) + x) = TRANS_COLOR_END(pixel);
}

void Gpu::WritePixel(int SurfaceHandle, int x, int y, int color)
{
	SDL_Surface *OptSurface = GetSurface(SurfaceHandle);
	if (SDL2Function::MustLock(OptSurface) && SDL2Function::LockSurface(OptSurface) < 0)
		return ;

    if ((x >= 0 && x < SDL2Function::GetSurfaceWidth(OptSurface)) && (y >= 0 && y < SDL2Function::GetSurfaceHeight(OptSurface)))
	{
		__put_pixel(OptSurface, x, y, color);
	}

	if (SDL2Function::MustLock(OptSurface))
		SDL2Function::UnlockSurface(OptSurface);
	WILL_REFRESH(x - 1, y - 1, 3, 3);
}

unsigned int Gpu::ReadPixel(int SurfaceHandle, int x, int y)
{
	unsigned int ColorValue = 0;
	SDL_Surface *OptSurface = GetSurface(SurfaceHandle);
	if (SDL2Function::MustLock(OptSurface) && SDL2Function::LockSurface(OptSurface) < 0)
		return 0;

    if ((x >= 0 && x < SDL2Function::GetSurfaceWidth(OptSurface)) && (y >= 0 && y < SDL2Function::GetSurfaceHeight(OptSurface)))
	{
		ColorValue = __get_pixel(OptSurface, x, y);
	}

	if (SDL2Function::MustLock(OptSurface))
		SDL2Function::UnlockSurface(OptSurface);
	return ColorValue;
}

int Gpu::GetPictureWidth(int SurfaceHandle)
{
	if (this->ResPool.find(SurfaceHandle)->second == NULL)
		return 0;
	else
		return SDL2Function::GetSurfaceWidth(this->ResPool[SurfaceHandle]);
}

int Gpu::GetPictureHeigth(int SurfaceHandle)
{
	if (this->ResPool.find(SurfaceHandle)->second == NULL)
		return 0;
	else
		return SDL2Function::GetSurfaceHeight(this->ResPool[SurfaceHandle]);
}

void Gpu::SetPenColor(int SurfaceHandle, int Color)
{
	int _handle = SurfaceHandle + 1;
	if (_handle < 0 || _handle > 10)
		return ;
	SDL_Renderer *_renderer = SDL2Function::CreateSoftwareRenderer(GetSurface(SurfaceHandle));
	SDL2Function::SetRenderDrawColor(_renderer,
							(Color & 0xFF),
							(Color & 0xFF00) >> 8,
							(Color & 0xFF0000) >> 16,
							255);
	if (this->DrawRenderer[_handle] != NULL)
		SDL2Function::DestroyRenderer(this->DrawRenderer[_handle]);
	this->DrawRenderer[_handle] = _renderer;
}

void Gpu::MovePen(int SurfaceHandle, int x, int y)
{
	int _handle = SurfaceHandle + 1;
	if (_handle < 0 || _handle > 10)
		return ;
	this->StartDrawPoint[_handle].x = x;
	this->StartDrawPoint[_handle].y = y;
}

void Gpu::DrawLine(int SurfaceHandle, int x, int y)
{
	int _handle = SurfaceHandle + 1;
	if (_handle < 0 || _handle > 10)
		return ;
	int _x = this->StartDrawPoint[_handle].x, _y = this->StartDrawPoint[_handle].y;
	SDL2Draw::RenderDrawLine(this->DrawRenderer[_handle], _x, _y, x - 1, y);
	SDL2Draw::RenderPresent(this->DrawRenderer[_handle]);
	this->StartDrawPoint[_handle].x = this->StartDrawPoint[_handle].y = 0;

	WILL_REFRESH(MIN(x, _x), MIN(y, _y), abs(x - 1 - _x), abs(y - _y));
}

void Gpu::DrawRectangle(int SurfaceHandle, int x1, int y1, int x2, int y2)
{
	int _handle = SurfaceHandle + 1;
	if (_handle < 0 || _handle > 10)
		return ;
	Rect _rect;
	_rect.x = x1, _rect.y = y1; _rect.w = x2 - x1 + 1; _rect.h = y2 - y1 + 1;
	SDL2Draw::RenderDrawRect(this->DrawRenderer[_handle], &_rect);
	SDL2Draw::RenderPresent(this->DrawRenderer[_handle]);
	WILL_REFRESH(MIN(x1, x2), MIN(y1, y2), abs(x2 - x1), abs(y2 - y1));
}

void Gpu::__plot_circle_points(SDL_Surface *Surface, int xc, int yc, int x, int y, Uint32 c)
{
	__put_pixel(Surface, xc + x, yc + y, c);
	__put_pixel(Surface, xc - x, yc + y, c);
	__put_pixel(Surface, xc + x, yc - y, c);
	__put_pixel(Surface, xc - x, yc - y, c);
	__put_pixel(Surface, xc + y, yc + x, c);
	__put_pixel(Surface, xc - y, yc + x, c);
	__put_pixel(Surface, xc + y, yc - x, c);
	__put_pixel(Surface, xc - y, yc - x, c);
}

void Gpu::__bresenham_circle(SDL_Surface *Surface, int xc, int yc, int radius, Uint32 c)
{
	int x = 0, y = radius, p = 3 - 2 * radius;
	while(x < y)
	{
		__plot_circle_points(Surface, xc, yc, x, y, c);
		if(p<0) {
			p = p + 4 * x + 6;
		} else {
			p = p + 4 * (x - y) + 10;
			y -= 1;
		}
		x += 1;
	}
	if(x == y)
		__plot_circle_points(Surface, xc, yc, x, y, c);
}

void Gpu::DrawCircle(int SurfaceHandle, int x, int y, int r)
{
	int _handle = SurfaceHandle + 1;
	if (_handle < 0 || _handle > 10)
		return ;
	Uint8 _a, _r, _g, _b;
	SDL2Function::GetRenderDrawColor(this->DrawRenderer[_handle], &_r, &_g, &_b, &_a);
	Uint32 c = _r << 16 | _g << 8 | _b << 0;
	__bresenham_circle(GetSurface(SurfaceHandle), x, y, r, c);

	WILL_REFRESH(x - r - 1, y - r - 1, 2 * r + 1, 2 * r + 1);
}
