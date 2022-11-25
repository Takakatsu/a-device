#include "def.h"

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

Array<MyWindow*> my_wins;
Array<MyIcon*> my_icons;
MyWindow* win_active;

void initialize_lib()
{
	{
		ItemData id;
		id.name = U"木材α";
		ItemLib.emplace(ITEMTYPE::IT_WOOD1, id);
		id.name = U"木材β";
		ItemLib.emplace(ITEMTYPE::IT_WOOD2, id);
		id.name = U"石材α";
		ItemLib.emplace(ITEMTYPE::IT_ROCK1, id);
		id.name = U"石材β";
		ItemLib.emplace(ITEMTYPE::IT_ROCK2, id);
		id.name = U"鉱物α";
		ItemLib.emplace(ITEMTYPE::IT_ORE1, id);
		id.name = U"鉱物β";
		ItemLib.emplace(ITEMTYPE::IT_ORE2, id);
	}
	{
		TileData td;
		td.c = Color(255, 0, 0);
		TileLib.emplace(MAPTILE::MT_SHIP, td);
		td.c = Color(0, 255, 0);
		TileLib.emplace(MAPTILE::MT_GRASS, td);
		td.c = Color(116, 80, 48);
		TileLib.emplace(MAPTILE::MT_ROCK, td);
		td.c = Color(237, 180, 130);
		TileLib.emplace(MAPTILE::MT_SAND, td);
		td.c = Color(0, 0, 180);
		TileLib.emplace(MAPTILE::MT_WATER, td);
	}
	{
		PerlinNoise pn;
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
				else if (d < 0.2)t.tile = MAPTILE::MT_GRASS;
				else t.tile = MAPTILE::MT_ROCK;
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
			if (robo->endurance >= 100)
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

				}
				break;
				case ROBOTTYPE::RT_COLLECT2://アイテム収集系**must**
				{

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
				lg.text = robo->name + U"は探索に向かいました";
				logs.push_back(lg);
			}
			//死亡可能性ログの出力
			if (is_break)
			{
				GameLog lg;
				lg.text = robo->name + U"は予定された時間までに帰還しませんでした";
				lg.remain_time = b_rnd_max + distance;
				logs_will.push_back(lg);
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
