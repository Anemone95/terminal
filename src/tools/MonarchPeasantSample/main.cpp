﻿#include "pch.h"
#include <conio.h>
#include "Monarch.h"
#include "Peasant.h"
#include "AppState.h"
#include "../../types/inc/utils.hpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace ::Microsoft::Console;

////////////////////////////////////////////////////////////////////////////////
std::mutex m;
std::condition_variable cv;
bool dtored = false;
winrt::weak_ref<MonarchPeasantSample::implementation::Monarch> g_weak{ nullptr };

struct MonarchFactory : implements<MonarchFactory, IClassFactory>
{
    MonarchFactory() :
        _guid{ Monarch_clsid } {};

    HRESULT __stdcall CreateInstance(IUnknown* outer, GUID const& iid, void** result) noexcept final
    {
        *result = nullptr;
        if (outer)
        {
            return CLASS_E_NOAGGREGATION;
        }

        if (!g_weak)
        {
            auto strong = make_self<MonarchPeasantSample::implementation::Monarch>();

            g_weak = (*strong).get_weak();
            return strong.as(iid, result);
        }
        else
        {
            auto strong = g_weak.get();
            return strong.as(iid, result);
        }
    }

    HRESULT __stdcall LockServer(BOOL) noexcept final
    {
        return S_OK;
    }

private:
    winrt::guid _guid;
};
////////////////////////////////////////////////////////////////////////////////

DWORD registerAsMonarch()
{
    DWORD registrationHostClass{};
    check_hresult(CoRegisterClassObject(Monarch_clsid,
                                        make<MonarchFactory>().get(),
                                        CLSCTX_LOCAL_SERVER,
                                        REGCLS_MULTIPLEUSE,
                                        &registrationHostClass));
    return registrationHostClass;
}

void electNewMonarch(AppState& state)
{
    state._monarch = AppState::instantiateAMonarch();
    bool isMonarch = state.areWeTheKing(true);

    printf("LONG LIVE THE %sKING\x1b[m\n", isMonarch ? "\x1b[33m" : "");

    if (isMonarch)
    {
        state.remindKingWhoTheyAre(state._peasant);
    }
    else
    {
        // Add us to the new monarch
        state._monarch.AddPeasant(state._peasant);
    }
}

void appLoop(AppState& state)
{
    // Tricky - first, we have to ask the monarch to handle the commandline.
    // They will tell us if we need to create a peasant.

    auto dwRegistration = registerAsMonarch();
    // IMPORTANT! Tear down the registration as soon as we exit. If we're not a
    // real peasant window (the monarch passed our commandline to someone else),
    // then the monarch dies, we don't want our registration becoming the active
    // monarch!
    auto cleanup = wil::scope_exit([&]() {
        check_hresult(CoRevokeClassObject(dwRegistration));
    });

    // state.createMonarchAndPeasant();
    state.createMonarch();

    if (state.processCommandline())
    {
        return;
    }
    bool isMonarch = state.areWeTheKing(true);
    bool exitRequested = false;
    while (!exitRequested)
    {
        if (isMonarch)
        {
            exitRequested = monarchAppLoop(state);
        }
        else
        {
            exitRequested = peasantAppLoop(state);
            if (!exitRequested)
            {
                electNewMonarch(state);
                isMonarch = state.areWeTheKing(false);
            }
        }
    }
}

int main(int argc, char** argv)
{
    AppState state;
    state.initializeState();

    printf("args:[");
    for (auto& elem : wil::make_range(argv, argc))
    {
        printf("%s, ", elem);
        // This is obviously a bad way of converting args to a vector of
        // hstrings, but it'll do for now.
        state.args.emplace_back(winrt::to_hstring(elem));
    }
    printf("]\n");

    try
    {
        appLoop(state);
    }
    catch (hresult_error const& e)
    {
        printf("Error: %ls\n", e.message().c_str());
    }

    printf("We've left the app. Press any key to close.\n");
    const auto ch = _getch();
    ch;
    printf("Exiting client\n");
}
