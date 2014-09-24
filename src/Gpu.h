#pragma once
#include <map>
using namespace std;

class Gpu
{
private:
	SDL_Surface *SurfacePool[9];
	SDL_Renderer *DrawRenderer[10];
	Point StartDrawPoint[10];
	map<int, SDL_Surface*> ResPool;
	SDL_Surface *CreateSurface(int width, int height);
	int RequestResourceHandle();
	SDL_Surface *GetSurface(int handle);
	Uint32 __get_pixel(SDL_Surface *Surface, int x, int y);
	void __put_pixel(SDL_Surface *Surface, int x, int y, Uint32 pixel);
	void __plot_circle_points(SDL_Surface *Surface, int xc, int yc, int x, int y, Uint32 c);
	void __bresenham_circle(SDL_Surface *Surface, int xc, int yc, int radius, Uint32 c);
public:
	Gpu();
	~Gpu();
	int AllocSurface();
	void FreeSurface(int index);
	int LoadPicture(const char *FileName, unsigned int index);
	void FreePicture(int Handle);
	void DrawPicture(int page, int pic, int dx, int dy, int w, int h, int x, int y, int mode);
	void CloneSurface(int SrcSurfaceHandle, int DestSurfaceHandle, int x = 0, int y = 0);
	void FillSurface(int SurfaceHandle, int x, int y, int w, int h, int color);
	void WritePixel(int SurfaceHandle, int x, int y, int color);
	unsigned int ReadPixel(int SurfaceHandle, int x, int y);
	int GetPictureWidth(int Handle);
	int GetPictureHeigth(int Handle);
	void SetPenColor(int Handle, int Color);
	void MovePen(int Handle, int x, int y);
	void DrawLine(int Handle, int x, int y);
	void DrawRectangle(int Handle, int x1, int y1, int x2, int y2);
	void DrawCircle(int Handle, int x, int y, int r);
};
