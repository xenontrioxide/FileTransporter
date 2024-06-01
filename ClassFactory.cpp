#include <new>
#include <Shlwapi.h>

#include "shared.hpp"
#include "ClassFactory.hpp"
#include "ContextMenuHandler.hpp"


ClassFactory::ClassFactory() : RefCount(1)
{
    _InterlockedIncrement(&Shared::DllReferenceCount);
}

ClassFactory::~ClassFactory()
{
    _InterlockedDecrement(&Shared::DllReferenceCount);
}

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(ClassFactory, IClassFactory),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return _InterlockedIncrement(&RefCount);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
    uint32_t Refs = _InterlockedDecrement(&RefCount);
    if (!Refs)
    {
        delete this;
    }
    return Refs;
}

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    HRESULT hr = CLASS_E_NOAGGREGATION;

    // pUnkOuter is used for aggregation. We do not support it in the sample.
    if (pUnkOuter == NULL)
    {
        hr = E_OUTOFMEMORY;

        // Create the COM component.
        ContextMenuHandler* pExt = new (std::nothrow) ContextMenuHandler();
        if (pExt)
        {
            // Query the specified interface.
            hr = pExt->QueryInterface(riid, ppv);
            pExt->Release();
        }
    }

    return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        _InterlockedIncrement(&Shared::DllReferenceCount);
    }
    else
    {
        _InterlockedDecrement(&Shared::DllReferenceCount);
    }
    return S_OK;
}
