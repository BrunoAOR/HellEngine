#ifndef __H_SERIALIZER__
#define __H_SERIALIZER__

#include <string>
#include <stack>
#include "globals.h"

class Serializer
{
public:
	Serializer();
	~Serializer();

	void Test();
	void Start();

	std::string End();
	
	void OpenArray();
	void CloseArray();



private:
	Json mainJson;
	std::stack<Json*> jsonStack;
	Json* activeJson;
};

#endif // !__H_SERIALIZER__
