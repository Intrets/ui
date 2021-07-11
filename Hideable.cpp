#include "Hideable.h"

namespace ui
{
	void Hideable::hide() {
		this->hidden = true;
	}

	void Hideable::show() {
		this->hidden = false;
		if (this->focusOnShow) {
			this->shouldFocus = true;
		}
	}

	CallBackBindResult Hideable::runGlobalBinds(UIInfo& uiInfo, UserData& userData) {
		int32_t res = 0;
		if (this->shouldFocus) {
			res |= BIND::RESULT::FOCUS;
			this->shouldFocus = false;
		}

		if (!this->hidden) {
			res |= this->BaseSingle::runGlobalBinds(uiInfo, userData);
			if (res & BIND::RESULT::HIDE) {
				res &= ~BIND::RESULT::HIDE;
				this->hidden = true;
			}
			return res;
		}
		else {
			return res | BIND::RESULT::CONTINUE;
		}
	}

	CallBackBindResult Hideable::runFocussedBinds(UIInfo& uiInfo, UserData& userData) {
		if (!this->hidden) {
			int32_t res = this->BaseSingle::runFocussedBinds(uiInfo, userData);
			if (res & BIND::RESULT::HIDE) {
				res &= ~BIND::RESULT::HIDE;
				this->hidden = true;
			}
			return res;
		}
		else {
			return BIND::RESULT::CONTINUE;
		}
	}

	CallBackBindResult Hideable::runOnHoverBinds(UIInfo& uiInfo, UserData& userData) {
		if (!this->hidden) {
			int32_t res = this->BaseSingle::runOnHoverBinds(uiInfo, userData);
			if (res & BIND::RESULT::HIDE) {
				res &= ~BIND::RESULT::HIDE;
				this->hidden = true;
			}
			return res;
		}
		else {
			return BIND::RESULT::CONTINUE;
		}
	}

	CallBackBindResult Hideable::runActiveBinds(UIInfo& uiInfo, UserData& userData) {
		if (!this->hidden) {
			int32_t res = this->BaseSingle::runActiveBinds(uiInfo, userData);
			if (res & BIND::RESULT::HIDE) {
				res &= ~BIND::RESULT::HIDE;
				this->hidden = true;
			}
			return res;
		}
		else {
			return BIND::RESULT::CONTINUE;
		}
	}

	CallBackBindResult Hideable::runGameWorldBinds(UIInfo& uiInfo, UserData& userData) {
		if (!this->hidden) {
			int32_t res = this->BaseSingle::runGameWorldBinds(uiInfo, userData);
			if (res & BIND::RESULT::HIDE) {
				res &= ~BIND::RESULT::HIDE;
				this->hidden = true;
			}
			return res;
		}
		else {
			return BIND::RESULT::CONTINUE;
		}
	}

	int32_t Hideable::addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) {
		if (!this->hidden) {
			return this->BaseSingle::addRenderInfo(ticks, renderInfo, depth);
		}
		else {
			return depth;
		}
	}
}
