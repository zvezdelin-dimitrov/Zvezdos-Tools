#include "pch.h"
#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <shobjidl_core.h>
#include <wil/resource.h>
#include <shellapi.h>
#include <string>
#include <vector>

using namespace Microsoft::WRL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

class SubExplorerCommandHandler : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand> {
public:
    SubExplorerCommandHandler(std::wstring title, std::wstring argument)
        : commandTitle(std::move(title)), extraArgument(std::move(argument)) {
    }

    IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* name) {
        *name = nullptr;
        auto title = wil::make_cotaskmem_string_nothrow(commandTitle.c_str());
        RETURN_IF_NULL_ALLOC(title);
        *name = title.release();
        return S_OK;
    }

    IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* icon) {
        *icon = nullptr;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip) {
        *infoTip = nullptr;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName) {
        *guidCommandName = GUID_NULL;
        return S_OK;
    }

    IFACEMETHODIMP GetState(_In_opt_ IShellItemArray*, _In_ BOOL, _Out_ EXPCMDSTATE* cmdState) {
        *cmdState = ECS_ENABLED;
        return S_OK;
    }

    IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray* selection, _In_opt_ IBindCtx*) noexcept try {
        if (!selection) {
            return E_INVALIDARG;
        }

        DWORD count = 0;
        selection->GetCount(&count);

        if (count == 0) {
            return E_FAIL;
        }

        wil::com_ptr_nothrow<IShellItem> shellItem;
        selection->GetItemAt(0, &shellItem);

        wil::unique_cotaskmem_string folderPath;
        if (FAILED(shellItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath))) {
            return E_FAIL;
        }

        std::wstring parameters = L"\"" + std::wstring(folderPath.get()) + L"\" " + extraArgument;

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.lpFile = L"zvezdostools.exe";
        sei.lpParameters = parameters.c_str();
        sei.nShow = SW_SHOWNORMAL;

        if (!ShellExecuteExW(&sei)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return S_OK;
    } CATCH_RETURN();

    IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags) {
        *flags = ECF_DEFAULT;
        return S_OK;
    }

    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands) {
        *enumCommands = nullptr;
        return E_NOTIMPL;
    }

private:
    std::wstring commandTitle;
    std::wstring extraArgument;
};

class ExplorerCommandHandler : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand> {
public:
    ExplorerCommandHandler() {
        subCommands.push_back(Make<SubExplorerCommandHandler>(L"Clean iPhone Photos", L"1"));
        // Add other tools here with other int arguments
    }

    IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* name) {
        *name = nullptr;
        auto title = wil::make_cotaskmem_string_nothrow(L"Zvezdo's Tools");
        RETURN_IF_NULL_ALLOC(title);
        *name = title.release();
        return S_OK;
    }

    IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* icon) {
        *icon = nullptr;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip) {
        *infoTip = nullptr;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName) {
        *guidCommandName = GUID_NULL;
        return S_OK;
    }

    IFACEMETHODIMP GetState(_In_opt_ IShellItemArray*, _In_ BOOL, _Out_ EXPCMDSTATE* cmdState) {
        *cmdState = ECS_ENABLED;
        return S_OK;
    }

    IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray*, _In_opt_ IBindCtx*) noexcept {
        return S_OK; // Parent command does nothing
    }

    IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags) {
        *flags = ECF_HASSUBCOMMANDS;
        return S_OK;
    }

    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands) {
        return Make<EnumCommands>(subCommands)->QueryInterface(IID_PPV_ARGS(enumCommands));
    }

private:
    std::vector<ComPtr<IExplorerCommand>> subCommands;

    class EnumCommands : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IEnumExplorerCommand> {
    public:
        EnumCommands(const std::vector<ComPtr<IExplorerCommand>>& commands)
            : m_commands(commands), m_current(m_commands.begin()) {
        }

        IFACEMETHODIMP Next(ULONG celt, __out_ecount_part(celt, *pceltFetched) IExplorerCommand** apUICommand, __out_opt ULONG* pceltFetched) {
            ULONG fetched{ 0 };
            wil::assign_to_opt_param(pceltFetched, 0ul);

            for (ULONG i = 0; (i < celt) && (m_current != m_commands.end()); i++) {
                m_current->CopyTo(&apUICommand[0]);
                m_current++;
                fetched++;
            }

            wil::assign_to_opt_param(pceltFetched, fetched);
            return (fetched == celt) ? S_OK : S_FALSE;
        }

        IFACEMETHODIMP Skip(ULONG) { return E_NOTIMPL; }
        IFACEMETHODIMP Reset() { m_current = m_commands.begin(); return S_OK; }
        IFACEMETHODIMP Clone(__deref_out IEnumExplorerCommand**) { return E_NOTIMPL; }

    private:
        std::vector<ComPtr<IExplorerCommand>> m_commands;
        std::vector<ComPtr<IExplorerCommand>>::const_iterator m_current;
    };
};

class __declspec(uuid("13531E9A-CB9F-4829-9D92-E7ACB070D8B3")) ExplorerCommand final : public ExplorerCommandHandler {};

CoCreatableClass(ExplorerCommand)
CoCreatableClassWrlCreatorMapInclude(ExplorerCommand)

STDAPI DllCanUnloadNow() {
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance) {
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
}