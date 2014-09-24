#ifdef __linux__
#	include <iconv.h>
#else
#	include <windows.h>
#endif
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mouse.h>
#include "anvm.h"

#define FPSNEED 24
#define DEBUG_LAYER 0

Screen::Screen(int width, int height)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
    this->ScreenSurface = NULL;
    this->buffer = NULL;
    this->Transparent = false;
    this->FontFile = NULL;

	this->MainWnd = SDL_CreateWindow(VERSION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if(this->MainWnd == NULL)
	{
		Error("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return ;
	}
	if (TTF_Init() == -1)
	{
		Error("Initialize ttf failure");
	} else {
		SetFont(0);
		SetScreenSize(width, height);
#ifndef __linux__
		this->DebugFont = TTF_OpenFont("C:\\Windows\\Fonts\\CONSOLA.TTF", 12);
#else
		this->DebugFont = TTF_OpenFont("/system/fonts/DroidSansMono.ttf", 12);
#endif

		this->TextColor = SDL_MapRGB(this->ScreenSurface->format, 0xFF, 0xFF, 0xFF);
		this->BackgroundColor = SDL_MapRGB(this->ScreenSurface->format, 0x00, 0x00, 0x00);
	}
}

void Screen::Refresh(Rect *RefRect)
{
	Uint32 StartTime = SDL_GetTicks();
	static SDL_mutex *mutex;
	if (mutex == NULL)
		mutex = SDL_CreateMutex();
	//if (SDL_LockMutex(mutex) == 0)
	{
		int _w, _h;
		SDL_GetWindowSize(this->MainWnd, &_w, &_h);
		double Multi = (double)_w / this->width;
		SDL_Rect _rect, _r;
		_r.x = ((RefRect == NULL)? 0 : RefRect->x);
		_r.y = ((RefRect == NULL)? 0 : RefRect->y);
		_r.w = ((RefRect == NULL)? this->width : RefRect->w);
		_r.h = ((RefRect == NULL)? this->height : RefRect->h);
		_rect.x = _r.x * Multi;
		_rect.y = _r.y * Multi;
		_rect.w = _r.w * Multi;
		_rect.h = _r.h * Multi;
		SDL_BlitScaled(this->ScreenSurface, &_r, this->buffer, &_rect);

		Uint32 EndTime = SDL_GetTicks();
		Uint32 MustDelay = (1000 / FPSNEED) - (EndTime - StartTime);
		if (MustDelay > 10)
			SDL_Delay(MustDelay);
#if DEBUG_LAYER == 1
		DrawDebugLayer(this->buffer, 1000.0f / (SDL_GetTicks() - StartTime));
#endif // DEBUG_LAYER
		SDL_UpdateWindowSurface(this->MainWnd);
		SDL_UnlockMutex(mutex);
	}
}

void Screen::SetScreenSize(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
    SDL_SetWindowSize(this->MainWnd, width, height);
    this->buffer = SDL_GetWindowSurface(this->MainWnd);
	this->ScreenSurface = SDL2Function::CreateRGBSurface(0, (int)width, (int)height);
	CleanScreen();
}

Screen::~Screen()
{
	SDL2Input::HiddenKeyboard();
    //Destroy window
	SDL_DestroyWindow(this->MainWnd);
	//Quit SDL subsystems
	TTF_CloseFont((TTF_Font*)this->Font);
	TTF_Quit();
	SDL_Quit();
}

void Screen::DrawDebugLayer(SDL_Surface *dst, float FPS)
{
	SDL_Rect __debug_rect;
	SDL_Surface *layer;
	SDL_Color fg = {0, 0, 0}, bg = {0xFF, 0xFF, 0xFF};
	char buf[255];

	sprintf(buf, "[Debug Layer]");
	layer = TTF_RenderText_Shaded((TTF_Font*)this->DebugFont, buf, fg, bg);
	__debug_rect.w = layer->w; __debug_rect.h = layer->h;
	__debug_rect.x = dst->w - layer->w; __debug_rect.y = 0;
	SDL_BlitSurface(layer, NULL, dst, &__debug_rect);

	sprintf(buf, "FPS: %f", FPS);
	layer = TTF_RenderText_Shaded((TTF_Font*)this->DebugFont, buf, fg, bg);
	__debug_rect.x = dst->w - layer->w; __debug_rect.y += __debug_rect.h;
	__debug_rect.w = layer->w; __debug_rect.h = layer->h;
	SDL_BlitSurface(layer, NULL, dst, &__debug_rect);

	SDL_FreeSurface(layer);
	return ;
}

void Screen::__set_font(const char *FontFile, const int Size)
{
	if (this->FontFile != NULL)
		free(this->FontFile);
	if (FontFile != NULL)
		this->FontFile = strdup(FontFile);
	this->Font = TTF_OpenFont(this->FontFile, Size);
	if(this->Font == NULL) {
		Error("font open failure %s\n", SDL_GetError());
	}
	TTF_SizeText((TTF_Font*)this->Font, " ", &this->FontWidth, &this->FontHeigth);
}

void Screen::GetScreenSize(int *width, int *height)
{
	(*width) = this->width;
	(*height) = this->height;
}

Rect Screen::ShowText(SDL_Surface *Text, int x, int y, int width, int height, SDL_Surface *Dest)
{
	SDL_Rect textPosition;
	textPosition.x = x; textPosition.y = y;
	textPosition.w = (width == 0)? Text->w : width;
	textPosition.h = (height == 0)? Text->h : height;
	if (this->Transparent == false) {
		SDL_FillRect(this->ScreenSurface, &textPosition, this->BackgroundColor);
	}
	SDL_BlitSurface(Text, NULL, Dest, &textPosition);
	this->OutputPosition.x += Text->w;
	CheckPosition();
	Rect Ref;
	Ref.x = textPosition.x;
	Ref.y = textPosition.y;
	Ref.w = textPosition.w;
	Ref.h = textPosition.h;
	return Ref;
}

void Screen::CheckPosition()
{
	if (this->OutputPosition.x >= this->ScreenSurface->w)
	{
		NewLine();
	}
	if (this->OutputPosition.y >= this->ScreenSurface->h)
	{
		this->OutputPosition.x = 0;
		this->OutputPosition.y = 0;
		CleanScreen();
	}
}

void Screen::NewLine()
{
	this->OutputPosition.y += TTF_FontHeight((TTF_Font*)this->Font) - 1;
	this->OutputPosition.x = 0;
	CheckPosition();
}

void Screen::SetTransparent(bool value)
{
	this->Transparent = value;
}

void Screen::Display(const char *Text, int x, int y)
{
	if (x >= 0) this->OutputPosition.x = x;
	if (y >= 0) this->OutputPosition.y = y;
	for (int i = 0; i < (int)strlen(Text); i++)
	{
		if (Text[i] == 10)
		{
			NewLine();
		} else if (Text[i] == 9) {
			this->OutputPosition.x += this->FontWidth * 8;
			CheckPosition();
		} else {
			int len = SDL2Function::GetAllowCharNum(Text);
			char *t = (char*)malloc(len + 1);
			strncpy(t, Text + i, len);
			t[len] = '\0';
			i += len - 1;
			char *text = GB2312toUTF8(t);
			free(t);
			SDL_Surface *SDLText;
			SDL_Color FrontColor;
			Uint8 r, g, b;
			SDL_GetRGB(this->TextColor, this->ScreenSurface->format, &r, &g, &b);
			FrontColor.a = 255; FrontColor.r = r; FrontColor.g = g; FrontColor.b = b;
			SDLText = TTF_RenderUTF8_Solid((TTF_Font*)this->Font, text, FrontColor);
			Rect RefRect = ShowText(SDLText, this->OutputPosition.x, this->OutputPosition.y, 0, 0, this->ScreenSurface);
			this->Refresh(&RefRect);
		}
	}
}

void Screen::Display(const int Integer, int x, int y)
{
	char buf[16];
	sprintf(buf, "%d", Integer);
	Display(buf, x, y);
	return ;
}

void Screen::Display(const float Float, int x, int y)
{
	char buf[32];
	sprintf(buf, "%f", Float);
	Display(buf, x, y);
	return ;
}

void Screen::Display(const char Charactor, int x, int y)
{
	char buf[2];
	buf[0] = Charactor; buf[1] = '\0';
	Display(buf, x, y);
	return ;
}

char *Screen::GB2312toUTF8(const char *GB2312str)
{
#ifdef __linux__
	char *pIn = (char *)GB2312str;
	size_t iMaxOutLen = sizeof(char) * strlen(pIn) * 3, iInLen = strlen(pIn);
	char *pOut = (char *)malloc(iMaxOutLen);
	char *pBuf = pOut;
	size_t ret, iLeftLen = iMaxOutLen;
	iconv_t cd;
	cd = iconv_open("utf-8", "gb2312");
	if (cd == (iconv_t) - 1)
	{
		return NULL;
	}
	size_t iSrcLen = iInLen;
	ret = iconv(cd, &pIn, &iSrcLen, &pOut, &iLeftLen);
	if (ret == (size_t) -1)
	{
		iconv_close(cd);
		return NULL;
	}
	iconv_close(cd);
	*pOut = '\0';
	return pBuf;
#else
	int len = MultiByteToWideChar(CP_ACP, 0, GB2312str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, GB2312str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
#endif
}

void Screen::CleanScreen()
{
	//Fill the surface
	SDL_FillRect(this->ScreenSurface, NULL, SDL_MapRGB(this->ScreenSurface->format, 0x00, 0x00, 0x00));
	this->OutputPosition.x = this->OutputPosition.y = 0;
	this->Refresh();
}

Point Screen::GetDispPosition()
{
	return this->OutputPosition;
}

void Screen::SetDispPosition(int x, int y)
{
	this->OutputPosition.x = x;
	this->OutputPosition.y = y;
}

void Screen::Delay(int ticks)
{
	SDL_Delay((Uint32)ticks);
}

SDL_Surface *Screen::GetScreenSurface()
{
	return this->ScreenSurface;
}

void Screen::SetTextLocation(int l, int r)
{
	SetDispPosition(this->FontWidth * (r - 1), this->FontHeigth * (l - 1));
}

void Screen::SetTextColor(Uint32 FrontColor, Uint32 BackColor)
{
	this->TextColor = SDL_MapRGB(
								this->ScreenSurface->format,
								(FrontColor & 0x0000FF) >> 0,
								(FrontColor & 0x00FF00) >> 8,
								(FrontColor & 0xFF0000) >> 16);
	this->BackgroundColor = SDL_MapRGB(
								this->ScreenSurface->format,
								(BackColor & 0x0000FF) >> 0,
								(BackColor & 0x00FF00) >> 8,
								(BackColor & 0xFF0000) >> 16);
}

void Screen::SetFont(int FontID)
{
#ifdef __linux__
	static const char *__font_files[] = {"./Resource/simsun.ttc", "./Resource/simkai.ttf", "./Resource/simhei.ttf"};
#else
	static const char *__font_files[] = {"C:\\Windows\\Fonts\\simsun.ttc", "C:\\Windows\\Fonts\\simkai.ttf", "C:\\Windows\\Fonts\\simhei.ttf"};
#endif
	static const int __font_size[] = {12, 16, 24};
	int Size = FontID / 3, Type = FontID % 3;
	char *FontType = strdup(__font_files[Type]);
	__set_font(FontType, __font_size[Size]);
}

int Screen::GetFontWidth()
{
	return this->FontWidth;
}

int Screen::GetFontHeigth()
{
	return this->FontHeigth;
}

SDL_Window *Screen::GetMainWindow()
{
	return this->MainWnd;
}

// ---- SDL2Function ----

void SDL2Function::DestroyRenderer(SDL_Renderer *renderer)
{
	SDL_DestroyRenderer(renderer);
}

SDL_Surface *SDL2Function::CreateRGBSurface(Uint32 flags, int width, int height)
{
	return SDL_CreateRGBSurface(flags, width, height, 32, 0, 0, 0, 0);
}

SDL_PixelFormat *SDL2Function::GetFormat(SDL_Surface *surface)
{
	return surface->format;
}

Uint32 SDL2Function::MapRGB(const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(format, r, g, b);
}

void SDL2Function::FreeSurface(SDL_Surface *surface)
{
	SDL_FreeSurface(surface);
}

SDL_Surface *SDL2Function::LoadBMP_RW(SDL_RWops *src, int freesrc)
{
	return SDL_LoadBMP_RW(src, freesrc);
}

SDL_RWops *SDL2Function::RWFromConstMem(const void *mem, int size)
{
	return SDL_RWFromConstMem(mem, size);
}

int SDL2Function::GetSurfaceWidth(SDL_Surface *surface)
{
	return surface->w;
}

int SDL2Function::GetSurfaceHeight(SDL_Surface *surface)
{
	return surface->h;
}

void *SDL2Function::GetSurfacePixels(SDL_Surface *surface)
{
	return surface->pixels;
}

bool SDL2Function::MustLock(SDL_Surface *surface)
{
	return MustLock(surface);
}

int SDL2Function::LockSurface(SDL_Surface *surface)
{
	return SDL_LockSurface(surface);
}

void SDL2Function::UnlockSurface(SDL_Surface *surface)
{
	SDL_UnlockSurface(surface);
}

SDL_Renderer *SDL2Function::CreateSoftwareRenderer(SDL_Surface * surface)
{
	return SDL_CreateSoftwareRenderer(surface);
}

int SDL2Function::SetRenderDrawColor(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	return SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

int SDL2Function::GetRenderDrawColor(SDL_Renderer *renderer, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
	return SDL_GetRenderDrawColor(renderer, r, g, b, a);
}

int SDL2Function::GetAllowCharNum(const char *str)
{
	int FreeChar = 0, len = strlen(str);
	int fw = scn->GetFontWidth(), sw, sh, x = scn->GetDispPosition().x;
	scn->GetScreenSize(&sw, &sh);
	for (int i = 0; i < len && x <= sw && str[i] != '\n'; i++)
	{
		FreeChar++; x += fw;
		if ((str[i - 1] & 0x80) != 0) { FreeChar++; i++; x += fw; }
	}
	return FreeChar;
}

// ---- SDL2Draw ----

int SDL2Draw::FillRect(SDL_Surface *dst, const Rect *rect, Uint32 color)
{
	int retv;
	if (rect != NULL)
	{
		SDL_Rect _rect;
		_rect.x = rect->x; _rect.y = rect->y; _rect.w = rect->w; _rect.h = rect->h;
		retv = SDL_FillRect(dst, &_rect, color);
	} else {
		retv = SDL_FillRect(dst, NULL, color);
	}
	return retv;
}

int SDL2Draw::RenderDrawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{
	int retv = SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	return retv;
}

void SDL2Draw::RenderPresent(SDL_Renderer *renderer)
{
	SDL_RenderPresent(renderer);
}

int SDL2Draw::RenderDrawRect(SDL_Renderer *renderer, const Rect *rect)
{
	SDL_Rect _rect;
	_rect.x = rect->x; _rect.y = rect->y; _rect.w = rect->w; _rect.h = rect->h;
	return SDL_RenderDrawRect(renderer, &_rect);
}

int SDL2Draw::BlitSurfaceWithColorKey(SDL_Surface *src, const Rect *srcrect, SDL_Surface *dst, Rect *dstrect, Uint32 ColorKey)
{
	SDL_Rect __src_rect, __dest_rect;
	__src_rect.x = srcrect->x; __src_rect.y = srcrect->y; __src_rect.w = srcrect->w; __src_rect.h = srcrect->h;
	__dest_rect.x = dstrect->x; __dest_rect.y = dstrect->y; __dest_rect.w = dstrect->w; __dest_rect.h = dstrect->h;
	SDL_SetColorKey(src, SDL_TRUE, ColorKey);
	int __ret_value = SDL_BlitSurface(src, &__src_rect, dst, &__dest_rect);
	SDL_SetColorKey(src, SDL_FALSE, ColorKey);
	return __ret_value;
}

// ---- SDL2Input ----

inline bool _isalpha(int c)
{
	c = c % 0xFF;
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline int SDL2Input::__escape_charactor(int keymod, int key)
{
	if (key == SDLK_CAPSLOCK)
		return 0;
	if ((keymod & KMOD_CAPS) != 0 && _isalpha(key))
		if ((keymod & KMOD_SHIFT) == 0)
			return key + 'A' - 'a';
		else
			return key;
	else
		return key;
}

unsigned char SDL2Input::KeyPress()
{
	SDL2Input::ShownKeyboard();
	SDL_Event *Event = new SDL_Event;
	SDL_PollEvent(Event);
	if (Event->type == SDL_KEYDOWN) {
		return __escape_charactor(Event->key.keysym.mod, Event->key.keysym.sym);
	} else {
		return 0;
	}
}

int SDL2Input::WaitKey(bool OnlyKeyboard)
{
	SDL_Event *Event = new SDL_Event;
	while(1) {
		SDL2Input::ShownKeyboard();
		if (SDL_WaitEvent(Event) != 0) {
			if (Event->type == SDL_KEYDOWN && SDL_IsTextInputActive() == SDL_FALSE) {
				int kc = __escape_charactor(Event->key.keysym.mod, Event->key.keysym.sym);
				if (kc != 0)
					return kc;
			} else if (Event->type == SDL_MOUSEBUTTONDOWN && OnlyKeyboard == false) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				return y * 65536 + x;
			} else if (Event->type == SDL_QUIT) {
				exit(0);
			} else if (Event->type == SDL_TEXTINPUT) {
				int kc = __escape_charactor(Event->key.keysym.mod, Event->text.text[0]);
				if (kc != 0)
					return kc;
			}
		}
	}
}

void SDL2Input::ShownKeyboard()
{
	if (SDL_HasScreenKeyboardSupport() == SDL_TRUE)
		SDL_StartTextInput();
}

void SDL2Input::HiddenKeyboard()
{
	if (SDL_HasScreenKeyboardSupport() == SDL_TRUE)
		SDL_StopTextInput();
}
