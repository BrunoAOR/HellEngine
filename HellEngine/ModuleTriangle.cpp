#include "SDL/include/SDL_scancode.h"
#include "ModuleTriangle.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "globals.h"

ModuleTriangle::ModuleTriangle()
{
}

/* Destructor */
ModuleTriangle::~ModuleTriangle()
{
}

/* Called after all baisc Engine Inits */
bool ModuleTriangle::Init()
{
	v[0][0] = -0.8f;
	v[0][1] = -0.5f;
	v[0][2] = 0.0f;
	c[0][0] = 1.0f;
	c[0][1] = 0.0f;
	c[0][2] = 0.0f;

	v[1][0] = 0.8f;
	v[1][1] = -0.5f;
	v[1][2] = 0.0f;
	c[1][0] = 0.0f;
	c[1][1] = 1.0f;
	c[1][2] = 0.0f;

	v[2][0] = 0.0f;
	v[2][1] = 0.5f;
	v[2][2] = 0.0f;
	c[2][0] = 0.0f;
	c[2][1] = 0.0f;
	c[2][2] = 1.0f;
	activeVertex = -1;
	activeColor = -1;
	return true;
}

/* Called each loop iteration */
UpdateStatus ModuleTriangle::Update()
{
	// Vertex selection: S=disable, A=0, D=1, W=2
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_DOWN)
	{
		activeVertex = -1;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_DOWN)
	{
		activeVertex = 0;
		LogInfo();
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_DOWN)
	{
		activeVertex = 1;
		LogInfo();
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_DOWN)
	{
		activeVertex = 2;
		LogInfo();
	}

	// Vertex motion speed +=increase, -=decrease
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KeyState::KEY_DOWN)
	{
		moveSpeed += 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KeyState::KEY_DOWN)
	{
		moveSpeed -= 5;
	}

	// Vertex motion using arrow keys
	if (activeVertex != -1)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
		{
			v[activeVertex][1] += moveSpeed * 2 / SCREEN_HEIGHT * App->time->GetDeltaTime() / 1000.0f;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
		{
			v[activeVertex][1] -= moveSpeed * 2 / SCREEN_HEIGHT * App->time->GetDeltaTime() / 1000.0f;
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
		{
			v[activeVertex][0] += moveSpeed * 2 / SCREEN_WIDTH * App->time->GetDeltaTime() / 1000.0f;
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
		{
			v[activeVertex][0] -= moveSpeed * 2 / SCREEN_WIDTH * App->time->GetDeltaTime() / 1000.0f;
		}
	}

	// Color selection : N=disable, R=0, G=1, B=2
	if (App->input->GetKey(SDL_SCANCODE_N) == KeyState::KEY_DOWN)
	{
		activeColor = -1;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KeyState::KEY_DOWN)
	{
		activeColor = 0;
	}
	if (App->input->GetKey(SDL_SCANCODE_G) == KeyState::KEY_DOWN)
	{
		activeColor = 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_B) == KeyState::KEY_DOWN)
	{
		activeColor = 2;
	}

	// Color change: [=decrease, ]=increase
	if (activeColor != -1 && activeVertex != -1)
	{
		if (App->input->GetKey(SDL_SCANCODE_RIGHTBRACKET) == KeyState::KEY_DOWN)
		{
			c[activeVertex][activeColor] += colorChangeSpeed;
			if (c[activeVertex][activeColor] > 1.0f)
			{
				c[activeVertex][activeColor] = 1.0f;
			}
			LogInfo();
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFTBRACKET) == KeyState::KEY_DOWN)
		{
			c[activeVertex][activeColor] -= colorChangeSpeed;
			if (c[activeVertex][activeColor] < 0)
			{
				c[activeVertex][activeColor] = 0;
			}
			LogInfo();
		}
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleTriangle::LogInfo() const
{
	if (activeColor != -1)
	{
		LOGGER("\nActive Vertex: %s\nActive Color:  %s\n Color: R-%i, G-%i, B-%i", activeVertex == -1 ? "none" : std::to_string(activeVertex).c_str(), activeColor == -1 ? "none" : std::to_string(activeColor).c_str(), (int)(c[activeVertex][0] * 255), (int)(c[activeVertex][1] * 255), (int)(c[activeVertex][2] * 255));
	}
	else
	{
		LOGGER("\nActive Vertex: %s\nActive Color:  none", activeVertex == -1 ? "none" : std::to_string(activeVertex).c_str());
	}
}
