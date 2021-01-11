#ifndef CHESSBOARD_H_INCLUDED
#define CHESSBOARD_H_INCLUDED

#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#include <math.h>
#include "Label.h"


// Casella della scacchiera
typedef struct Square
{
   SDL_Color *color;  // Colore personalizzato di quando la casella viene selezionata oppure l'utente sta facendo una presentazione
   bool selected;  // Indica se la casella è selezionata o meno
   int piece;     // Indica quale pezzo è presente sulla casella
}Square;

// Variabili indicanti se la Torre o il Re si sono mossi. Informazioni necessari per eseguire l'arrocco
typedef struct Castling
{
   bool king;
   bool left_tower;
   bool right_tower;

// Variabile indicante dove l'arrocco deve essere eseguito.
// Index 0: Arrocco a sinistra
// Index 1: Arrocco a destra
   bool castling[2];
}Castling;

typedef struct Promotion
{
   bool moment;  // Indica se è il momento della promozione
//   int piece;       // Indica il pezzo scelto dall'utente dopo la promozione
   int x;
   int y;
}Promotion;

enum
{
   queen_w,
   tower_w,
   bishop_w,
   horse_w,
   pawn_w,
   king_w,

   queen_b,
   tower_b,
   bishop_b,
   horse_b,
   pawn_b,
   king_b,

   none,
};

// Scacchiera
typedef struct Chessboard
{
   Square square[8][8];       // Caselle della scacchiera
   Castling castling[2];      // Variabile utilizzata per verificare se l'arrocco è possibile. 0 -> Bianco. 1 -> Nero
   Promotion promotion;       // Promozione
   bool en_passant[2][8];     // En Passant.     0 -> Bianco. 1 -> Nero
   bool turn;                 // Turno del giocatore
   SDL_Point *last_move[2];   // Coordinate dell'ultima mossa
   SDL_Point *select[2];      // Coordinate selezionate dall'utente
   bool is_rotate;            // Indica se la scacchiera è ruotata
   SDL_Rect first_square;     // Struttura indicante informazioni quali posizione e dimensione della prima casella in alto a sinistra
   SDL_Texture **pieces;      // Variabile utilizzata per memorizzare i pezzi come immagine
   Label numbers[8];          // Array rappresentati i numeri della scacchiera
   Label letters[8];          // Array rappresentati le lettere della scacchiera
   Label message[3];          // Messaggio
}Chessboard;

// Impedisco al Bianco la Presal al Varco
void reset_en_passant_white(Chessboard *chessboard)
{
   for(int i = 0; i < 8; i++)
      chessboard->en_passant[0][i] = false;
}

// Impedisco al Nero la Presal al Varco
void reset_en_passant_black(Chessboard *chessboard)
{
   for(int i = 0; i < 8; i++)
      chessboard->en_passant[1][i] = false;
}

// Funzione per inizializzare le variabili della struttura Scacchiera
void init_variable_chessboard(SDL_Renderer *renderer, Chessboard *chessboard)
{
   for(int row = 0; row < 8; row++)
      for(int col = 0; col < 8; col++)
      {
         chessboard->square[row][col].color = NULL;      // La casella non ha nessun colore specifico
         chessboard->square[row][col].selected = false;  // La casella non è selezionata
      }

   for(int i = 0; i < 2; i++)
   {
      chessboard->castling[i].king = false;        // Il Re non si è ancora mosso
      chessboard->castling[i].left_tower = false;  // La Torre sinistra non si è ancora mossa
      chessboard->castling[i].right_tower = false; // La Torre destra non si è ancora mossa
      chessboard->castling[i].castling[0] = false; // Nessun arrocco da eseguire al momento. Inebisco l'Arrocco a sinistra
      chessboard->castling[i].castling[1] = false; // Nessun arrocco da eseguire al momento. Inebisco l'Arrocco a destra
   }

   chessboard->promotion.moment = false;  // Non è ancora il momento di promuovere un pezzo

// Impedisco la Presa al Varco appena la partita inizia
   reset_en_passant_white(chessboard);
   reset_en_passant_black(chessboard);

   chessboard->turn = true;   // Indico che è il turno del Bianco

// Appena la partita inizia l'utente non ha selezionato nessuna mossa
   chessboard->select[0] = NULL;
   chessboard->select[1] = NULL;

// Appena la partita inizia l'ultima mossa non esiste
   chessboard->last_move[0] = NULL;
   chessboard->last_move[1] = NULL;

// La scacchiera non è ruotata
   chessboard->is_rotate = false;

// Al momento non ci sono pezzi (immagine) nell'array
   chessboard->pieces = NULL;

// Lettere
   for(int i = 0; i < 8; i++)
   {
      init_label(chessboard->letters + i);
      init_label(chessboard->numbers + i);
   }

// Messaggio
   for(int i = 0; i < 3; i++)
      init_label(chessboard->message + i);
}

// Funzione per posizionare i pezzi sulla scacchiera
void init_position_pieces_chessboard(Chessboard *chessboard)
{
// Posiziono i pezzi del Nero
   chessboard->square[0][0].piece = chessboard->square[0][7].piece = tower_b;
   chessboard->square[0][1].piece = chessboard->square[0][6].piece = horse_b;
   chessboard->square[0][2].piece = chessboard->square[0][5].piece = bishop_b;
   chessboard->square[0][3].piece = queen_b;
   chessboard->square[0][4].piece = king_b;

// Posiziono i pedoni del Nero
   for(int i = 0; i < 8; i++)
      chessboard->square[1][i].piece = pawn_b;

// Centro scacchiera
   for(int row = 0; row < 4; row++)
      for(int col = 0; col < 8; col++)
         chessboard->square[row + 2][col].piece = none;

// Posiziono i pedoni del Bianco
   for(int i = 0; i < 8; i++)
      chessboard->square[6][i].piece = pawn_w;

// Posiziono i pezzi restanti del Bianco
   chessboard->square[7][0].piece = chessboard->square[7][7].piece = tower_w;
   chessboard->square[7][1].piece = chessboard->square[7][6].piece = horse_w;
   chessboard->square[7][2].piece = chessboard->square[7][5].piece = bishop_w;
   chessboard->square[7][3].piece = queen_w;
   chessboard->square[7][4].piece = king_w;
}

// Funzione per ridimensionare la prima casella in alto a sinistra della scacchiera
void resize_first_square_chessboard(SDL_Window *window, Chessboard *chessboard)
{
// Dimensioni della finestra
   int w, h;
   SDL_GetWindowSize(window, &w, &h);

// Se l'utente ha ridimensionato la finestra con dimensioni troppo piccole ridimensiono la finestra con dimensioni più grandi
   if(w < 200)
   {
      w = 200;
      SDL_SetWindowSize(window, w, h);
   }

   if(h < 200)
   {
      h = 200;
      SDL_SetWindowSize(window, w, h);
   }

   int Min = min(w, h);


   int margin_left = Min*0.03;
   int margin_right = 10;  // Non usato ancora
   int margin_top = 10;
   int margin_bottom = Min*0.03;

   int width = (Min - margin_bottom - margin_top)/8;  // Dimensione di una singola casella

   chessboard->first_square.x = margin_left;
   chessboard->first_square.y = margin_top;
   chessboard->first_square.w = chessboard->first_square.h = width;
}

// Funzione per caricare in memoria una determinata immagine con dimensioni in pixel
void load_svg_pixel(SDL_Renderer *renderer, SDL_Texture **texture, const char *filename, int dim);

// Funzione per eliminare il contenuto dell'array immagine
void delete_pieces(SDL_Texture ***texture, int N)
{
   if(*texture != NULL)
   {
      for(int i = 0; i < N; i++)
         SDL_DestroyTexture((*texture)[i]);

      free(*texture);
   }
}

// Funzione per creare l'array contenente i pezzi come immagine
void create_pieces(SDL_Texture ***texture, int N)
{
   if(*texture != NULL)
      delete_pieces(texture, N);

   *texture = (SDL_Texture**)malloc(sizeof(SDL_Texture**)*N);
}

void print(SDL_Texture ***texture, int N)
{
   if(*texture != NULL)
      printf("Null\n");
   else
   {
      for(int i = 0; i < N; i++)
         printf("%p\n", (*texture)[i]);
   }
}

// Funzione per caricare i pezzi come immagine in memoria
void resize_pieces_chessboard(SDL_Renderer *renderer, Chessboard *chessboard)
{
   create_pieces(&chessboard->pieces, 12);

   load_svg_pixel(renderer, &chessboard->pieces[0], "Pezzi/Bianco/Regina_B.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[1], "Pezzi/Bianco/Torre_B.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[2], "Pezzi/Bianco/Alfiere_B.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[3], "Pezzi/Bianco/Cavallo_B.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[4], "Pezzi/Bianco/Pedone_B.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[5], "Pezzi/Bianco/Re_B.svg", chessboard->first_square.h);

   load_svg_pixel(renderer, &chessboard->pieces[6], "Pezzi/Nero/Regina_N.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[7], "Pezzi/Nero/Torre_N.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[8], "Pezzi/Nero/Alfiere_N.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[9], "Pezzi/Nero/Cavallo_N.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[10], "Pezzi/Nero/Pedone_N.svg", chessboard->first_square.h);
   load_svg_pixel(renderer, &chessboard->pieces[11], "Pezzi/Nero/Re_N.svg", chessboard->first_square.h);
}

// Funzione per creare le lettere
void create_letters(SDL_Renderer *renderer, Chessboard *chessboard)
{
   SDL_Color color = {0, 0, 0, 255};
   int x;
   int y = chessboard->first_square.y + chessboard->first_square.h*8;

   char text[2];
   text[0] = 'A';
   text[1] = 0;

   for(int i = 0; i < 8; i++)
   {
      make_label(renderer, "consolas.ttf", text, chessboard->first_square.w/4, color, &chessboard->letters[i]);

      x = chessboard->first_square.x + chessboard->first_square.w*i + chessboard->first_square.w/2 - chessboard->letters[i].rect.w/2;
      set_position_label(&chessboard->letters[i], x, y + chessboard->first_square.h*0.04);
      text[0]++;
   }
}

int get(const char *filename, int size)
{
   TTF_Font *font = TTF_OpenFont(filename, size);

   int minx, maxx, miny, maxy, advance;

   if(TTF_GlyphMetrics(font, 'g', &minx, &maxx, &miny, &maxy, &advance)==-1)
       printf("%s\n", TTF_GetError());
//   else {
//       printf("minx    : %d\n", minx);
//       printf("maxx    : %d\n", maxx);
//       printf("miny    : %d\n", miny);
//       printf("maxy    : %d\n", maxy);
//       printf("advance : %d\n", advance);
//   }

   return miny;
}

// Funzione per creare i numeri
void create_numbers(SDL_Renderer *renderer, Chessboard *chessboard)
{
   SDL_Color color = {0, 0, 0, 255};
   int x;
   int y;
   int Min;

   char text[2];
   text[0] = '1';
   text[1] = 0;

   for(int i = 0; i < 8; i++)
   {
      make_label(renderer, "consolas.ttf", text, chessboard->first_square.w/4, color, &chessboard->numbers[i]);

      Min = get("consolas.ttf", chessboard->first_square.w/4);

      y = chessboard->first_square.y + chessboard->first_square.h*i + Min;
      x = chessboard->first_square.x - chessboard->numbers[i].rect.w - chessboard->first_square.w*0.04;

      set_position_label(&chessboard->numbers[i], x, y + chessboard->first_square.h*0.07);
      text[0]++;
   }
}

// Funzione per creare le frasi
void create_messages_chessboard(SDL_Renderer *renderer, Chessboard *chessboard)
{
// Colore del testo
   SDL_Color color = {0, 0, 0, 255};
   int x = chessboard->first_square.x + chessboard->first_square.w*8.5;
   int y = chessboard->first_square.y;

   make_label(renderer, "consolas.ttf", "Muove il Bianco", chessboard->first_square.w/2.5, color, &chessboard->message[0]);
   set_position_label(&chessboard->message[0], x, y);

   make_label(renderer, "consolas.ttf", "Muove il Nero", chessboard->first_square.w/2.5, color, &chessboard->message[1]);
   set_position_label(&chessboard->message[1], x, y);

   make_label(renderer, "consolas.ttf", "Scacco al Re", chessboard->first_square.w/2.5, color, &chessboard->message[2]);
   set_position_label(&chessboard->message[2], x, y + chessboard->message[0].rect.h + chessboard->message[0].rect.h*0.3);
}

// Funzione per creare le label necessarie
void create_labels_chessboard(SDL_Renderer *renderer, Chessboard *chessboard)
{
// Creo le frasi
   create_messages_chessboard(renderer, chessboard);
// Creo le lettere
   create_letters(renderer, chessboard);
// Creo i numeri
   create_numbers(renderer, chessboard);
}

// Funzione per ridimensionare la scachiera
void resize_chessboard(SDL_Window *window, SDL_Renderer *renderer, Chessboard *chessboard)
{
// Ridimensiono la prima casella in alto a sinistra
   resize_first_square_chessboard(window, chessboard);

// Carico i pezzi come immagine
   resize_pieces_chessboard(renderer, chessboard);

// Creo le label
   create_labels_chessboard(renderer, chessboard);
}

// Funzione per inizializzare la scacchiera
void init_chessboard(SDL_Window *window, SDL_Renderer *renderer, Chessboard *chessboard)
{
// Inizializzo le variabili della struttura Chessboard
   init_variable_chessboard(renderer, chessboard);

// Posiziono i pezzi sulla scacchiera
   init_position_pieces_chessboard(chessboard);

// Ridimensiono la scacchiera
   resize_chessboard(window, renderer, chessboard);
}

// Funzione per creare l'ultima mossa
void make_last_move_chessboard(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
// Alloco spazio per l'ultima mossa se non è stato fatto in precedenza o è stata eliminata l'area di memoria
   if(chessboard->last_move[0] == NULL)
   {
      chessboard->last_move[0] = (SDL_Point*)malloc(sizeof(SDL_Point));
      chessboard->last_move[1] = (SDL_Point*)malloc(sizeof(SDL_Point));
   }

// Prima coordinata
   chessboard->last_move[0]->x = x1;
   chessboard->last_move[0]->y = y1;

// Seconda coordinata
   chessboard->last_move[1]->x = x2;
   chessboard->last_move[1]->y = y2;
}

// Funzione per cancellare l'ultima mossa
void delete_last_move(Chessboard *chessboard)
{
// Elimino l'ultima mossa se c'è ne già un altro
   if(chessboard->last_move[0] != NULL)
   {
      free(chessboard->last_move[0]);
      free(chessboard->last_move[1]);

      chessboard->last_move[0] = NULL;
      chessboard->last_move[1] = NULL;
   }
}

// Funzione per deallocare lo spazio allocato
void close_chessboard(Chessboard *chessboard)
{
// Libero le areee allocate dinamicamente
   delete_last_move(chessboard);
   delete_pieces(&chessboard->pieces, 12);

// Lettere e numeri
   for(int i = 0; i < 8; i++)
   {
      close_label(chessboard->letters + i);
      close_label(chessboard->numbers + i);
   }

// Messaggi
   for(int i = 0; i < 3; i++)
      close_label(chessboard->message + i);
}

// Funzione per ottenere il colore della casella
SDL_Color get_square_color_chessboard(Chessboard *chessboard, int x, int y)
{
   SDL_Color white = {238, 238, 210, 255};   // Bianco
   SDL_Color black = {127, 166, 80, 255};    // Verde
   SDL_Color yellow = {255, 192, 0, 255};    // Giallo

   SDL_Color color = white;   // Bianco
   if(x%2 != y%2)
      color = black;  // Verde

// Evidenzio l'ultima mossa
   if(chessboard->last_move[0])
      if(x == chessboard->last_move[0]->x && y == chessboard->last_move[0]->y)
         color = yellow;

   if(chessboard->square[y][x].color != NULL)
      color = *chessboard->square[y][x].color;

   return color;
}

// Funzione per disegnare un cerchio
void draw_circle_chessboard(SDL_Renderer *renderer, Chessboard *chessboard, SDL_Rect center);

// Funzione per stampare i pezzi che si possono promuovere
void print_promotion_chessboard(SDL_Renderer *renderer, Chessboard *chessboard)
{
   SDL_Rect rect = chessboard->first_square;
   rect.x += rect.w*9;
   rect.y += rect.h*2;

   SDL_Color white = {238, 238, 210, 255};   // Bianco
   SDL_Color black = {127, 166, 80, 255};    // Verde

   SDL_Color color;

   int counter = 0;     // Bianco
   if(chessboard->promotion.y == 7)
      counter = 6;      // Nero

   for(int i = 0; i < 4; i++)
   {
   // Colore di sfondo
      if(i%2 == 0)
         color = white;
      else
         color = black;

      SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
      SDL_RenderFillRect(renderer, &rect);

   // Stampo il pezzo
      SDL_RenderCopy(renderer, chessboard->pieces[i + counter], NULL, &rect);

   // Contorno
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

   // Riga in alto
      SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
   // Fianco sinistro
      SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
   // Fianco destro
      SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);

      rect.y += rect.h;
   }

// Riga in basso
   SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
}

// Funzione per determinare se nelle coordinate date c'è un pedone
bool is_pawn_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece == pawn_w || chessboard->square[y][x].piece == pawn_b)
      return true;

   return false;
}

// Funzione per scegliere il pezzo dalla Promozione
void choose_piece_promotion_chessboard(Chessboard *chessboard, int mouse_x, int mouse_y)
{
// Verifico se è il momento della Promozione
   if(!chessboard->promotion.moment)
      return;

// Verifico se in fondo scacchiera c'è un pedone
   if(!is_pawn_chessboard(chessboard, chessboard->promotion.x, chessboard->promotion.y))
      return;

   SDL_Rect rect = chessboard->first_square;
   rect.x += rect.w*9;
   rect.y += rect.h*2;

   int counter = 0;  // Pezzo Bianco
   if(chessboard->promotion.y == 7)
      counter = 6;   // Pezzo Nero

   for(int i = 0; i < 4; i++)
   {
      if(mouse_x >= rect.x && mouse_x <= rect.x + rect.w)
         if(mouse_y >= rect.y && mouse_y <= rect.y + rect.h)
         {
         // Imposto il pezzo scelto dall'utente
            chessboard->square[chessboard->promotion.y][chessboard->promotion.x].piece = i + counter;

            chessboard->promotion.moment = false;
            return;
         }

      rect.y += rect.h;
   }
}

// Funzione per ottenere le coordinate della scacchiera date le coordinate del mouse
SDL_Point get_coordinate_chessboard(Chessboard *chessboard, SDL_Point mouse)
{
   SDL_Rect rect = chessboard->first_square;
   SDL_Point point = {-1, -1};

   for(int row = 0; row < 8; row++)
   {
      for(int col = 0; col < 8; col++)
      {
         if(mouse.x >= rect.x && mouse.x <= rect.x + rect.w)
            if(mouse.y >= rect.y && mouse.y <= rect.y + rect.h)
            {
               point.x = col;
               point.y = row;
               return point;
            }

         rect.x += rect.w;
      }

      rect.y += rect.h;
      rect.x = chessboard->first_square.x;
   }

   return point;
}

// Funzione per verificare se le coordinate siano corrette
bool correct_coordinates_chessboard(int x, int y)
{
   if(x >= 0 && x < 8)
      if(y >= 0 && y < 8)
         return true;

// Le coordinate sono sbagliate
   return false;
}

// Funzione per verificare se nelle coordinate date c'è un pezzo del Bianco
bool is_white_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece >= queen_w && chessboard->square[y][x].piece <= king_w)
      return true;

   return false;
}

// Funzione per verificare se nelle coordinate date c'è un pezzo del Nero
bool is_black_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece >= queen_b && chessboard->square[y][x].piece <= king_b)
      return true;

   return false;
}

// Condizioni per muovere un pezzo sulla scacchiera
bool condition_moving_chessboard(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
// Controllo che la prima casella non sia una casella vuota
   if(chessboard->square[y1][x1].piece == none)
      return false;

// Controllo se nelle coordinate date ci siano dei pezzi del Bianco
   if(is_white_chessboard(chessboard, x1, y1))
      if(is_white_chessboard(chessboard, x2, y2))
         return false;

// Controllo se nelle coordinate date ci siano dei pezzi del Nero
   if(is_black_chessboard(chessboard, x1, y1))
      if(is_black_chessboard(chessboard, x2, y2))
         return false;

   return true;
}

// Funzione booleana per determinare se è il turno del giocatore che vuole giocare
bool is_my_turn_chessboard(Chessboard *chessboard, int x, int y)
{
// Controllo che sia il turno del giocatore che vuole giocare
   if(is_white_chessboard(chessboard, x, y) && chessboard->turn)
      return true;

   if(is_black_chessboard(chessboard, x, y) && !chessboard->turn)
      return true;

   return false;
}

// Funzione utilizzata per muovere la Torre l'Alfiere e la Regina
void set_possible_moves_tower_bishop_queen_chessboard(Chessboard *chessboard, int x, int y, int increment_x, int increment_y)
{
   for(int row = y + increment_y, col = x + increment_x; row >= 0 && row < 8 && col >= 0 && col < 8; row += increment_y, col += increment_x)
      if(chessboard->square[row][col].piece == none)
         chessboard->square[row][col].selected = true;
      else
      {
         if(condition_moving_chessboard(chessboard, x, y, col, row))
            chessboard->square[row][col].selected = true;

         break;
      }
}

// Funzione per impostare tutte le possibili mosse della Torre
void set_possible_moves_tower_chessboard(Chessboard *chessboard, int x, int y)
{
// Alto
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, 0, -1);
// Basso
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, 0, 1);
// Destra
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, 1, 0);
// Sinistra
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, -1, 0);

}

// Funzione per impostare tutte le possibili mosse dell'Alfiere
void set_possible_moves_bishop_chessboard(Chessboard *chessboard, int x, int y)
{
// Alto a destra
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, 1, -1);
// Basso a destra
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, 1, 1);
// Alto a sinistra
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, -1, -1);
// Basso a destra
   set_possible_moves_tower_bishop_queen_chessboard(chessboard, x, y, -1, 1);
}

// Funzione per impostare tutte le possibili mosse della Regina
void set_possible_moves_queen_chessboard(Chessboard *chessboard, int x, int y)
{
   set_possible_moves_tower_chessboard(chessboard, x, y);
   set_possible_moves_bishop_chessboard(chessboard, x, y);
}

// Funzione per impostare tutte le possibili mosse del Cavallo
void set_possible_moves_horse_chessboard(Chessboard *chessboard, int x, int y)
{
   int var_y = -2;
   int var_x = 1;
   int temp;

   for(int k = 0; k < 2; k++)
   {
      for(int j = 0; j < 2; j++)
      {
         for(int i = 0; i < 2; i++)
         {
            if(correct_coordinates_chessboard(x + var_x, y + var_y))
               if(condition_moving_chessboard(chessboard, x, y, x + var_x, y + var_y))
               {
//                  printf("x = %d  y = %d\n", x + var_x, y + var_y);
                  chessboard->square[y + var_y][x + var_x].selected = true;
               }

            var_x = -var_x;
         }

         var_y = -var_y;
      }

      temp = var_x;
      var_x = var_y;
      var_y = temp;
   }

//   printf("---------------------------------------------\n");
}

bool is_square_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece == none)
      return true;

   return false;
}

// Funzione per settare a true se le Torri non sono nelle case di partenza
void mark_moved_tower_chessboard(Chessboard *chessboard)
{
// Controllo la Torre a sinistra del Nero
   if(chessboard->square[0][0].piece != tower_b)
      chessboard->castling[1].left_tower = true;  // Faccio in modo che non sia più possibile l'Arrocco

// Controllo la Torre a destra del Nero
   if(chessboard->square[0][7].piece != tower_b)
      chessboard->castling[1].right_tower = true;  // Faccio in modo che non sia più possibile l'Arrocco


// Controllo la Torre a sinistra del Bianco
   if(chessboard->square[7][0].piece != tower_w)
      chessboard->castling[0].left_tower = true;  // Faccio in modo che non sia più possibile l'Arrocco

// Controllo la Torre a destra del Bianco
   if(chessboard->square[7][7].piece != tower_w)
      chessboard->castling[0].right_tower = true;  // Faccio in modo che non sia più possibile l'Arrocco
}

// Funzione per impostare tutte le possibili mosse del Re
void set_possible_moves_king_chessboard(Chessboard *chessboard, int x, int y)
{
// Verifico tutti i movimenti del Re
   int new_x, new_y;

   for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
      {
         new_x = x + j - 1;
         new_y = y + i - 1;

         if(correct_coordinates_chessboard(new_x, new_y))
            if(condition_moving_chessboard(chessboard, x, y, new_x, new_y))
               chessboard->square[new_y][new_x].selected = true;
      }

// Verifico che agli estremi ci siano le Torri, altrimenti faccio in modo che l'Arrocco non sia possibile
   mark_moved_tower_chessboard(chessboard);

// Verifico se sia possibile l'Arrocco
   int index = 0;
   if(is_black_chessboard(chessboard, x, y))
      index = 1;

// Per prima cosa verifico che il Re non si sia mosso
   if(!chessboard->castling[index].king)
   {
   // Poi verifico che una delle due Torri non si sia mosso

   // Verifico che la Torre sinistra non si sia ancora mossa
      if(!chessboard->castling[index].left_tower)
      // Controllo che le caselle tra il Re e la Torre siano vuote
         if(is_square_chessboard(chessboard, x - 1, y) && is_square_chessboard(chessboard, x - 2, y) && is_square_chessboard(chessboard, x - 3, y))
         {
            chessboard->square[y][x - 2].selected = true;
            chessboard->castling[index].castling[0] = true;
         }

   // Verifico che la Torre destra non si sia ancora mossa
      if(!chessboard->castling[index].right_tower)
      // Controllo che le caselle tra il Re e la Torre siano vuote
         if(is_square_chessboard(chessboard, x + 1, y) && is_square_chessboard(chessboard, x + 2, y))
         {
            chessboard->square[y][x + 2].selected = true;
            chessboard->castling[index].castling[1] = true;
         }
   }
}

// Funzione per impostare tutte le possibili mosse del pedone
void set_possible_moves_pawn_chessboard(Chessboard *chessboard, int x, int y)
{
   int uno = -1, index = 1;
   if(is_black_chessboard(chessboard, x, y))
   {
      uno = 1;
      index = 0;
   }

// Se il pedone non ha nessun pezzo di fronte (in alto) allora può avanzare
   if(correct_coordinates_chessboard(x, y + uno))
      if(chessboard->square[y + uno][x].piece == none)
      {
         chessboard->square[y + uno][x].selected = true;

      // Verifico se il pedone si trova nella casella di partenza e quindi può muovere di 2 caselle alla sua prima mossa
         if(y == 1 || y == 6)
            if(chessboard->square[y + uno*2][x].piece == none)
               chessboard->square[y + uno*2][x].selected = true;
      }

// Verifico se il pedone possa mangiare nelle diagonali
   int var_y = y + uno;
   for(int i = 0; i < 2; i++)
   {
   // Controllo che le coordinate siano giuste
      if(correct_coordinates_chessboard(x + uno, var_y))
      // Controllo che nelle coordinate di arrivo ci sia un pezzo
         if(chessboard->square[var_y][x + uno].piece != none)
         // Controllo che il pezzo sia dell'avversario
            if(condition_moving_chessboard(chessboard, x, y, x + uno, var_y))
               chessboard->square[var_y][x + uno].selected = true;

      uno = -uno;
   }

// Controllo se il pedone possa cattura con la mossa Presa al Varco
   if(y < 7 && y > 0)   // Nelle ultime traverse non è necessario fare il controllo
   {
   // Controllo a sinistra
      if(correct_coordinates_chessboard(x - 1, y))    // Verifico che la coordinata sia corretta
         if(chessboard->en_passant[index][x - 1])     // Verifico che il giocatore del turno precedente abbia spostato un pedone in avanti di due caselle
            if(is_pawn_chessboard(chessboard, x - 1, y))    // Verifico che nelle casella a lato al mio pedone ci sia un altro pedone
               if(condition_moving_chessboard(chessboard, x, y, x - 1, y)) // Verifico che nella casella al mio pedone ci sia un pezzo dell'avversario
                  chessboard->square[y + uno][x - 1].selected = true;


   // Controllo a destra
      if(correct_coordinates_chessboard(x + 1, y))
         if(chessboard->en_passant[index][x + 1])
            if(is_pawn_chessboard(chessboard, x + 1, y))
               if(condition_moving_chessboard(chessboard, x, y, x + 1, y))
                  chessboard->square[y + uno][x + 1].selected = true;
   }
}

// Funzione per impostare tutte le possibili mosse di un pezzo qualunque
void set_possible_moves_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece == none)
      return;

   switch(chessboard->square[y][x].piece)
   {
   case queen_w:
   case queen_b:
      set_possible_moves_queen_chessboard(chessboard, x, y);
      break;

   case tower_w:
   case tower_b:
      set_possible_moves_tower_chessboard(chessboard, x, y);
      break;

   case bishop_w:
   case bishop_b:
      set_possible_moves_bishop_chessboard(chessboard, x, y);
      break;

   case horse_w:
   case horse_b:
      set_possible_moves_horse_chessboard(chessboard, x, y);
      break;

   case pawn_w:
   case pawn_b:
      set_possible_moves_pawn_chessboard(chessboard, x, y);
      break;

   case king_w:
   case king_b:
      set_possible_moves_king_chessboard(chessboard, x, y);
      break;
   }
}

// Funzione per verificare se nelle coordinate date c'è il Re
bool is_king_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece == king_w || chessboard->square[y][x].piece == king_b)
      return true;

   return false;
}

// Funzione per verificare se nelle coordinate date c'è la Torre
bool is_tower_chessboard(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].piece == tower_w || chessboard->square[y][x].piece == tower_b)
      return true;

   return false;
}

// Funzione per segnare i pezzi che si sono mossi
void mark_moved_piece(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
   if(chessboard->square[y1][x1].piece == none)
      return;

   int index = 0;    // Bianco
   if(is_black_chessboard(chessboard, x1, y1))
      index = 1;     // Nero

// Arrocco
// Verifico se nelle coordinate ci sia il Re
   if(is_king_chessboard(chessboard, x1, y1))
      chessboard->castling[index].king = true;

// Verifico se nelle coordinate ci sia la Torre
   if(is_tower_chessboard(chessboard, x1, y1))
   {
   // Verifico se si è mosso la Torre sinistra
      if(x1 == 0)
         chessboard->castling[index].left_tower = true;
      else
         if(x1 == 7)  // Verifico se si è mosso la Torre destra
            chessboard->castling[index].right_tower = true;
   }


// Presa al Varco
   if(is_pawn_chessboard(chessboard, x1, y1))
      if(abs(y1 - y2) == 2)   // Controllo che il pedone dell'avversario si sia mosso di due caselle alla mossa precedente
      {
      // Perchè la Presa al Varco possa avvenire, quando l'avversario muove di due caselle il suo pedone, ci deve essere almeno un pedone (mio) adiacente alla casella d'arrivo del pedone dell'avversario

      // Controllo se c'è un pedone mio a sinistra
         if(correct_coordinates_chessboard(x2 - 1, y2))
         // La mossa non è stata ancora fatta per cui devo confrontare la casa di partenza
            if(condition_moving_chessboard(chessboard, x1, y1, x2 - 1, y2))   // Controllo che ci sia un pezzo dell'avversario
               if(is_pawn_chessboard(chessboard, x2 - 1, y2))  // Controllo che ci sia il pedone dell'avversario
                  chessboard->en_passant[index][x2] = true;


      // Controllo se c'è un pedone mio a destra
         if(correct_coordinates_chessboard(x2 + 1, y2))
         // La mossa non è stata ancora fatta per cui devo confrontare la casa di partenza
            if(condition_moving_chessboard(chessboard, x1, y1, x2 + 1, y2))   // Controllo che ci sia un pezzo dell'avversario
               if(is_pawn_chessboard(chessboard, x2 + 1, y2))  // Controllo che ci sia il pedone dell'avversario
                  chessboard->en_passant[index][x2] = true;
      }
}

// Funzione per inebire l'Arrocco
void reset_castling_chessboard(Chessboard *chessboard)
{
   for(int i = 0; i < 2; i++)
   {
   // Inebisco l'Arrocco a sinistra
      chessboard->castling[i].castling[0] = false;
   // Inebisco l'Arrocco a destra
      chessboard->castling[i].castling[1] = false;
   }
}

// Funzione per arroccare
void move_castling_chessboard(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
   if(!is_king_chessboard(chessboard, x1, y1))
      return;

   int index = 0;
   if(is_black_chessboard(chessboard, x1, y1))
      index = 1;

// Controllo se è possibile arroccare a sinistra
   if(chessboard->castling[index].castling[0])
   {
   // Verifico che effettivamente l'utente voglia Arroccare
      if(x1 - x2 == 2 && y1 - y2 == 0)
      {
      // Sposto semplicemente la Torre
         chessboard->square[y1][3].piece = chessboard->square[y1][0].piece;
         chessboard->square[y1][0].piece = none;
      }
   }

// Controllo se è possibile Arroccare a destra
   if(chessboard->castling[index].castling[1])
   {
   // Verifico che effettivamente l'utente voglia Arroccare
      if(x2 - x1 == 2 && y1 - y2 == 0)
      {
      // Sposto semplicemente la Torre
         chessboard->square[y1][5].piece = chessboard->square[y1][7].piece;
         chessboard->square[y1][7].piece = none;
      }
   }

}

// Funzione per determinare se una casella è suggerita come mossa
bool is_suggested(Chessboard *chessboard, int x, int y)
{
   if(chessboard->square[y][x].selected == true)
      return true;

   return false;
}

// Funzione per impedire la presa al Varco
void reset_en_passant_chessboard(Chessboard *chessboard)
{
   if(chessboard->turn)
      reset_en_passant_black(chessboard);
   else
      reset_en_passant_white(chessboard);
}

// Funzione per Cattura con la mossa Presa al Varco
void move_en_passant(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
// La coordinata deve contenere un pedone
   if(!is_pawn_chessboard(chessboard, x1, y1))
      return;

   int index = 1;
   if(is_black_chessboard(chessboard, x1, y1))
      index = 0;

// Controllo che il pedone dell'avversario si sia mosso di due caselle alla mossa precedente
   if(!chessboard->en_passant[index][x2])
      return;

   chessboard->square[y1][x2].piece = none;
}

// Funzione per verificare se è il momento della promozione
void set_moment_promotion_chessboard(Chessboard *chessboard)
{
// Controllo se il Bianco deve promuovere
   for(int i = 0; i < 8; i++)
      if(chessboard->square[0][i].piece == pawn_w)
      {
         chessboard->promotion.moment = true;
         chessboard->promotion.x = i;
         chessboard->promotion.y = 0;
      }

// Controllo se il Nero deve promuovere
   for(int i = 0; i < 8; i++)
      if(chessboard->square[7][i].piece == pawn_b)
      {
         chessboard->promotion.moment = true;
         chessboard->promotion.x = i;
         chessboard->promotion.y = 7;
      }
}

// Funzione per cercare un pezzo
bool find_piece_chessboard(Chessboard *chessboard, int piece, int *x, int *y)
{
   for(int row = 0; row < 8; row++)
      for(int col = 0; col < 8; col++)
         if(chessboard->square[row][col].piece == piece)
         {
            *x = col;
            *y = row;
            return true;
         }

   return false;
}

// Funzione per cancellare tutte le possibili mosse suggerite
void reset_possible_moves_chessboard(Chessboard *chessboard)
{
   for(int row = 0; row < 8; row++)
      for(int col = 0; col < 8; col++)
         chessboard->square[row][col].selected = false;
}

// Funzione per verificare se il Re è sotto scacco
bool is_chess_king(Chessboard *chessboard, int king, bool (*f)(Chessboard *, int , int))
{
// Coordinate del Re
   int x, y;
   find_piece_chessboard(chessboard, king, &x, &y);

// Faccio una copia della scacchiera
   Chessboard temp = *chessboard;

// Deseleziono tutte le possibili mosse
   reset_possible_moves_chessboard(&temp);

// Imposto tutte le possibili mosse
   for(int row = 0; row < 8; row++)
      for(int col = 0; col < 8; col++)
         if(f(&temp, col, row))
            set_possible_moves_chessboard(&temp, col, row);

   if(temp.square[y][x].selected)
      return true;

   return false;
}

// Funzione per verificare se il giocatore che ha giocato ha il Re sotto scacco
bool is_chess(Chessboard *chessboard)
{
// Se il Bianco ha giocato verifico che il suo Re non sia sotto scacco
   if(chessboard->turn)
      return is_chess_king(chessboard, king_w, is_black_chessboard);

   return is_chess_king(chessboard, king_b, is_white_chessboard);
}

// Funzione per stampare le etichette
void print_label(SDL_Renderer *renderer, Chessboard *chessboard)
{
   if(chessboard->turn)
      SDL_RenderCopy(renderer, chessboard->message[0].texture, NULL, &chessboard->message[0].rect);
   else
      SDL_RenderCopy(renderer, chessboard->message[1].texture, NULL, &chessboard->message[1].rect);

// Se il Re è sotto scacco lo notifico
   if(is_chess(chessboard))
      SDL_RenderCopy(renderer, chessboard->message[2].texture, NULL, &chessboard->message[2].rect);

// Stampo le lettere e i numeri
   for(int i = 0; i < 8; i++)
   {
      SDL_RenderCopy(renderer, chessboard->letters[i].texture, NULL, &chessboard->letters[i].rect);
      SDL_RenderCopy(renderer, chessboard->numbers[i].texture, NULL, &chessboard->numbers[i].rect);
   }
}

// Funzione per stampare la scacchiera
void print_chessboard(SDL_Renderer *renderer, Chessboard *chessboard)
{
   SDL_Rect rect = chessboard->first_square;
   SDL_Color color;

   for(int row = 0; row < 8; row++)
   {
      for(int col = 0; col < 8; col++)
      {
      // Ottengo il colore della scacchiera
         color = get_square_color_chessboard(chessboard, col, row);
      // Imposto il colore della casella
         SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
      // Riempo l'area con il colore della casella
         SDL_RenderFillRect(renderer, &rect);

      // Visualizzo il pezzo sulla scacchiera
         if(chessboard->square[row][col].piece != none)
            SDL_RenderCopy(renderer, chessboard->pieces[chessboard->square[row][col].piece], NULL, &rect);

      // Evidenzio eventuali mosse suggerite
         if(chessboard->square[row][col].selected)
            draw_circle_chessboard(renderer, chessboard, rect);

      // Se è il momento della Promozione stampo la lista dei pezzi che possono essere promossi
         if(chessboard->promotion.moment)
            print_promotion_chessboard(renderer, chessboard);

      // Stampo le etichette
         print_label(renderer, chessboard);

         rect.x += rect.w;
      }

      rect.y += rect.h;
      rect.x = chessboard->first_square.x;
   }
}

// Funzione per verificare se una mossa è valida
bool is_valid_move_chessboard(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
// Verifico se le condizioni per muovere sono soddisfatte
   if(condition_moving_chessboard(chessboard, x1, y1, x2, y2) && is_my_turn_chessboard(chessboard, x1, y1) && is_suggested(chessboard, x2, y2))
   {
   // Imposto tutte le possibili mosse di un pezzo
      set_possible_moves_chessboard(chessboard, x1, y1);
   // Tengo in memoria i pezzi mossi (quelli necessari per seguire le regole degli scacchi)
      mark_moved_piece(chessboard, x1, y1, x2, y2);

   // Se il Re è sotto scacco non posso Arroccare
      if(is_chess(chessboard))
         if(abs(x1 - x2) > 1)
            return false;


      move_castling_chessboard(chessboard, x1, y1, x2, y2);
      move_en_passant(chessboard, x1, y1, x2, y2);

   // Muovo il pezzo
      chessboard->square[y2][x2].piece = chessboard->square[y1][x1].piece;
      chessboard->square[y1][x1].piece = none;

   // Impedisco la Presa al Varco
      reset_en_passant_chessboard(chessboard);

   // Controllo se è il momento della promozione
      set_moment_promotion_chessboard(chessboard);

   // Controllo se il Re è sotto scacco
      if(is_chess(chessboard))
         return false;

   // Cambio il turno del giocatore
      chessboard->turn = !chessboard->turn;
      return true;
   }

   return false;
}

// Funzione per cancellare tutte le caselle selezionate
void reset_square_presentation_chessboard(Chessboard *chessboard)
{
   for(int row = 0; row < 8; row++)
      for(int col = 0; col < 8; col++)
      {
         free(chessboard->square[row][col].color);
         chessboard->square[row][col].color = NULL;
      }
}

// Funzione per cancellare la mossa corrente
void reset_last_move_chessboard(Chessboard *chessboard)
{
   if(chessboard->select[0] == NULL || chessboard->select[1] == NULL)
   {
      MessageBox(NULL, "Impossibile cancellare la mossa corrente", "Attenzione", MB_ICONERROR);
      exit(-1);
   }

   free(chessboard->select[0]);
   free(chessboard->select[1]);

   chessboard->select[0] = NULL;
   chessboard->select[1] = NULL;
}

// Funzione per muovere un pezzo
void move_chessboard(Chessboard *chessboard, int x1, int y1, int x2, int y2)
{
// Faccio una copia temporanea della scacchiera
   Chessboard temp = *chessboard;

// Deseleziono tutte le possibili mosse sulla scacchiera suggerite precedentemente
   reset_possible_moves_chessboard(chessboard);

// Verifico che la mossa sia valida
   if(is_valid_move_chessboard(&temp, x1, y1, x2, y2))
   {
      *chessboard = temp;
   // Creo l'ultima mossa
      make_last_move_chessboard(chessboard, x1, y1, x2, y2);
   }


// Deseleziono tutte le caselle
   reset_square_presentation_chessboard(chessboard);
// Deseleziono tutte le possibili mosse sulla scacchiera
   reset_possible_moves_chessboard(chessboard);
// Deseleziono la mossa corrente
   reset_last_move_chessboard(chessboard);
// Inebisco l'Arrocco
   reset_castling_chessboard(chessboard);
}

// Funzione per gestire i click dell'utente
void handle_click(SDL_Point mouse, bool pressed_CTRL, Chessboard *chessboard)
{
// Ottengo le coordinate della scacchiera
   SDL_Point point = get_coordinate_chessboard(chessboard, mouse);

// Controllo che le coordinate siano corrette
   if(correct_coordinates_chessboard(point.x, point.y))
   {
      printf("x = %d   y = %d\n", point.x, point.y);

      SDL_Color red = {255, 0, 0, 255};    // Rosso
      SDL_Color yellow = {255, 255, 0, 255}; // Giallo

   // Se l'utente sta premendo CTRL significa che sta facendo una presentazione
      if(pressed_CTRL)
      {
         chessboard->square[point.y][point.x].color = (SDL_Color *)malloc(sizeof(SDL_Color));
         *chessboard->square[point.y][point.x].color = red;
      }
      else if(!chessboard->promotion.moment)
      {
      // Imposto le coordinate del pezzo da dove si vuole muovere
         if(chessboard->select[0] == NULL)
         {
            chessboard->select[0] = (SDL_Point*)malloc(sizeof(SDL_Point));
            *chessboard->select[0] = point;

         // Evidenzio la casella di giallo
            chessboard->square[point.y][point.x].color = (SDL_Color*)malloc(sizeof(SDL_Color));
            *chessboard->square[point.y][point.x].color = yellow;

         // Imposto tutte le possibili mosse di un pezzo in modo da visualizzarle sulla scacchiera
            set_possible_moves_chessboard(chessboard, point.x, point.y);
         }
         else  // Imposto le coordinate del pezzo di dove si vuole muovere
         {
            chessboard->select[1] = (SDL_Point*)malloc(sizeof(SDL_Point));
            *chessboard->select[1] = point;

         // Muovo il pezzo sulla scacchiera
            move_chessboard(chessboard, chessboard->select[0]->x, chessboard->select[0]->y, chessboard->select[1]->x, chessboard->select[1]->y);
         }
      }
   }
   else
      choose_piece_promotion_chessboard(chessboard, mouse.x, mouse.y);
}



























// Funzione per caricare in memoria una determinata immagine
void load_svg_scale(SDL_Renderer *renderer, SDL_Texture **texture, const char *filename, float scale)
{
   NSVGimage *svg_image = nsvgParseFromFile(filename, "px", 96.0);
   NSVGrasterizer *rasterizer = nsvgCreateRasterizer();

// Dimensioni dell'immagine scalata
   int width = svg_image->width*scale;

// Dimensione dell'immagine scalata
   unsigned long long int s = width*4*width;
// Pixels dell'immagine scalata
   unsigned char *char_image = (unsigned char*) malloc(s*sizeof(unsigned char));
// Imposto i pixels dell'immagine scalata
   nsvgRasterize(rasterizer, svg_image, 0, 0, scale, char_image, width, width, width*4);

   *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, width);
   if(*texture == NULL)
   {
      MessageBox(NULL, "Impossibile creare texture!", "Attenzione", MB_ICONERROR);
      exit(-1);
   }

   SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
   SDL_UpdateTexture(*texture, NULL, char_image, 4*width);

   free(char_image);
   nsvgDelete(svg_image);
   nsvgDeleteRasterizer(rasterizer);
}

// Funzione per caricare in memoria una determinata immagine con dimensioni in pixel
void load_svg_pixel(SDL_Renderer *renderer, SDL_Texture **texture, const char *filename, int dim)
{
   int aumenta_px = dim - 64;
   float aumenta_percentuale = 1 + aumenta_px/64.0;

   load_svg_scale(renderer, texture, filename, aumenta_percentuale);
}

// Funzione per disegnare un cerchio
void draw_circle_chessboard(SDL_Renderer *renderer, Chessboard *chessboard, SDL_Rect center)
{
   SDL_Color color = {0, 176, 246, 240};
   int radius = chessboard->first_square.h/5;

   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
   SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
   int dx, dy;

   center.x += chessboard->first_square.w/2;
   center.y += chessboard->first_square.h/2;

   for(int w = 0; w < radius * 2; w++)
      for(int h = 0; h < radius * 2; h++)
      {
         dx = radius - w; // horizontal offset
         dy = radius - h; // vertical offset

         if((dx*dx + dy*dy) <= (radius * radius))
            SDL_RenderDrawPoint(renderer, center.x + dx, center.y + dy);
      }

   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

#endif // CHESSBOARD_H_INCLUDED
