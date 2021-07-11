#pragma once

#include <optional>
#include <variant>
#include <functional>
#include <vector>

#include <wglm/glm.hpp>

#include <mem/ReferenceManager.h>

#include <misc/Rectangle.h>

#include <render/infos/UIRenderInfo.h>
#include <render/infos/TextRenderInfo.h>

#include "ControlState.h"
#include "Enums.h"

namespace render
{
	struct UIInfos;
}

namespace game
{
	struct GameState;
}

struct UserData;

namespace ui
{
	class State;

	struct UIInfo
	{
		ControlState& controlState;
		State& uiState;
	};

	class Base;

	namespace BIND
	{
		enum RESULT
		{
			CONTINUE = 1 << 0,
			CLOSE = 1 << 1,
			STOP = 1 << 2,
			FOCUS = 1 << 3,
			CONSUME = 1 << 4,
			HIDE = 1 << 5,
		};
	}

	using CallBackBindResult = int32_t;
	using CallBack = std::function<CallBackBindResult(UIInfo& uiInfo, UserData& userData)>;

	class State;

	class Base
	{
	protected:
		bool active = false;

		using Bind = std::pair<BindControl, CallBack>;

		std::vector<Bind> onHoverBinds;
		std::vector<Bind> activeBinds;
		std::vector<Bind> focussedBinds;
		std::vector<Bind> globalBinds;
		std::vector<Bind> gameWorldBinds;

	public:
		Handle selfHandle;

		Handle getSelfHandle();

		ScreenRectangle screenRectangle;

		virtual void addElement(UniqueReference<Base, Base> element) = 0;
		virtual void translate(glm::vec2 p) = 0;
		virtual void setScreenPixels(glm::ivec2 px) = 0;

		bool contains(glm::vec2 p) const;

		void activate();
		void deactivate();
		bool isActive() const;

		void addGlobalBind(BindControl bindControl, CallBack callBack);
		void addFocussedBind(BindControl bindControl, CallBack callBack);
		void addOnHoverBind(BindControl bindControl, CallBack callBack);
		void addActiveBind(BindControl bindControl, CallBack callBack);
		void addGameWorldBind(BindControl bindControl, CallBack callBack);

		void addGlobalBinds(std::vector<BindControl> const& bindControls, CallBack callBack);
		void addFocussedBinds(std::vector<BindControl> const& bindControls, CallBack callBack);
		void addOnHoverBinds(std::vector<BindControl> const& bindControls, CallBack callBack);
		void addActiveBinds(std::vector<BindControl> const& bindControls, CallBack callBack);
		void addGameWorldBinds(std::vector<BindControl> const& bindControls, CallBack callBack);

		virtual CallBackBindResult runGlobalBinds(UIInfo& uiInfo, UserData& userData);
		virtual CallBackBindResult runFocussedBinds(UIInfo& uiInfo, UserData& userData);
		virtual CallBackBindResult runOnHoverBinds(UIInfo& uiInfo, UserData& userData);
		virtual CallBackBindResult runActiveBinds(UIInfo& uiInfo, UserData& userData);
		virtual CallBackBindResult runGameWorldBinds(UIInfo& uiInfo, UserData& userData);

		virtual ScreenRectangle updateSize(ScreenRectangle newScreenRectangle) = 0;

		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) = 0;

		ScreenRectangle const& getScreenRectangle() const;

		virtual TYPE getUIOType();

		virtual ~Base() = default;
	};

	class BaseMulti : public Base
	{
	protected:
		std::vector<UniqueReference<Base, Base>> elements;

	public:
		void clear();

		virtual void addElement(UniqueReference<Base, Base> element) override;

		virtual void translate(glm::vec2 p) override;
		virtual void setScreenPixels(glm::ivec2 px) override;

		virtual ScreenRectangle updateSize(ScreenRectangle newScreenRectangle) override;

		virtual CallBackBindResult runGlobalBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runFocussedBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runOnHoverBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runActiveBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runGameWorldBinds(UIInfo& uiInfo, UserData& userData) override;

		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;

		BaseMulti() = default;
		virtual ~BaseMulti() = default;
	};

	class BaseSingle : public Base
	{
	protected:
		UniqueReference<Base, Base> main;

	public:
		virtual void addElement(UniqueReference<Base, Base> element) override;

		virtual void translate(glm::vec2 p) override;
		virtual void setScreenPixels(glm::ivec2 px) override;

		virtual ScreenRectangle updateSize(ScreenRectangle newScreenRectangle) override;

		virtual CallBackBindResult runGlobalBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runFocussedBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runOnHoverBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runActiveBinds(UIInfo& uiInfo, UserData& userData) override;
		virtual CallBackBindResult runGameWorldBinds(UIInfo& uiInfo, UserData& userData) override;

		virtual int32_t addRenderInfo(int32_t ticks, render::UIInfos& renderInfo, int32_t depth) override;

		BaseSingle() = default;
		virtual ~BaseSingle() = default;
	};

	class BaseEnd : public Base
	{
	private:
		virtual void addElement(UniqueReference<Base, Base> element) override;

	public:
		virtual void translate(glm::vec2 p) override;
		virtual void setScreenPixels(glm::ivec2 px) override;

		BaseEnd() = default;
		virtual ~BaseEnd() = default;
	};

	class BaseInvisibleEnd : public Base
	{
	private:
		virtual void addElement(UniqueReference<Base, Base> element) override;

	public:
		virtual void translate(glm::vec2 p) override;
		virtual void setScreenPixels(glm::ivec2 px) override;

		virtual ScreenRectangle updateSize(ScreenRectangle newScreenRectangle) override;

		BaseInvisibleEnd() = default;
		virtual ~BaseInvisibleEnd() = default;
	};


}
