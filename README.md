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



