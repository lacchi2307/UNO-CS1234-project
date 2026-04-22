#include <ncurses.h>
#include <string.h>
#include "uno.h"

/*
 * display.c
 * ncurses drawing. As simple as possible.
 *
 * Color pairs:
 *   1 = Red    (white on red)
 *   2 = Yellow (black on yellow)
 *   3 = Green  (white on green)
 *   4 = Blue   (white on blue)
 *   5 = Wild   (black on white)
 *   6 = header (black on cyan)
 */

void init_display(void) {
    start_color();
    init_pair(1, COLOR_WHITE,  COLOR_RED);
    init_pair(2, COLOR_BLACK,  COLOR_YELLOW);
    init_pair(3, COLOR_WHITE,  COLOR_GREEN);
    init_pair(4, COLOR_WHITE,  COLOR_BLUE);
    init_pair(5, COLOR_BLACK,  COLOR_WHITE);
    init_pair(6, COLOR_BLACK,  COLOR_CYAN);
}


static int cpair(int color) { return color + 1; }

// draw and check it is playable 
static void draw_card_box(int row, int col, int card, int playable) {
    char label[16];
    
    static const char *faces[] = {"0","1","2","3","4","5","6","7","8","9",
                                  "Skip","Rev","+2","Wild","W+4"};
    strcpy(label, faces[card_face(card)]);

    int pair = cpair(card_color(card));
    if (playable) attron(A_BOLD);
    attron(COLOR_PAIR(pair));
    mvprintw(row,     col, "+-------+");
    mvprintw(row + 1, col, "|       |");
    mvprintw(row + 2, col, "| %-5s |", label);
    mvprintw(row + 3, col, "|       |");
    mvprintw(row + 4, col, "+-------+");
    attroff(COLOR_PAIR(pair));
    if (playable) attroff(A_BOLD);
}

void draw_game(void) {
    int rows, cols, i, row, col;
    getmaxyx(stdscr, rows, cols);
    clear();

    
    attron(COLOR_PAIR(6) | A_BOLD);
    mvhline(0, 0, ' ', cols);
    mvprintw(0, (cols - 11) / 2, "=== UNO ===");
    attroff(COLOR_PAIR(6) | A_BOLD);

    
    mvprintw(2, 2, "Last played:");
    draw_card_box(3, 2, pile[pile_top - 1], 0);

    // active color swatch
    mvprintw(9, 2, "Active color:");
    {
        static const char *cnames[] = {"Red","Yellow","Green","Blue","Wild"};
        attron(COLOR_PAIR(cpair(active_color)) | A_BOLD);
        mvprintw(10, 2, " %-8s ", cnames[active_color]);
        attroff(COLOR_PAIR(cpair(active_color)) | A_BOLD);
    }

 // other players
    mvprintw(12, 2, "Other players:");
    row = 13;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == turn) continue;
        mvprintw(row++, 2, "%-12s %2d cards", name[i], hand_size[i]);
    }

    // current player hand's
    mvprintw(2, 30, "Your hand (%s):", name[turn]);
    row = 3;
    col = 30;
    for (i = 0; i < hand_size[turn]; i++) {
        int playable = can_play(hand[turn][i]);
        draw_card_box(row, col, hand[turn][i], playable);

        if (playable) attron(A_BOLD);
        else          attron(A_DIM);
        mvprintw(row + 5, col + 1, "[%2d]%s", i + 1, playable ? "*" : " ");
        if (playable) attroff(A_BOLD);
        else          attroff(A_DIM);

        col += 10;
        if (col + 10 > 22 + 80) { col = 30; row += 7; }
    }

    /* status bar */

    attron(COLOR_PAIR(5));
    mvhline(rows - 2, 0, ' ', cols);
    mvprintw(rows - 2, 1, "Last: %s", last_action);
    attroff(COLOR_PAIR(5));

    attron(A_DIM);
    mvprintw(rows - 1, 1, "number = play card   d = draw   q = quit");
    attroff(A_DIM);

    refresh();
}

void draw_bot_screen(void) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    clear();

    attron(COLOR_PAIR(6) | A_BOLD);
    mvhline(0, 0, ' ', cols);
    mvprintw(0, (cols - 11) / 2, "=== UNO ===");
    attroff(COLOR_PAIR(6) | A_BOLD);

    /* other players */
    mvprintw(2, 2, "Other players:");
    int row = 3, i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == turn) continue;
        mvprintw(row++, 2, "%-12s %2d cards", name[i], hand_size[i]);
    }

    attron(A_BOLD);
    mvprintw(rows / 2, (cols - 30) / 2, "%s is thinking...", name[turn]);
    attroff(A_BOLD);

    attron(COLOR_PAIR(5));
    mvhline(rows - 2, 0, ' ', cols);
    mvprintw(rows - 2, 1, "Last: %s", last_action);
    attroff(COLOR_PAIR(5));

    attron(A_DIM);
    mvprintw(rows - 1, 1, "Press any key...");
    attroff(A_DIM);

    refresh();
    getch();
}

void draw_winner(int winner) {
    int rows, cols, i;
    getmaxyx(stdscr, rows, cols);
    clear();

    attron(COLOR_PAIR(6) | A_BOLD);
    mvhline(0, 0, ' ', cols);
    mvprintw(0, (cols - 11) / 2, "=== UNO ===");
    attroff(COLOR_PAIR(6) | A_BOLD);

    attron(A_BOLD);
    mvprintw(rows / 2 - 2, (cols - 34) / 2,
             "*** %s WINS! Congratulations! ***", name[winner]);
    attroff(A_BOLD);

    mvprintw(rows / 2, (cols - 26) / 2, "Cards left in other hands:");
    int row = rows / 2 + 2;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i == winner) continue;
        mvprintw(row++, (cols - 22) / 2, "  %-12s : %d cards",
                 name[i], hand_size[i]);
    }

    mvprintw(rows - 2, (cols - 22) / 2, "Press any key to exit.");
    refresh();
    getch();
}

int ask_color(void) {
    int rows, cols, key;
    getmaxyx(stdscr, rows, cols);
    clear();

    attron(A_BOLD);
    mvprintw(rows / 2 - 3, (cols - 26) / 2, "Pick a color for your wild:");
    attroff(A_BOLD);

    attron(COLOR_PAIR(1)); mvprintw(rows/2 - 1, (cols-12)/2, "  1. Red   "); attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2)); mvprintw(rows/2,     (cols-12)/2, "  2. Yellow"); attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3)); mvprintw(rows/2 + 1, (cols-12)/2, "  3. Green "); attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4)); mvprintw(rows/2 + 2, (cols-12)/2, "  4. Blue  "); attroff(COLOR_PAIR(4));

    refresh();
    while (1) {
        key = getch();
        if (key == '1') return 0;   /* Red    */
        if (key == '2') return 1;   /* Yellow */
        if (key == '3') return 2;   /* Green  */
        if (key == '4') return 3;   /* Blue   */
    }
}

int welcome_screen(void) {
    int rows, cols, key;
    getmaxyx(stdscr, rows, cols);
    clear();

    attron(COLOR_PAIR(6) | A_BOLD);
    mvhline(0, 0, ' ', cols);
    mvprintw(0, (cols - 11) / 2, "=== UNO ===");
    attroff(COLOR_PAIR(6) | A_BOLD);

    attron(A_BOLD);
    mvprintw(rows/2 - 3, (cols - 16) / 2, "Welcome to UNO!");
    attroff(A_BOLD);

    mvprintw(rows/2 - 1, (cols - 16) / 2, "4 player game");
    mvprintw(rows/2 + 1, (cols - 50) / 2, "How many humans? Press 1, 2, 3, or 4.Remaining players will be bots.");

    refresh();
    while (1) {
        key = getch();
        if (key >= '1' && key <= '4') return key - '0';
    }
}
