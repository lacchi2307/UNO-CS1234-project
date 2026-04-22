#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "uno.h"

// taking input from the human 

static int human_turn(void) {
    char input[8];
    int rows, cols, choice, color, answer;
    getmaxyx(stdscr, rows, cols);
    (void)cols;

    while (1) {
        draw_game();
        mvprintw(rows - 3, 1, "Your move > ");
        clrtoeol();
        refresh();

        echo();
        getnstr(input, 6);
        noecho();

        // Quit
        if (input[0] == 'q' || input[0] == 'Q') {
            endwin();
            printf("Thanks for playing!\n");
            exit(0);
        }

        // draw a card
        if (input[0] == 'd' || input[0] == 'D') {
            give_cards(turn, 1);
            sprintf(last_action, "%s drew a card", name[turn]);

            int drawn = hand[turn][hand_size[turn] - 1];
            if (can_play(drawn)) {
                draw_game();
                mvprintw(rows - 3, 1, "Drawn card is playable. Play it? (y/n) > ");
                clrtoeol();
                refresh();
                answer = getch();
                if (answer == 'y' || answer == 'Y') {
                    color = is_wild(drawn) ? ask_color() : card_color(drawn);
                    return play_card(turn, hand_size[turn] - 1, color);
                }
            }
            next_turn();
            return 0;
        }

        // choose the number
        choice = atoi(input) - 1;

        if (choice < 0 || choice >= hand_size[turn]) {
            mvprintw(rows - 3, 1, "Not a valid number. Try again.");
            refresh();
            napms(900);
            continue;
        }
        if (!can_play(hand[turn][choice])) {
            mvprintw(rows - 3, 1, "You can't play that card.");
            refresh();
            napms(900);
            continue;
        }

        color = is_wild(hand[turn][choice]) ? ask_color() : card_color(hand[turn][choice]);
        return play_card(turn, choice, color);
    }
}

int main(void) {
    int num_humans, game_over, i, winner;

    srand((unsigned int)time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Terminal doesn't support colors.\n");
        return 1;
    }

    init_display();
    num_humans = welcome_screen();
    setup_game(num_humans);

    game_over = 0;
    while (!game_over) {
        if (is_bot[turn]) {
            draw_bot_screen();
            game_over = bot_turn();
        } else {
            game_over = human_turn();
        }
    }

    // finding out  is anyone is won
    winner = 0;
    for (i = 0; i < NUM_PLAYERS; i++)
        if (hand_size[i] == 0) winner = i;

    draw_winner(winner);
    endwin();
    return 0;
}

