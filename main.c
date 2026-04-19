#ifdef _WIN32
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

void printGrid(char **grid, char **truth, int *sizex, int *sizey){
    clear();
    for(int i = 0; i < *sizey; i++){
        for(int k = 0; k < *sizex; k++){
            (truth[i][k] == ' ') ? printw("%c ", grid[i][k]) : printw("%c ", truth[i][k]);
        }
        printw("\n");
    }
}

int main(){
    //Initialise ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(2);
    bool colCheck = has_colors();
    if(colCheck){
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE); //Neutral colors
        init_pair(2, COLOR_WHITE, COLOR_RED); //Lose colors
        init_pair(3, COLOR_WHITE, COLOR_GREEN); //Win colors
    }
    srand(time(NULL)); //Seed the randomizer

    //Initialise the game
    char **word; //The actual grid
    char **visibilityGrid; //What blocks are visible
    char input = 'z'; //user input
    int sizex = 8, sizey = 8, mine_count = 10; //The grid's X, Y and bomb count
    while(input == 'z'){
        //Allocate the grid in memory
        word = calloc(sizey, 8);
        visibilityGrid = calloc(sizey, 8);
        for(int i = 0; i < sizey; i++){
            word[i] = calloc(sizex, 1);
            visibilityGrid[i] = calloc(sizex, 1);
            for(int k = 0; k < sizex; k++){
                word[i][k] = '0';
                visibilityGrid[i][k] = '#';
            }
        }

        //Generate the grid
        for(int i = 0; i < mine_count; i++){
            uint8_t x, y;
            //If the coordinates generated already have a bomb on it, regenerate until the current slot is clear
            do{
                x = rand() % sizey;
                y = rand() % sizex;
            }while (word[x][y] == '*');
            word[x][y] = '*';
            //Increment the value of every block around the bomb unless the said block is also a bomb and doesn't go off-bounds
            if(x != 0 && y != sizex - 1 && word[x - 1][y + 1] != '*') word[x - 1][y + 1]++;
            if(y != sizex - 1 && word[x][y + 1] != '*') word[x][y + 1]++;
            if(x != sizey - 1 && y != sizex - 1 && word[x + 1][y + 1] != '*') word[x + 1][y + 1]++;
            if(x != 0 && word[x - 1][y] != '*') word[x - 1][y]++;
            if(x != sizey - 1 && word[x + 1][y] != '*') word[x + 1][y]++;
            if(x != 0 && y != 0 && word[x - 1][y - 1] != '*') word[x - 1][y - 1]++;
            if(y != 0 && word[x][y - 1] != '*') word[x][y - 1]++;
            if(x != sizey - 1 && y != 0 && word[x + 1][y - 1] != '*') word[x + 1][y - 1]++;
        }

        //Initialise more of the game and start up the color
        int currx = 0, curry = 0; //Current XY cursor coords
        int slotsRemaining = sizex * sizey - mine_count; //The remaining slots
        bool escape = false; //Whether or not the game is over
        bool state = true; //Whether or not the game is worn
        if(colCheck) attron(COLOR_PAIR(1));
        for(;;){
            //Print the game
            printGrid(word, visibilityGrid, &sizex, &sizey);
            printw("\nControls:\nMove: [Arrow keys]\nOpen: [Z]\nMark: [X]\nQuit: [Q]\n\nDifficulty:\nEasy: [1]\nMedium: [2]\nHard: [3]");
            move(curry, currx);
            //Process user input
            input = getch();
            if(input == 'q' || input == '1' || input == '2' || input == '3'){
                escape = true;
                state = false;
            }else{
                switch (input) {
                    #ifdef _WIN32
                    case (char)75:
                        if(currx != 0) currx-= 2;
                        break;
                    case (char)77:
                        if(currx != (sizex - 1) * 2) currx+= 2;
                        break;
                    case (char)72:
                        if(curry != 0) curry--;
                        break;
                    case (char)80:
                        if(curry != sizey - 1) curry++;
                        break;
                    #else
                    case (char)68:
                        if(currx != 0) currx-= 2;
                        break;
                    case (char)67:
                        if(currx != (sizex - 1) * 2) currx+= 2;
                        break;
                    case (char)65:
                        if(curry != 0) curry--;
                        break;
                    case (char)66:
                        if(curry != sizey - 1) curry++;
                        break;
                    #endif
                    case 'z':
                        if(visibilityGrid[curry][currx / 2] != '#') break; //If the block is already unlocked, exit (Prevents incorrect decrease of slotsRemaining)
                        if(word[curry][currx / 2] == '*'){
                            escape = true;
                            state = false;
                        }
                        visibilityGrid[curry][currx / 2] = ' ';
                        slotsRemaining--;
                        //If the cleared block is 0, clear everything around it
                        if(word[curry][currx / 2] == '0'){
                            if(curry != 0 && currx / 2 != sizex - 1 && visibilityGrid[curry - 1][currx / 2 + 1] == '#'){
                                visibilityGrid[curry - 1][currx / 2 + 1] = ' ';
                                slotsRemaining--;
                            }
                            if(currx / 2 != sizex - 1 && visibilityGrid[curry][currx / 2 + 1] == '#'){
                                visibilityGrid[curry][currx / 2 + 1] = ' ';
                                slotsRemaining--;
                            }
                            if(curry != sizey - 1 && currx / 2 != sizex - 1 && visibilityGrid[curry + 1][currx / 2 + 1] == '#'){
                                visibilityGrid[curry + 1][currx / 2 + 1] = ' ';
                                slotsRemaining--;
                            }
                            if(curry != 0 && visibilityGrid[curry - 1][currx / 2] == '#'){
                                visibilityGrid[curry - 1][currx / 2] = ' ';
                                slotsRemaining--;
                            }
                            if(curry != sizey - 1 && visibilityGrid[curry + 1][currx / 2] == '#'){
                                visibilityGrid[curry + 1][currx / 2] = ' ';
                                slotsRemaining--;
                            }
                            if(curry != 0 && currx / 2 != 0 && visibilityGrid[curry - 1][currx / 2 - 1] == '#'){
                                visibilityGrid[curry - 1][currx / 2 - 1] = ' ';
                                slotsRemaining--;
                            }
                            if(currx / 2 != 0 && visibilityGrid[curry][currx / 2 - 1] == '#'){
                                visibilityGrid[curry][currx / 2 - 1] = ' ';
                                slotsRemaining--;
                            }
                            if(curry != sizey - 1 && currx / 2 != 0 && visibilityGrid[curry + 1][currx / 2 - 1] == '#'){
                                visibilityGrid[curry + 1][currx / 2 - 1] = ' ';
                                slotsRemaining--;
                            }
                        }
                        break;
                    case 'x':
                        if(visibilityGrid[curry][currx / 2] != ' '){
                            //Cycle through Clear(#), Flag(>), and question mark(?)
                            switch(visibilityGrid[curry][currx / 2]){
                                case '#':
                                    visibilityGrid[curry][currx / 2] = '>';
                                    break;
                                case '>':
                                    visibilityGrid[curry][currx / 2] = '?';
                                    break;
                                default:
                                    visibilityGrid[curry][currx / 2] = '#';
                                    break;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            if(!slotsRemaining && state != false){
                escape = true;
            }
            if(escape){
                //Reveal every block
                for(int i = 0; i < sizey; i++){
                    for(int k = 0; k < sizex; k++){
                        visibilityGrid[i][k] = ' ';
                    }
                }
                //Change the color
                colCheck && !state ? attron(COLOR_PAIR(2)) : attron(COLOR_PAIR(3));
                printGrid(word, visibilityGrid, &sizex, &sizey);
                !state ? printw("\nYou Lost") : printw("\nYou Win");
                printw(". Press Z to start a new game.\nPress any other key to quit.");
                //Set the difficulty
                switch(input){
                    case '1':
                        sizex = 8;
                        sizey = 8;
                        mine_count = 10;
                        break;
                    case '2':
                        sizex = 16;
                        sizey = 16;
                        mine_count = 40;
                        break;
                    case '3':
                        sizex = 30;
                        sizey = 16;
                        mine_count = 99;
                        break;
                }
                input = getch();
                //Free the board
                free(word);
                free(visibilityGrid);
                break;
            }
        }
    }
    //End ncurses
    endwin();
    return 0;
}
