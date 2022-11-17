#pragma once
#include "def.h"

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
			const ScopedViewport2D viewport(rect);
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
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(rect);
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(0, 0), size).draw(Color(0));
			Circle(Cursor::Pos(), 100).draw(Color(255, 255, 255));
		}
		drawFlame();
	}
};

class MailSoft : public MyWindow
{
private:
	Font font01;
	bool is_reading;//メールを読んでるか否か
	int mail_num;//選択中のメールの番号
public:
	MailSoft() : MyWindow()
	{
		font01 = Font(30);
		is_reading = false;
		mail_num = 0;
	};
	MailSoft(Vec2 p, Vec2 s) : MyWindow(p, s)
	{
		font01 = Font(30);
		is_reading = false;
		mail_num = 0;
	};
	void draw()
	{
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(rect);
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(0, 0), size).draw(Color(127));//背景
			if (is_reading)
			{

			}
			else
			{
				RectF mailline = RectF(Vec2(0, 0), Vec2(180, 50));
				for (int i = 0; i < MailLib.size(); i++)
				{
					font01(MailLib[i].from).draw(mailline, Color(0));
					mailline.moveBy(Vec2(0, 50));
				}
			}
		}
		drawFlame();
	}
};

class MAPViewer : public MyWindow
{
private:
	Font font01;
	Vec2 pos_camera;
	const double r = 10;//図形の半径
	const double m_r = 12;//マージンの半径
public:
	MAPViewer() : MyWindow()
	{
		font01 = Font(30);
		pos_camera = Vec2(0, 0);
	};
	MAPViewer(Vec2 p, Vec2 s) : MyWindow(p, s)
	{
		font01 = Font(30);
		pos_camera = Vec2(0, 0);
	};
	void click(Vec2 pos, bool is_left)
	{
		if (is_left)
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(rect);
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };

			//メインの地図
			{
				const Transformer2D t_cam{ Mat3x2::Translate(pos_camera) };
				for (int i = 0; i < MAINMAP.size(); i++)
				{
					for (int j = 0; j < MAINMAP[i].size(); j++)
					{
						//各種タイル
						Vec2 dis = Vec2(
							(i - MAP_CENTER_X) * m_r * 1.5,
							(j - MAP_CENTER_Y + ((i % 2 == 0) ? 0 : 0.5)) * m_r * sqrt(3));
						Polygon poly = Shape2D::Hexagon(r, rect.size / 2 + dis, 30_deg)/*.draw(TileLib[MAINMAP[i][j].tile].c)*/.asPolygon();
						if (poly.contains(pos - pos_camera))
						{
							//発見済みの座標の処理
							if (MAINMAP[i][j].is_found)
							{
							}
							//未発見の座標の処理
							else
							{
								//発見済みの座標との隣接判定
								bool is_touch = false;
								{
									Array<Point> ps;
									if (i % 2 == 0)ps = { Point(0, 1), Point(0, -1), Point(-1, -1), Point(-1, 0), Point(1, -1), Point(1, 0) };
									else ps = { Point(0, 1), Point(0, -1), Point(-1, 1), Point(-1, 0), Point(1, 1), Point(1, 0) };
									for (int k = 0; k < ps.size(); k++)
									{
										Point p = Point(i, j) + ps[k];
										if (0 <= p.x && p.x < MAINMAP.size())
										{
											if (0 <= p.y && p.y < MAINMAP[p.x].size())
											{
												if (MAINMAP[p.x][p.y].is_found)
												{
													is_touch = true;
													break;
												}
											}
										}
									}
								}
								if (is_touch)
								{
									//仮で強制発見処理
									MAINMAP[i][j].is_found = true;
								}
							}
						}
					}
				}
			}
		}
	}
	void drag(Vec2 delta, bool is_left)
	{
		if (!is_left)
		{
			pos_camera += delta;
		}
	}
	void draw()
	{
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(rect);
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(-10, -10), size + Vec2(20, 20)).draw(Color(0));//背景

			//ロボットUIの描画
			{
				for (int i = 0; i < robots_stay.size(); i++)
				{
					rect.w / 4;
					RectF(Vec2(rect.w * 3 / 4, -10), size + Vec2(10, 20)).draw(Color(255));//背景
				}
			}

			//メインの地図
			{
				const Transformer2D t_cam{ Mat3x2::Translate(pos_camera) };
				const Transformer2D t_cam2{ Mat3x2::Identity(), Mat3x2::Translate(-2 * pos_camera) };
				for (int i = 0; i < MAINMAP.size(); i++)
				{
					for (int j = 0; j < MAINMAP[i].size(); j++)
					{
						//発見済みの座標
						if (MAINMAP[i][j].is_found)
						{
							Vec2 dis = Vec2(
								(i - MAP_CENTER_X) * m_r * 1.5,
								(j - MAP_CENTER_Y + ((i % 2 == 0) ? 0 : 0.5)) * m_r * sqrt(3));
							Shape2D::Hexagon(r, rect.size / 2 + dis, 30_deg).draw(TileLib[MAINMAP[i][j].tile].c);
						}
						//未発見の座標
						else
						{
							//周囲の描画(発見済みの座標との隣接)
							Array<Point> ps;
							if (i % 2 == 0)ps = { Point(0, 1), Point(0, -1), Point(-1, -1), Point(-1, 0), Point(1, -1), Point(1, 0) };
							else ps = { Point(0, 1), Point(0, -1), Point(-1, 1), Point(-1, 0), Point(1, 1), Point(1, 0) };
							Vec2 c_pos = cursor_pos - getContentsRectF().pos;//カーソル座標の取得
							for (int k = 0; k < ps.size(); k++)
							{
								Point p = Point(i, j) + ps[k];
								if (0 <= p.x && p.x < MAINMAP.size())
								{
									if (0 <= p.y && p.y < MAINMAP[p.x].size())
									{
										if (MAINMAP[p.x][p.y].is_found)
										{
											Vec2 dis = Vec2(
												(i - MAP_CENTER_X) * m_r * 1.5,
												(j - MAP_CENTER_Y + ((i % 2 == 0) ? 0 : 0.5)) * m_r * sqrt(3));
											//マウスオーバー判定等
											Shape2D shape = Shape2D::Hexagon(r, rect.size / 2 + dis, 30_deg);
											shape.draw(shape.asPolygon().contains(c_pos - pos_camera) ? Color(255) : Color(127));
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		drawFlame();
	}
};
