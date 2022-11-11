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
