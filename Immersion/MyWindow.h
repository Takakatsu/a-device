#pragma once
#include "def.h"
#include "func.h"

class MyWindow
{
private:
protected:
	bool is_min;//最小化か否か
	bool is_max;//最大化か否か
	Vec2 pos;//座標
	Vec2 pos_tmp;//最大化されていない座標
	Vec2 size;//ウィンドウサイズ
	Vec2 size_tmp;//最大化されていないサイズ
	Vec2 size_min;//サイズ最小値
public:
	MyWindow()
	{
		is_max = false;
		is_min = false;
		pos_tmp = Vec2();
		size_tmp = Vec2();
		size_min = Vec2(MARGIN_FLAME * 2 + MARGIN_TITLEBAR_BUTTON * 3, MARGIN_FLAME * 2 + MARGIN_TITLEBAR);
		pos = Vec2(0, 0);
		size = Vec2(0, 0);
	}
	MyWindow(Vec2 p, Vec2 s)
	{
		is_max = false;
		is_min = false;
		pos_tmp = Vec2();
		size_tmp = Vec2();
		size_min = Vec2(MARGIN_FLAME * 2 + MARGIN_TITLEBAR_BUTTON * 3, MARGIN_FLAME * 2 + MARGIN_TITLEBAR);
		pos = p;
		size = s;
	}
	virtual void update()
	{

	}
	virtual void draw()
	{
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			Circle(Cursor::Pos(), 100).draw(Color(255, 255, 0));
		}
		drawFlame();
	}
	virtual void drawFlame()
	{
		//枠
		for (int i = 1; i < 10; i++)
		{
			if (i == 5)continue;
			Color c = (i % 2 == 0) ? Color(255, 0, 0) : Color(0, 255, 0);
			getFlameRectF(i).draw(c);
		}
		//タイトルバー
		for (int i = 0; i < 4; i++)
		{
			getTitleBar(i).draw(Color(0, 0, (i * 50 + 30)));
		}
	}
	virtual void click(Vec2 pos, bool is_left)//呼び出すときは内部座標で処理する
	{
		Print(pos);
	}
	virtual void drag(Vec2 delta, bool is_left)
	{

	}

	//コンテンツやフレームの取得
	CLICKED_TYPE getPosType(Vec2 p)
	{
		//非接触
		if (!getAllRectF().contains(p))return CLICKED_TYPE::NONE;

		//メイン
		if (getContentsRectF().contains(p))
		{
			return CLICKED_TYPE::CONTENTS;
		}
		//枠
		{
			Array<CLICKED_TYPE> cts = {
				CLICKED_TYPE::FLAME_DOWN_LEFT,CLICKED_TYPE::FLAME_DOWN,CLICKED_TYPE::FLAME_DOWN_RIGHT,
				CLICKED_TYPE::FLAME_LEFT,CLICKED_TYPE::NONE,CLICKED_TYPE::FLAME_RIGHT,
				CLICKED_TYPE::FLAME_UP_LEFT,CLICKED_TYPE::FLAME_UP,CLICKED_TYPE::FLAME_UP_RIGHT
			};
			for (int i = 0; i < 9; i++)
			{
				if (i == 4)continue;
				if (getFlameRectF(i + 1).contains(p))return cts[i];
			}
		}
		//タイトルバー
		{
			Array<CLICKED_TYPE> cts = {
				CLICKED_TYPE::TITLE_BAR,CLICKED_TYPE::T_BAR_MIN,CLICKED_TYPE::T_BAR_MAX,CLICKED_TYPE::T_BAR_CLOSE
			};
			for (int i = 0; i < cts.size(); i++)
			{
				if (getTitleBar(i).contains(p))return cts[i];
			}
		}

		return CLICKED_TYPE::NONE;
	}
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
				return RectF(pos + v1, Vec2(size.x, 0) - 2 * v1);
			case 2:
				return RectF(pos + Vec2(0, size.y) + v1, Vec2(size.x, 0) - 2 * v1);
			case 4:
				return RectF(pos - v1, Vec2(0, size.y) + 2 * v1);
			case 6:
				return RectF(pos + Vec2(size.x, 0) - v1, Vec2(0, size.y) + 2 * v1);
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
				return RectF(pos - v2, v2 * 2);
			case 9:
				return RectF(pos + Vec2(size.x, 0) - v2, v2 * 2);
			case 1:
				return RectF(pos + Vec2(0, size.y) - v2, v2 * 2);
			case 3:
				return RectF(pos + size - v2, v2 * 2);
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
	RectF getAllRectF()
	{
		return RectF(pos - Vec2(MARGIN_FLAME, MARGIN_FLAME), size + 2 * Vec2(MARGIN_FLAME, MARGIN_FLAME));
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
		if (size.x < size_min.x)size.x = size_min.x;
		if (size.y < size_min.y)size.y = size_min.y;
	}
	Vec2 getSize()
	{
		return size;
	}
	//タイトルバーのボタン処理
	void dealSizeMax()
	{
		if (is_max)//最大化されている場合
		{
			pos = pos_tmp;
			size = size_tmp;
		}
		else//最大化する場合
		{
			pos_tmp = pos;
			size_tmp = size;
			pos = Vec2(0, 0);
			size = Scene::Size();
		}
		is_max = !is_max;
	}
	void dealSizeMin()
	{
		is_min = true;
	}
};
