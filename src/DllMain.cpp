#include "Shared.hpp"
#include <memory>

#include "ClassFactory.hpp"
#include "Reg.hpp"

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if (!IsEqualGUID(Shared::ClsidContextMenuHandler, rclsid))
        return CLASS_E_CLASSNOTAVAILABLE;

    try
    {
        const auto Factory = new ClassFactory();
        if (Factory)
        {
            const auto Result = Factory->QueryInterface(riid, ppv);
            Factory->Release();
            return Result;
        }
    }
    catch ([[maybe_unused]] const std::bad_alloc& e)
    {
        return E_OUTOFMEMORY;
    }

    return E_UNEXPECTED;
}

STDAPI DllCanUnloadNow()
{
    if (Shared::DllReferenceCount)
    {
        return S_FALSE;
    }

    return S_OK;
}

STDAPI DllRegisterServer()
{
    const auto ModuleName = Shared::GetModulename();
    if (ModuleName.empty())
        return HRESULT_FROM_WIN32(GetLastError());

    const auto ServerRegisterStatus = RegisterInprocServer(ModuleName.c_str(), Shared::ClsidContextMenuHandler, Shared::FriendlyClassName, Shared::ThreadingModel);
    if (!SUCCEEDED(ServerRegisterStatus))
        return ServerRegisterStatus;

    const auto ServerContextRegisterStatus = RegisterShellExtContextMenuHandler(L"*", Shared::ClsidContextMenuHandler, Shared::FriendlyClassName);
    return ServerContextRegisterStatus;
}

STDAPI DllUnregisterServer()
{
    const auto ServerUnregisterStatus = UnregisterInprocServer(Shared::ClsidContextMenuHandler);
    if (!SUCCEEDED(ServerUnregisterStatus))
    {
        return ServerUnregisterStatus;
    }

    const auto ServerContextUnregisterStatus = UnregisterShellExtContextMenuHandler(L"*", Shared::ClsidContextMenuHandler);
    return ServerContextUnregisterStatus;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        Shared::DllHandle = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

