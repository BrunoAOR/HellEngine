#include "ComponentUiInputText.h"

ComponentUiInputText::ComponentUiInputText(GameObject * owner) : ComponentUIElement(owner)
{
}

ComponentUiInputText::~ComponentUiInputText()
{
}

void ComponentUiInputText::OnEditor()
{
	
}

int ComponentUiInputText::GetMaxChars()
{
	return maxChars;
}

void ComponentUiInputText::SetMaxChars(int maxCharsValue)
{
	maxChars = maxCharsValue;
}

bool ComponentUiInputText::GetIsPassword()
{
	return isPassword;
}

void ComponentUiInputText::SetIsPassword(bool isPasswordValue)
{
	isPassword = isPasswordValue;
}
