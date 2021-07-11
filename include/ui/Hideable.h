#pragma once

#include "Base.h"

struct PlayerInfo;

namespace render
{
	struct RenderInfo;
}

namespace ui
{
	class Hideable : public BaseSingle
	{
	private:
		bool hidden = false;

		// if the parent container should be brought to focus
		bool shouldFocus = false;

		// if the Hideable should bring the parent back to focus on being shown again
		bool focusOnShow = true;

	public:
		Hideable() = default;
		virtual ~Hideable() = default;

		void hide();
		void show();

		virtual CallBackBindResult runGlobalBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runFocussedBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runOnHoverBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runActiveBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runGameWorldBinds(UIInfo& uiInfo, UserData& userData) override;

		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;
	};
}
