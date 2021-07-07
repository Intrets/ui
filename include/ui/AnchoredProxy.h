#pragma once

#include "Base.h"

namespace ui
{
	class Destructible;

	class AnchoredProxy : public BaseEnd
	{
	private:
		ALIGNMENT alignment = ALIGNMENT::TOP;

	public:
		ManagedReference<Base, Base> proxyBase;
		WeakReference<Base, Destructible> destructible;

		void closeProxy();
		bool hasProxy();
		void setProxy(UniqueReference<Base, Base> ref, State& uiState);

		virtual ScreenRectangle updateSize(ScreenRectangle newScreenRectangle) override;
		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;

		AnchoredProxy() = default;
		virtual ~AnchoredProxy();
	};
}