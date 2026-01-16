/**
 * @file dllmain.cpp
 * @brief DLL entry point and initialization
 * 
 * This file handles the DLL lifecycle and provides
 * the main entry point for the renderer.
 */

#include <Windows.h>
#include <stdio.h>

#ifdef _DEBUG
#define DEBUG_LOG(x) OutputDebugStringA(x)
#else
#define DEBUG_LOG(x)
#endif

/**
 * @brief DLL entry point
 */
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved
)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            DEBUG_LOG("[OFP Renderer] DLL loaded\n");
            break;
        }
        
        case DLL_THREAD_ATTACH:
        {
            break;
        }
        
        case DLL_THREAD_DETACH:
        {
            break;
        }
        
        case DLL_PROCESS_DETACH:
        {
            DEBUG_LOG("[OFP Renderer] DLL unloaded\n");
            break;
        }
    }
    
    return TRUE;
}

/**
 * @brief Get plugin information
 */
extern "C" __declspec(dllexport) const char* __cdecl GetPluginName()
{
    return "OFP Vulkan Renderer";
}

extern "C" __declspec(dllexport) const char* __cdecl GetPluginVersion()
{
    return "1.0.0.0";
}

extern "C" __declspec(dllexport) const char* __cdecl GetPluginAuthor()
{
    return "Your Name";
}

extern "C" __declspec(dllexport) const char* __cdecl GetPluginDescription()
{
    return "Modern Vulkan renderer for Operation Flashpoint";
}
