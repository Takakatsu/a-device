#include "def.h"

bool is_game_exit = false;
bool is_exit_phase = false;
double passed_time = 0;
double delta;
Vec2 cursor_pos;
Vec2 newWin_pos = Vec2(64, 64);
int hit_enemy_cnt;

CLICKED_TYPE g_clicktype;

Array<MailData> MailLib;

HashTable<ITEMTYPE, ItemData> ItemLib;
HashTable<ITEMTYPE, double> ItemBox;

HashTable<MAPTILE, TileData> TileLib;
Array<Array<MapTip>> MAINMAP;

HashTable<ROBOTTYPE, RobotData> RobotLib;
Array<std::pair<ROBOTTYPE, Array<Item>>> Recipes;
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
			rd.name = U"惑星探査機";
			rd.max_endurance = 2000;
			rd.texture_name = U"Machine_Search";
			RobotLib.emplace(ROBOTTYPE::RT_SEARCH, rd);
		}
		{
			rd.name = U"物質回収機α";
			rd.max_endurance = 1500;
			rd.texture_name = U"Machine_Collect1";
			RobotLib.emplace(ROBOTTYPE::RT_COLLECT1, rd);
		}
		{
			rd.name = U"物質回収機β";
			rd.max_endurance = 1500;
			rd.texture_name = U"Machine_Collect2";
			RobotLib.emplace(ROBOTTYPE::RT_COLLECT2, rd);
		}
		{
			rd.name = U"高出力攻撃兵器α";
			rd.max_endurance = 1000;
			rd.texture_name = U"Machine_Fight1";
			RobotLib.emplace(ROBOTTYPE::RT_FIGHT1, rd);
		}
		{
			rd.name = U"高強度攻撃兵器β";
			rd.max_endurance = 1000;
			rd.texture_name = U"Machine_Fight2";
			RobotLib.emplace(ROBOTTYPE::RT_FIGHT2, rd);
		}
	}
	{
		//レシピの初期化
		std::pair<ROBOTTYPE, Array<Item>> rc;
		Item it;
		{
			rc.first = ROBOTTYPE::RT_SEARCH;
			Array<std::pair<ITEMTYPE, double>> its = {
				{IT_WOOD1,3.2},{IT_WOOD2,3.2},
				{IT_ROCK1,4.3},{IT_ROCK2,4.3},{IT_ROCK3,4.3},
				{IT_ORE1,2.6},{IT_ORE2,2.6},{IT_ORE3,2.6},{IT_ORE4,2.6}
			};
			for (int i = 0; i < its.size(); i++)
			{
				it.amount = its[i].second + Random(0.4, 0.6);
				it.it = its[i].first;
				rc.second.push_back(it);
				Recipes.push_back(rc);
				rc.second.clear();
			}
		}
		{
			rc.first = ROBOTTYPE::RT_COLLECT1;
			Array<ITEMTYPE> its_0 = { IT_WOOD1,IT_WOOD2 };
			Array<ITEMTYPE> its_1 = { IT_ROCK1,IT_ROCK2,IT_ROCK3 };
			for (int i = 0; i < its_0.size(); i++)
			{
				for (int j = 0; j < its_1.size(); j++)
				{
					it.amount = 2.5 + Random(0.4, 0.6);
					it.it = its_0[i];
					rc.second.push_back(it);
					it.amount = 1.3 + Random(0.4, 0.6);
					it.it = its_1[j];
					rc.second.push_back(it);
					Recipes.push_back(rc);
					rc.second.clear();
				}
			}
			its_0 = { IT_LIQUID1,IT_LIQUID2 };
			its_1 = { IT_ROCK1,IT_ROCK2,IT_ROCK3 };
			for (int i = 0; i < its_0.size(); i++)
			{
				for (int j = 0; j < its_1.size(); j++)
				{
					it.amount = 7.9 + Random(0.4, 0.6);
					it.it = its_0[i];
					rc.second.push_back(it);
					it.amount = 4.5 + Random(0.4, 0.6);
					it.it = its_1[j];
					rc.second.push_back(it);
					Recipes.push_back(rc);
					rc.second.clear();
				}
			}
		}
		{
			rc.first = ROBOTTYPE::RT_COLLECT2;
			Array<ITEMTYPE> its_0 = { IT_WOOD1,IT_WOOD2 };
			Array<ITEMTYPE> its_1 = { IT_ROCK1,IT_ROCK2,IT_ROCK3 };
			for (int i = 0; i < its_0.size(); i++)
			{
				for (int j = 0; j < its_1.size(); j++)
				{
					it.amount = 1.3 + Random(0.4, 0.6);
					it.it = its_0[i];
					rc.second.push_back(it);
					it.amount = 2.5 + Random(0.4, 0.6);
					it.it = its_1[j];
					rc.second.push_back(it);
					Recipes.push_back(rc);
					rc.second.clear();
				}
			}
			its_0 = { IT_LIQUID1,IT_LIQUID2 };
			its_1 = { IT_ROCK1,IT_ROCK2,IT_ROCK3 };
			for (int i = 0; i < its_0.size(); i++)
			{
				for (int j = 0; j < its_1.size(); j++)
				{
					it.amount = 4.5 + Random(0.4, 0.6);
					it.it = its_0[i];
					rc.second.push_back(it);
					it.amount = 7.9 + Random(0.4, 0.6);
					it.it = its_1[j];
					rc.second.push_back(it);
					Recipes.push_back(rc);
					rc.second.clear();
				}
			}
		}
		{
			rc.first = ROBOTTYPE::RT_FIGHT1;
			Array<ITEMTYPE> its_0 = { IT_ROCK1,IT_ROCK2,IT_ROCK3 };
			Array<ITEMTYPE> its_1 = { IT_ORE1,IT_ORE2,IT_ORE3,IT_ORE4 };
			for (int i = 0; i < its_0.size(); i++)
			{
				for (int j = 0; j < its_1.size(); j++)
				{
					it.amount = 4.3 + Random(0.4, 0.6);
					it.it = its_0[i];
					rc.second.push_back(it);
					it.amount = 8.5 + Random(0.4, 0.6);
					it.it = its_1[j];
					rc.second.push_back(it);
					Recipes.push_back(rc);
					rc.second.clear();
				}
			}
		}
		{
			rc.first = ROBOTTYPE::RT_FIGHT2;
			Array<ITEMTYPE> its_0 = { IT_ROCK1,IT_ROCK2,IT_ROCK3 };
			Array<ITEMTYPE> its_1 = { IT_ORE1,IT_ORE2,IT_ORE3,IT_ORE4 };
			for (int i = 0; i < its_0.size(); i++)
			{
				for (int j = 0; j < its_1.size(); j++)
				{
					it.amount = 8.5 + Random(0.4, 0.6);
					it.it = its_0[i];
					rc.second.push_back(it);
					it.amount = 4.3 + Random(0.4, 0.6);
					it.it = its_1[j];
					rc.second.push_back(it);
					Recipes.push_back(rc);
					rc.second.clear();
				}
			}
		}
	}
	{
		//表示の時だけ係数をかける

		ItemData id;
		id.name = U"液体α";
		ItemLib.emplace(ITEMTYPE::IT_LIQUID1, id);
		id.name = U"液体β";
		ItemLib.emplace(ITEMTYPE::IT_LIQUID2, id);
		id.name = U"木材α";
		ItemLib.emplace(ITEMTYPE::IT_WOOD1, id);
		id.name = U"木材β";
		ItemLib.emplace(ITEMTYPE::IT_WOOD2, id);
		id.name = U"石材α";
		ItemLib.emplace(ITEMTYPE::IT_ROCK1, id);
		id.name = U"石材β";
		ItemLib.emplace(ITEMTYPE::IT_ROCK2, id);
		id.name = U"石材γ";
		ItemLib.emplace(ITEMTYPE::IT_ROCK3, id);
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
			constexpr double r2at1 = 0.65 * 0.5, r2at2 = 0.20 * 0.5, r2at3 = 0.10 * 0.5, r2at4 = 0.05 * 0.5;
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
		{
			constexpr double dis_1 = 0.2, dis_2 = 0.5;//ゆらぎ(番号は機械)
			//縦に見て和が1ならok
			//以下は石
			constexpr double rat1_1 = 0.33 * 0.5;//素材1の存在率
			constexpr double rat2_1 = 0.33 * 0.5;//素材2の存在率
			constexpr double rat3_1 = 0.33 * 0.5;//素材3の存在率
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

			td.c = Color(237, 180, 130);
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
			TileLib.emplace(MAPTILE::MT_SAND, td);
		}
		//水場
		{
			constexpr double dis_1 = 0.1, dis_2 = 0.2;//ゆらぎ(番号は機械)
			double t = 1.0;
			td.c = Color(0, 0, 180);
			td.irs_1.clear();
			td.irs_1.push_back(makeItemRate(ITEMTYPE::IT_LIQUID2, t* (1 - dis_1), t* (1 + dis_1)));
			td.irs_2.clear();
			td.irs_2.push_back(makeItemRate(ITEMTYPE::IT_LIQUID2, t * (1 - dis_2), t * (1 + dis_2)));
			TileLib.emplace(MAPTILE::MT_WATER, td);
		}
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
				{
					t.et = ENEMYTYPE::ET_NONE;
					double distance = Vec2(Abs(MAP_CENTER_X - i), Abs(MAP_CENTER_Y - j)).length();
					double parcent = Clamp((distance - MAP_CENTER_X / 7) / 21, 0.0, 1.0);
					if (RandomBool(parcent))
					{
						if (RandomBool()) t.et = ENEMYTYPE::ET_TYPE2;
						else t.et = ENEMYTYPE::ET_TYPE1;
					}
					//超えられない壁
					distance = Vec2(Abs(MAP_CENTER_X - i), Abs(MAP_CENTER_Y - j)).length();
					parcent = Clamp((distance - MAP_CENTER_X / 3) / 21, 0.0, 1.0);
					if (RandomBool(parcent))
					{
						t.et = ENEMYTYPE::ET_WALL;
					}
				}

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
		md.from = U"Developer's Support";
		md.title = U"このソフトの使い方";
		md.text = U"こちらはメッセージソフトです。\n各種メッセージを送受信することができます。\n\n現在は受信専用に設定されています。\n設定の変更には管理者の権限が必要です。";
		MailLib.push_back(md);
		md.from = U"admin";
		md.title = U"指令書";
		md.text = U"こちらは指令内容の確認として送られたメッセージになります。\n\n今回の指令は以下になります。\n・未知の惑星の探査、及び生存圏の確保\n　・可能な限り広い場所を探索し、地図を埋めてください。\n　・船外ユニットを用いて生存可能領域を広げてください。\n\n我らが市民として正しい行動規範に則って活動することを切に願います。";
		MailLib.push_back(md);
		md.from = U"admin";
		md.title = U"操作指示書";
		md.text = U"この端末には、今回の探索に向けて最適化された以下のソフトウェアが搭載されております。\n・message + \n　・メッセージ送受信ソフトウェアです\n・!nvent\n　・探査に必要な装置を作成するソフトウェアです\n・WLD\n　・探査機の派遣が行えます\n・ken\n　・探査機の様子を確認できます\n・(cast)4\n　・気候観測ソフトウェアです\n・stash\n　・コマンドにより制御できるソフトウェアです";
		MailLib.push_back(md);
		md.from = U"supporter-bot";
		md.title = U"通知";
		md.text = U"無事発射が完了しました。";
		MailLib.push_back(md);
		md.from = U"supporter-bot";
		md.title = U"警告";
		md.text = U"不安定な磁場が発生しています。";
		MailLib.push_back(md);
		md.from = U"不明";
		md.title = U"無題";
		md.text = U"fjzh0n348flka04hoah3hqfaj-zj0rhaonwo4hhfnalalonosjda";
		MailLib.push_back(md);
		md.from = U"supporter-bot";
		md.title = U"警告";
		md.text = U"目的の航路から逸脱しています。";
		MailLib.push_back(md);
		md.from = U"supporter-bot";
		md.title = U"警告";
		md.text = U"不安定な磁場が発生しています。";
		MailLib.push_back(md);
		md.from = U"supporter-bot";
		md.title = U"警告";
		md.text = U"本艇は制御を失いました。\n強い衝撃に備えてください。";
		MailLib.push_back(md);
		md.from = U"supporter-bot";
		md.title = U"警告";
		md.text = U"幾つかの装置が破損しました。\n一時的に端末を停止します。";
		MailLib.push_back(md);
	}
	{
		//各種音源のセットアップ
		AudioLib.emplace(U"SetUpBGM", Audio(U"resource/computer_startup.ogg", Arg::loopBegin = 30s, Arg::loopEnd = 60s));
		AudioLib.emplace(U"LastBGM", Audio(U"resource/lastbgm.ogg"));
		AudioLib.emplace(U"SE_MSG", Audio(U"resource/se_message.ogg"));
		AudioLib.emplace(U"SE_TOUCH", Audio(U"resource/se_touch.ogg"));
	}
	{
		//各種画像のセットアップ
		TextureLib.emplace(U"BackGround", Texture(U"resource/bg.png"));
		TextureLib.emplace(U"OSIcon", Texture(U"resource/icon.png"));
		TextureLib.emplace(U"ICON_OS", Texture(U"resource/icon_os.png"));

		TextureLib.emplace(U"ICON_MAP", Texture(U"resource/icon_map.png"));
		TextureLib.emplace(U"ICON_INV", Texture(U"resource/icon_inventor.png"));
		TextureLib.emplace(U"ICON_CMP", Texture(U"resource/icon_command.png"));
		TextureLib.emplace(U"ICON_MIL", Texture(U"resource/icon_mail.png"));

		TextureLib.emplace(U"Frame", Texture(U"resource/frame.png"));
		TextureLib.emplace(U"Button_Close", Texture(U"resource/button_close.png"));
		TextureLib.emplace(U"Button_Max", Texture(U"resource/button_max.png"));
		TextureLib.emplace(U"Button_Min", Texture(U"resource/button_min.png"));

		TextureLib.emplace(U"Machine_Search", Texture(U"resource/machine_find.png"));
		TextureLib.emplace(U"Machine_Collect1", Texture(U"resource/machine_get1.png"));
		TextureLib.emplace(U"Machine_Collect2", Texture(U"resource/machine_get2.png"));
		TextureLib.emplace(U"Machine_Fight1", Texture(U"resource/machine_attack1.png"));
		TextureLib.emplace(U"Machine_Fight2", Texture(U"resource/machine_attack2.png"));
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
					hit_enemy_cnt++;
					break;
				case ROBOTTYPE::RT_COLLECT1:
					is_break = true;
					hit_enemy_cnt++;
					break;
				case ROBOTTYPE::RT_COLLECT2:
					is_break = true;
					hit_enemy_cnt++;
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
					hit_enemy_cnt++;
					break;
				case ROBOTTYPE::RT_COLLECT1:
					is_break = true;
					hit_enemy_cnt++;
					break;
				case ROBOTTYPE::RT_COLLECT2:
					is_break = true;
					hit_enemy_cnt++;
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
					while (RandomBool(0.1))
					{
						Item ifr;
						ifr.it = ITEMTYPE(Random(int(ITEMTYPE::IT_NUM)));
						ifr.amount = Random(0.1);
						rw.items.push_back(ifr);
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
					while (RandomBool(0.1))
					{
						Item ifr;
						ifr.it = ITEMTYPE(Random(int(ITEMTYPE::IT_NUM)));
						ifr.amount = Random(0.1);
						rw.items.push_back(ifr);
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
				hit_enemy_cnt++;
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
				AudioLib[U"SE_TOUCH"].playOneShot();
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

RectF getOverlappingRectF(const RectF& rf1, const RectF& rf2)
{
	Vec2 lu = Vec2(Max(rf1.pos.x, rf2.pos.x), Max(rf1.pos.y, rf2.pos.y));
	Vec2 rd = Vec2(Min(rf1.pos.x + rf1.size.x, rf2.pos.x + rf2.size.x), Min(rf1.pos.y + rf1.size.y, rf2.pos.y + rf2.size.y));
	RectF rc = RectF(lu, rd - lu);
	if (lu.x > rd.x || lu.y > rd.y)rc = RectF(Vec2(0, 0), Vec2(0, 0));
	return rc;
}
