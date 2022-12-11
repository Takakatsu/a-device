#include "def.h"

bool is_game_exit = false;
double passed_time = 0;
double delta;
Vec2 cursor_pos;

CLICKED_TYPE g_clicktype;

Array<MailData> MailLib;

HashTable<ITEMTYPE, ItemData> ItemLib;
HashTable<ITEMTYPE, double> ItemBox;

HashTable<MAPTILE, TileData> TileLib;
Array<Array<MapTip>> MAINMAP;

HashTable<ROBOTTYPE, RobotData> RobotLib;
Array<Robot> robots_stay;
Array<Robot_Activated> robots_active;

Array<GameLog> logs;
Array<GameLog> logs_will;
Array<GameLog> logs_tmp;

Array<MyWindow*> my_wins;
Array<MyIcon*> my_icons;
MyWindow* win_active;

HashTable<String, Audio> AudioLib;
HashTable<String, Texture> TextureLib;

String DateTime2GameTime(const DateTime& t)
{
	return String(
		Format(t.year + 1372) + U" " + Format(t.month / 2 + 1) + U" "
		+ Format(t.month % 2 + 1) + U" " + Format(t.day) + U" "
		+ Format(t.hour / 6) + U":" + Format(t.hour % 6) + U":"
		+ Format(t.minute / 15) + U":" + Format(t.minute % 15) + U":"
		+ Format(t.second / 2));
}

double ItemAmount2Visual(ITEMTYPE it, double n)
{
	switch (it)
	{
	case IT_LIQUID1:return n * 0.8734;
	case IT_LIQUID2:return n * 3.1628;
	case IT_WOOD1:return n * 5.4753;
	case IT_WOOD2:return n * 6.1734;
	case IT_ROCK1:return n * 8.8374;
	case IT_ROCK2:return n * 10.8473;
	case IT_ROCK3:return n * 9.2749;
	case IT_ORE1:return n * 0.2485;
	case IT_ORE2:return n * 0.1294;
	case IT_ORE3:return n * 0.4182;
	case IT_ORE4:return n * 0.1837;
	default:
		break;
	}
	return 0;
}

ItemRate makeItemRate(ITEMTYPE it, double min, double max)
{
	ItemRate ir;
	ir.it = it;
	ir.min = min;
	ir.max = max;
	return ir;
}

void initialize_lib()
{
	{
		//機械の初期化
		RobotData rd;
		Item it;
		{
			rd.materials.clear();
			rd.name = U"探査機";
			rd.max_endurance = 1000;
			it.it = ITEMTYPE::IT_WOOD1;
			it.amount = 0.5;
			rd.materials.push_back(it);
			RobotLib.emplace(ROBOTTYPE::RT_SEARCH, rd);
		}
		{
			rd.materials.clear();
			rd.name = U"回収機α";
			rd.max_endurance = 1000;
			it.it = ITEMTYPE::IT_WOOD1;
			it.amount = 1.5;
			rd.materials.push_back(it);
			RobotLib.emplace(ROBOTTYPE::RT_COLLECT1, rd);
		}
		{
			rd.materials.clear();
			rd.name = U"回収機β";
			rd.max_endurance = 1000;
			it.it = ITEMTYPE::IT_WOOD1;
			it.amount = 2.5;
			rd.materials.push_back(it);
			RobotLib.emplace(ROBOTTYPE::RT_COLLECT2, rd);
		}
		{
			rd.materials.clear();
			rd.name = U"戦闘機α";
			rd.max_endurance = 1000;
			it.it = ITEMTYPE::IT_WOOD1;
			it.amount = 3.5;
			rd.materials.push_back(it);
			RobotLib.emplace(ROBOTTYPE::RT_FIGHT1, rd);
		}
		{
			rd.materials.clear();
			rd.name = U"戦闘機β";
			rd.max_endurance = 1000;
			it.it = ITEMTYPE::IT_WOOD1;
			it.amount = 4.5;
			rd.materials.push_back(it);
			RobotLib.emplace(ROBOTTYPE::RT_FIGHT2, rd);
		}
	}
	{
		//表示の時だけ係数をかける

		ItemData id;
		//液体の価値(消費量)は 1 : 3.5
		id.name = U"液体α";
		ItemLib.emplace(ITEMTYPE::IT_LIQUID1, id);
		id.name = U"液体β";
		ItemLib.emplace(ITEMTYPE::IT_LIQUID2, id);
		//木の価値(消費量)は 1 : 1.5
		id.name = U"木材α";
		ItemLib.emplace(ITEMTYPE::IT_WOOD1, id);
		id.name = U"木材β";
		ItemLib.emplace(ITEMTYPE::IT_WOOD2, id);
		//石の価値(消費量)は 3 : 5 : 7
		id.name = U"石材α";
		ItemLib.emplace(ITEMTYPE::IT_ROCK1, id);
		id.name = U"石材β";
		ItemLib.emplace(ITEMTYPE::IT_ROCK2, id);
		id.name = U"石材γ";
		ItemLib.emplace(ITEMTYPE::IT_ROCK3, id);
		//鉱物の価値(消費量)は 1 : 2.5 : 4 : 9.5
		id.name = U"鉱物α";
		ItemLib.emplace(ITEMTYPE::IT_ORE1, id);
		id.name = U"鉱物β";
		ItemLib.emplace(ITEMTYPE::IT_ORE2, id);
		id.name = U"鉱物γ";
		ItemLib.emplace(ITEMTYPE::IT_ORE3, id);
		id.name = U"鉱物δ";
		ItemLib.emplace(ITEMTYPE::IT_ORE4, id);
	}
	{
		TileData td;
		//自分の船
		td.c = Color(255, 0, 0);
		td.irs_1.clear();
		td.irs_2.clear();
		TileLib.emplace(MAPTILE::MT_SHIP, td);
		//草原
		{
			//比率にして総和が1になるように。

			constexpr double rat_1 = 0.75, rat_2 = 0.2;//メイン素材の存在率
			constexpr double dis_1 = 0.2, dis_2 = 0.5;//ゆらぎ(番号は機械)

			td.c = Color(0, 130, 0);
			td.irs_1.clear();
			{
				constexpr double t0 = rat_1, t1 = dis_1;
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_WOOD1, t0 * (1 - t1), t0 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_WOOD2, (1 - t0) * (1 - t1), (1 - t0) * (1 + t1)));
			}
			td.irs_2.clear();
			{
				constexpr double t0 = rat_1, t1 = dis_2;
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_WOOD1, t0 * (1 - t1), t0 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_WOOD2, (1 - t0) * (1 - t1), (1 - t0) * (1 + t1)));
			}
			TileLib.emplace(MAPTILE::MT_GRASS1, td);//木1メイン
			td.c = Color(0, 255, 0);
			td.irs_1.clear();
			{
				constexpr double t0 = rat_2, t1 = dis_1;
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_WOOD1, t0 * (1 - t1), t0 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_WOOD2, (1 - t0) * (1 - t1), (1 - t0) * (1 + t1)));
			}
			td.irs_2.clear();
			{
				constexpr double t0 = rat_2, t1 = dis_2;
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_WOOD1, t0 * (1 - t1), t0 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_WOOD2, (1 - t0) * (1 - t1), (1 - t0) * (1 + t1)));
			}
			TileLib.emplace(MAPTILE::MT_GRASS2, td);//木2メイン
		}
		//岩場
		{
			constexpr double dis_1 = 0.2, dis_2 = 0.5;//ゆらぎ(番号は機械)
			//縦に見て和が1ならok
			//以下は石
			constexpr double rat1_1 = 0.75, rat1_2 = 0.05, rat1_3 = 0.15;//各場所での素材1の存在率
			constexpr double rat2_1 = 0.10, rat2_2 = 0.90, rat2_3 = 0.20;//各場所での素材2の存在率
			constexpr double rat3_1 = 0.15, rat3_2 = 0.05, rat3_3 = 0.65;//各場所での素材3の存在率
			//鉱石は等しく分布している
			constexpr double r2at1 = 0.65, r2at2 = 0.20, r2at3 = 0.10, r2at4 = 0.05;
			const ItemRate ir_or1_d1 = makeItemRate(ITEMTYPE::IT_ORE1, r2at1 * (1 - dis_1), r2at1 * (1 + dis_1));
			const ItemRate ir_or2_d1 = makeItemRate(ITEMTYPE::IT_ORE2, r2at2 * (1 - dis_1), r2at2 * (1 + dis_1));
			const ItemRate ir_or3_d1 = makeItemRate(ITEMTYPE::IT_ORE3, r2at3 * (1 - dis_1), r2at3 * (1 + dis_1));
			const ItemRate ir_or4_d1 = makeItemRate(ITEMTYPE::IT_ORE4, r2at4 * (1 - dis_1), r2at4 * (1 + dis_1));
			const ItemRate ir_or1_d2 = makeItemRate(ITEMTYPE::IT_ORE1, r2at1 * (1 - dis_2), r2at1 * (1 + dis_2));
			const ItemRate ir_or2_d2 = makeItemRate(ITEMTYPE::IT_ORE2, r2at2 * (1 - dis_2), r2at2 * (1 + dis_2));
			const ItemRate ir_or3_d2 = makeItemRate(ITEMTYPE::IT_ORE3, r2at3 * (1 - dis_2), r2at3 * (1 + dis_2));
			const ItemRate ir_or4_d2 = makeItemRate(ITEMTYPE::IT_ORE4, r2at4 * (1 - dis_2), r2at4 * (1 + dis_2));

			td.c = Color(80, 120, 48);
			td.irs_1.clear();
			td.irs_1.push_back(ir_or1_d1);
			td.irs_1.push_back(ir_or2_d1);
			td.irs_1.push_back(ir_or3_d1);
			td.irs_1.push_back(ir_or4_d1);
			{
				constexpr double t1 = dis_1;
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK1, rat1_1 * (1 - t1), rat1_1 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK2, rat2_1 * (1 - t1), rat2_1 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK3, rat3_1 * (1 - t1), rat3_1 * (1 + t1)));
			}
			td.irs_2.clear();
			td.irs_2.push_back(ir_or1_d2);
			td.irs_2.push_back(ir_or2_d2);
			td.irs_2.push_back(ir_or3_d2);
			td.irs_2.push_back(ir_or4_d2);
			{
				constexpr double t1 = dis_2;
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK1, rat1_1 * (1 - t1), rat1_1 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK2, rat2_1 * (1 - t1), rat2_1 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK3, rat3_1 * (1 - t1), rat3_1 * (1 + t1)));
			}
			TileLib.emplace(MAPTILE::MT_ROCK1, td);//場所1
			td.c = Color(180, 60, 48);
			td.irs_1.clear();
			td.irs_1.push_back(ir_or1_d1);
			td.irs_1.push_back(ir_or2_d1);
			td.irs_1.push_back(ir_or3_d1);
			td.irs_1.push_back(ir_or4_d1);
			{
				constexpr double t1 = dis_1;
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK1, rat1_2 * (1 - t1), rat1_2 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK2, rat2_2 * (1 - t1), rat2_2 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK3, rat3_2 * (1 - t1), rat3_2 * (1 + t1)));
			}
			td.irs_2.clear();
			td.irs_2.push_back(ir_or1_d2);
			td.irs_2.push_back(ir_or2_d2);
			td.irs_2.push_back(ir_or3_d2);
			td.irs_2.push_back(ir_or4_d2);
			{
				constexpr double t1 = dis_2;
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK1, rat1_2 * (1 - t1), rat1_2 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK2, rat2_2 * (1 - t1), rat2_2 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK3, rat3_2 * (1 - t1), rat3_2 * (1 + t1)));
			}
			TileLib.emplace(MAPTILE::MT_ROCK2, td);//場所2
			td.c = Color(116, 80, 48);
			td.irs_1.clear();
			td.irs_1.push_back(ir_or1_d1);
			td.irs_1.push_back(ir_or2_d1);
			td.irs_1.push_back(ir_or3_d1);
			td.irs_1.push_back(ir_or4_d1);
			{
				constexpr double t1 = dis_1;
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK1, rat1_3 * (1 - t1), rat1_3 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK2, rat2_3 * (1 - t1), rat2_3 * (1 + t1)));
				td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_ROCK3, rat3_3 * (1 - t1), rat3_3 * (1 + t1)));
			}
			td.irs_2.clear();
			td.irs_2.push_back(ir_or1_d2);
			td.irs_2.push_back(ir_or2_d2);
			td.irs_2.push_back(ir_or3_d2);
			td.irs_2.push_back(ir_or4_d2);
			{
				constexpr double t1 = dis_2;
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK1, rat1_3 * (1 - t1), rat1_3 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK2, rat2_3 * (1 - t1), rat2_3 * (1 + t1)));
				td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_ROCK3, rat3_3 * (1 - t1), rat3_3 * (1 + t1)));
			}
			TileLib.emplace(MAPTILE::MT_ROCK3, td);//場所3
		}
		//砂場
		td.c = Color(237, 180, 130);
		td.irs_1.clear();
		td.irs_2.clear();
		TileLib.emplace(MAPTILE::MT_SAND, td);
		//水場
		td.c = Color(0, 0, 180);
		td.irs_1.clear();
		td.irs_2.clear();
		TileLib.emplace(MAPTILE::MT_WATER, td);
	}
	{
		PerlinNoise pn = PerlinNoise(Random(UINT64_MAX));
		PerlinNoise pn_sub = PerlinNoise(Random(UINT64_MAX));
		for (int i = 0; i < MAP_CENTER_X * 2 + 1; i++)
		{
			Array<MapTip> tmptile;
			for (int j = 0; j < MAP_CENTER_Y * 2 + 1; j++)
			{
				MapTip t;
				t.is_found = false;
				//マップタイルの指定
				double d = pn.noise2D(i / 10.0, j / 10.0);
				if (d < -0.3)t.tile = MAPTILE::MT_WATER;
				else if (d < -0.1)t.tile = MAPTILE::MT_SAND;
				else if (d < 0.2)//草は複数種類存在
				{
					double d_s = pn_sub.noise2D(i / 10.0, j / 10.0);
					if (d_s > 0.1)t.tile = MAPTILE::MT_GRASS1;
					else t.tile = MAPTILE::MT_GRASS2;
				}
				else//岩は複数種類存在
				{
					double d_s = pn_sub.noise2D(i / 10.0, j / 10.0);
					if (d_s > 0.1)t.tile = MAPTILE::MT_ROCK1;
					else if (d_s > 0)t.tile = MAPTILE::MT_ROCK2;
					else t.tile = MAPTILE::MT_ROCK3;
				}
				//敵の指定
				t.et = ENEMYTYPE::ET_NONE;
				t.e_life = 0;
				if (i == MAP_CENTER_X && j == MAP_CENTER_Y)
				{
					t.is_found = true;
					t.tile = MAPTILE::MT_SHIP;
				}
				tmptile.push_back(t);
			}
			MAINMAP.push_back(tmptile);
		}
	}
	{
		MailData md;
		/*md.from = U"不明";
		md.title = U"無題";
		md.text = U"fjzh0n348flka04hoah3hqfaj-zj0rhaonwo4hhfnalalonosjda";
		MailLib.push_back(md);
		md.from = U"第五支部管制室";
		md.title = U"指令";
		md.text = U"異星を調査せよ";*/
		/*MailLib.push_back(md);*/
	}
	{
		//各種音源のセットアップ
		AudioLib.emplace(U"SetUpBGM", Audio(U"resource/computer_startup.ogg"));
	}
	{
		//各種画像のセットアップ
		TextureLib.emplace(U"BackGround", Texture(U"resource/bg.png"));
		TextureLib.emplace(U"OSIcon", Texture(U"resource/icon.png"));
		TextureLib.emplace(U"ICON_MAP", Texture(U"resource/icon_map.png"));
		TextureLib.emplace(U"ICON_INV", Texture(U"resource/icon_inventor.png"));
		TextureLib.emplace(U"ICON_CMP", Texture(U"resource/icon_command.png"));
	}
}

bool search_map(Point pos, Robot* robo)
{
	//active側への追加、stay側からの消去
	for (auto it = robots_stay.begin(); it != robots_stay.end();)
	{
		if ((&(*it)) == robo)
		{
			//ここで機械へのダメージ処理などを実行
			robo->count_go++;
			robo->endurance += Random(1, 200);
			double distance = Math::Sqrt(Math::Pow(pos.x - MAP_CENTER_X, 2) + Math::Pow(pos.y - MAP_CENTER_Y, 2));
			const double b_rnd_min = 10.0;
			const double b_rnd_max = 30.0;
			robo->remain_time = Random(b_rnd_min, b_rnd_max) + Random(distance) / 2;

			Reward rw;
			rw.pos = pos;
			rw.found = false;

			bool is_break = false;
			//耐久値が一定以上で破壊される
			if (robo->endurance >= RobotLib[robo->rt].max_endurance)
			{
				is_break = true;
			}
			//敵によって処理が変化
			//敵の処理に関しては報酬に含まない(即時処理)
			switch (MAINMAP[pos.x][pos.y].et)
			{
			case ENEMYTYPE::ET_TYPE1:
			{
				switch (robo->rt)
				{
				case ROBOTTYPE::RT_SEARCH:
					is_break = true;
					break;
				case ROBOTTYPE::RT_COLLECT1:
					is_break = true;
					break;
				case ROBOTTYPE::RT_COLLECT2:
					is_break = true;
					break;
				case ROBOTTYPE::RT_FIGHT1:
					MAINMAP[pos.x][pos.y].et = ENEMYTYPE::ET_NONE;
					break;
				case ROBOTTYPE::RT_FIGHT2:
					MAINMAP[pos.x][pos.y].e_life -= Random(2, 4);
					if (MAINMAP[pos.x][pos.y].e_life <= 0)MAINMAP[pos.x][pos.y].et = ENEMYTYPE::ET_NONE;
					is_break = true;
					break;
				default:
					break;
				}
			}
			break;
			case ENEMYTYPE::ET_TYPE2:
			{
				switch (robo->rt)
				{
				case ROBOTTYPE::RT_SEARCH:
					is_break = true;
					break;
				case ROBOTTYPE::RT_COLLECT1:
					is_break = true;
					break;
				case ROBOTTYPE::RT_COLLECT2:
					is_break = true;
					break;
				case ROBOTTYPE::RT_FIGHT1:
					MAINMAP[pos.x][pos.y].e_life -= Random(1, 5);
					if (MAINMAP[pos.x][pos.y].e_life <= 0)MAINMAP[pos.x][pos.y].et = ENEMYTYPE::ET_NONE;
					is_break = true;
					break;
				case ROBOTTYPE::RT_FIGHT2:
					MAINMAP[pos.x][pos.y].et = ENEMYTYPE::ET_NONE;
					break;
				default:
					break;
				}
			}
			break;
			case ENEMYTYPE::ET_NONE:
			{
				switch (robo->rt)
				{
				case ROBOTTYPE::RT_SEARCH:
				{
					rw.found = true;
				}
				break;
				case ROBOTTYPE::RT_COLLECT1://アイテム収集系**must**
				{
					for (auto it = TileLib[MAINMAP[pos.x][pos.y].tile].irs_1.begin(); it != TileLib[MAINMAP[pos.x][pos.y].tile].irs_1.end();)
					{
						Item ifr;
						ifr.it = it->it;
						ifr.amount = Random(it->min, it->max);
						rw.items.push_back(ifr);
						++it;
					}
				}
				break;
				case ROBOTTYPE::RT_COLLECT2://アイテム収集系**must**
				{
					for (auto it = TileLib[MAINMAP[pos.x][pos.y].tile].irs_2.begin(); it != TileLib[MAINMAP[pos.x][pos.y].tile].irs_2.end();)
					{
						Item ifr;
						ifr.it = it->it;
						ifr.amount = Random(it->min, it->max);
						rw.items.push_back(ifr);
						++it;
					}
				}
				break;
				case ROBOTTYPE::RT_FIGHT1:
					break;
				case ROBOTTYPE::RT_FIGHT2:
					break;
				default:
					break;
				}
			}
			break;
			case ENEMYTYPE::ET_WALL:
			{
				is_break = true;
			}
			break;
			default:
				break;
			}
			//ログの送信
			{
				GameLog lg;
				lg.time = DateTime::Now();
				lg.text = robo->name + U"は探索に向かいました";
				logs.push_front(lg);
			}
			//死亡可能性ログの出力
			if (is_break)
			{
				GameLog lg;
				lg.text = robo->name + U"は予定された時間までに帰還しませんでした";
				lg.remain_time = b_rnd_max + distance;
				logs_will.push_front(lg);
			}
			//破壊された機械が確率で復活
			bool is_revive = false;
			if (is_break)
			{
				if (RandomBool(0.1))
				{
					is_revive = true;
					is_break = false;
					//復活時は適当な時間を加算される
					robo->remain_time += b_rnd_max + distance;
				}
			}
			//報酬の登録
			if (!is_break)
			{
				Robot_Activated ra;
				ra.rb = *robo;
				ra.rw = rw;
				robots_active.push_back(ra);
			}
			it = robots_stay.erase(it);
			return true;
		}
		++it;
	}
	return false;
}
