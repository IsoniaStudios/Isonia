#pragma once

#ifdef DLL_BUILD
#define ISONIA_DLL_API __declspec(dllexport)

extern "C" ISONIA_DLL_API void* CreateIsoniaWindow();
extern "C" ISONIA_DLL_API void DestroyIsoniaWindow(void* windowHandle);
extern "C" ISONIA_DLL_API void Run();
#endif
