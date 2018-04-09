#ifndef __H_DLL_CONTROL_
#define __H_DLL_CONTROL__

#ifdef GAMEDLL_EXPORTS  
#define GAMEDLL_API __declspec(dllexport)   
#define ENGINE_API __declspec(dllimport)
#else  
#define GAMEDLL_API __declspec(dllimport)   
#define ENGINE_API __declspec(dllexport)
#endif

#endif