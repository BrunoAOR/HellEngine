#include "ComponentUiButton.h"

ComponentUiButton::ComponentUiButton(GameObject * owner) : ComponentUIElement(owner)
{
}

ComponentUiButton::~ComponentUiButton()
{
}

void ComponentUiButton::OnEditor()
{
}

ButtonStatusType ComponentUiButton::GetButtonStatus()
{
	return buttonStatusType;
}

void ComponentUiButton::SetButtonStatus(ButtonStatusType buttonStatusTypeValue)
{
	buttonStatusType = buttonStatusTypeValue;
}
