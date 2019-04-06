#ifndef BOT_LIST
#define BOT_LIST

#include "bot.h"
#include "bots\random.h"
#include "bots\srvive.h"

const bot_t bot_list [] = {
	{
		"random",
		random
	},
	{
		"srvive",
		srvive
	}
};

#endif
