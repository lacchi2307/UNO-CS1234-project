#ifndef UNO_H
#define UNO_H

/*
 * uno.h
 *
 * A card is just an int from 0 to 107.
 *
 * 0-99   colored cards, 25 per color (color = card/25)
 *        within each color, slot = card%25:
 *          slot 0       -> 0
 *          slots 1-2    -> 1,  slots 3-4 -> 2, ... slots 17-18 -> 9
 *          slots 19-20  -> Skip
 *          slots 21-22  -> Reverse
 *          slots 23-24  -> DrawTwo
 *
 * 100-103  Wild
 * 104-107  Wild Draw Four
 *
 * Colors: 0=Red 1=Yellow 2=Green 3=Blue 4=Wild(no color)
 */

#define DECK_SIZE   108
#define NUM_PLAYERS   4
#define MAX_HAND     30

/* card decoders - defined in card.c */
int  card_color(int card);   /* 0-3 for colored, 4 for wilds */
int  card_face(int card);    /* 0-9 for numbers, 10=Skip, 11=Reverse, 12=DrawTwo, 13=Wild, 14=WildDraw4 */
int  is_wild(int card);
void card_label(int card, char *buf);  /* buf[20] is enough */

/* game state - flat */
extern int  deck[DECK_SIZE], deck_top;
extern int  pile[DECK_SIZE], pile_top;
extern int  hand[NUM_PLAYERS][MAX_HAND], hand_size[NUM_PLAYERS];
extern int  is_bot[NUM_PLAYERS];
extern char name[NUM_PLAYERS][24];
extern int  turn, direction, active_color;
extern char last_action[128];

/* game.c */
void setup_game(int num_humans);
int  can_play(int card);
void next_turn(void);
void give_cards(int player, int count);
void remove_card(int player, int index);
int  play_card(int player, int index, int chosen_color);  /* 1 = that player won */
int  bot_turn(void);                                       /* 1 = bot won */

/* display.c */
void init_display(void);
void draw_game(void);
void draw_bot_screen(void);
void draw_winner(int winner);
int  ask_color(void);
int  welcome_screen(void);

#endif
