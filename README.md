# Steam Icon Fix
##### Fix the blank icon of steam apps
[中文版本](README_CN.md)
### Requirements
- libcurl
- jsoncpp
- winsock32
- wldap32
- winmm
- crypt32
- normaliz
- bcrypt
### Usage
Run it, and it will fix all icons found in Desktop&your library.
### Errors
- _Download Failed. Check your network! Error 23_ icon file may in use, try to reboot your machine.
- _E[GetDirFiles] Failed open dir._ No enough Premissions(it seems impossible).
- _E Steam is not installed, exiting..._ Please check your steam installation,or click this [link](https://store.steampowered.com/about/) to download steam client.
- _E Cannot find Desktop, exiting..._ Run regedit and find this location:HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders,right click the right window and create string value named Desktop,paste your desktop location in the data textbox.
- _W Cannot find steam icon dir, manual input>_ Drag your steam icon dir in {SteamInstallPath}/steam/games.
- _W Cannot find desktop dir, manual input>_ Drag your desktop dir into it
- _E invalid shortcut_ It seems broken, try delete the shortcut and re-create it.
- _E download failed. Check your network!_ Curl error code will display.(Maybe Turn off your vpn?)
### Warning
It used a third-party API to get clientIcon from appId.
