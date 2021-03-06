#pragma once

#include "Base.h"

#include <render/Colors.h>

namespace ui
{
	class Button : public BaseSingle
	{
	private:
		bool shrinkToFit = true;
		bool highlighted;

		glm::vec4 color = COLORS::UI::BACKGROUND;

		bool down = false;
		glm::ivec2 mousePressOffset;

	private:
		friend class Hotbar;

		CallBack onPress;
		CallBack onRelease;

	public:
		void setColor(glm::vec4 c);

		void setOnPress(CallBack f);
		void setOnRelease(CallBack f);
		void setShrinkToFit(bool b);

		bool isDown();
		void dePress();

		glm::ivec2 const& getMousePressOffset() const;

		Button();
		virtual ~Button() = default;

		virtual ScreenRectangle updateSize(ScreenRectangle newScreenRectangle) override;
		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;
	};
}