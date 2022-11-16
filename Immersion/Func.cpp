#include "def.h"

Array<MailData> MailLib;
HashTable<MAPTILE, TileData> TileLib;
Array<Array<MapTip>> MAINMAP;
HashTable<ROBOTTYPE, RobotData> RobotLib;
Array<Robot> robots;

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
		md.from = U"不明";
		md.title = U"無題";
		md.text = U"fjzh0n348flka04hoah3hqfaj-zj0rhaonwo4hhfnalalonosjda";
		MailLib.push_back(md);
		md.from = U"第五支部管制室";
		md.title = U"指令";
		md.text = U"異星を調査せよ";
		MailLib.push_back(md);
	}
}
