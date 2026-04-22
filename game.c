#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uno.h"

// declering required variable
int  deck[DECK_SIZE], deck_top;
int  pile[DECK_SIZE], pile_top;
int  hand[NUM_PLAYERS][MAX_HAND], hand_size[NUM_PLAYERS];
int  is_bot[NUM_PLAYERS];
char name[NUM_PLAYERS][24];
int  turn, direction, active_color;
char last_action[128];

// shuffling of deck

static void shuffle(int *arr, int n) {
    int i, j, tmp;
    for (i = n - 1; i > 0; i--) {
        j      = rand() % (i + 1);
        tmp    = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}


static int draw_one(void) {
    int i, saved_top;
    if (deck_top == 0) {
        saved_top = pile[pile_top - 1];      
        deck_top  = pile_top - 1;
        for (i = 0; i < deck_top; i++)
            deck[i] = pile[i];
        pile[0]   = saved_top;
        pile_top  = 1;
        shuffle(deck, deck_top);
    }
    return deck[--deck_top];
}


void setup_game(int num_humans) {
    int i, j;

    //  building of deck

    for (i = 0; i < DECK_SIZE; i++) deck[i] = i;
    deck_top = DECK_SIZE;
    shuffle(deck, deck_top);

    direction = 1;
    turn      = 0;

    for (i = 0; i < NUM_PLAYERS; i++) {
        hand_size[i] = 0;
        if (i < num_humans) {
            sprintf(name[i], "Player %d", i + 1);
            is_bot[i] = 0;
        } else {
            sprintf(name[i], "Bot %d", i - num_humans + 1);
            is_bot[i] = 1;
        }
    }

    for (i = 0; i < NUM_PLAYERS; i++)
        for (j = 0; j < 7; j++)
            hand[i][hand_size[i]++] = draw_one();

    
    do { pile[0] = draw_one(); } while (is_wild(pile[0]));
    pile_top     = 1;
    active_color = card_color(pile[0]);

    strcpy(last_action, "Game started. Good luck!");
}

// validity and rules

int can_play(int card) {
    if (is_wild(card))                        return 1;
    if (card_color(card) == active_color)     return 1;
    if (card_face(card) == card_face(pile[pile_top - 1])) return 1;
    return 0;
}

void next_turn(void) {
    turn = (turn + direction + NUM_PLAYERS) % NUM_PLAYERS;
}

void give_cards(int player, int count) {
    int i;
    for (i = 0; i < count && hand_size[player] < MAX_HAND; i++)
        hand[player][hand_size[player]++] = draw_one();
}

void remove_card(int player, int index) {
    int i;
    for (i = index; i < hand_size[player] - 1; i++)
        hand[player][i] = hand[player][i + 1];
    hand_size[player]--;
}

int play_card(int player, int index, int chosen_color) {
    int card = hand[player][index];
    int next;
    char desc[24];
    static const char *cnames[] = {"Red","Yellow","Green","Blue"};

    card_label(card, desc);
    remove_card(player, index);
    pile[pile_top++] = card;

    if (is_wild(card)) {
        active_color = chosen_color;
        sprintf(last_action, "%s played %s, chose %s",
                name[player], desc, cnames[chosen_color]);
    } else {
        active_color = card_color(card);
        sprintf(last_action, "%s played %s", name[player], desc);
    }

    if (hand_size[player] == 0) return 1;   //winner

    int face = card_face(card);

    if (face == 10) {           // skip
        next_turn(); next_turn();
    } else if (face == 11) {    // reverse
        direction = -direction;
        next_turn();
    } else if (face == 12) {    // drawing2 caerds
        next = (turn + direction + NUM_PLAYERS) % NUM_PLAYERS;
        give_cards(next, 2);
        next_turn(); next_turn();
    } else if (face == 14) {    //wilddraw4
        next = (turn + direction + NUM_PLAYERS) % NUM_PLAYERS;
        give_cards(next, 4);
        next_turn(); next_turn();
    } else {
        next_turn();
    }

    return 0;
}

// for bots

int bot_turn(void) {
    int i, j, count[4], best;

    for (i = 0; i < hand_size[turn]; i++) {
        if (!can_play(hand[turn][i])) continue;

        int pick = 0;   

        if (is_wild(hand[turn][i])) {
            count[0] = count[1] = count[2] = count[3] = 0;
            for (j = 0; j < hand_size[turn]; j++)
                if (card_color(hand[turn][j]) < 4)
                    count[card_color(hand[turn][j])]++;
            best = 0;
            for (j = 1; j < 4; j++)
                if (count[j] > count[best]) best = j;
            pick = best;
        } else {
            pick = card_color(hand[turn][i]);
        }

        return play_card(turn, i, pick);
    }

    
    give_cards(turn, 1);
    sprintf(last_action, "%s drew a card", name[turn]);
    if (can_play(hand[turn][hand_size[turn] - 1])) {
        int drawn = hand[turn][hand_size[turn] - 1];
        int pick  = is_wild(drawn) ? 0 : card_color(drawn);
        return play_card(turn, hand_size[turn] - 1, pick);
    }

    next_turn();
    return 0;
}
