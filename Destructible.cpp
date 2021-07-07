#include "Destructible.h"

#include <game/player/PlayerInfo.h>

namespace ui
{
	Destructible::Destructible() {
		this->addGlobalBind({ CONTROL::KEY::EVERY_TICK }, [this](PlayerInfo& playerInfo) -> CallBackBindResult
		{
			if (this->destruct) {
				return BIND::RESULT::CLOSE;
			}
			else {
				return BIND::RESULT::CONTINUE;
			}
		});
	}
}