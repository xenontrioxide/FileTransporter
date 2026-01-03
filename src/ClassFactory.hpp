#pragma once

#include <windows.h>
#include <unknwn.h>

class ClassFactory : public IClassFactory
{
public:
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);
    ClassFactory();
protected:
    ~ClassFactory();

private:
    uint32_t RefCount;
};