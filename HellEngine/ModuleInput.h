#ifndef __H_MODULEINPUT__
#define __H_MODULEINPUT__

#include "SDL/include/SDL_scancode.h"
#include "EventWindow.h"
#include "KeyState.h"
#include "Module.h"
#include "Point.h"

#define NUM_MOUSE_BUTTONS 5
#define MAX_KEYS 300

class ModuleInput : public Module
{

public:

	ModuleInput();

	/* Destructor */
	virtual ~ModuleInput();

	/* Called before render is available */
	bool Init();

	/* Called before the first frame */
	bool Start();

	/* Called each loop iteration */
	UpdateStatus PreUpdate();

	/* Called before quitting */
	bool CleanUp();

	/* Check key states (includes mouse and joy buttons) */
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouseButtons[id - 1];
	}

	/* Check for window events last frame */
	bool GetWindowEvent(EventWindow code) const;

	/* Get mouse / axis position */
	const iPoint& GetMouseMotion() const;
	const iPoint& GetMousePosition() const;

private:
	bool		windowEvents[(int)EventWindow::WE_COUNT];
	KeyState*	keyboard = nullptr;
	KeyState	mouseButtons[NUM_MOUSE_BUTTONS];
	iPoint mouseMotion;
	iPoint mouse;
};

#endif /* __MODULEINPUT_H__ */
