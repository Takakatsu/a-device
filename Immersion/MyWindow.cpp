#include "def.h"

MyWindow::MyWindow()
{
	pos = Vec2(0, 0);
	size = Vec2(0, 0);
}
MyWindow::MyWindow(Vec2 p, Vec2 s)
{
	pos = p;
	size = s;
}

void MyWindow::update()
{

}

void MyWindow::draw()
{
	RectF(pos, size).drawFrame(1, Color(0));
}

void MyWindow::setPos(Vec2 p)
{
	pos = p;
}

void MyWindow::setSize(Vec2 s)
{
	size = s;
}
