#pragma once

#include <vector>
#include <unordered_map>
#include <list>

#include <wglm/glm.hpp>

#include <mem/ReferenceManager.h>
#include <misc/Misc.h>

#include "Base.h"

namespace render
{
	struct UIInfos;
}

namespace ui
{
	class State
	{
	private:
		bool shouldReset_ = false;

		glm::vec2 cursorWorld;

		glm::ivec2 cursorMovement;
		glm::ivec2 cursor;
		glm::ivec2 windowSize;

		CallBackBindResult runFrontBinds(UIInfo& uiInfo, UserData& userData);

		std::unordered_map<std::string, ManagedReference<Base, Base>> namedUIs;

	public:
		std::optional<std::string> loadGame;
		std::optional<std::string> saveGame;

		std::list<UniqueReference<Base, Base>> UIs;

		std::vector<UniqueReference<Base, Base>> UIsBuffer;
		std::unordered_map<std::string, UniqueReference<Base, Base>> namedUIsBuffer;

		std::vector<UniqueReference<Base, Base>> closedBuffer;

		glm::vec2 getCursorPositionWorld();
		glm::ivec2 getCursor() const;
		glm::ivec2 getWindowSize() const;
		glm::ivec2 getCursorMovement() const;

		void runUIBinds(UIInfo& uiInfo, UserData& userData);
		void run(UIInfo& uiInfo, UserData& userData);

		bool updateSize(GLFWwindow* window);
		void updateCursor(GLFWwindow* window, glm::vec2 cam);
		void appendRenderInfo(int32_t ticks, render::UIInfos& renderInfo);

		void addUI(UniqueReference<Base, Base> ref);

		// Returns if new UI is created or previous one is brought to the front
		// true - new object created
		// false - already exists and brought to the front
		bool addNamedUI(std::string const& name, std::function<UniqueReference<Base, Base>()> f);

		void addNamedUIReplace(std::string const& name, UniqueReference<Base, Base> ref);

		void closeNamedUI(std::string const& name);
		void closeUI(WeakReference<Base, Base> ref);

		void reset();
		bool shouldReset();

		void init();
		void clear();

		State();
		~State() = default;

		NO_COPY_MOVE(State);
	};
}