#pragma once
#include "def.h"
#include "MyWindow.h"

class MailSoft : public MyWindow
{
private:
	Font font01;
	bool is_reading;//メールを読んでるか否か
	int mail_num;//選択中のメールの番号
	Array<Rect> mail_rects;
public:
	MailSoft() : MyWindow()
	{
		size_min = Vec2(300, size_min.y + font01.height());
		font01 = Font(15, U"resource/AozoraMinchoRegular.ttf");
		is_reading = false;
		mail_num = 0;
	};
	MailSoft(Vec2 p, Vec2 s) : MyWindow(U"message+", p, s, Vec2(300, size_min.y + font01.height()))
	{
		size_min = Vec2(300, size_min.y + font01.height());
		font01 = Font(15, U"resource/AozoraMinchoRegular.ttf");
		is_reading = false;
		mail_num = 0;
	};
	void update()
	{
		if (win_active == this)
		{
			if (KeySpace.down())
			{
				is_reading = !is_reading;
			}
			if (KeyUp.down())
			{
				mail_num = (mail_num + 1) % MailLib.size();
			}
			if (KeyDown.down())
			{
				mail_num = (mail_num + MailLib.size() - 1) % MailLib.size();
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
				int cnt = 0;
				for (int i = mail_rects.size() - 1; i >= 0; i--)
				{
					if (mail_rects[i].contains(pos) && MailLib.size() > i)
					{
						if (mail_num == cnt || !is_reading)
						{
							is_reading = !is_reading;
						}
						mail_num = cnt;
					}
					cnt++;
				}
			}
		}
	}
	void draw()
	{
		{
			mail_rects.clear();
			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(0, 0), size).draw(Color(127));//背景
			{
				double y_pos = 0;
				constexpr double margin = 5;
				Rect rf = getContentsRectF().asRect();
				const double x_pos_title_start = rf.w / 5, x_pos_text_start = rf.w * 2 / 5, x_pos_text_end = rf.w;
				for (int i = MailLib.size() - 1; i >= 0; i--)
				{
					double y_pos_tmp = y_pos;
					if (i != MailLib.size() - 1)Line(Vec2(0, y_pos), Vec2(rf.w, y_pos)).draw(Color(0));
					Line(Vec2(x_pos_title_start, y_pos + margin), Vec2(x_pos_title_start, y_pos + margin + font01.height())).draw(Color(0));
					Line(Vec2(x_pos_text_start, y_pos + margin), Vec2(x_pos_text_start, y_pos + margin + font01.height())).draw(Color(0));
					double text_y_pos = y_pos + margin;
					{
						Vec2 penPos = Vec2(margin, text_y_pos);
						const double x_max = x_pos_title_start - margin;
						for (const auto& glyph : font01.getGlyphs(MailLib[i].from))
						{
							if (glyph.codePoint == U'\n')continue;
							if (penPos.x + glyph.xAdvance > x_max)break;
							glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), Color(0));
							penPos.x += glyph.xAdvance;
						}
					}
					{
						Vec2 penPos = Vec2(x_pos_title_start + margin, text_y_pos);
						const double x_max = x_pos_text_start - margin;
						for (const auto& glyph : font01.getGlyphs(MailLib[i].title))
						{
							if (glyph.codePoint == U'\n')continue;
							if (penPos.x + glyph.xAdvance > x_max)break;
							glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), Color(0));
							penPos.x += glyph.xAdvance;
						}
					}
					{
						Vec2 penPos = Vec2(x_pos_text_start + margin, text_y_pos);
						const double x_max = x_pos_text_end - margin;
						bool first_n = true;
						for (const auto& glyph : font01.getGlyphs(MailLib[i].text))
						{
							if (is_reading && mail_num == i)
							{
								if (glyph.codePoint == U'\n' || penPos.x + glyph.xAdvance > x_max)
								{
									penPos.x = x_pos_text_start + margin;
									penPos.y += font01.height() + margin * 2;
									y_pos += font01.height() + margin * 2;
									Line(Vec2(x_pos_text_start + margin, y_pos), Vec2(x_pos_text_end - margin, y_pos)).draw(Color(0));
									if (glyph.codePoint == U'\n')continue;
								}
								glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), Color(0));
								penPos.x += glyph.xAdvance;
							}
							else
							{
								if (glyph.codePoint == U'\n')continue;
								if (penPos.x + glyph.xAdvance > x_max)break;
								glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), Color(0));
								penPos.x += glyph.xAdvance;
							}
						}
					}
					y_pos += font01.height() + margin * 2;
					Rect rc = Rect(Point(0, (int)y_pos_tmp), Point((int)x_pos_text_end, (int)(y_pos - y_pos_tmp)));
					if (rc.contains(cursor_pos - rf.pos))rc.draw(ColorF(1.0, 1.0, 1.0, 0.3));
					mail_rects.push_back(rc);
				}
			}
		}
		drawFlame();
	}
};

class Inventor : public MyWindow
{
private:
	Font font01, font02;
	TextEditState txtstt;
	bool is_buying = false;
	Robot robo;
	Array<Item> buy_resource;
	const int frame_padding = 5;
	const Color
		col_bg = Color(0xa7, 0x54, 0xcb),
		col_lt = Color(0xc3, 0x8b, 0xdc),
		col_dk = Color(0x8b, 0x36, 0xb0);
public:
	Inventor() : MyWindow()
	{
		size_min = Vec2(400, 600);
		font01 = Font(30, U"resource/GenEiNuGothic-EB.ttf");
		font02 = Font(14, U"resource/GenEiNuGothic-EB.ttf");
	};
	Inventor(Vec2 p, Vec2 s) : MyWindow(U"!nvent", p, s, Vec2(400, 600))
	{
		size_min = Vec2(400, 600);
		font01 = Font(30, U"resource/GenEiNuGothic-EB.ttf");
		font02 = Font(14, U"resource/GenEiNuGothic-EB.ttf");
	};
	RectF getSelectionRect(int i)
	{
		int sep = int(size.x) / 400 + 1;//分割数
		int seplin = i % sep;//列番号
		i /= sep;
		double margin = 10;
		Vec2 s_size = Vec2(getContentsRectF().w - margin * 2, font01.height() + font02.height() + frame_padding * 2);
		s_size.x /= sep;
		return RectF(Vec2(margin + seplin * s_size.x, margin + (margin + s_size.y) * i), s_size - Vec2(sep - 1 == seplin ? 0 : margin, 0)).movedBy(getContentsRectF().pos);
	}
	RectF getButtonRect()
	{
		RectF rf = getContentsRectF();
		RectF btn = RectF(Vec2(rf.w / 2, rf.h * 2 / 3), Vec2(80, 30));
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
		for (int i = 0; i < buy_resource.size(); i++)
		{
			ItemBox[buy_resource[i].it] -= buy_resource[i].amount;
		}
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
			else if (!is_left)
			{
				is_buying = false;
			}
		}
		else
		{
			for (int i = 0; i < Recipes.size(); i++)
			{
				bool is_creatable = true;
				for (int j = 0; j < Recipes[i].second.size(); j++)
				{
					if (ItemBox[Recipes[i].second[j].it] < Recipes[i].second[j].amount)is_creatable = false;
				}
				if (is_creatable && getSelectionRect(i).movedBy(-getContentsRectF().pos).contains(pos))
				{
					Robot rb;
					rb.count_go = 0;
					rb.endurance = 0;
					rb.remain_time = 0;
					rb.rt = Recipes[i].first;
					buy_resource = Recipes[i].second;
					robo = rb;
					txtstt.clear();
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
			RectF(Vec2(0, 0), size).draw(col_bg);//背景
			for (int i = 0; i < Recipes.size(); i++)
			{
				bool is_creatable = true;
				for (int j = 0; j < Recipes[i].second.size(); j++)
				{
					if (ItemBox[Recipes[i].second[j].it] < Recipes[i].second[j].amount)is_creatable = false;
				}
				RectF rc = getSelectionRect(i).movedBy(-getContentsRectF().pos);
				rc.draw(is_creatable ? col_lt : col_dk).drawFrame(frame_padding / 2, 0, Color(0));
				rc = RectF(rc.pos + Vec2(frame_padding, frame_padding), rc.size - Vec2(1, 1) * 2 * frame_padding);
				{
					Rect rc_dim = getOverlappingRectF(rect, rc.movedBy(rect.pos)).asRect();
					const ScopedViewport2D viewport2(Rect(rc_dim.pos - Point(1, 1), rc_dim.size + Point(2, 2)));
					const Transformer2D transformer2{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos + rc.pos) };
					//内容の描画
					TextureLib[RobotLib[Recipes[i].first].texture_name].resized(font01.height()).draw(Vec2(0, 0));
					font01(RobotLib[Recipes[i].first].name).draw(Vec2(font01.height(), 0), Color(0));
					String str = U"";
					for (int j = 0; j < Recipes[i].second.size(); j++)
					{
						if (j != 0)str += U", ";
						str += ItemLib[Recipes[i].second[j].it].name + U": " + U"{:.2f}"_fmt(ItemAmount2Visual(Recipes[i].second[j].it, Recipes[i].second[j].amount));
					}
					font02(str).draw(Vec2(0, font01.height()), Color(0));
				}
			}

			//購入中は
			if (is_buying)
			{
				RectF(Vec2(0, 0), size).draw(ColorF(0.0, 0.0, 0.0, 0.5));
				RectF rf = getContentsRectF();
				font01(U"名前を入力してください").drawAt(Vec2(rf.w / 2, rf.h / 3), Color(255));
				font01(txtstt.text).drawAt(Vec2(rf.w / 2, rf.h / 2), Color(255));
				getButtonRect().draw(getButtonRect().mouseOver() ? Color(255) : Color(127));
				font02(U"作成").drawAt(getButtonRect().center(), Color(255));
			}
		}
		drawFlame(col_bg);
	}
};

class MAPViewer : public MyWindow
{
private:
	Font font01, font02;
	Vec2 pos_camera;
	const double r = 10;//図形の半径
	const double m_r = 12;//マージンの半径
	double scale = 1;//拡大量
	double scroll = 0;//機械スクロール
	Robot* selected_robo = nullptr;//選択されている機械
	const double margin_frame = 3;
	const Color col_main = Color(0x41, 0x69, 0xe1),
		col_br = Color(0x1e, 0x90, 0xff),
		col_cl = Color(0x00, 0xbf, 0xff);
public:
	MAPViewer() : MyWindow()
	{
		size_min = Vec2(400, 400);
		font01 = Font(25, U"resource/GenEiNuGothic-EB.ttf");
		font02 = Font(13, U"resource/GenEiNuGothic-EB.ttf");
		pos_camera = Vec2(0, 0);
	};
	MAPViewer(Vec2 p, Vec2 s) : MyWindow(U"WLD", p, s, Vec2(400, 400))
	{
		size_min = Vec2(400, 400);
		font01 = Font(25, U"resource/GenEiNuGothic-EB.ttf");
		font02 = Font(13, U"resource/GenEiNuGothic-EB.ttf");
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
		const double mag_f = 10;
		RectF robo_rect = RectF(
			Vec2(getContentsRectF().rightX() - get_right_panel_width() / 2, getContentsRectF().y + 30 + scroll),
			Vec2(get_right_panel_width() - mag_f * 2, font01.height() + font02.height() + margin_frame * 2));
		return robo_rect.movedBy(Vec2(0, (robo_rect.h + mag_f) * n)).movedBy(-robo_rect.size / 2);
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
							Shape2D::Hexagon(r * scale, rect.size / 2 + dis * scale, 30_deg).draw(TileLib[MAINMAP[i][j].tile].c).drawFrame(2, Color(200, 200, 0));
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
				right_panel().movedBy(-getContentsRectF().pos).draw(col_main.withAlpha(128));//背景
				for (int i = 0; i < robots_stay.size(); i++)
				{
					RectF rf = right_panel_icons(i).movedBy(-getContentsRectF().pos);
					rf.draw((selected_robo == &robots_stay[i]) ? col_cl : (rf.contains(c_pos)) ? col_br : col_main)
						.drawFrame(margin_frame / 2, 0, Color(0));
					rf = RectF(rf.pos + Vec2(1, 1) * margin_frame, rf.size - Vec2(1, 1) * 2 * margin_frame);
					Rect rc_dim = getOverlappingRectF(rect, rf.movedBy(rect.pos)).asRect();
					const ScopedViewport2D viewport2(Rect(rc_dim.pos - Point(1, 1), rc_dim.size + Point(2, 2)));
					const Transformer2D transformer2{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos + rf.pos) };

					TextureLib[RobotLib[robots_stay[i].rt].texture_name].resized(font01.height()).draw(Vec2(0, 0));
					font01(robots_stay[i].name).draw(Vec2(font01.height(), 0), Color(0));
					font02(RobotLib[robots_stay[i].rt].name).draw(Vec2(0, font01.height()), Color(0));
				}
			}
		}
		drawFlame(col_main);
	}
};

class CommandPrompt : public MyWindow
{
private:
	TextEditState txtstt;
	Font font00, font01, font02, font03, font04;
	char fontsize = 2;
	Array<String> clogs;
	Color col_bg = Color(0x00), col_txt = Color(0x26, 0xc3, 0x0f);
	double scroll = 0;
	double time = 0;
	const String dir = U"s: > ";
	const String indent = U"   ";
protected:
public:
	CommandPrompt() : MyWindow()
	{
		font00 = Font(10, U"resource/GenEiNuGothic-EB.ttf");
		font01 = Font(13, U"resource/GenEiNuGothic-EB.ttf");
		font02 = Font(17, U"resource/GenEiNuGothic-EB.ttf");
		font03 = Font(20, U"resource/GenEiNuGothic-EB.ttf");
		font04 = Font(25, U"resource/GenEiNuGothic-EB.ttf");
		size_min = Vec2(Max(Max(size_min.x, SCENE_WIDTH / 3.0), 300.0), size_min.y + font04.height());
	};
	CommandPrompt(Vec2 p, Vec2 s) : MyWindow(U"stash", p, s, Vec2(Max(Max(size_min.x, SCENE_WIDTH / 3.0), 300.0), size_min.y + font04.height()))
	{
		font00 = Font(10, U"resource/GenEiNuGothic-EB.ttf");
		font01 = Font(13, U"resource/GenEiNuGothic-EB.ttf");
		font02 = Font(17, U"resource/GenEiNuGothic-EB.ttf");
		font03 = Font(20, U"resource/GenEiNuGothic-EB.ttf");
		font04 = Font(25, U"resource/GenEiNuGothic-EB.ttf");
		size_min = Vec2(Max(Max(size_min.x, SCENE_WIDTH / 3.0), 300.0), size_min.y + font04.height());
	};
	void update()
	{
		SimpleGUI::TextBox(txtstt, Vec2(-100, -100));
		if (win_active == this)
		{
			time += delta;
			txtstt.active = true;
			if ((KeyControl + KeyNumAdd).down())
			{
				fontsize = (fontsize + 1) % 5;
			}
			if ((KeyControl + KeyNumSubtract).down())
			{
				fontsize = (fontsize + 4) % 5;
			}
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
			const Array<String> strs = txtstt.text.split(' ');
			int len = strs.size();
			if (len > 0)
			{
				//言語変更
				if (strs[0] == U"help")
				{
					is_cmd = true;
					if (len == 1)
					{
						clogs.push_front(U"\"cat\", \"clear\", \"date\", \"echo\", \"eval\", \"help\", \"log\", \"machine\", \"resource\", \"style\"");
					}
					else if (len == 2)
					{
						if (strs[1] == U"cat")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Show cat.");
						}
						if (strs[1] == U"clear")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Delete logs on display.");
						}
						if (strs[1] == U"date")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays date and time.");
						}
						if (strs[1] == U"echo")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays the characters entered.");
						}
						if (strs[1] == U"eval")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Calculate the given expression.");
						}
						if (strs[1] == U"help")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays a list of commands.");
							clogs.push_front(indent + U"Displays the usage of the command given as an argument.");
						}
						if (strs[1] == U"log")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays the log.");
						}
						if (strs[1] == U"machine")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays a list of owned machines.");
						}
						if (strs[1] == U"resource")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays a list of resources owned.");
						}
						if (strs[1] == U"style")
						{
							clogs.push_front(strs[1]);
							clogs.push_front(indent + U"Displays a list of color styles.");
							clogs.push_front(indent + U"Changes to the color style given as an argument.");
						}
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"cat")
				{
					is_cmd = true;
					if (len == 1)
					{
						Array<Array<String>> cats =
						{
							{U"  ｡｡",U" ﾟ●゜"},
							{U"    ∧,,∧",U"（=・ω・）",U"（,, ｕｕﾉ"},
							{U"(=･ω･=)"}
						};
						Array<String> cat = cats.choice();
						for (int i = 0; i < cat.size(); i++)
						{
							clogs.push_front(indent + cat[i]);
						}
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
						clogs.push_front(DateTime2GameTime(t));
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"echo")
				{
					is_cmd = true;
					Array<String> t_strs = strs;
					t_strs.pop_front();
					if (t_strs.isEmpty())
					{
						clogs.push_front(U"There is no argument. Please Retype Command.");
					}
					else
					{
						String expl = t_strs.join(U" ", U"\0", U"\0");
						clogs.push_front(indent + expl);
					}
				}
				if (strs[0] == U"eval")
				{
					is_cmd = true;
					Array<String> t_strs = strs;
					t_strs.pop_front();
					if (t_strs.isEmpty())
					{
						clogs.push_front(U"There is no argument. Please Retype Command.");
					}
					else
					{
						String expl = t_strs.join(U" ", U"\0", U"\0");
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
							clogs.push_front(indent + DateTime2GameTime(logs[i].time) + logs[i].text);
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
								clogs.push_front(indent + ItemLib[(ITEMTYPE)i].name + U" " + U"{:.2f}"_fmt(ItemAmount2Visual((ITEMTYPE)i, ItemBox[(ITEMTYPE)i])) + U"kg");
							}
						}
						if (!is_printed)clogs.push_front(U"There is no resource.");
					}
					else
					{
						is_too_many_arg = true;
					}
				}
				if (strs[0] == U"style")
				{
					is_cmd = true;
					if (len == 1)
					{
						clogs.push_front(indent +
							U"\"blood\", \"default\", \"monochrome\", \"wood\"");
					}
					else if (len == 2)
					{
						if (strs[1] == U"blood")
						{
							col_bg = Color(0x2b, 0x00, 0x00);
							col_txt = Color(0xcc, 0x0e, 0x13);
						}
						if (strs[1] == U"default")
						{
							col_bg = Color(0x00);
							col_txt = Color(0x26, 0xc3, 0x0f);
						}
						if (strs[1] == U"monochrome")
						{
							col_bg = Color(0x22, 0x23, 0x23);
							col_txt = Color(0xf0, 0xf6, 0xf0);
						}
						if (strs[1] == U"wood")
						{
							col_bg = Color(0x4a, 0x3b, 0x2a);
							col_txt = Color(0xaa, 0x66, 0x00);
						}
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
			Font* use_font;
			switch (fontsize)
			{
			case 0:
				use_font = &font00;
				break;
			case 1:
				use_font = &font01;
				break;
			case 2:
				use_font = &font02;
				break;
			case 3:
				use_font = &font03;
				break;
			case 4:
				use_font = &font04;
				break;
			default:
				use_font = &font00;
				fontsize = 0;
				break;
			}

			//枠外描画を禁止&マウス移動
			Rect rect = getContentsRectF().asRect();
			const ScopedViewport2D viewport(Rect(rect.pos - Point(1, 1), rect.size + Point(2, 2)));
			const Transformer2D transformer{ Mat3x2::Identity(), Mat3x2::Translate(rect.pos) };
			//以下で描画
			RectF(Vec2(-10, -10), size + Vec2(20, 20)).draw(col_bg);//背景

			Vec2 basePos = Vec2(0, getContentsRectF().h - use_font->height() + scroll);
			Vec2 penPos = Vec2(basePos);
			//入力中の奴
			{
				int i = -(int)dir.size();
				for (const auto& glyph : use_font->getGlyphs(dir + txtstt.text))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.y -= use_font->height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = 0;
						penPos.y -= use_font->height();
					}
					penPos.x += glyph.xAdvance;
				}
				Vec2 tmpPos = penPos;
				penPos.x = basePos.x;
				for (const auto& glyph : use_font->getGlyphs(dir + txtstt.text))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.x = basePos.x;
						penPos.y += use_font->height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = basePos.x;
						penPos.y += use_font->height();
					}
					glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), col_txt);
					penPos.x += glyph.xAdvance;
					if (i == txtstt.cursorPos - 1 && int(time * 2) % 2 == 0)
					{
						RectF(penPos, Vec2(10, use_font->height())).draw(col_txt);
					}
					i++;
				}
				penPos = tmpPos;
				penPos.x = basePos.x;
				penPos.y -= use_font->height();
			}
			//過去ログ
			for (int i = 0; i < clogs.size(); i++)
			{
				for (const auto& glyph : use_font->getGlyphs(clogs[i]))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.y -= use_font->height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = 0;
						penPos.y -= use_font->height();
					}
					penPos.x += glyph.xAdvance;
				}
				Vec2 tmpPos = penPos;
				penPos.x = basePos.x;
				for (const auto& glyph : use_font->getGlyphs(clogs[i]))
				{
					// 改行文字なら
					if (glyph.codePoint == U'\n')
					{
						penPos.x = basePos.x;
						penPos.y += use_font->height();
						continue;
					}
					//画面内に描画可能かの判定
					if (penPos.x + glyph.xAdvance > rect.w)
					{
						penPos.x = basePos.x;
						penPos.y += use_font->height();
					}
					glyph.texture.draw(Math::Round(penPos + glyph.getOffset()), col_txt);
					penPos.x += glyph.xAdvance;
				}
				penPos = tmpPos;
				penPos.x = basePos.x;
				penPos.y -= use_font->height();
				if (penPos.y < -use_font->height())
				{
					break;
				}
			}
		}
		drawFlame(col_txt);
	}
};
