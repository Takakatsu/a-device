#include "def.h"

Array<MailData> MailLib;

Array<MyWindow*> my_wins;
Array<MyIcon*> my_icons;
MyWindow* win_active;

void initialize_lib()
{
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
