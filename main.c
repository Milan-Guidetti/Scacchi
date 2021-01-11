#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <windows.h>
#include "Window.h"

void init();
void close();

int main(int argc, char *argv[])
{
// Inizializzo le librerie. Se ci sono errori di inizializzazione il programma viene interrotto
   init();

// Creo la struttura contenente le informazioni della finestra
   Window window;
// Inizializzo gli elementi della struttura window
   init_window(&window);

   const int FPS = 30;  // Limito gli FPS del programma in modo da stressare meno la CPU
   unsigned int frame_start, frame_end, frame_time, frame_delay = 1000/FPS;

   while(window.running)   // Finchè la finestra è in esecuzione
   {
      frame_start = SDL_GetTicks();    // Ottengo i millisecondi da quando la libreria SDL2 è stata inizializzata

   // Gestisco gli eventi della finestra. Esempio eventi della finestra, eventi del mouse
      handle_events_window(&window);

   // Visualizzo il contenuto della finestra
      render_window(&window);

      frame_end = SDL_GetTicks();      // Ottengo i millisecondi da quando la libreria SDL2 è stata inizializzata
      frame_time = frame_end - frame_start;
      if(frame_time < frame_delay)     // Faccio una piccola pausa se necessario per stressare meno la CPU
         SDL_Delay(frame_delay - frame_time);
   }

// Chiudo la finestra liberando gli spazi di memoria allocati dinamicamente
   close_window(&window);

// Chiudo le librerie inizializzate prima
   close();

   return 0;
}

void init()
{
   if(SDL_Init(SDL_INIT_EVERYTHING))
   {
      MessageBox(NULL, "Impossibile inizializzare SDL2!", "Attenzione", MB_ICONERROR);
      exit(-1);
   }

   if(TTF_Init())
   {
      MessageBox(NULL, "Impossibile inizializzare TTF!", "Attenzione", MB_ICONERROR);
      exit(-1);
   }

   int flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP;
   if(IMG_Init(flags) != flags)
   {
      MessageBox(NULL, "Impossibile inizializzare IMG!", "Attenzione", MB_ICONERROR);
      exit(-1);
   }
}

void close()
{
   TTF_Quit();
   IMG_Quit();
   SDL_Quit();
}
