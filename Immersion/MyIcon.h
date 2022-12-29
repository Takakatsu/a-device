#pragma once
#include "def.h"

class MyIcon
{
private:
protected:
	String texture;
	String name;
	Point pos;
	Vec2 start_size;
	Vec2 min_size;
	MyWindow* win;
public:
	MyIcon()
	{
		pos = Point();
		start_size = Vec2();
		win = nullptr;
	}
	MyIcon(Point p, MyWindow* w, String str_tex, String str_name, Vec2 size_defo,Vec2 size_min)
	{
		pos = Point(p);
		start_size = size_defo;
		min_size = size_min;
		win = w;
		texture = str_tex;
		name = str_name;
	}
	virtual void click()
	{
		if (!my_wins.includes(win))
		{
			Point p = (newWin_pos + start_size).asPoint();
			if (p.x > Scene::Width())newWin_pos.x = 64;
			if (p.y > Scene::Height())newWin_pos.y = 64;
			*win = MyWindow(name, newWin_pos, start_size, min_size);
			my_wins.push_front(win);
			newWin_pos += Point(1, 1) * 64;
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
			Point p = (newWin_pos + start_size).asPoint();
			if (p.x > Scene::Width())newWin_pos.x = 64;
			if (p.y > Scene::Height())newWin_pos.y = 64;
			*win = MyWindow(name, newWin_pos, start_size, min_size);
			my_wins.push_front(win);
			newWin_pos += Point(1, 1) * 64;
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
