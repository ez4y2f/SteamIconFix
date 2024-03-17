# Steam Icon Fix
##### Fix the blank icon of steam apps
[中文版本](README_CN.md)
### Requirements
- libcurl
- winsock32
- wldap32
- winmm
- crypt32
- normaliz
- bcrypt
### Usage
Run it, and it will fix all icons on desktop.
### Errors
- _E[GetDirFiles] Failed open dir._ No enough Premissions(it seems impossible).
- _E Cannot find programfiles(x86), exiting..._ Can't find environment variables.
- _E Cannot find UserProfile, exiting..._ The same.
- _W Cannot find steam icon dir, manual input>_ Drag your steam icon dir in {SteamInstallPath}/steam/games.
- _W Cannot find desktop dir, manual input>_ Drag your desktop dir into it
- _E invalid shortcut_ It seems broken, try delete the shortcut and re-create it.
- _E download failed. Check your network!_ Curl error code will display.(Maybe Turn off your vpn?)
### Warning
The http is unsecure while https leads to a error in my network environment.
