#include "Window.h"

#include <render/Colors.h>
#include <render/infos/UIRenderInfo.h>
#include <render/infos/proxy/UIInfos.h>

namespace ui
{
	int32_t Window::addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) {
		int32_t border = 1;
		if (this->minimized) {
			depth = this->topBar.get()->addRenderInfo(ticks, renderInfo, depth++);
			renderInfo.uiRenderInfo.addPixelRectangle(
				this->topBar.get()->getScreenRectangle().getPixelSize(),
				this->topBar.get()->getScreenRectangle().getBottomLeft() - border,
				this->topBar.get()->getScreenRectangle().getTopRight() + border,
				COLORS::UI::WINDOWBACKGROUND,
				depth++
			);
			return depth;
		}
		else {
			depth = this->BaseMulti::addRenderInfo(ticks, renderInfo, depth++);
			renderInfo.uiRenderInfo.addPixelRectangle(
				this->screenRectangle.getPixelSize(),
				this->screenRectangle.getBottomLeft() - border,
				this->screenRectangle.getTopRight() + border,
				COLORS::UI::WINDOWBACKGROUND,
				depth++
			);
			return depth;
		}
	}

	CallBackBindResult Window::runGlobalBinds(UIInfo& uiInfo, UserData& userData) {
		if (this->minimized) {
			return this->topBar.get()->runGlobalBinds(uiInfo, userData);
		}
		else {
			return this->BaseMulti::runGlobalBinds(uiInfo, userData);
		}
	}

	CallBackBindResult Window::runGameWorldBinds(UIInfo& uiInfo, UserData& userData) {
		if (this->minimized) {
			return this->topBar.get()->runGameWorldBinds(uiInfo, userData);
		}
		else {
			return this->BaseMulti::runGameWorldBinds(uiInfo, userData);
		}
	}

	void Window::addElement(UniqueReference<Base, Base> element) {
		assert(this->main == nullptr);
		this->main = element;
		this->addElementMulti(std::move(element));
	}

	void Window::addElementMulti(UniqueReference<Base, Base> element) {
		this->BaseMulti::addElement(std::move(element));
	}

	CallBackBindResult Window::runFocussedBinds(UIInfo& uiInfo, UserData& userData) {
		if (this->minimized) {
			return this->topBar.get()->runFocussedBinds(uiInfo, userData);
		}
		else {
			return this->BaseMulti::runFocussedBinds(uiInfo, userData);
		}
	}

	CallBackBindResult Window::runOnHoverBinds(UIInfo& uiInfo, UserData& userData) {
		if (this->minimized) {
			return this->topBar.get()->runOnHoverBinds(uiInfo, userData);
		}
		else {
			return  this->BaseMulti::runOnHoverBinds(uiInfo, userData);
		}
	}

	CallBackBindResult Window::runActiveBinds(UIInfo& uiInfo, UserData& userData) {
		if (this->minimized) {
			return this->topBar.get()->runActiveBinds(uiInfo, userData);
		}
		else {
			return this->BaseMulti::runActiveBinds(uiInfo, userData);
		}
	}
}