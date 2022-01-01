# HostSpot
Create wifi hotspot with shared internet connection from another adapter.

![alt text](https://raw.githubusercontent.com/proxytype/hostspot/main/hostspot.gif)

Use WIN32 api to create wifi hotspot with shared internet connection from different device by creating "host network adapter" and shared the internet connection from another device using INetSharingManager.

## WIN32 Objects And Structures
[- INetSharingManager](https://docs.microsoft.com/en-us/windows/win32/api/netcon/nn-netcon-inetsharingmanager)<br>
[- INetSharingEveryConnectionCollection](https://docs.microsoft.com/en-us/windows/win32/api/netcon/nn-netcon-inetsharingeveryconnectioncollection)<br>
[- NETCON_PROPERTIES](https://docs.microsoft.com/en-us/windows/win32/api/netcon/ns-netcon-netcon_properties)<br>
[- INetConnection](https://docs.microsoft.com/en-us/windows/win32/api/netcon/nn-netcon-inetconnection)<br>
[- INetSharingConfiguration](https://docs.microsoft.com/en-us/windows/win32/api/netcon/nn-netcon-inetsharingconfiguration)<br>
[- WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS](https://docs.microsoft.com/en-us/windows/win32/api/wlanapi/ns-wlanapi-wlan_hosted_network_connection_settings)<br>

## Win32 Functions
 - WlanOpenHandle
 - WlanCloseHandle
 - WlanHostedNetworkSetProperty
 - WlanHostedNetworkSetSecondaryKey
 - WlanHostedNetworkForceStop
 - WlanHostedNetworkStartUsing




