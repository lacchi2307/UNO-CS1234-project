#include <stdio.h>
#include "uno.h"


int card_color(int card) {
    if (card >= 100) return 4;   /* wild, no color */
    return card / 25;            /* 0=Red 1=Yellow 2=Green 3=Blue */
}

/*
 * Returns a normalised face value 0-14:
 *   0-9   number cards
 *   10    Skip
 *   11    Reverse
 *   12    DrawTwo
 *   13    Wild
 *   14    Wild Draw Four
 */
int card_face(int card) {
    if (card >= 104) return 14;
    if (card >= 100) return 13;

    int slot = card % 25;
    if (slot == 0)  return 0;
    if (slot <= 18) return (slot + 1) / 2;   /* pairs: slots 1-2 -> 1, 3-4 -> 2, ... */
    if (slot <= 20) return 10;               /* Skip */
    if (slot <= 22) return 11;               /* Reverse */
    return 12;                               /* DrawTwo */
}

int is_wild(int card) {
    return card >= 100;
}

void card_label(int card, char *buf) {
    /* color names and face names as plain arrays - no switch needed */
    static const char *colors[] = { "Red", "Yellow", "Green", "Blue" };
    static const char *faces[]  = { "0","1","2","3","4","5","6","7","8","9",
                                    "Skip","Rev","+2","Wild","W+4" };
    int f = card_face(card);
    int c = card_color(card);

    if (c == 4)                       /* wild card, no color prefix */
        sprintf(buf, "%s", faces[f]);
    else
        sprintf(buf, "%s %s", colors[c], faces[f]);
}
