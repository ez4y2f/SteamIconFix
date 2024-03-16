# Steam Icon Fix
##### Fix the blank icon of steam apps
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
- _W Cannot find steam icon dir, manual input>__ Drag your steam icon dir in {SteamInstallPath}/steam/games.
- _E Cannot find desktop dir, exiting..._ Bruh.
- _E invalid shortcut_ It seems broken, try delete the shortcut and re-create it.
- _E download failed. Check your network!_ Curl error code will display.(Maybe Turn off your vpn?)
### Warning
The http is unsecure while https leads to a error in my network environment.
