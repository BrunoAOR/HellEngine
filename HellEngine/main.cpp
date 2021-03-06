#pragma comment( lib, "Brofiler/libx86/ProfilerCore32.lib")
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#include "Brofiler/include/Brofiler.h"
#include "SDL/include/SDL_main.h"
#include "Application.h"
#include "MainStates.h"
#include "UpdateStatus.h"
#include "globals.h"
#include "memLeaks.h"

Application* App = nullptr;

int main(int argc, char ** argv)
{
	ReportMemoryLeaks();

	int mainReturn = EXIT_FAILURE;
	MainStates state = MainStates::MAIN_CREATION;

	while (state != MainStates::MAIN_EXIT)
	{
		switch (state)
		{
		case MainStates::MAIN_CREATION:

			LOGGER("Application Creation --------------");
			App = new Application();
			state = MainStates::MAIN_START;
			break;

		case MainStates::MAIN_START:

			LOGGER("Application Init --------------");
			if (App->Init() == false)
			{
				LOGGER("Application Init exits with error -----");
				state = MainStates::MAIN_EXIT;
			}
			else
			{
				state = MainStates::MAIN_UPDATE;
				LOGGER("Application Update --------------");
			}

			break;

		case MainStates::MAIN_UPDATE:
		{
			BROFILER_FRAME("HellEngine Loop")
			UpdateStatus updateReturn = App->Update();

			if (updateReturn == UpdateStatus::UPDATE_ERROR)
			{
				LOGGER("Application Update exits with error -----");
				state = MainStates::MAIN_EXIT;
			}

			if (updateReturn == UpdateStatus::UPDATE_STOP)
				state = MainStates::MAIN_FINISH;
		}
		break;

		case MainStates::MAIN_FINISH:

			LOGGER("Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOGGER("Application CleanUp exits with error -----");
			}
			else
				mainReturn = EXIT_SUCCESS;

			state = MainStates::MAIN_EXIT;

			break;

		}
	}

	RELEASE(App);
	LOGGER("Bye :)\n");
	return mainReturn;
}
