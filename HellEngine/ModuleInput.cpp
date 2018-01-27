#include "SDL/include/SDL.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "Application.h"
#include "KeyState.h"
#include "ModuleEditorCamera.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "UpdateStatus.h"
#include "globals.h"

ModuleInput::ModuleInput() : Module(), mouse({0, 0}), mouseMotion({0,0})
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (int)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouseButtons, (int)KeyState::KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

/* Destructor */
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

/* Called before render is available */
bool ModuleInput::Init()
{
	LOGGER("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOGGER("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

/* Called before the first frame */
bool ModuleInput::Start()
{
	return true;
}

/* Called each loop iteration */
UpdateStatus ModuleInput::PreUpdate()
{
	static SDL_Event event;

	mouseMotion.SetToZero();
	mouseWheel.SetToZero();
	memset(windowEvents, false, (int)EventWindow::WE_COUNT * sizeof(bool));
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KeyState::KEY_IDLE)
				keyboard[i] = KeyState::KEY_DOWN;
			else
				keyboard[i] = KeyState::KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
				keyboard[i] = KeyState::KEY_UP;
			else
				keyboard[i] = KeyState::KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouseButtons[i] == KeyState::KEY_DOWN)
			mouseButtons[i] = KeyState::KEY_REPEAT;

		if(mouseButtons[i] == KeyState::KEY_UP)
			mouseButtons[i] = KeyState::KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSdlGL3_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			windowEvents[(int)EventWindow::WE_QUIT] = true;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				App->window->onWindowResize();
				App->renderer->onWindowResize();
				App->editorCamera->onWindowResize();
				break;

				/* case SDL_WINDOWEVENT_LEAVE: */
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				windowEvents[(int)EventWindow::WE_HIDE] = true;
				break;

				/* case SDL_WINDOWEVENT_ENTER: */
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				windowEvents[(int)EventWindow::WE_SHOW] = true;
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouseButtons[event.button.button - 1] = KeyState::KEY_DOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			mouseButtons[event.button.button - 1] = KeyState::KEY_UP;
			break;

		case SDL_MOUSEMOTION:
			mouseMotion.x += event.motion.xrel;
			mouseMotion.y += event.motion.yrel;
			mouse.x = event.motion.x;
			mouse.y = event.motion.y;
			break;

		case SDL_MOUSEWHEEL:
			mouseWheel.x = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? event.wheel.x : -event.wheel.x;
			mouseWheel.y = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? event.wheel.y : -event.wheel.y;
			break;
		}
	}

	if(GetWindowEvent(EventWindow::WE_QUIT) == true || GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
		return UpdateStatus::UPDATE_STOP;

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called before quitting */
bool ModuleInput::CleanUp()
{
	LOGGER("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool ModuleInput::GetWindowEvent(EventWindow ev) const
{
	return windowEvents[(int)ev];
}

const iPoint& ModuleInput::GetMouseMotion() const
{
	return mouseMotion;
}

const iPoint& ModuleInput::GetMousePosition() const
{
	return mouse;
}

const iPoint & ModuleInput::GetMouseWheel() const
{
	return mouseWheel;
}
