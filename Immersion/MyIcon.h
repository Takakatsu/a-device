#pragma once
#include "def.h"

class MyIcon
{
private:
protected:
	Point pos;
	MyWindow* win;
public:
	MyIcon()
	{
		pos = Point();
		win = nullptr;
	}
	MyIcon(Point p,MyWindow* w)
	{
		pos = Point(p);
		win = w;
	}
	virtual void click()
	{
		if (!my_wins.includes(win))
		{
			*win = MyWindow(Vec2(300, 300), Vec2(200, 200));
			my_wins.push_front(win);
		}
		else
		{
			win_active = win;
			my_wins.remove(win);
			my_wins.push_front(win_active);
		}
	}
	virtual void draw()
	{
		getRect().draw(Palette::Beige);
	}
	RectF getRect()
	{
		return RectF(pos * MARGIN_ICON + Vec2(16, 16), Vec2(ICON_SIZE, ICON_SIZE));
	}
};
