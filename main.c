#ifdef _WIN32
#include <ncurses/curses.h>
#else
#include <ncurses.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIZEX 8
#define SIZEY 8
#define MINE_COUNT 10

void printGrid(char **grid, char **truth){
    clear();
    for(int i = 0; i < SIZEY; i++){
        for(int k = 0; k < SIZEX; k++){
            if(truth[i][k] == ' '){
                printw("%c ", grid[i][k]);
            }else{
                printw("%c ", truth[i][k]);
            }
        }
        printw("\n");
    }
}

int main(){
    //Initialise ncurses and seed the randomizer
    initscr();
    noecho();
    cbreak();
    curs_set(2);
    bool colCheck = has_colors();
    if(colCheck){
        start_color();
    }
    srand(time(NULL));

    char **word;
    char **visibilityGrid;
    char s = 'z';
    while(s == 'z'){
        //Allocate the grid in memory
        word = calloc(SIZEY, 8);
        visibilityGrid = calloc(SIZEY, 8);
        for(int i = 0; i < SIZEY; i++){
            word[i] = calloc(SIZEX, 1);
            visibilityGrid[i] = calloc(SIZEX, 1);
            for(int k = 0; k < SIZEX; k++){
                word[i][k] = '0';
                visibilityGrid[i][k] = '#';
            }
        }

        //Generate the grid
        for(int i = 0; i < MINE_COUNT; i++){
            uint8_t x = rand() % SIZEY;
            uint8_t y = rand() % SIZEX;
            while (word[x][y] == '*') {
                x = rand() % SIZEY;
                y = rand() % SIZEX;
            }
            word[x][y] = '*';
            if(x != 0 && y != SIZEX - 1 && word[x - 1][y + 1] != '*') word[x - 1][y + 1]++;
            if(y != SIZEX - 1 && word[x][y + 1] != '*') word[x][y + 1]++;
            if(x != SIZEY - 1 && y != SIZEX - 1 && word[x + 1][y + 1] != '*') word[x + 1][y + 1]++;
            if(x != 0 && word[x - 1][y] != '*') word[x - 1][y]++;
            if(x != SIZEY - 1 && word[x + 1][y] != '*') word[x + 1][y]++;
            if(x != 0 && y != 0 && word[x - 1][y - 1] != '*') word[x - 1][y - 1]++;
            if(y != 0 && word[x][y - 1] != '*') word[x][y - 1]++;
            if(x != SIZEY - 1 && y != 0 && word[x + 1][y - 1] != '*') word[x + 1][y - 1]++;
        }

        int currx = 0, curry = 0;
        int slotsRemaining = SIZEX * SIZEY;
        bool escape = false;
        bool state = false;
        char a;
        if(colCheck){
            attroff(COLOR_PAIR(1));
            init_pair(1, COLOR_WHITE, COLOR_BLUE);
            attron(COLOR_PAIR(1));
        }
        for(;;){
            printGrid(word, visibilityGrid);
            printw("\nControls:\nMove: [Arrow keys]\nOpen: [Z]\nMark: [X]\nQuit: [Q]");
            move(curry, currx);
            a = getch();
            switch (a) {
                case (char)68:
                    if(currx != 0) currx-= 2;
                    break;
                case (char)67:
                    if(currx != (SIZEX - 1) * 2) currx+= 2;
                    break;
                case (char)65:
                    if(curry != 0) curry--;
                    break;
                case (char)66:
                    if(curry != SIZEY - 1) curry++;
                    break;
                case 'z':
                    if(visibilityGrid[curry][currx / 2] == ' ' || visibilityGrid[curry][currx / 2] == '>' || visibilityGrid[curry][currx / 2] == '?'){
                        break;
                    }
                    if(word[curry][currx / 2] == '*'){
                        escape = true;
                    }
                    visibilityGrid[curry][currx / 2] = ' ';
                    slotsRemaining--;
                    if(word[curry][currx / 2] == '0'){
                        if(curry != 0 && currx / 2 != SIZEX - 1 && visibilityGrid[curry - 1][currx / 2 + 1] != ' '){
                            visibilityGrid[curry - 1][currx / 2 + 1] = ' ';
                            slotsRemaining--;
                        }
                        if(currx / 2 != SIZEX - 1 && visibilityGrid[curry][currx / 2 + 1] != ' '){
                            visibilityGrid[curry][currx / 2 + 1] = ' ';
                            slotsRemaining--;
                        }
                        if(curry != SIZEY - 1 && currx / 2 != SIZEX - 1 && visibilityGrid[curry + 1][currx / 2 + 1] != ' '){
                            visibilityGrid[curry + 1][currx / 2 + 1] = ' ';
                            slotsRemaining--;
                        }
                        if(curry != 0 && visibilityGrid[curry - 1][currx / 2] != ' '){
                            visibilityGrid[curry - 1][currx / 2] = ' ';
                            slotsRemaining--;
                        }
                        if(curry != SIZEY - 1 && visibilityGrid[curry + 1][currx / 2] != ' '){
                            visibilityGrid[curry + 1][currx / 2] = ' ';
                            slotsRemaining--;
                        }
                        if(curry != 0 && currx / 2 != 0 && visibilityGrid[curry - 1][currx / 2 - 1] != ' '){
                            visibilityGrid[curry - 1][currx / 2 - 1] = ' ';
                            slotsRemaining--;
                        }
                        if(currx / 2 != 0 && visibilityGrid[curry][currx / 2 - 1] != ' '){
                            visibilityGrid[curry][currx / 2 - 1] = ' ';
                            slotsRemaining--;
                        }
                        if(curry != SIZEY - 1 && currx / 2 != 0 && visibilityGrid[curry + 1][currx / 2 - 1] != ' '){
                            visibilityGrid[curry + 1][currx / 2 - 1] = ' ';
                            slotsRemaining--;
                        }
                    }
                    break;
                case 'x':
                    if(visibilityGrid[curry][currx / 2] == ' ') break;
                    if(visibilityGrid[curry][currx / 2] == '#'){
                        visibilityGrid[curry][currx / 2] = '>';
                    }else if(visibilityGrid[curry][currx / 2] == '>'){
                        visibilityGrid[curry][currx / 2] = '?';
                    }else{
                        visibilityGrid[curry][currx / 2] = '#';
                    }
                    break;
                case 'q':
                    escape = true;
                    break;
                default:
                    break;
            }
            if(slotsRemaining == MINE_COUNT){
                escape = true;
                state = true;
            }
            if(escape){
                for(int i = 0; i < SIZEY; i++){
                    for(int k = 0; k < SIZEX; k++){
                        visibilityGrid[i][k] = ' ';
                    }
                }
                printGrid(word, visibilityGrid);
                if(colCheck){
                    attroff(COLOR_PAIR(1));
                }
                if(!state){
                    if(colCheck){
                        init_pair(1, COLOR_WHITE, COLOR_RED);
                        attron(COLOR_PAIR(1));
                    }
                    printw("\nYou Lost");
                }else{
                    if(colCheck){
                        init_pair(1, COLOR_WHITE, COLOR_GREEN);
                        attron(COLOR_PAIR(1));
                    }
                    printw("\nYou Win");
                }
                printw(". Press Z to start a new game.\nPress any other key to quit.");
                s = getch();
                free(word);
                free(visibilityGrid);
                break;
            }
        }
    }


    //Free the memory and end ncurses
    if(colCheck){
        attroff(COLOR_PAIR(1));
    }
    endwin();
    exit(0);
}
