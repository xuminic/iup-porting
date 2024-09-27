# iup-porting
IUP is a multi-platform toolkit for building graphical user interfaces.
It was developed at Tecgraf by means of the partnership with PETROBRAS/CENPES.
It offers APIs in three basic languages: C, Lua and LED. 

The iup-porting is a maintenance project to make IUP a little bit easier to 
integrate into other project. It also hosts some test snippets.
I simply downloaded the IUP release package, made my own patches, built and 
tested some functions, then commit here.

The IUP details and documents can be found at [tecgraf.puc-rio.br](http://webserver2.tecgraf.puc-rio.br/iup/).

The downloadable packages of IUP can be found at [SourceForge](https://sourceforge.net/projects/iup/files/).

IUP is hosted in [SourceForge](https://sourceforge.net/projects/iup/).
You may find it is MIT License so iup-porting is set to MIT accordingly.
IUP License is attached in the project as `COPYRIGHT`.

## Build IUP
The default build:
```
make
```
Clean everything, include the dependency files:
```
make clean
```
When both libgtk2_dev and libgtk3_dev were installed, IUP would prefer to libgtk3. 
To override this:
```
USE_GTK2=1 make
```
On the other way, this should also work:
```
USE_GTK3=1 make
```

### Windows XP
Managed to build the IUP in Windows XP, though there are many trade-offs.
Need to install MinGW 2013 from SourceForge, then
- set environment variable `MSYSTEM` to `MINGWXP`
  ```
  export MSYSTEM=MINGWXP
  ```
  so the build system will known the host system in Windows XP.
  - It will set compiler path to `mingw3`, where is `/mingw/bin`
  - It will disable the Draw driver with alpha and anti-aliasing

- manually building by `make iup_all`

- The pre-built libaraies for Windows XP are available in `WinXP_mingw4_lib`

  You don't have to compile the IUP in Windows. The iup-porting packed a set of library for Windows XP
  from the time machine in `WinXP_mingw4_lib`. It has been tested working in MinGW


