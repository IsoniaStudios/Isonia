#ifdef DLL_BUILD
// internal
#include "../Isonia.h"
#include "IsoniaDLL.h"

Isonia::Isonia* global_isonia_instance = nullptr;

void* createIsoniaWindow() {
    global_isonia_instance = new Isonia::Isonia();
    return global_isonia_instance->get_window_handle();
}

void destroyIsoniaWindow(void* window_handle) {
    if (global_isonia_instance != nullptr) {
        delete global_isonia_instance;
    }
}

void run() {
    if (global_isonia_instance != nullptr) {
        global_isonia_instance->run();
    }
}
#endif
