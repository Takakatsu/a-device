#pragma once
#include "def.h"
#include "MyWindow.h"

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
	Robot* selected_robo = nullptr;//選択されている機械
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

			//UI
			{
				double t = size.x / 4;
				RectF robo_rect = RectF(Vec2(getContentsRectF().size.x * 7 / 8, -((int)robots_stay.size() - 1) * 0.5 * t + getContentsRectF().size.y / 2), Vec2(t, t));
				for (int i = 0; i < robots_stay.size(); i++)
				{
					RectF rf = robo_rect.movedBy(-robo_rect.size / 2).scaled(0.9);
					if (rf.contains(pos))
					{
						selected_robo = &(robots_stay[i]);
						return;
					}
					robo_rect.moveBy(Vec2(0, t));
				}
			}

			//メインの地図
			//選択している機械があった場合、それを利用し探索
			if (selected_robo != nullptr)
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
								search_map(Point(i, j), selected_robo);
								selected_robo = nullptr;//エラー対策
								return;
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
									search_map(Point(i, j), selected_robo);
									selected_robo = nullptr;//エラー対策
									return;
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

			Vec2 c_pos = cursor_pos - getContentsRectF().pos;//カーソル座標の取得
			//メインの地図
			{
				const Transformer2D t_cam{ Mat3x2::Translate(pos_camera) };
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

			//ロボットUIの描画
			{
				RectF(Vec2(getContentsRectF().size.x * 3 / 4, -10), getContentsRectF().size + Vec2(10, 20)).draw(ColorF(1.0, 1.0, 1.0, 0.5));//背景
				double t = size.x / 4;
				RectF robo_rect = RectF(Vec2(getContentsRectF().size.x * 7 / 8, -((int)robots_stay.size() - 1) * 0.5 * t + getContentsRectF().size.y / 2), Vec2(t, t));
				for (int i = 0; i < robots_stay.size(); i++)
				{
					RectF rf = robo_rect.movedBy(-robo_rect.size / 2).scaled(0.9);
					rf.draw((selected_robo == &robots_stay[i]) ? Color(0, 0, 255) : (rf.contains(c_pos)) ? Color(0, 255, 0) : Color(255, 0, 0));
					//font01(robots_stay[i].)
					robo_rect.moveBy(Vec2(0, t));
				}
			}
		}
		drawFlame();
	}
};

class CommandPrompt : public MyWindow
{
private:
	TextEditState txtstt;
	Font font01;
	Array<String> clogs;
	Color col_bg = Color(0), col_txt = Color(0, 255, 0);
	String dir = U"s: > ";
protected:
public:
	CommandPrompt() : MyWindow()
	{
		font01 = Font(20);
	};
	CommandPrompt(Vec2 p, Vec2 s) : MyWindow(p, s)
	{
		font01 = Font(20);
	};
	void update()
	{
		SimpleGUI::TextBox(txtstt, Vec2(-100, -100));
		if (win_active == this)
		{
			txtstt.active = true;
		}
		if (txtstt.enterKey)
		{
			clogs.push_front(dir + txtstt.text);

			bool is_cmd = false;
			//ここでコマンドの処理をしなければならない
			Array<String> strs = txtstt.text.split(' ');
			if (strs.size() > 0)
			{
				if (strs[0] == U"help")
				{
					is_cmd = true;
					clogs.push_front(U"\"log\", \"resource\"");
				}
				if (strs[0] == U"log")
				{
					is_cmd = true;
					for (int i = 0; i < logs.size(); i++)
					{
						clogs.push_front(U"  " + logs[i].text);
					}
				}
				if (strs[0] == U"resource")
				{
					is_cmd = true;
					bool is_printed = false;
					for (int i = 0; i < (int)ITEMTYPE::IT_NUM; i++)
					{
						if (ItemBox[(ITEMTYPE)i] > 0)
						{
							is_printed = true;
							clogs.push_front(U"  " + ItemLib[(ITEMTYPE)i].name + U" " + Format(ItemBox[(ITEMTYPE)i])+U"kg");
						}
					}
					if (!is_printed)clogs.push_front(U"there is no resource");
				}
			}
			if (!is_cmd)
			{
				clogs.push_front(U"undefined command. Prease Type \"help\" Command.");
			}

			txtstt.clear();
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
			RectF(Vec2(-10, -10), size + Vec2(20, 20)).draw(col_bg);//背景

			Vec2 basePos = Vec2();
			Vec2 penPos = Vec2(basePos);
			//入力中の奴
			{
				int i = -(int)dir.size();
				for (const auto& glyph : font01.getGlyphs(dir + txtstt.text))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.x = basePos.x;
						penPos.y += font01.height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = basePos.x;
						penPos.y += font01.height();
					}
					glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), col_txt);
					penPos.x += glyph.xAdvance;
					if (i == txtstt.cursorPos - 1)
					{
						RectF(penPos, Vec2(10, font01.height())).draw(col_txt);
					}
					i++;
				}
				penPos.x = basePos.x;
				penPos.y += font01.height();

			}
			//過去ログ
			for (int i = 0; i < clogs.size(); i++)
			{
				for (const auto& glyph : font01.getGlyphs(clogs[i]))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.x = basePos.x;
						penPos.y += font01.height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = basePos.x;
						penPos.y += font01.height();
					}
					glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), col_txt);
					penPos.x += glyph.xAdvance;
				}
				penPos.x = basePos.x;
				penPos.y += font01.height();
				if (penPos.y > rect.h)
				{
					break;
				}
			}
		}
		drawFlame();
	}
};
