#pragma once

#ifdef DLL_BUILD
#define ISONIA_DLL_API __declspec(dllexport)

extern "C" ISONIA_DLL_API void* createIsoniaWindow();
extern "C" ISONIA_DLL_API void destroyIsoniaWindow(void* window_handle);
extern "C" ISONIA_DLL_API void run();
#endif
