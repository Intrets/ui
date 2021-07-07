#include "AnchoredProxy.h"

#include <game/player/PlayerInfo.h>

#include "State.h"
#include "Constructer.h"
#include "Destructible.h"

namespace ui
{
	void AnchoredProxy::closeProxy() {
		if (this->proxyBase.isValid()) {
			this->destructible.get()->destruct = true;
		}
	}

	bool AnchoredProxy::hasProxy() {
		return this->proxyBase.isValid();
	}

	void AnchoredProxy::setProxy(UniqueReference<Base, Base> ref, State& uiState) {
		this->closeProxy();

		Global::push();

		free();
		this->destructible = ui::destructible();
		this->proxyBase.set(this->destructible);

		auto ptr = makeEnd(std::move(ref));
		ptr.get()->addGlobalBind({ CONTROL::KEY::ACTION0 }, [this](PlayerInfo& playerInfo) -> CallBackBindResult
		{
			if (!this->getScreenRectangle().contains(playerInfo.uiState.getCursor())) {
				return BIND::RESULT::CLOSE;
			}
			else {
				return BIND::RESULT::CONTINUE;
			}
		});

		ptr.get()->addOnHoverBind({ CONTROL::KEY::ACTION0 }, [](PlayerInfo& playerInfo) -> CallBackBindResult
		{
			return BIND::RESULT::CONSUME;
		});

		uiState.addUI(Global::pop());
	}

	ScreenRectangle AnchoredProxy::updateSize(ScreenRectangle newScreenRectangle) {
		this->screenRectangle = newScreenRectangle;
		if (auto proxyBaseRef = this->proxyBase.getRef()) {
			const int32_t inf = 5000;
			switch (this->alignment) {
				case ALIGNMENT::TOP:
					newScreenRectangle.setBot(-inf);
					break;
				case ALIGNMENT::BOTTOM:
					newScreenRectangle.setTop(inf);
					break;
				case ALIGNMENT::LEFT:
					newScreenRectangle.setRight(inf);
					break;
				case ALIGNMENT::RIGHT:
					newScreenRectangle.setLeft(-inf);
					break;
				case ALIGNMENT::TOPRIGHT:
					newScreenRectangle.setBottomLeft({ -inf, -inf });
					break;
				case ALIGNMENT::TOPLEFT:
					newScreenRectangle.setBottomRight({ inf, -inf });
					break;
				case ALIGNMENT::BOTTOMLEFT:
					newScreenRectangle.setTopRight({ inf, inf });
					break;
				case ALIGNMENT::BOTTOMRIGHT:
					newScreenRectangle.setTopLeft({ -inf, inf });
					break;
				default:
					break;
			}
			proxyBaseRef.get()->updateSize(newScreenRectangle);
		}
		return this->screenRectangle;
	}

	int32_t AnchoredProxy::addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) {
		return depth;
	}

	AnchoredProxy::~AnchoredProxy() {
		if (this->proxyBase.isValid()) {
			this->destructible.get()->destruct = true;
		}
	}
}