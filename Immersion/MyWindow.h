#pragma once
#include "def.h"

class MyWindow
{
private:
protected:
	Vec2 pos;
	Vec2 size;
public:
	MyWindow();
	MyWindow(Vec2, Vec2);
	virtual void update();
	virtual void draw();
	void setPos(Vec2);
	void setSize(Vec2);
};

class Calculator : public MyWindow
{
private:
protected:
public:
	Calculator() :MyWindow() {};
	Calculator(Vec2 p, Vec2 s) : MyWindow(p, s) {};
	void update()
	{
	}
	void draw()
	{
		RectF(pos, size).draw(Color(127));
	}
};
