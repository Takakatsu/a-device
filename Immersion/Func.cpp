#include "def.h"

double delta;
Vec2 cursor_pos;
CLICKED_TYPE g_clicktype;
Array<MailData> MailLib;
HashTable<MAPTILE, TileData> TileLib;
Array<Array<MapTip>> MAINMAP;
HashTable<ROBOTTYPE, RobotData> RobotLib;
Array<Robot> robots_stay;
Array<Robot> robots_active;

Array<MyWindow*> my_wins;
Array<MyIcon*> my_icons;
MyWindow* win_active;

void initialize_lib()
{
	{
		TileData td;
		td.c = Color(255, 0, 0);
		TileLib.emplace(MAPTILE::SHIP, td);
		td.c = Color(0, 255, 0);
		TileLib.emplace(MAPTILE::GRASS, td);
	}
	{
		for (int i = 0; i < 30; i++)
		{
			Array<MapTip> tmptile;
			for (int j = 0; j < 30; j++)
			{
				MapTip t;
				t.is_found = false;
				t.tile = MAPTILE::GRASS;
				if (i == MAP_CENTER_X && j == MAP_CENTER_Y)
				{
					t.is_found = true;
					t.tile = MAPTILE::SHIP;
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
			robo->remain_time = Random(10.0, 30.0) + Random(distance) / 2;

			switch (MAINMAP[pos.x][pos.y].et)
			{
			case ENEMYTYPE::TYPE1:
			{
				switch (robo->rt)//任意の機械での処理**must**
				{
				case ROBOTTYPE::SEARCH:
					break;
				case ROBOTTYPE::COLLECT1:
					break;
				case ROBOTTYPE::COLLECT2:
					break;
				case ROBOTTYPE::FIGHT1:
					break;
				case ROBOTTYPE::FIGHT2:
					break;
				default:
					break;
				}
			}
				break;
			case ENEMYTYPE::TYPE2:
			{
				switch (robo->rt)//任意の機械での処理**must**
				{
				case ROBOTTYPE::SEARCH:
					break;
				case ROBOTTYPE::COLLECT1:
					break;
				case ROBOTTYPE::COLLECT2:
					break;
				case ROBOTTYPE::FIGHT1:
					break;
				case ROBOTTYPE::FIGHT2:
					break;
				default:
					break;
				}
			}
				break;
			case ENEMYTYPE::NONE:
			{
				switch (robo->rt)//任意の機械での処理**must**
				{
				case ROBOTTYPE::SEARCH:
					break;
				case ROBOTTYPE::COLLECT1:
					break;
				case ROBOTTYPE::COLLECT2:
					break;
				case ROBOTTYPE::FIGHT1:
					break;
				case ROBOTTYPE::FIGHT2:
					break;
				default:
					break;
				}
			}
				break;
			default:
				break;
			}
			MAINMAP[pos.x][pos.y].is_found = true;
			robots_active.push_back(*robo);
			it = robots_stay.erase(it);
			return true;
		}
		++it;
	}
	return false;
}
