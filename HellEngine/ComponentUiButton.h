#ifndef __H_COMPONENT_UI_BUTTON__
#define __H_COMPONENT_UI_BUTTON__

#include "ComponentUIElement.h"
#include "Color.h"
class ComponentUiImage;

enum class ButtonStatus { DEFAULT, HOVER, PRESSED, DISABLED };
enum class ButtonTransitionType { COLOR, SPRITE };

class ComponentUiButton : public ComponentUIElement
{
public:

	ComponentUiButton(GameObject* owner);
	virtual ~ComponentUiButton();

	virtual void OnEditor() override;

	void SetTargetImage(ComponentUiImage* componentUiImage);

	ButtonStatus GetButtonStatus();
	void SetButtonStatus(ButtonStatus buttonStatus);

	ButtonTransitionType GetTransitionType() const;
	void SetTransitionType(ButtonTransitionType buttonTransitionType);
	void SetTransitionColor(ButtonStatus buttonStatus, const Color& color);
	void SetTransitionColor(ButtonStatus buttonStatus, const float* color);

	bool SetTransitionImage(ButtonStatus buttonStatus, const char* path);

private:
	void UpdateUiImage();

private:
	struct ButtonState
	{
		float color[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
		char imagePath[256] = "";
		unsigned int textureId = 0;
	};

	unsigned int activeButtonState = 0;
	ButtonState buttonStates[4];
	ComponentUiImage *uiImage = nullptr;

	ButtonStatus currentButtonStatus;
	ButtonTransitionType currentButtonTransitionType;


};

#endif __H_COMPONENT_UI_BUTTON__
