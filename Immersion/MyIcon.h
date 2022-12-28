#pragma once
#include "def.h"

class MyIcon
{
private:
protected:
	String texture;
	String name;
	Point pos;
	MyWindow* win;
public:
	MyIcon()
	{
		pos = Point();
		win = nullptr;
	}
	MyIcon(Point p, MyWindow* w, String str_tex,String str_name)
	{
		pos = Point(p);
		win = w;
		texture = str_tex;
		name = str_name;
	}
	virtual void click()
	{
		if (!my_wins.includes(win))
		{
			*win = MyWindow(name,Vec2(300, 300), Vec2(200, 200));
			my_wins.push_front(win);
		}
		else
		{
			if (!my_wins.isEmpty())
			{
				//一番上のウィンドウだった場合は場所をリセット
				if (my_wins[0] == win)
				{
					win->setPos(Point(0, 0));
				}
			}
			win_active = win;
			my_wins.remove(win);
			my_wins.push_front(win_active);
			if (win_active->getIsMin())win_active->dealSizeMin();
		}
	}
	virtual void click_undericon()
	{
		if (!my_wins.includes(win))
		{
			*win = MyWindow(name,Vec2(300, 300), Vec2(200, 200));
			my_wins.push_front(win);
		}
		else
		{
			if (!my_wins.isEmpty())
			{
				//一番上のウィンドウだった場合は場所をリセット
				if (my_wins[0] == win)
				{
					win->setPos(Point(0, 0));
				}
			}
			win_active = win;
			my_wins.remove(win);
			my_wins.push_front(win_active);
			if (win_active->getIsMin())win_active->dealSizeMin();
		}
	}
	virtual void draw()
	{
		TextureLib[texture].draw(getRect().pos);
	}
	RectF getRect()
	{
		return RectF(pos * MARGIN_ICON + Vec2(16, 16), Vec2(ICON_SIZE, ICON_SIZE));
	}
	String getTexture()
	{
		return texture;
	}
};
