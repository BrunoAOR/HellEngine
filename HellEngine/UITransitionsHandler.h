#ifndef __H_UI_TRANSITIONS_HANDLER
#define __H_UI_TRANSITIONS_HANDLER

#include "Color.h"
class ComponentUiImage;

struct TransitionStateInfo
{
	float color[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	char imagePath[256] = "";
	unsigned int textureId = 0;
};
enum class TransitionState { DEFAULT, HOVER, PRESSED, DISABLED };
enum class TransitionType { COLOR, SPRITE };

class UITransitionsHandler
{
public:
	UITransitionsHandler();
	~UITransitionsHandler();

	void SetTargetImage(ComponentUiImage* componentUiImage);

	TransitionState GetTransitionState();
	void SetTransitionState(TransitionState transitionState);

	TransitionType GetTransitionType() const;
	void SetTransitionType(TransitionType transitionType);
	void SetTransitionColor(TransitionState transitionState, const Color& color);
	void SetTransitionColor(TransitionState transitionState, const float* color = nullptr);

	bool SetTransitionImage(TransitionState transitionState, const char* path = nullptr);

	void UpdateUiImage();

public:
	TransitionStateInfo stateInfos[4];

private:
	unsigned int activeStateInfo = 0;
	ComponentUiImage *uiImage = nullptr;

	TransitionState currentTransitionState;
	TransitionType currentTransitionType;
};

#endif // !__H_UI_TRANSITIONS_HANDLER
