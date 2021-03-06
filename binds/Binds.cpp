#include "binds/Binds.h"

#include <misc/StringHelpers.h>

#include <mem/Global.h>

#include <misc/Log.h>

#include <sound/SoundPlayer.h>

#include "ControlState.h"
#include "Base.h"
#include "State.h"
#include "TextDisplay.h"
#include "Button.h"

namespace ui
{
	namespace BASE
	{
		void activatable(Base* ptr) {
			ptr->addGlobalBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::PRESSED }, [ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (!ptr->getScreenRectangle().contains(uiInfo.uiState.getCursor())) {
						ptr->deactivate();
						return BIND::RESULT::CONTINUE;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addOnHoverBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::PRESSED }, [ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->activate();
					return BIND::RESULT::CONTINUE | BIND::RESULT::FOCUS | BIND::RESULT::CONSUME;
				});
		}

		void focusable(Base* ptr) {
			ptr->addOnHoverBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::PRESSED }, [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME | BIND::RESULT::FOCUS;
				});
		}
		void blockWorldBinds(Base* ptr) {
			ptr->addOnHoverBind({ CONTROL::KEY::EVERY_TICK }, [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					uiInfo.controlState.setBlockWorldBinds(true);
					return BIND::RESULT::CONTINUE;
				});
		}
	}

	namespace TEXTEDIT
	{
		void clickSelect(TextDisplay* ptr) {
			ptr->setClickSupport(true);
			ptr->addOnHoverBind({ CONTROL::KEY::ACTION0, CONTROL::STATE::PRESSED },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{

					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_CLICK, 80);

					glm::ivec2 click = uiInfo.uiState.getCursor() - ptr->getScreenRectangle().getBottomLeft();

					auto screenClick = pixelToScreen(click, ptr->getScreenRectangle().getPixelSize());
					screenClick += ptr->text.getView();

					if (ptr->text.cachedRenderInfo.has_value()) {
						auto maybeIndex = ptr->text.cachedRenderInfo.value().getIndex(screenClick);
						if (maybeIndex.has_value()) {
							ptr->text.selectIndex(maybeIndex.value());
							Global<misc::Log>->putStreamLine(std::stringstream() << maybeIndex.value());
						}
					}

					return BIND::RESULT::CONTINUE | BIND::RESULT::FOCUS;
				});
		}

		void input(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::CHAR_BUFFER_CHANGED, CONTROL::STATE::PRESSED },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::INSERT) {
						Global<sound::SoundPlayer>->playSound(sound::Sample::TEXT_EDIT);

						auto lines = misc::split(0, uiInfo.controlState.getCharBuffer(), '\n', true, true);
						for (size_t i = 0; i < lines.size() - 1; i++) {
							ptr->insertText(lines[i]);
							ptr->text.matchWhiteSpace();
						}

						ptr->insertText(lines.back());
						if (uiInfo.controlState.getCharBuffer().back() == '\n') {
							ptr->text.matchWhiteSpace();
						}
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});
		}

		void inputNoLineBreaks(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::CHAR_BUFFER_CHANGED, CONTROL::STATE::PRESSED },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					auto text = uiInfo.controlState.getCharBuffer();
					text.erase(std::remove_if(text.begin(), text.end(), [](auto const c) -> bool
						{
							return c == '\n';
						}));
					ptr->insertText(text);
					Global<sound::SoundPlayer>->playSound(sound::Sample::TEXT_EDIT);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void backspace(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::BACKSPACE, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->backspaceChar();
					Global<sound::SoundPlayer>->playSound(sound::Sample::TEXT_DELETE);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void tab(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::TAB, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->insertText("  ");
					Global<sound::SoundPlayer>->playSound(sound::Sample::TEXT_EDIT);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void del(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::DELETE, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->deleteChar();
					Global<sound::SoundPlayer>->playSound(sound::Sample::TEXT_DELETE);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void down(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::TEXT_DOWN, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->moveCursor({ 0,1 });
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void up(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::TEXT_UP, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->moveCursor({ 0,-1 });
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void right(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::TEXT_RIGHT, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->moveCursor({ 1,0 });
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void left(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::TEXT_LEFT, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->moveCursor({ -1,0 });
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void viewUp(TextDisplay* ptr) {
			ptr->addOnHoverBind({ CONTROL::KEY::SCROLL_UP, CONTROL::STATE::PRESSED },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->text.moveView({ 0,-1 });
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void viewDown(TextDisplay* ptr) {
			ptr->addOnHoverBind({ CONTROL::KEY::SCROLL_DOWN, CONTROL::STATE::PRESSED },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->text.moveView({ 0,1 });
					Global<sound::SoundPlayer>->playSound(sound::Sample::BUTTON_HOVER, 30);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void wrap(TextDisplay* ptr) {
			ptr->setWrap(true);
		}

		void nowrap(TextDisplay* ptr) {
			ptr->setWrap(false);
		}

		void normalbinds(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::J, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveCursor({ 0,1 });
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::K, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveCursor({ 0,-1 });
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::L, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveCursor({ 1,0 });
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::H, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveCursor({ -1,0 });
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::W, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveStartWordForward();
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					else {
						return BIND::RESULT::CONTINUE;
					}
				});

			ptr->addActiveBind({ CONTROL::KEY::B, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveStartWordBackward();
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::E, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->moveEndWord();
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::X, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->deleteChar();
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::P, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->paste(uiInfo.controlState)) {
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::Y, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->yank(uiInfo.controlState)) {
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});

			ptr->addActiveBind({ CONTROL::KEY::O, CONTROL::STATE::PRESSED | CONTROL::STATE::REPEAT },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL && ptr->insertLineAfter()) {
						uiInfo.controlState.consumeControl(CONTROL::KEY::CHAR_BUFFER_CHANGED);
						ptr->setMode(TEXTDISPLAY::MODE::INSERT);
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});
		}

		void normal(TextDisplay* ptr) {
			ptr->addActiveBinds(
				{ { CONTROL::KEY::CANCEL },
				  { CONTROL::KEY::C, CONTROL::STATE::PRESSED, CONTROL::MODIFIER::CONTROL }
				},
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					ptr->setMode(TEXTDISPLAY::MODE::NORMAL);
					return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
				});
		}

		void insert(TextDisplay* ptr) {
			ptr->addActiveBind({ CONTROL::KEY::I, CONTROL::STATE::PRESSED },
				[ptr](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					if (ptr->mode == TEXTDISPLAY::MODE::NORMAL) {
						ptr->setMode(TEXTDISPLAY::MODE::INSERT);
						uiInfo.controlState.consumeControl(CONTROL::KEY::CHAR_BUFFER_CHANGED);
						return BIND::RESULT::CONTINUE | BIND::RESULT::CONSUME;
					}
					return BIND::RESULT::CONTINUE;
				});
		}
	}

	void BUTTON::close(Button* ptr) {
		ptr->setOnRelease([](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				return BIND::RESULT::CLOSE;
			});
	}

	void BUTTON::hide(Button* ptr) {
		ptr->setOnRelease([](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				return BIND::RESULT::HIDE;
			});
	}
}
