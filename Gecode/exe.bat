cl /DNDEBUG /EHsc /MD /0x /wd4355 -I"%GECODEDIR%\include" -c -Folife.obj -Tplife.cpp
cl /DNDEBUG /EHsc /MD /0x /wd4355 -I"%GECODEDIR%\include" -Felife.exe life.obj /link /LIBPATH:"%GECODEDIR%\lib"