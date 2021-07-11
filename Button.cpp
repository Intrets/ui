#include "Button.h"

#include <mem/Global.h>

#include <sound/SoundPlayer.h>

#include <render/infos/proxy/UIInfos.h>
#include <render/infos/UIRenderInfo.h>

#include "State.h"

#include <SDL_mixer.h>

namespace ui
{
	void Button::setColor(glm::vec4 c) {
		this->color = c;
	}

	void Button::setOnPress(CallBack f) {
		this->onPress = f;
	}

	void Button::setOnRelease(CallBack f) {
		this->onRelease = f;
	}

	void Button::setShrinkToFit(bool b) {
		this->shrinkToFit = b;
	}

	bool Button::isDown() {
		return this->down;
	}

	void Button::dePress() {
		this->down = false;
	}

	glm::ivec2 const& Button::getMousePressOffset() const {
		return this->mousePressOffset;
	}

	Button::Button() {
		this->onPress = [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
		{
			return BIND::RESULT::CONTINUE;
		};

		this->onRelease = [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
		{
			return BIND::RESULT::CONTINUE;
		};

		this->addOnHoverBind({ CONTROL::KEY::MOUSE_POS_CHANGED_TOPLEVEL, CONTROL::STATE::PRESSED },
			[this](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				if (this->isDown() && !this->isActive()) {
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
				}
				this->activate();
				return BIND::RESULT::CONSUME;
			});

		this->addGlobalBind({ CONTROL::KEY::MOUSE_POS_CHANGED, CONTROL::STATE::PRESSED }, [this](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				if (!this->getScreenRectangle().contains(uiInfo.uiState.getCursor()) ||
					!uiInfo.controlState.activated({ CONTROL::KEY::MOUSE_POS_CHANGED_TOPLEVEL })) {
					this->deactivate();
				}
				return BIND::RESULT::CONTINUE;
			});

		this->addOnHoverBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::PRESSED }, [this](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				this->down = true;
				this->mousePressOffset = uiInfo.uiState.getCursor() - this->getScreenRectangle().getTopLeft();
				Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_CLICK, 80);
				return this->onPress(uiInfo, userData) | BIND::RESULT::FOCUS | BIND::RESULT::CONSUME;
			});

		this->addGlobalBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::RELEASED }, [this](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				if (this->down) {
					this->down = false;

					if (this->getScreenRectangle().contains(uiInfo.uiState.getCursor())) {
						return this->onRelease(uiInfo, userData) | BIND::RESULT::CONTINUE;
					}
					else {
						return BIND::RESULT::CONTINUE;
					}
				}
				else {
					return BIND::RESULT::CONTINUE;
				}
			});
	}

	ScreenRectangle Button::updateSize(ScreenRectangle newScreenRectangle) {
		auto temp = this->main.get()->updateSize(newScreenRectangle);
		if (this->shrinkToFit) {
			newScreenRectangle = temp;
		}
		this->screenRectangle = newScreenRectangle;
		return this->screenRectangle;
	}

	int32_t Button::addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) {
		glm::vec4 c = this->color;
		if (down) {
			c = COLORS::DARKEN(c);
		}
		else if (this->active) {
			c = COLORS::LIGHTEN(c);
		}

		depth = this->main.get()->addRenderInfo(ticks, renderInfo, depth + 1);
		renderInfo.uiRenderInfo.addPixelRectangle(
			this->screenRectangle.getPixelSize(),
			this->screenRectangle.getBottomLeft(),
			this->screenRectangle.getTopRight(),
			c,
			depth++
		);
		return depth;
	}
}