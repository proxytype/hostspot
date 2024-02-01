# HostSpot
Create wifi hotspot with shared internet connection from another adapter.

Leveraging the WIN32 API, our solution enables the creation of a WiFi hotspot with a shared internet connection across various devices. This is achieved by dynamically generating a "host network adapter" through the WIN32 API. The internet connection is seamlessly shared from another device, facilitated by the INetSharingManager interface. This robust approach ensures a reliable and efficient method for establishing a WiFi hotspot, promoting connectivity and resource sharing across multiple devices.

![alt text](https://raw.githubusercontent.com/proxytype/hostspot/main/hostspot.gif)

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




