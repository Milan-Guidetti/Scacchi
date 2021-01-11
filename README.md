# Scacchi
Semplice gioco di scacchi!

Il gioco è realizzato con interfaccia grafica tramite la libreria multimediale SDL2 scritto in linguaggio C.

## Dipendenze del programma
Development Libraries
-SDL2 https://www.libsdl.org/download-2.0.php
-SDL2_ttf https://www.libsdl.org/projects/SDL_ttf/
-SDL2_image https://www.libsdl.org/projects/SDL_image/
-Nano SVG https://github.com/memononen/nanosvg

È possibile compilare il programma con MSYS2
Termina l'installazione di MSYS2 con pacman -Syu e pacman -Su
Assicurati di riavviare il programma dopo il primo comando

Installazione di MinGW
pacman -S mingw-w64-x86_64-gcc

Installazione delle librerie SDL2
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-SDL2_ttf
pacman -S mingw-w64-x86_64-SDL2_image

E infine scarica Nano SVG https://github.com/memononen/nanosvg
File necessari:
src/nanosvg.h
src/nanosvgrast.h

Se non utilizzi MinGW offerto da MSYS2 ricordati di linkare le librerie di SDL2

Se preferisci puoi compilare il programma da riga di comando:

gcc main.c -o Scacchi -IC:\msys64\mingw64\include -LC:\msys64\mingw64\lib -lmingw32 -lSDL2main -lSDL2.dll -lSDL2_ttf.dll -lSDL2_image.dll -Wl,--no-undefined -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid -s -static-libstdc++ -static-libgcc -static -m64

Sostituisci C:\msys64\mingw64\include la cartella contenente i file.h
Sostituisci C:\msys64\mingw64\lib la cartella contenente i file.a o file.dll.a
flag -m64 per creare programmi a 64 bit
Aggiungi il flag -mwindows se vuoi nascondere la console
