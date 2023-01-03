# vita-webdav-client
Simple WebDAV Client App for Vita for connecting to local WebDAV server or Cloud Storage that supports WebDAV

Tested with following:
- **(Recommeded)** [SFTPgo](https://github.com/drakkan/sftpgo) - For local hosted WebDAV server. Can also be used as a webdav frontend for Cloud Storage like AWS S3, Azure Blob or Google Storage.
- box.com
- mega.nz
- 4shared.com


![Preview](/screenshot.jpg)

# Requirements
The app is built with imgui-vita which needs the **libshacccg.suprx** extracted. A message will be displayed that you are missing the libshacccg.suprx files on starting the app.

Follow this wiki to extract the libshaccg.suprx

https://samilops2.gitbook.io/vita-troubleshooting-guide/shader-compiler/extract-libshacccg.suprx

# Know Issues
 - Opening the Input Editor multiple times crashes the application. I can't figure out what's wrong. If somebody knows what's the problem. Help me out.

# Multi Language support
The appplication can auto-detect the following standard PSVITA languages
```
Japanese
English
French
Spanish
German
Italian
Dutch
Portuguese_BR
Protugese_PT
Russian
Korean
Chinese
Polish
```

For **Non-Standard** Languages like **Croatian, Catalan, Euskera, Galego, Hungarian, Indonesian, Greek, Thai**

User must modify the file **ux0:data/SMBCLI0001/config.ini** and update the **language** setting to Croatian, Catalan, Euskera, Galego, Hungarian, Indonesian, Greek or Thai

# Credits
Author of webdav-client-cpp used to build this app. https://github.com/CloudPolis/webdav-client-cpp

Copied some of the language code from yoyoloader https://github.com/Rinnegatamante/yoyoloader_vita

Also thx to @Rinnegatamante for imgui-vita library
