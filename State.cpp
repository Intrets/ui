#include "State.h"

#include <wglm/gtx/euler_angles.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <misc/Option.h>

#include "Constructer.h"
#include "Invisible.h"
#include "SizeType.h"

namespace ui
{
	CallBackBindResult State::runFrontBinds(UIInfo& uiInfo, UserData& userData) {
		if (this->UIs.size() == 0) {
			return BIND::RESULT::CONTINUE;
		}

		CallBackBindResult activeResult =
			this->UIs.front().get()->runOnHoverBinds(uiInfo, userData) |
			this->UIs.front().get()->runFocussedBinds(uiInfo, userData) |
			this->UIs.front().get()->runActiveBinds(uiInfo, userData) |
			this->UIs.front().get()->runGlobalBinds(uiInfo, userData);
		CallBackBindResult res = 0;

		bool shouldExit = false;
		if (activeResult & BIND::RESULT::CLOSE) {
			this->UIs.pop_front();
			res |= BIND::RESULT::CLOSE;
			shouldExit = true;
		}
		if (activeResult & BIND::RESULT::STOP) {
			res |= BIND::RESULT::STOP;
			shouldExit = true;
		}

		if (shouldExit) {
			return res;
		}

		return BIND::RESULT::CONTINUE;
	}

	glm::vec2 State::getCursorPositionWorld() {
		assert(0);
		return this->cursorWorld;
	}

	glm::ivec2 State::getCursor() const {
		return this->cursor;
	}

	glm::ivec2 State::getWindowSize() const {
		return this->windowSize;
	}

	glm::ivec2 State::getCursorMovement() const {
		return this->cursorMovement;
	}

	void State::runUIBinds(UIInfo& uiInfo, UserData& userData) {
		auto front = this->runFrontBinds(uiInfo, userData);

		if (front & BIND::RESULT::STOP) {
			return;
		}

		if (this->UIs.empty()) {
			return;
		}

		uiInfo.controlState.writeConsumedBuffer();

		if (this->UIs.size() > 1) {
			auto it = this->UIs.begin(), last = this->UIs.end();
			if (front & BIND::RESULT::CLOSE) {
				front &= ~BIND::RESULT::CLOSE;
			}
			else {
				++it;
			}
			for (; it != last;) {
				auto& UI = *it;
				CallBackBindResult res = UI.get()->runOnHoverBinds(uiInfo, userData) | UI.get()->runGlobalBinds(uiInfo, userData);

				if (res & BIND::RESULT::CLOSE) {
					it = this->UIs.erase(it);
				}
				else if (res & BIND::RESULT::FOCUS) {
					auto temp = std::move(UI);
					it = this->UIs.erase(it);
					this->UIs.push_front(std::move(temp));
				}
				else {
					it++;
				}
				if (res & BIND::RESULT::STOP) {
					return;
				}
				uiInfo.controlState.writeConsumedBuffer();
			}
		}

		if (!uiInfo.controlState.worldBindsBlocked()) {
			for (auto it = this->UIs.begin(); it != this->UIs.end();) {
				auto& UI = *it;
				CallBackBindResult res = UI.get()->runGameWorldBinds(uiInfo, userData);

				if (res & BIND::RESULT::CLOSE) {
					it = this->UIs.erase(it);
				}
				else if (res & BIND::RESULT::FOCUS) {
					auto temp = std::move(UI);
					it = this->UIs.erase(it);
					this->UIs.push_front(std::move(temp));
				}
				else {
					it++;
				}
				if (res & BIND::RESULT::STOP) {
					return;
				}
				uiInfo.controlState.writeConsumedBuffer();
			}
		}
	}

	void State::run(UIInfo& uiInfo, UserData& userData) {
		this->runUIBinds(uiInfo, userData);

		for (auto it = this->namedUIs.begin(), last = this->namedUIs.end(); it != last;) {
			if (!it->second.isValid()) {
				it = this->namedUIs.erase(it);
			}
			else {
				++it;
			}
		}

		for (auto& UI : this->UIsBuffer) {
			this->UIs.push_front(std::move(UI));
		}
		this->UIsBuffer.clear();

		for (auto& [name, ui] : this->namedUIsBuffer) {
			this->namedUIs.emplace(std::make_pair(name, ManagedReference<Base, Base>(ui)));
			this->UIs.push_front(std::move(ui));
		}
		this->namedUIsBuffer.clear();

		this->closedBuffer.clear();
	}

	bool State::updateSize(GLFWwindow* window) {
		int32_t x;
		int32_t y;
		glfwGetWindowSize(window, &x, &y);

		if (x <= 0 || y <= 0) {
			return false;
		}

		ScreenRectangle r{ glm::ivec2(x, y) };
		for (auto& UI : this->UIs) {
			UI.get()->updateSize(r);
		}
		return true;
	}

	void State::updateCursor(GLFWwindow* window, glm::vec2 cam) {
		double x;
		double y;
		glfwGetCursorPos(window, &x, &y);

		int32_t frameSizeX, frameSizeY;
		glfwGetFramebufferSize(window, &frameSizeX, &frameSizeY);

		this->windowSize = glm::ivec2(frameSizeX, frameSizeY);
		auto newCursor = glm::ivec2(glm::floor(x), frameSizeY - glm::floor(y));
		this->cursorMovement = newCursor - this->cursor;
		this->cursor = newCursor;


		x = x / frameSizeX;
		y = y / frameSizeY;
		y = 1 - y;
		x = 2 * x - 1;
		y = 2 * y - 1;

		float ratio = frameSizeX / static_cast<float>(frameSizeY);
		glm::vec2 viewport(ratio, 1.0f);

		viewport *= misc::Option<misc::OPTION::CL_VIEWPORTSCALE, float>::getVal();

		this->cursorWorld = cam + glm::vec2(x, y) * viewport;
	}

	void State::appendRenderInfo(int32_t ticks, render::UIInfos& renderInfo) {
		int32_t depth = 10;
		for (auto& UI : this->UIs) {
			depth = UI.get()->addRenderInfo(ticks, renderInfo, depth);
		}
	}

	void State::addUI(UniqueReference<Base, Base> ref) {
		ref.get()->addOnHoverBind({ CONTROL::KEY::MOUSE_POS_CHANGED_TOPLEVEL }, [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				return BIND::RESULT::CONSUME;
			});
		this->UIsBuffer.push_back(std::move(ref));
	}

	bool State::addNamedUI(std::string const& name, std::function<UniqueReference<Base, Base>()> f) {
		auto namedUI = this->namedUIs.find(name);

		if (namedUI != this->namedUIs.end() && namedUI->second.isValid()) {
			if (auto ui = namedUI->second.getRef()) {
				for (auto it = this->UIs.begin(); it != this->UIs.end(); it++) {
					if (*it == ui) {
						this->UIsBuffer.push_back(std::move(*it));
						this->UIs.erase(it);
						break;
					}
				}
			}
			return false;
		}

		auto namedUIBuffered = this->namedUIsBuffer.find(name);

		if (namedUIBuffered != this->namedUIsBuffer.end()) {
			return false;
		}

		this->namedUIsBuffer[name] = f();
		this->namedUIsBuffer[name].get()->addOnHoverBind({ CONTROL::KEY::MOUSE_POS_CHANGED_TOPLEVEL }, [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				return BIND::RESULT::CONSUME;
			});
		return true;
	}

	void State::addNamedUIReplace(std::string const& name, UniqueReference<Base, Base> ref) {
		this->closeNamedUI(name);

		ref.get()->addOnHoverBind({ CONTROL::KEY::MOUSE_POS_CHANGED_TOPLEVEL }, [](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
			{
				return BIND::RESULT::CONSUME;
			});

		this->namedUIsBuffer[name] = std::move(ref);
	}

	void State::closeNamedUI(std::string const& name) {
		auto namedUI = this->namedUIs.find(name);

		if (namedUI != this->namedUIs.end()) {
			if (auto ui = namedUI->second.getRef()) {
				for (auto it = this->UIs.begin(); it != this->UIs.end(); it++) {
					if (*it == ui) {
						this->closedBuffer.push_back(std::move(*it));
						this->UIs.erase(it);
						break;
					}
				}
			}
			this->namedUIs.erase(namedUI);
		}

		auto namedUIBuffered = this->namedUIsBuffer.find(name);

		if (namedUIBuffered != this->namedUIsBuffer.end()) {
			this->closedBuffer.push_back(std::move(namedUIBuffered->second));
			this->namedUIsBuffer.erase(namedUIBuffered);
		}
	}

	void State::closeUI(WeakReference<Base, Base> ref) {
		for (auto it = this->UIs.begin(); it != this->UIs.end(); it++) {
			if (*it == ref) {
				this->closedBuffer.push_back(std::move(*it));
				this->UIs.erase(it);
				break;
			}
		}
	}

	void State::reset() {
		this->shouldReset_ = true;
	}

	bool State::shouldReset() {
		return this->shouldReset_;
	}

	void State::init() {
		{
			UniqueReference<Base, Invisible> someBinds = Global::getManager().makeUniqueRef<Invisible>();

			someBinds.get()->addGlobalBind(
				{ CONTROL::KEY::TOGGLE_DEBUG_RENDER },
				[](UIInfo& uiInfo, UserData& userData) -> CallBackBindResult
				{
					misc::Option<misc::OPTION::GR_DEBUG, bool>::toggle();
					return BIND::RESULT::CONTINUE;
				});

			this->UIs.push_back(std::move(someBinds));
		}


	}

	void State::clear() {
		this->shouldReset_ = false;
		this->namedUIs.clear();
		this->UIs.clear();
		this->UIsBuffer.clear();
	}

	State::State() {
		this->init();
	}
}
