#include "ColoredBackground.h"

#include <render/infos/proxy/UIInfos.h>

namespace ui
{
	int32_t ColoredBackground::addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) {
		depth = this->BaseSingle::addRenderInfo(ticks, renderInfo, depth);
		renderInfo.uiRenderInfo.addPixelRectangle(
			this->screenRectangle.getPixelSize(),
			this->screenRectangle.getBottomLeft(),
			this->screenRectangle.getTopRight(),
			this->color,
			depth++
		);
		return depth;
	}
}