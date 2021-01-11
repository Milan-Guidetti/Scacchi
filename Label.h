#ifndef LABEL_H_INCLUDED
#define LABEL_H_INCLUDED

#include <windows.h>

typedef struct Label
{
   SDL_Texture *texture;
   SDL_Rect rect;
}Label;

void make_label(SDL_Renderer *renderer, const char *filename, const char *text, int size, SDL_Color color, Label *label)
{
   TTF_Font *font = TTF_OpenFont(filename, size);
   if(!font)
      MessageBox(NULL, "Impossibile aprire il font", "Attenzione", MB_ICONERROR);

   TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

   SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
   if(!surface)
      MessageBox(NULL, "Impossibile creare surface", "Attenzione", MB_ICONERROR);

   if(label->texture)
      SDL_DestroyTexture(label->texture);

   label->texture = SDL_CreateTextureFromSurface(renderer, surface);
   if(!label->texture)
      MessageBox(NULL, "Impossibile creare texture", "Attenzione", MB_ICONERROR);

   SDL_QueryTexture(label->texture, NULL, NULL, &label->rect.w, &label->rect.h);

//   printf("w = %d\n", label->rect.w);
//   printf("h = %d\n", label->rect.h);

//   IMG_SavePNG(surface, "Prova.png");
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

void init_label(Label *label)
{
   label->texture = NULL;
}

void set_position_label(Label *label, int x, int y)
{
   label->rect.x = x;
   label->rect.y = y;
}

void close_label(Label *label)
{
   if(label->texture)
      SDL_DestroyTexture(label->texture);
}

#endif // LABEL_H_INCLUDED
