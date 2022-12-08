﻿#pragma once
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
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
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
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
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

class Inventor : public MyWindow
{
private:
	Font font01;
	TextEditState txtstt;
	bool is_buying = false;
	Robot robo;
public:
	Inventor() : MyWindow()
	{
		font01 = Font(20);
	};
	Inventor(Vec2 p, Vec2 s) : MyWindow(p, s)
	{
		font01 = Font(20);
	};
	RectF getSelectionRect(int i)
	{
		double margin = 10;
		Vec2 s_size = Vec2(getContentsRectF().w - margin * 2, 30);
		return RectF(Vec2(margin, margin + (margin + s_size.y) * i), s_size).movedBy(getContentsRectF().pos);
	}
	RectF getButtonRect()
	{
		RectF rf = getContentsRectF();
		RectF btn = RectF(Vec2(rf.w / 2, rf.h * 2 / 3), Vec2(30, 20));
		btn.moveBy(-btn.size / 2);
		return btn;
	}
	void update()
	{
		SimpleGUI::TextBox(txtstt, Vec2(-100, -100));
		if (win_active == this && is_buying)
		{
			txtstt.active = true;
			if (txtstt.enterKey)
			{
				buy();
			}
		}
	}
	void buy()
	{
		robo.name = txtstt.text;
		robots_stay.push_back(robo);
		is_buying = false;
	}
	void click(Vec2 pos, bool is_left)
	{
		if (is_buying)
		{
			if (getButtonRect().contains(pos))
			{
				buy();
			}
		}
		else
		{
			for (int i = 0; i < (int)ROBOTTYPE::RT_NUM; i++)
			{
				bool is_creatable = true;
				for (int j = 0; j < RobotLib[(ROBOTTYPE)i].materials.size(); j++)
				{
					if (ItemBox[RobotLib[(ROBOTTYPE)i].materials[j].it] < RobotLib[(ROBOTTYPE)i].materials[j].amount)is_creatable = false;
				}
				if (is_creatable && getSelectionRect(i).movedBy(-getContentsRectF().pos).contains(pos))
				{
					Robot rb;
					rb.count_go = 0;
					rb.endurance = RobotLib[(ROBOTTYPE)i].max_endurance;
					rb.remain_time = 0;
					rb.rt = (ROBOTTYPE)i;
					robo = rb;
					is_buying = true;
					break;
				}
			}
		}
	}
	void draw()
	{
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(0, 0), size).draw(Color(32));//背景
			for (int i = 0; i < (int)ROBOTTYPE::RT_NUM; i++)
			{
				bool is_creatable = true;
				for (int j = 0; j < RobotLib[(ROBOTTYPE)i].materials.size(); j++)
				{
					if (ItemBox[RobotLib[(ROBOTTYPE)i].materials[j].it] < RobotLib[(ROBOTTYPE)i].materials[j].amount)is_creatable = false;
				}
				getSelectionRect(i).movedBy(-getContentsRectF().pos).draw(is_creatable ? Color(255) : Color(127));
			}
			//購入中は
			if (is_buying)
			{
				RectF(Vec2(0, 0), size).draw(ColorF(0.0, 0.0, 0.0, 0.5));
				RectF rf = getContentsRectF();
				font01(U"Enter the Name").drawAt(Vec2(rf.w / 2, rf.h / 3), Color(255));
				font01(txtstt.text).drawAt(Vec2(rf.w / 2, rf.h / 2), Color(255));
				getButtonRect().draw(getButtonRect().mouseOver() ? Color(255) : Color(127));
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
	double scale = 1;//拡大量
	double scroll = 0;//機械スクロール
	Robot* selected_robo = nullptr;//選択されている機械
public:
	MAPViewer() : MyWindow()
	{
		font01 = Font(20);
		pos_camera = Vec2(0, 0);
	};
	MAPViewer(Vec2 p, Vec2 s) : MyWindow(p, s)
	{
		font01 = Font(20);
		pos_camera = Vec2(0, 0);
	};
	double get_right_panel_width()
	{
		return getContentsRectF().w / 4;
	}
	RectF right_panel()
	{
		double width = get_right_panel_width();
		double s_x = width + 10;
		double s_y = getContentsRectF().h + 20;
		double p_x = getContentsRectF().rightX() - width;
		double p_y = getContentsRectF().y - 10;
		return RectF(Vec2(p_x, p_y), Vec2(s_x, s_y));
	}
	RectF right_panel_icons(int n)
	{
		RectF robo_rect = RectF(Vec2(getContentsRectF().rightX() - get_right_panel_width() / 2, getContentsRectF().y + 30 + scroll), Vec2(get_right_panel_width() - 10, 60));
		return robo_rect.movedBy(Vec2(0, 65 * n)).movedBy(-robo_rect.size / 2);
	}
	void update()
	{
		if (win_active == this)
		{
			if (right_panel().contains(cursor_pos))
			{
				scroll -= Mouse::Wheel() * 10;
			}
			else
			{
				scale -= Mouse::Wheel() * 0.1;
				scale = Clamp(scale, 1.0, 5.0);
			}
		}
	}
	void click(Vec2 pos, bool is_left)
	{
		if (is_left)
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };

			//UI
			{
				for (int i = 0; i < robots_stay.size(); i++)
				{
					RectF rf = right_panel_icons(i).movedBy(-getContentsRectF().pos);
					if (rf.contains(pos))
					{
						selected_robo = &(robots_stay[i]);
						return;
					}
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
						Polygon poly = Shape2D::Hexagon(r * scale, rect.size / 2 + dis * scale, 30_deg).asPolygon();
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
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
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
							Shape2D::Hexagon(r * scale, rect.size / 2 + dis * scale, 30_deg).draw(TileLib[MAINMAP[i][j].tile].c);
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
											Shape2D shape = Shape2D::Hexagon(r * scale, rect.size / 2 + dis * scale, 30_deg);
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
				right_panel().movedBy(-getContentsRectF().pos).draw(ColorF(1.0, 1.0, 1.0, 0.5));//背景
				for (int i = 0; i < robots_stay.size(); i++)
				{
					RectF rf = right_panel_icons(i).movedBy(-getContentsRectF().pos);
					rf.draw((selected_robo == &robots_stay[i]) ? Color(0, 0, 255) : (rf.contains(c_pos)) ? Color(0, 255, 0) : Color(255, 0, 0));
					font01(robots_stay[i].name).draw(rf.pos);
					font01(RobotLib[robots_stay[i].rt].name).draw(rf.pos + Vec2(0, rf.size.y / 2));
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
	double scroll = 0;
	const String dir = U"s: > ";
	const String indent = U"   ";
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
		if (getContentsRectF().contains(cursor_pos))
		{
			scroll -= Mouse::Wheel() * 10;
		}
		if (txtstt.textChanged && (-scroll < 0 || getContentsRectF().h < -scroll))
		{
			scroll = 0;
		}
		if (txtstt.enterKey)
		{
			clogs.push_front(dir + txtstt.text);

			bool is_cmd = false;
			bool is_too_many_arg = false;
			//ここでコマンドの処理をしなければならない
			Array<String> strs = txtstt.text.split(' ');
			int len = strs.size();
			if (len > 0)
			{
				if (strs[0] == U"help")
				{
					is_cmd = true;
					if (len == 1)
					{
						clogs.push_front(U"\"clear\", \"date\", \"echo\", \"eval\", \"help\", \"log\", \"machine\", \"resource\"");
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"clear")
				{
					is_cmd = true;
					if (len == 1)
					{
						clogs.clear();
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"date")
				{
					is_cmd = true;
					if (len == 1)
					{
						const DateTime t = DateTime::Now();
						clogs.push_front(
							Format(t.year + 1372) + U" " + Format(t.month / 2 + 1) + U" "
							+ Format(t.month % 2 + 1) + U" " + Format(t.day) + U" "
							+ Format(t.hour / 6) + U":" + Format(t.hour % 6) + U":"
							+ Format(t.minute / 15) + U":" + Format(t.minute % 15) + U":"
							+ Format(t.second / 2));
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"echo")
				{
					is_cmd = true;
					strs.pop_front();
					if (strs.isEmpty())
					{
						clogs.push_front(U"There is no argument. Please Retype Command.");
					}
					else
					{
						String expl = strs.join(U" ", U"\0", U"\0");
						clogs.push_front(indent + expl);
					}
				}
				if (strs[0] == U"eval")
				{
					is_cmd = true;
					strs.pop_front();
					if (strs.isEmpty())
					{
						clogs.push_front(U"There is no argument. Please Retype Command.");
					}
					else
					{
						String expl = strs.join(U" ", U"\0", U"\0");
						clogs.push_front(indent + expl);
						clogs.push_front(indent + Format(Eval(expl)));
					}
				}
				if (strs[0] == U"log")
				{
					is_cmd = true;
					if (len == 1)
					{
						for (int i = 0; i < logs.size(); i++)
						{
							clogs.push_front(indent + logs[i].text);
						}
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"machine")
				{
					is_cmd = true;
					if (len == 1)
					{
						if (robots_active.isEmpty())clogs.push_front(U"There is no active machine.");
						else clogs.push_front(U"Active machine.");
						for (int i = 0; i < robots_active.size(); i++)
						{
							clogs.push_front(indent + robots_active[i].rb.name + U" " + RobotLib[robots_active[i].rb.rt].name);
						}
						if (robots_stay.isEmpty())clogs.push_front(U"There is no inactive machine.");
						else clogs.push_front(U"Inactive machine.");
						for (int i = 0; i < robots_stay.size(); i++)
						{
							clogs.push_front(indent + robots_stay[i].name + U" " + RobotLib[robots_stay[i].rt].name);
						}
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"resource")
				{
					is_cmd = true;
					if (len == 1)
					{
						bool is_printed = false;
						for (int i = 0; i < (int)ITEMTYPE::IT_NUM; i++)
						{
							if (ItemBox[(ITEMTYPE)i] > 0)
							{
								is_printed = true;
								clogs.push_front(indent + ItemLib[(ITEMTYPE)i].name + U" " + Format(ItemAmount2Visual((ITEMTYPE)i, ItemBox[(ITEMTYPE)i])) + U"kg");
							}
						}
						if (!is_printed)clogs.push_front(U"There is no resource.");
					}
					else
					{
						is_too_many_arg = true;
					}
				}
			}
			if (!is_cmd)
			{
				clogs.push_front(U"Undefined command. Please Type \"help\" Command.");
			}
			if (is_too_many_arg)
			{
				clogs.push_front(U"Too many argument. Please Retype Command.");
			}

			txtstt.clear();
		}
	}
	void draw()
	{
		{
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(-10, -10), size + Vec2(20, 20)).draw(col_bg);//背景

			Vec2 basePos = Vec2(0, getContentsRectF().h - font01.height() + scroll);
			Vec2 penPos = Vec2(basePos);
			//入力中の奴
			{
				int i = -(int)dir.size();
				for (const auto& glyph : font01.getGlyphs(dir + txtstt.text))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.y -= font01.height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = 0;
						penPos.y -= font01.height();
					}
					penPos.x += glyph.xAdvance;
				}
				Vec2 tmpPos = penPos;
				penPos.x = basePos.x;
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
				penPos = tmpPos;
				penPos.x = basePos.x;
				penPos.y -= font01.height();
			}
			//過去ログ
			for (int i = 0; i < clogs.size(); i++)
			{
				for (const auto& glyph : font01.getGlyphs(clogs[i]))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.y -= font01.height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = 0;
						penPos.y -= font01.height();
					}
					penPos.x += glyph.xAdvance;
				}
				Vec2 tmpPos = penPos;
				penPos.x = basePos.x;
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
				penPos = tmpPos;
				penPos.x = basePos.x;
				penPos.y -= font01.height();
				if (penPos.y < -font01.height())
				{
					break;
				}
			}
		}
		drawFlame();
	}
};
