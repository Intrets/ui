#include "TextDisplay.h"

#include <render/Colors.h>
#include <render/infos/UIRenderInfo.h>
#include <render/infos/proxy/UIInfos.h>

namespace ui
{
	TextDisplay::TextDisplay() {
		this->lineWrap = false;
	}

	TextDisplay::TextDisplay(bool lineWrap_) {
		this->lineWrap = lineWrap_;
	}

	void TextDisplay::translate(glm::vec2 p) {
		this->BaseEnd::translate(p);
		if (this->text.cachedRenderInfo.has_value()) {
			this->text.cachedRenderInfo.value().screenRectangle.translate(p);
			this->text.lastScreenRectangle.translate(p);
		}
	}

	void TextDisplay::setText(std::string const& text_) {
		this->text.setString(text_);
		this->moveCursor(glm::ivec2(0));
	}

	void TextDisplay::setText(std::vector<std::string> const& text_) {
		this->text.setLines(text_);
		this->moveCursor(glm::ivec2(0));
	}

	void TextDisplay::moveCursor(glm::ivec2 p) {
		this->text.moveCursor(p);
		this->ticksSelected = 0;
	}

	void TextDisplay::setCursor(glm::ivec2 p) {
		this->text.setCursor(p);
		this->ticksSelected = 0;
	}

	void TextDisplay::moveStartWordForward() {
		this->text.moveStartWordForward();
		this->ticksSelected = 0;
	}

	void TextDisplay::moveStartWordBackward() {
		this->text.moveStartWordBackward();
		this->ticksSelected = 0;
	}

	void TextDisplay::moveEndWord() {
		this->text.moveEndWord();
		this->ticksSelected = 0;
	}

	bool TextDisplay::yank(ControlState& controlState) {
		if (this->mode == TEXTDISPLAY::MODE::NORMAL) {
			controlState.setClipboard(this->text.getSelectedLine());
			return true;
		}
		return false;
	}

	bool TextDisplay::paste(ControlState& controlState) {
		if (this->mode == TEXTDISPLAY::MODE::NORMAL) {
			this->text.insertString(controlState.getClipboard());
			return true;
		}
		return false;
	}

	bool TextDisplay::insertLineAfter() {
		if (this->text.insertLineAfter()) {
			this->ticksSelected = 0;
			return true;
		}
		return false;
	}

	void TextDisplay::insertText(std::string text_) {
		if (this->mode == TEXTDISPLAY::MODE::INSERT) {
			this->text.insertString(text_);
			this->text.invalidateCache();
			this->moveCursor(glm::ivec2(0));
		}
	}

	void TextDisplay::backspaceChar() {
		if (this->text.backspaceChar()) {
			this->text.invalidateCache();
			moveCursor(glm::ivec2(0));
		}
	}

	void TextDisplay::deleteChar() {
		if (this->text.deleteChar()) {
			this->text.invalidateCache();
			moveCursor(glm::ivec2(0));
		}
	}

	CallBackBindResult TextDisplay::runActiveBinds(UIInfo& uiInfo, UserData& userData) {
		auto result = Base::runActiveBinds(uiInfo, userData);
		if (this->active) {
			uiInfo.controlState.blockUserInput = true;
		}
		return result;
	}

	void TextDisplay::setMode(TEXTDISPLAY::MODE mode_) {
		this->mode = mode_;
		this->ticksSelected = 0;
	}

	void TextDisplay::setShrinkToFit(bool b) {
		this->shrinkToFit = b;
	}

	void TextDisplay::setClickSupport(bool b) {
		this->clickSupport = b;
	}

	void TextDisplay::setWrap(bool b) {
		this->lineWrap = b;
	}

	ScreenRectangle TextDisplay::updateSize(ScreenRectangle newScreenRectangle) {
		if (newScreenRectangle != this->screenRectangle) {
			this->text.invalidateCache();
		}
		if (this->shrinkToFit) {
			// TODO: font customization/setting in text
			if (!this->text.cachedRenderInfo.has_value()) {
				this->text.makeRenderInfo(newScreenRectangle, render::FONT::ROBOTO_12, this->lineWrap, this->clickSupport);
			}

			auto& renderInfo = this->text.cachedRenderInfo.value();
			glm::ivec2 screenSize = renderInfo.getRenderedScreenSize();
			screenSize = glm::min(newScreenRectangle.getSize(), screenSize);
			newScreenRectangle.setWidth(screenSize.x);
			newScreenRectangle.setHeight(screenSize.y);
			this->screenRectangle = newScreenRectangle;
			return this->screenRectangle;
		}
		else {
			this->screenRectangle = newScreenRectangle;
			return this->screenRectangle;
		}
	}

	int32_t TextDisplay::addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) {
		if (this->ticksSelected < 30) {
			ticks = 0;
		}

		CURSOR::TYPE cursorType = CURSOR::TYPE::BLOCK;

		switch (this->mode) {
			case TEXTDISPLAY::MODE::INSERT:
				cursorType = CURSOR::TYPE::LINE;
				break;
			case TEXTDISPLAY::MODE::NORMAL:
				cursorType = CURSOR::TYPE::BLOCK;
				break;
			case TEXTDISPLAY::MODE::VISUAL:
				cursorType = CURSOR::TYPE::BLOCK;
				break;
			default:
				assert(0);
				break;
		}

		depth = this->text.addRenderInfo(
			this->screenRectangle,
			renderInfo,
			render::FONT::ROBOTO_12,
			depth,
			this->lineWrap,
			ticks,
			this->active,
			this->clickSupport,
			cursorType
		);

		if (this->active) {
			renderInfo.uiRenderInfo.addPixelRectangle(
				this->screenRectangle.getPixelSize(),
				this->screenRectangle.getBottomLeft(),
				this->screenRectangle.getTopRight(),
				COLORS::UI::FOCUSSED,
				depth++
			);

			this->ticksSelected++;
		}

		return depth;
	}
}
