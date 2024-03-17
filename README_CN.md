# Steam Icon Fix
##### 修复Steam空白的桌面图标
### 食用方法
直接运行，选择CDN时可以直接回车，自动修复桌面和开始菜单的快捷方式
### 常见问题
- _E[GetDirFiles] Failed open dir._ 试试管理员方式运行？
- _E Cannot find programfiles(x86), exiting..._ 环境变量丢了，没见过这种
- _E Cannot find UserProfile, exiting..._ 一样
- _W Cannot find steam icon dir, manual input>_ 把你Steam安装目录拖进来(Steam安装目录/steam/games)
- _W Cannot find desktop dir, manual input>_ 把桌面目录拖进来
- _E invalid shortcut_ 试试把这个快捷方式删掉，重新在库里创建个快捷方式
- _E download failed. Check your network!_ 可以把错误码发给我，试着关掉vpn加速器反代，或者换个网络环境
- _下载下来的图标还是0KB空白的_ 在选择CDN时候换个其他的，关掉反代，或者换个网络环境
- _有些图标显示Needn't re-download但仍然空白_ 等待程序运行完，出现Success
### 警告
采用http下载图标，可能不安全
### 感谢
- libcurl
- winsock32
- wldap32
- winmm
- crypt32
- normaliz
- bcrypt
- @inory121 Github 手动选择桌面路径
- @Zinc-in Github 在开始菜单查找
- @在下叶板 Bilibili 帮我找出好多bug（（