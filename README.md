# UNO Game (C + ncurses)

## Overview

This is a terminal-based UNO game implemented in C using the ncurses library.
It supports 4 players with both human and bot players.

---

## Features

* Standard UNO gameplay
* Special cards: Skip, Reverse, Draw 2, Wild, Wild +4
* Simple bot (plays first valid card)
* Terminal UI using ncurses
* Input-based gameplay

---

## Requirements

### Install ncurses (Linux)

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

### Compile

```bash
make
```


## Run the Game

```bash
./uno
```

---

## Game Setup

* Enter number of human players 

---

## Gameplay

### On Your Turn

You can:

* Play a card
* Draw a card

---

### Play a Card

A card can be played if it matches:

* Current color
* Same number/symbol
* Wild card

---

### Draw a Card

* Press **D**
* If playable, you may play it

---

## Special Cards

* **Skip (SKP)** → next player skips turn
* **Reverse (REV)** → changes direction
* **+2** → next player draws 2 cards
* **Wild (WLD)** → choose a color
* **Wild +4 (W+4)** → choose color, next draws 4

---


## Controls

| Action    | Input                 |
| --------- | --------------------- |
| Play card | number (e.g., 1,2,3)  |
| Draw card | D                     |

---

## Notes

* No stacking of +2 or +4 cards
* Simple bot logic
* Game ends when a player has 0 cards

---

## Author Notes

This project focuses on a clean and simple implementation of UNO using C and ncurses.

