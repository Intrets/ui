#pragma once

#include <render/Colors.h>

#include "Base.h"

namespace render
{
	struct UIInfos;
}

namespace ui
{
	class ColoredBackground : public BaseSingle
	{
	public:
		glm::vec4 color = COLORS::UI::BACKGROUND;

		ColoredBackground() = default;
		~ColoredBackground() = default;

		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;
	};
}