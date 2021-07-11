#pragma once

#include "Base.h"

namespace ui
{
	class Window : public BaseMulti
	{
	public:
		enum TYPE
		{
			MINIMISE = 1 << 0,
			CLOSE = 1 << 1,
			RESIZE = 1 << 2,
			RESIZEHORIZONTAL = 1 << 3,
			RESIZEVERTICAL = 1 << 4,
			MOVE = 1 << 5,
			HIDE = 1 << 6,
		};

		WeakReference<Base, Base> main;
		WeakReference<Base, Base> topBar;

		bool minimized = false;

		Window() = default;
		virtual ~Window() = default;

		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;

		virtual CallBackBindResult runOnHoverBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runFocussedBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runActiveBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runGlobalBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runGameWorldBinds(UIInfo& uiInfo, UserData& userData) override;

		virtual void addElement(UniqueReference<Base, Base> element) override;
		virtual void addElementMulti(UniqueReference<Base, Base> element);
	};
}