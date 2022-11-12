#pragma once
#include "def.h"

//ウィンドウの当たり判定は、枠と上フレームと画面内が存在

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
		getRectF().drawFrame(1, Color(0));
		drawFlame();
	}
	virtual void drawFlame()
	{
		for (int i = 1; i < 10; i++)
		{
			if (i == 5)continue;
			Color c = (i % 2 == 0) ? Color(255, 0, 0) : Color(0, 255, 0);
			getFlameRectF(i).draw(c);
		}
		Vec2 v2 = Vec2(MARGIN_FLAME, MARGIN_FLAME);
		//タイトルバー
		{
			RectF rf = RectF(pos + v2, Vec2(size.x - MARGIN_FLAME * 2 - MARGIN_TITLEBAR_BUTTON * 3, MARGIN_TITLEBAR));
			rf.draw(Color(0, 255, 0));
			//各ボタン
			rf = RectF(Vec2(rf.rightX(), rf.pos.y), Vec2(MARGIN_TITLEBAR_BUTTON, MARGIN_TITLEBAR));
			rf.draw(Color(0, 0, 255));
			rf.moveBy(Vec2(MARGIN_TITLEBAR_BUTTON, 0));
			rf.draw(Color(0, 0, 200));
			rf.moveBy(Vec2(MARGIN_TITLEBAR_BUTTON, 0));
			rf.draw(Color(0, 0, 145));
		}
	}
	CLICKED_TYPE getPosType(Vec2 p)
	{
		//接触判定
		Vec2 v1 = Vec2(MARGIN_FLAME, -MARGIN_FLAME);
		Vec2 v2 = Vec2(MARGIN_FLAME, MARGIN_FLAME);

		//非接触
		if (!RectF(pos - v2, size + 2 * v2).contains(p))return CLICKED_TYPE::NONE;

		//メイン
		if (getContentsRectF().contains(p))
		{
			return CLICKED_TYPE::CONTENTS;
		}

		//タイトルバー
		{
			RectF rf = RectF(pos + v2, Vec2(size.x - MARGIN_FLAME * 2 - MARGIN_TITLEBAR_BUTTON * 3, MARGIN_TITLEBAR));
			if (rf.contains(p))return CLICKED_TYPE::TITLE_BAR;
			//各ボタン
			rf = RectF(Vec2(rf.rightX(), rf.pos.y), Vec2(MARGIN_TITLEBAR_BUTTON, MARGIN_TITLEBAR));
			if (rf.contains(p))return CLICKED_TYPE::T_BAR_MIN;
			rf.moveBy(Vec2(MARGIN_TITLEBAR_BUTTON, 0));
			if (rf.contains(p))return CLICKED_TYPE::T_BAR_MAX;
			rf.moveBy(Vec2(MARGIN_TITLEBAR_BUTTON, 0));
			if (rf.contains(p))return CLICKED_TYPE::T_BAR_CLOSE;
		}

		//枠
		RectF(pos + v1, Vec2(size.x, 0) - 2 * v1).draw(Color(255, 0, 0));
		RectF(pos + Vec2(0, size.y) + v1, Vec2(size.x, 0) - 2 * v1).draw(Color(255, 0, 0));
		RectF(pos - v1, Vec2(0, size.y) + 2 * v1).draw(Color(255, 0, 0));
		RectF(pos + Vec2(size.x, 0) - v1, Vec2(0, size.y) + 2 * v1).draw(Color(255, 0, 0));

		//角
		RectF(pos - v2, v2 * 2).draw(Color(0, 255, 0));
		RectF(pos + Vec2(0, size.y) - v2, v2 * 2).draw(Color(0, 255, 0));
		RectF(pos + Vec2(size.x, 0) - v2, v2 * 2).draw(Color(0, 255, 0));
		RectF(pos + size - v2, v2 * 2).draw(Color(0, 255, 0));

		return CLICKED_TYPE::CONTENTS;
	}
	//コンテンツやフレームの取得
	RectF getFlameRectF(char type)//1から9までの数値が数字キーに対応
	{
		Vec2 v1 = Vec2(MARGIN_FLAME, -MARGIN_FLAME);
		Vec2 v2 = Vec2(MARGIN_FLAME, MARGIN_FLAME);
		//枠か角かでまとめる
		if (type % 2 == 0)
		{
			//枠
			switch (type)
			{
			case 8:
				return RectF(pos + v1, Vec2(size.x, 0) - 2 * v1).draw(Color(255, 0, 0));
			case 2:
				return RectF(pos + Vec2(0, size.y) + v1, Vec2(size.x, 0) - 2 * v1);
			case 4:
				RectF(pos - v1, Vec2(0, size.y) + 2 * v1).draw(Color(255, 0, 0));
			case 6:
				RectF(pos + Vec2(size.x, 0) - v1, Vec2(0, size.y) + 2 * v1).draw(Color(255, 0, 0));
			default:
				break;
			}
		}
		else
		{
			//角
			switch (type)
			{
			case 7:
				RectF(pos - v2, v2 * 2).draw(Color(0, 255, 0));
			case 9:
				RectF(pos + Vec2(size.x, 0) - v2, v2 * 2).draw(Color(0, 255, 0));
			case 1:
				RectF(pos + Vec2(0, size.y) - v2, v2 * 2).draw(Color(0, 255, 0));
			case 3:
				RectF(pos + size - v2, v2 * 2).draw(Color(0, 255, 0));
			default:
				break;
			}
		}
		return RectF();
	}
	RectF getTitleBar(char type)//-1が全体、0がフレーム、1,2,3の順で最小化、最大化、閉じるボタン
	{
		RectF rf = RectF(pos + Vec2(MARGIN_FLAME, MARGIN_FLAME), Vec2(size.x - MARGIN_FLAME * 2, MARGIN_TITLEBAR));
		if (type == -1)return rf;
		rf.size = rf.size - Vec2(MARGIN_TITLEBAR_BUTTON * 3, 0);
		if (type == 0)return rf;
		rf = RectF(Vec2(rf.rightX(), rf.pos.y), Vec2(MARGIN_TITLEBAR_BUTTON, MARGIN_TITLEBAR));
		if (type == 1)return rf;
		rf.moveBy(Vec2(MARGIN_TITLEBAR_BUTTON, 0));
		if (type == 2)return rf;
		rf.moveBy(Vec2(MARGIN_TITLEBAR_BUTTON, 0));
		if (type == 3)return rf;
	}
	RectF getContentsRectF()
	{
		RectF rf = RectF(
			pos + Vec2(MARGIN_FLAME, MARGIN_FLAME + MARGIN_TITLEBAR),
			size + Vec2(-MARGIN_FLAME * 2, -MARGIN_TITLEBAR - MARGIN_FLAME * 2));
		return rf;
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

	RectF getRectF()
	{
		return RectF(pos, size);
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
