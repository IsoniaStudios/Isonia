#ifdef DLL_BUILD
#include "../Isonia.h"
#include "IsoniaDLL.h"

Isonia::Isonia* globalIsoniaInstance = nullptr;

void* CreateIsoniaWindow() {
    globalIsoniaInstance = new Isonia::Isonia();
    return globalIsoniaInstance->GetGLFWWindowHandle();
}

void DestroyIsoniaWindow(void* windowHandle) {
    if (globalIsoniaInstance != nullptr) {
        delete globalIsoniaInstance;
    }
}

void Run() {
    if (globalIsoniaInstance != nullptr) {
        globalIsoniaInstance->Run();
    }
}
#endif
