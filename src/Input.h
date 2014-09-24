#pragma once
#include <string>
#include <vector>

class Line
{
public:
	Point PrintPosition;
	string LineText;
};

class Keyboard
{
private:
	unsigned char *GetKeyString();
	vector<Line*> *InputBuffer;
	int InputNewline(Point Ptr, int ScnWidth, int CharWidth);
public:
	Keyboard();
	~Keyboard();

	unsigned int GetInteger();
	unsigned char *GetString();
	float GetFloat();
};
