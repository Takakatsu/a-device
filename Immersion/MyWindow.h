#pragma once
#include "def.h"

class MyWindow
{
private:
protected:
	Vec2 pos;//座標
	Vec2 size;//ウィンドウサイズ
public:
	MyWindow()
	{
		pos = Vec2(0, 0);
		size = Vec2(0, 0);
	}
	MyWindow(Vec2 p, Vec2 s)
	{
		pos = p;
		size = s;
	}
	virtual void update()
	{

	}
	virtual void draw()
	{
		RectF(pos, size).drawFrame(1, Color(0));
	}
	RectF getRectF()
	{
		return RectF(pos, size);
	}
	void setPos(Vec2 p)
	{
		pos = p;
	}
	Vec2 getPos()
	{
		return pos;
	}
	void setSize(Vec2 s)
	{
		size = s;
	}
	Vec2 getSize()
	{
		return size;
	}
};

class Calculator : public MyWindow
{
private:
protected:
public:
	Calculator() : MyWindow() {};
	Calculator(Vec2 p, Vec2 s) : MyWindow(p, s) {};
	void update()
	{
	}
	void draw()
	{
		RectF(pos, size).draw(Color(127));
	}
};
