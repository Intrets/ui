#pragma once

#include "../Enums.h"

#include <vector>
#include <string>
#include <optional>
#include <map>
#include <functional>

#include <misc/UpdatedCache.h>
#include <misc/Rectangle.h>

#include <render/infos/TextRenderInfo.h>
#include <render/Enums.h>

namespace render
{
	struct UIInfos;
}

namespace ui
{
	namespace impl
	{
		class Text
		{
		private:
			std::vector<std::string> lines = { "" };

			UpdatedCache<glm::ivec2> cursorCache{ glm::ivec2(0,0) };
			UpdatedCache<glm::vec2> viewCache{ glm::vec2(0.0f, 0.0f) };
			int32_t cursorIndex = 0;

			void validateView();

		private:
			bool lastClickSupport;
			glm::vec2 lastRenderedSize{ 2.0f, 2.0f };

		public:
			ScreenRectangle lastScreenRectangle;
			std::optional<render::WindowTextRenderInfo> cachedRenderInfo;
			render::FONT lastFont;
			bool lastWrap;

			glm::vec2 getView();
			glm::ivec2 getCursor();

			std::string getSelectedLine();
			std::vector<std::string> const& getLines() const;
			std::vector<std::string>& getLinesMutable();
			std::optional<Rect> getCursorQuadScreen();

			void invalidateCache();
			void makeRenderInfo(ScreenRectangle screenRectangle, render::FONT font, bool wrap, bool clickSupport);

			int32_t addRenderInfo(ScreenRectangle screenRectangle, render::UIInfos& renderInfo, render::FONT font, int32_t depth, bool wrap, int32_t tick, bool renderCursor, bool clickSupport, CURSOR::TYPE cursorType);

			bool deleteChar();
			bool backspaceChar();

			void insertString(std::string text);

			void hideCursor();
			bool moveCursor(glm::ivec2 p);
			bool moveCursor(int32_t p);
			void setCursor(glm::ivec2 p);
			void moveView(glm::ivec2 p);
			void startOfLine();

			void moveStartWordForward();
			void moveStartWordBackward();

			void moveEndWord();

			bool insertLineAfter();

			void matchWhiteSpace();

			std::optional<glm::ivec2> findNext(glm::ivec2 p, std::function<bool(char c)> f);
			std::optional<glm::ivec2> findPrevious(glm::ivec2 p, std::function<bool(char c)> f);

			// (!) leaves in invalid state (!) need to move the cursor after using this
			void zeroCursor();

			void selectIndex(int32_t index);

			// (!) will be left in invalid state if no lines are added (!)
			void empty();

			void addLine(std::string text);

			void setString(std::string text);
			void setLines(std::vector<std::string> lines);
		};
	}
}