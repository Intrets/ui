cmake_minimum_required(VERSION 3.20)

include("./cmake/MakeModule.cmake")

set(MODULE_NAME ui)

project(${MODULE_NAME} LANGUAGES CXX)

make_module(
	MODULE_NAME ${MODULE_NAME}
	MODULE_FILES
		Base
		ControlState
		Enums
		State
		Constructer
		Proxy
		TextDisplay
		impl/Text
		binds/Binds
		SizeType
		ConstrainSize
		Button
		Pad
		Window
		Invisible
		List
		Empty
		ColoredBackground
		AnchoredProxy
		Destructible
		Hideable
		Grid
		FreeSize
		InitialSize
	CXX_STANDARD 20
	REQUIRED_LIBS
		WGLM
		GLEW
		GLFW3
		mem
		misc
		game
		render
		SDL2::SDL2-static
		SDL_Mixer
		sound
)

