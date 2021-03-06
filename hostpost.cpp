// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <wlanapi.h>
#include <iostream>
#include <netcon.h>
#include <vector>
#include <codecvt>
#include <locale>
#pragma comment(lib,"Wlanapi.lib")

using namespace std;

HANDLE hConsole = NULL;
INetSharingManager* netSharingManager = nullptr;
IEnumVARIANT * enumVariants = NULL;
vector<wstring> devices;
vector<wstring> hosts;

WORD attributes = 0;

int srcDevice = -1;
int dstDevice = -1;


std::wstring stringToWstring(const char* utf8Bytes)
{
    //setup converter
    using convert_type = std::codecvt_utf8<typename std::wstring::value_type>;
    std::wstring_convert<convert_type, typename std::wstring::value_type> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(utf8Bytes);
}

DWORD openHandle(PHANDLE handle) {

    DWORD dwServiceVersion = 0;
    HANDLE hClient = NULL;
    DWORD dwError = 0;

    if (ERROR_SUCCESS != (dwError = WlanOpenHandle(
        WLAN_API_VERSION,
        NULL,               // reserved
        &dwServiceVersion,
        handle
    )))
    {
        return dwError;
    }

    if (WLAN_API_VERSION_MAJOR(dwServiceVersion) < WLAN_API_VERSION_MAJOR(WLAN_API_VERSION_2_0))
    {
        WlanCloseHandle(handle, NULL);
        return -1;
    }

    return TRUE;
}

BOOL isAllowedActive(HANDLE handle) {

    BOOL bIsAllow = TRUE;
    WLAN_HOSTED_NETWORK_REASON dwFailedReason;
    DWORD dwReturnValue = WlanHostedNetworkSetProperty(handle,
        wlan_hosted_network_opcode_enable,
        sizeof(BOOL),
        &bIsAllow,
        &dwFailedReason,
        NULL);

    if (ERROR_SUCCESS != dwReturnValue)
    {
        bIsAllow = FALSE;
    }

    return bIsAllow;
}

BOOL setPassword(HANDLE handle, std::wstring password) {

    UCHAR keyBuf[100] = { 0 };
    WideCharToMultiByte(CP_ACP,
        0,
        password.c_str(),
        password.length(),
        (LPSTR)keyBuf,
        100,
        NULL,
        NULL);

    DWORD dwReturnValue = WlanHostedNetworkSetSecondaryKey(handle,
        strlen((const char*)keyBuf) + 1,
        keyBuf,
        TRUE,
        FALSE,
        NULL,
        NULL);
    if (ERROR_SUCCESS != dwReturnValue)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL setSSID(HANDLE handle, std::wstring ssid) {

    WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS wlanConnectionSetting;
    memset(&wlanConnectionSetting, 0, sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS));

    WideCharToMultiByte(CP_ACP,
        0,
        ssid.c_str(),   // Save SSID CString types
        ssid.length(),    // SSID the length of a string
        (LPSTR)wlanConnectionSetting.hostedNetworkSSID.ucSSID,
        32,
        NULL,
        NULL);

    wlanConnectionSetting.hostedNetworkSSID.uSSIDLength = strlen((const char*)wlanConnectionSetting.hostedNetworkSSID.ucSSID);
    wlanConnectionSetting.dwMaxNumberOfPeers = 100;
    WLAN_HOSTED_NETWORK_REASON dwFailedReason;

    DWORD dwReturnValue = WlanHostedNetworkSetProperty(handle,
        wlan_hosted_network_opcode_connection_settings,
        sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS),
        &wlanConnectionSetting,
        &dwFailedReason,
        NULL);

    if (ERROR_SUCCESS != dwReturnValue)
    {
        return FALSE;
    }

    return TRUE;
}

int displayDevices() {

    for (int i = 0; i < devices.capacity(); i++)
    {
        wprintf(L"%d: %s\n", i, devices[i].c_str());
    }

    wprintf(L"Please Select Device:");

    int selectedIndex = -1;

    scanf_s("%d", &selectedIndex);

    return selectedIndex;

}

int displayHosts() {
    
    for (int i = 0; i < hosts.capacity(); i++)
    {
        wprintf(L"%d: %s\n", i, hosts[i].c_str());
    }

    wprintf(L"Please Select Host:");

    int selectedIndex = -1;

    scanf_s("%d", &selectedIndex);

    return selectedIndex;

}


BOOL getNetSharingManager() {
    CoInitialize(nullptr);
   HRESULT hr = CoCreateInstance(CLSID_NetSharingManager,
        nullptr,
        CLSCTX_ALL,
        IID_INetSharingManager,
        reinterpret_cast<LPVOID*>(&netSharingManager));

    if (hr != S_OK) {
        printf("Unable to get Sharing Manager...");
        return FALSE;
    }

    return TRUE;

}

BOOL getConnectionsList() {

    INetSharingEveryConnectionCollection * connectionList = NULL;
  
    IUnknown * pUnk = NULL;
   
    HRESULT hr = netSharingManager->get_EnumEveryConnection(&connectionList);

    if (!connectionList)
    {
        wprintf(L"failed to get EveryConnectionCollection!\r\n");
        return FALSE;
    }

    hr = connectionList->get__NewEnum(&pUnk);

    if (hr != S_OK) {
        wprintf(L"Unable to get Connection List");
        return FALSE;
    }

    if (pUnk)
    {
        hr = pUnk->QueryInterface(__uuidof(IEnumVARIANT), (void**)&enumVariants);
        pUnk->Release();
    }

    return TRUE;

}

BOOL createList() {

    HRESULT hr = E_FAIL;
    VARIANT variant;
    INetConnection * netConnection = NULL;
    VariantInit(&variant);

    while (S_OK == enumVariants->Next(1, &variant, NULL))
    {
        if (V_VT(&variant) == VT_UNKNOWN)
        {
            V_UNKNOWN(&variant)->QueryInterface(__uuidof(INetConnection), (void**)&netConnection);
            if (netConnection)
            {
                NETCON_PROPERTIES* properties = NULL;
                netConnection->GetProperties(&properties);

                if (properties->Status == NCS_CONNECTED) {
                 
                    if (wcsstr(properties->pszwDeviceName, L"Hosted Network") == NULL) {
                        devices.push_back(properties->pszwDeviceName);
                    }
                    else {
                        hosts.push_back(properties->pszwDeviceName);
                    }
                }
            }
        }
    }

    return TRUE;
}

BOOL setSharing(const std::wstring srcName, const std::wstring dstName) {

    HRESULT hr = E_FAIL;
    VARIANT variant;
    INetConnection * netConnection = NULL;
    INetSharingConfiguration * sharingConfiguration = NULL;
    VariantInit(&variant);
    enumVariants->Reset();

    while (S_OK == enumVariants->Next(1, &variant, NULL))
    {
        if (V_VT(&variant) == VT_UNKNOWN)
        {
            V_UNKNOWN(&variant)->QueryInterface(__uuidof(INetConnection), (void**)&netConnection);
            if (netConnection)
            {
                NETCON_PROPERTIES* properties = NULL;
                netConnection->GetProperties(&properties);

                if (properties->Status == NCS_CONNECTED) {
                    netSharingManager->get_INetSharingConfigurationForINetConnection(netConnection, &sharingConfiguration);
                    
                    if (wcsstr(properties->pszwDeviceName, srcName.c_str()) != NULL) {
                       
                        hr = sharingConfiguration->DisableSharing();
                        Sleep(500);
                        hr = sharingConfiguration->EnableSharing(ICSSHARINGTYPE_PUBLIC);
                        wprintf(L"%s\n", srcName.c_str());
                        wprintf(L"Set Connection sharing...\n");
                    }
                    
                    if (wcsstr(properties->pszwDeviceName, dstName.c_str())) {
                        hr = sharingConfiguration->DisableSharing();
                        Sleep(500);
                        hr = sharingConfiguration->EnableSharing(ICSSHARINGTYPE_PRIVATE);
                        wprintf(L"%s\n", dstName.c_str());
                        wprintf(L"Attach to share device...\n");
                    }

                }
            }
        }
    }

    return TRUE;

}

void showHeader() {
    SetConsoleTextAttribute(hConsole,
        FOREGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    printf("    )                (                      \n");
    printf(" ( /(              ) )\\ )                )  \n");
    printf(" )\\())          ( /((()/(             ( /(  \n");
    printf("((_)\\   (   (   )\\())/(_))`  )    (   )\\()) \n");
    printf(" _((_)  )\\  )\\ (_))/(_))  /(/(    )\\ (_))/  \n");
    printf("| || | ((_)((_)| |_ / __|((_)_\\  ((_)| |_   \n");
    printf("| __ |/ _ \\(_-<|  _|\\__ \\| '_ \\)/ _ \\|  _|  \n");
    printf("|_||_|\\___//__/ \\__||___/| .__/ \\___/ \\__|  \n");
    printf("                         |_|                \n");
    printf("[HostSpot] by: RudeNetworks.com | version: 0.1 beta\n");
    printf(" - administrator privileges required.\n\n");
}

void showOptions() {

    printf("  + <ssid> - hotspot name\n");
    printf("  + <password> - min 9 chars password\n");
    printf(" usage: hostspot.exe myssid 123456789\n");

}

void printError(const char * error, int exitcode) {

    SetConsoleTextAttribute(hConsole,
        FOREGROUND_RED | FOREGROUND_RED | FOREGROUND_INTENSITY);

    printf("%s\n" ,error);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
    
    exit(exitcode);
}

int main(int argc, char* argv[]) {

    BOOL result = TRUE;
    
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO Info;
    GetConsoleScreenBufferInfo(hConsole, &Info);
    attributes = Info.wAttributes;

    showHeader();
   
    if (argc <= 2) {
        showOptions();
        printError("", -1);
        return -1;
    }

    SetConsoleTextAttribute(hConsole,
        FOREGROUND_GREEN);

    wstring ssid(stringToWstring(argv[1]));
    wstring password(stringToWstring(argv[2]));

    HANDLE wLan;
    if (openHandle(&wLan) == -1) {
        printError("Unable to create Handle", -1);
    }

    if (!isAllowedActive(wLan)) {
        printError("Error getting WLAN device", -1);
    }

    if (!setSSID(wLan, ssid)) {
        printError("Error setting SSID to device", -1);
    }

    if (!setPassword(wLan, password)) {
        printError("Error setting Password to device", -1);
    }

    WlanHostedNetworkForceStop(wLan, NULL, NULL);
    WlanHostedNetworkStartUsing(wLan, NULL, NULL);


    if (!getNetSharingManager()) {
        printError("Error getting NetSharingManager", -1);
    }


    if (!getConnectionsList()) {
        printError("Error getting Connections List", -1);
    }

    createList();

    if (devices.capacity() == 0 || hosts.capacity() == 0) {
        printError("Not found Devices or Hosts", -1);
    }

    int device = displayDevices();
    int host = displayHosts();

    if (device == -1 || host == -1) {
        printError("Not found Devices or Hosts indexes", -1);
    }

    if (device > devices.capacity() || host > hosts.capacity()) {
        printError("Not found Devices or Hosts indexes", -1);
    }

    setSharing(devices[device], hosts[host]);

    wprintf(L"Hotspot is Active!");
    getchar();
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
    getchar();
}