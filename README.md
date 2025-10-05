## C语言学习实战项目

使用AI学习C语言并搭建实战项目

## 环境准备

需要安装 `gcc` 编译工具。

### Windows

- [WinLibs](https://winlibs.com/): 推荐。解压后，添加环境变量即可使用。
- make命令：重命名 `mingw32-make.exe` 即可: copy mingw32-make.exe make.exe

- UCRT 版本 - 现代 Windows 版本推荐（Win10+）: https://github.com/brechtsanders/winlibs_mingw/releases/download/15.2.0posix-13.0.0-msvcrt-r1/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1.zip

- MSVCRT 版本 - 更好的旧版 Windows 兼容性: https://github.com/brechtsanders/winlibs_mingw/releases/download/15.2.0posix-13.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-13.0.0-r2.zip

尽管 GCC 和 MinGW-w64 可以在其他平台（例如 Linux）上使用以生成 Windows 可执行文件，但 WinLibs 项目仅专注于构建可在 Windows 上原生运行的版本。

~~ MinGW-w64 ~~
~~ TDM-GCC ~~

### Linux

```bash
apt update
apt install gcc
```