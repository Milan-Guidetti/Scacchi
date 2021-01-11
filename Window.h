#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "Chessboard.h"

// Creo la struttua Window
/*
typedef struct Window
{

}Window;
*/
// Oppure
/*
struct Window
{

};
*/
// typedef struct Window Window;    // Per evitare di scrivere struct Window tutte le volte che creo un oggetto di tipo Window

typedef struct Window
{
   SDL_Window *window;     // Finestra contenente la barra del titolo, la posizione, le dimensioni, come deve essere visualizzata la finestra
   SDL_Renderer *renderer; // Strutta per visualizzare il contenuto della finestra
   SDL_Event e;   // Eventi della finestra
   bool running;  // Non è concesso inizializzare in C. Variabile booleana che indica se la finestra è in esecuzione

   Chessboard chessboard;  // Scacchiera
}Window;

// Funzione per creare una finestra
SDL_Window *create_window(const char *title, int x, int y, int w, int h, unsigned int flags)
{
   SDL_Window *window = SDL_CreateWindow(title, x, y, w, h, flags);
   if(!window)
   {
      MessageBox(NULL, "Impossibile Creare la finestra!", "Attenzione", MB_ICONERROR);
      exit(-1);
   }

   return window;
}

// Funzione per creare renderer. Struttura per visualizzare il contenuto della finestra
SDL_Renderer *create_renderer(SDL_Window *window)
{
   SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   if(!renderer)
   {
      MessageBox(NULL, "Impossibile Creare renderer!", "Attenzione", MB_ICONERROR);
      exit(-1);
   }

   return renderer;
}

// Possiamo definire questa funzione come il costruttore di Window se fosse C++
void init_window(Window *window)
{
   window->running = true;
   window->window = create_window("SDL2 Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1300, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
   window->renderer = create_renderer(window->window);

   init_chessboard(window->window, window->renderer, &window->chessboard);
}

// Possiamo definire questa funzione come il distruttore di Window se fosse C++
void close_window(Window *window)
{
   close_chessboard(&window->chessboard);
   SDL_DestroyRenderer(window->renderer);
   SDL_DestroyWindow(window->window);
}

// Funzione per gestire gli eventi della finestra
void handle_events_window(Window *window)
{
   while(SDL_PollEvent(&window->e))    // Finchè ci sono eventi in coda
   {
      switch(window->e.type)  // Tipo di evento che si è verificato
      {
      case SDL_WINDOWEVENT:
         switch(window->e.window.event)
         {
         case SDL_WINDOWEVENT_CLOSE:   // Evento che si verifica quando la finestra viene chiusa
            window->running = false;
            break;

         case SDL_WINDOWEVENT_SIZE_CHANGED:
         // Quando la finestra viene ridimensionata ridimensiono di conseguenza le dimensioni della scacchiera
            resize_chessboard(window->window, window->renderer, &window->chessboard);
            break;
         }
         break;   // Fine switch

      case SDL_MOUSEBUTTONDOWN:  // Evento che si verifica quando l'utente preme un tasto qualsiasi del mouse. Questo evento non si verifica se l'utente muove il mouse
         if(window->e.button.button == SDL_BUTTON_LEFT)  // Verifico se l'utente ha premuto il tasto sinistro
         {
            SDL_Point mouse = {window->e.button.x, window->e.button.y}; // Ottengo le coordinate del mouse
         // Gestisco i click del mouse
            handle_click(mouse, SDL_GetModState() & SDLK_LCTRL, &window->chessboard);  // SDL_GetModState() & SDLK_LCTRL. Espressione booleana che si verifica quando l'utente preme CTRL
         // Questo evento si dovrebbe verificare solo quando l'utente preme left CTRL, ma per uno strano motivo si verifica anche quando l'utente preme CTRL sia di sinistra che di destra
         // in più si verifica anche quando l'utente preme ALT Gr
         }
         break;

      case SDL_KEYDOWN:    // Evento che si verifica quando l'utente sta premendo un tasto qualsiasi della tastiera.
         // Quindi questo evento si verifica tante volte in termini di millisecondi fino a quando l'utente non rilascia il tasto
         // window->e.key.repeat è uguale al numero di volte che SDL_KEYDOWN si è verificato
         // window->e.key.keysym.scancode tasto che ha premuto l'utente
         if(window->e.key.repeat == 0 && window->e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
         {
            printf("Esc\n");
         // Rimuove le caselle colorate dell'utente
            reset_square_presentation_chessboard(&window->chessboard);
         }
         break;
      }
   }
}

// Funzione per visualizzare la finestra
void render_window(Window *window)
{
// Imposto il colore della finestra
   SDL_SetRenderDrawColor(window->renderer, 239, 239, 239, 255);
// Applico lo sfondo della finestra con il colore selezionato
   SDL_RenderClear(window->renderer);

// Funzione per preparare la stampa della scacchiera
   print_chessboard(window->renderer, &window->chessboard);

// Visualizzo la finestra e la scacchiera
   SDL_RenderPresent(window->renderer);
}


#endif // WINDOWS_H_INCLUDED
