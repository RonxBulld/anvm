#pragma once

typedef struct Point
{
	int x;
	int y;
} Point;

typedef struct Rect
{
	int x;
	int y;
	int w;
	int h;
} Rect;

typedef uint32_t Uint32;
typedef uint8_t Uint8;
struct SDL_Window;
struct SDL_Surface;
struct SDL_Point;
struct SDL_Renderer;
struct SDL_PixelFormat;
struct SDL_RWops;

class Screen
{
private:
    int width, height;
    int FontWidth, FontHeigth;
    SDL_Window *MainWnd;
    SDL_Surface *ScreenSurface, *buffer;
    void *Font, *DebugFont;
    void *RefreshThread;
    char *FontFile;
    bool Transparent;
    Uint32 TextColor, BackgroundColor;
    Point OutputPosition;

    void __set_font(const char *FontFile, const int Size);
    Rect ShowText(SDL_Surface *Text, int x, int y, int width, int height, SDL_Surface *Dest);
    bool IsUTF8(const void *Str, long Size);
    char *GB2312toUTF8(const char *GB2312str);
    void CheckPosition();
    void DrawDebugLayer(SDL_Surface *dst, float FPS);
public:
    Screen(int width, int height);
    ~Screen();

    void StartRefreshThread();
    void Refresh(Rect *RefRect = NULL);
    void Delay(int ticks);
    void SetScreenSize(unsigned int width, unsigned int height);
    void GetScreenSize(int *width, int *height);
    void CleanScreen();
    void Display(const char *Text, int x = -1, int y = -1);
    void Display(const int Integer, int x = -1, int y = -1);
    void Display(const float Float, int x = -1, int y = -1);
    void Display(const char Charactor, int x = -1, int y = -1);
    void NewLine();
    void SetTransparent(bool value);
    Point GetDispPosition();
    void SetDispPosition(int x, int y);
    SDL_Surface *GetScreenSurface();
    void SetTextLocation(int l, int r);
    void SetTextColor(Uint32 FrontColor, Uint32 BackColor);
    void SetFont(int FontID);
    int GetFontWidth();
    int GetFontHeigth();
    SDL_Window *GetMainWindow();
};

class SDL2Function
{
public:
	static void DestroyRenderer(SDL_Renderer *renderer);
	static SDL_Surface *CreateRGBSurface(Uint32 flags, int width, int height);
	static SDL_PixelFormat *GetFormat(SDL_Surface *surface);
	static Uint32 MapRGB(const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b);
	static void FreeSurface(SDL_Surface *surface);
	static SDL_Surface *LoadBMP_RW(SDL_RWops *src, int freesrc);
	static SDL_RWops *RWFromConstMem(const void *mem, int size);
	static int GetSurfaceWidth(SDL_Surface *surface);
	static int GetSurfaceHeight(SDL_Surface *surface);
	static void *GetSurfacePixels(SDL_Surface *surface);
	static bool MustLock(SDL_Surface *surface);
	static int LockSurface(SDL_Surface * surface);
	static void UnlockSurface(SDL_Surface * surface);
	static SDL_Renderer *CreateSoftwareRenderer(SDL_Surface *surface);
	static int SetRenderDrawColor(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	static int GetRenderDrawColor(SDL_Renderer *renderer, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
	static int GetAllowCharNum(const char *str);
};

class SDL2Draw
{
public:
	static int FillRect(SDL_Surface *dst, const Rect *rect, Uint32 color);
	static int BlitSurfaceWithColorKey(SDL_Surface *src, const Rect *srcrect, SDL_Surface *dst, Rect *dstrect, Uint32 ColorKey);
	static int RenderDrawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2);
	static void RenderPresent(SDL_Renderer *renderer);
	static int RenderDrawRect(SDL_Renderer *renderer, const Rect *rect);
};

class SDL2Input
{
public:
	static inline int __escape_charactor(int keymod, int key);
	static unsigned char KeyPress();
	static int WaitKey(bool OnlyKeyboard = false);
	static void ShownKeyboard();
	static void HiddenKeyboard();
};
