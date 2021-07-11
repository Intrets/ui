#include "Destructible.h"

namespace ui
{
	Destructible::Destructible() {
		this->addGlobalBind({ CONTROL::KEY::EVERY_TICK }, [this](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
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