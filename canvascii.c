/** @file canvascii.c
 *
 * This file contains the main method of the
 * program and all the fucntions used of it
 *
 *  @author Jonathan Labelle - LABJ28039006
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAXHEIGHT 40
#define MAXWIDTH 80
#define BUFFER 100
#define RESET "\033[0m"
#define BLACK "\x1B[40m"
#define RED "\x1B[41m"
#define GREEN "\x1B[42m"
#define YELLOW "\x1B[43m"
#define BLUE "\x1B[44m"
#define MAGENTA "\x1B[45m"
#define CYAN "\x1B[46m"
#define WHITE "\x1B[47m"
#define ERR_MES_1 "Error: wrong pixel value in canvas"
#define ERR_MES_2 "Error: canvas is too high (max height:"
#define ERR_MES_3 "Error: canvas is too wide"
#define ERR_MES_4 "Error: canvas is non rectangular"
#define ERR_MES_5 "Error: unrecognized option"
#define ERR_MES_6 "Error: option with missing value"
#define ERR_MES_7 "Error: problem with value"

#define USAGE "\
Usage: ./canvascii [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
Draws on an ASCII canvas. The canvas is provided on stdin and\n\
the result is printed on stdout. The dimensions of the canvas\n\
are limited to at most 40 rows and at most 80 columns.\n\
\n\
If no argument is provided, the program prints this help and exit.\n\
\n\
Canvas options:\n\
  -n HEIGHT,WIDTH           Creates a new empty canvas of HEIGHT rows and\n\
                            WIDTH columns. Should be used as first option,\n\
                            otherwise, the behavior is undefined.\n\
                            Ignores stdin.\n\
  -s                        Shows the canvas and exit.\n\
  -k                        Enables colored output. Replaces characters\n\
                            between 0 and 9 by their corresponding ANSI\n\
                            colors:\n\
                              0: black  1: red      2: green  3: yellow\n\
                              4: blue   5: magenta  6: cyan   7: white\n\
\n\
Drawing options:\n\
  -p CHAR                   Sets the pen to CHAR. Allowed pens are\n\
                            0, 1, 2, 3, 4, 5, 6 or 7. Default pen\n\
                            is 7.\n\
  -h ROW                    Draws an horizontal line on row ROW.\n\
  -v COL                    Draws a vertical line on column COL.\n\
  -r ROW,COL,HEIGHT,WIDTH   Draws a rectangle of dimension HEIGHTxWIDTH\n\
                            with top left corner at (ROW,COL).\n\
  -l ROW1,COL1,ROW2,COL2    Draws a discrete segment from (ROW1,COL1) to\n\
                            (ROW2,COL2) with Bresenham's algorithm.\n\
  -c ROW,COL,RADIUS         Draws a circle centered at (ROW,COL) of\n\
                            radius RADIUS with the midpoint algorithm.\n\
"

void createBlankCanvass(char[*][*]);
void readCanvassFile(char[*][*]);
int getWidthNFlag(int, char*[]);
int getHeightNFlag(int, char*[]);
int getWidthSFlag();
int getHeightSFlag();
void changeCrayon(char c);
void drawVerticalLine(char[*][*], int);
void drawHorizontalLine(char[*][*], int);
void drawRectangle(char[*][*],char *argv[]);
void drawCircle (char[*][*], char *argv[]);
void calculateThreeCoordinates (char  *argv[], int* x, int* y, int *r);
int validationInsideCanvass (int, int);
void calculateCoodinates(char *argv[], int*, int*, int*, int*);
void drawLine (char [*][*], char *argv[]);
void printColoredCanvass(char [*][*]);
void printCanvass(char[*][*]);
void validCommandLineOptions (int, char *argv[]);
int validationValidCharacter (int);
void validCommandLineSyntax (int, char *argv[]);

enum error {
    OK                         = 0, // Everything is ok
    ERR_WRONG_PIXEL            = 1,
    ERR_CANVAS_TOO_HIGH        = 2,
    ERR_CANVAS_TOO_WIDE        = 3, // Canvas is too wide
    ERR_CANVAS_NON_RECTANGULAR = 4, // Canvas is non rectangular
    ERR_UNRECOGNIZED_OPTION    = 5, // Unrecognized option
    ERR_MISSING_VALUE          = 6, // Option with missing value
    ERR_WITH_VALUE             = 7  // Problem with value
};

int width;
int height;
char crayon = '7';
FILE *f;

/*
 * CREATION OF THE CANVASS (option -n and -s)
 */

void createBlankCanvass (char (*canvass)[width]) {

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            *(*(canvass + i) + j) = '.';
        }
    }
}

void readCanvassFile(char (*canvass)[width]) {
    char buffer[BUFFER];
    int j = 0;
    while (  fgets(buffer, sizeof(buffer), stdin) != NULL )
    {
        for (int i = 0; i <= width; i++) {
            if (validationInsideCanvass(j, i)) {
                validationValidCharacter(buffer[i]);
                canvass[j][i] = buffer[i];
            }
        }
        j++;
    }
}

int getHeightSFlag() {
    char buffer[BUFFER];
    int i = 0;
    while (  fgets(buffer, sizeof(buffer), stdin) != NULL ) {i++;}
    rewind(stdin);
    if (i > MAXHEIGHT) {
        fprintf(stderr,"Error: canvas is too high (max height: %d)\n", MAXHEIGHT );
        fprintf(stderr, "%s", USAGE);
        exit(2);
    }
    return i;
}

int getWidthSFlag() {
    char buffer[BUFFER];
    unsigned int line1 = 0;
    unsigned int line2 = 0;
    if (  fgets(buffer, sizeof(buffer), stdin) != NULL ) {
        line1 = strlen(buffer);
        line2 = strlen(buffer);
    }
    while (  fgets(buffer, sizeof(buffer), stdin) != NULL )
    {
        line1 = strlen(buffer);
        if (line1 != line2) {
            fprintf(stderr, "Error: canvas should be rectangular\n");
            fprintf(stderr, "%s", USAGE);
            exit(4);
        }
        line2 = line1;
    }
    rewind(stdin);
    if (line1 > MAXWIDTH + 1) {
        fprintf(stderr,"Error: canvas is too wide (max width: %d)\n", MAXWIDTH );
        fprintf(stderr, "%s", USAGE);
        exit(3);
    }
    return (int)line1 - 1;
}

int getWidthNFlag(int argc, char *argv[]) {
    width = 0;
    int argvPosition = 1;

    while (argc > argvPosition) {

        if (strcmp("-n", argv[argvPosition]) == 0) {

            if (argvPosition + 1 == argc) {
                fprintf(stderr, "Error: missing value with option %s", argv[argvPosition]);
                fprintf(stderr, "\n%s", USAGE);
                exit(6);
            } else {
                char *str = strstr(argv[argvPosition+1], ",") + 1;
                char *ptr;
                width = strtol(str, &ptr, 10);
                break;
            }
        }
        argvPosition++;
    }
    return width;
}

int getHeightNFlag(int argc, char *argv[]) {
    int argvPosition = 0;

    while (argc > argvPosition) {
        if (strcmp("-n", argv[argvPosition]) == 0) {
            char *ptr;
            height = strtol(argv[argvPosition+1], &ptr, 10);
            break;
        }
        argvPosition++;
    }
    return height;
}

/*
 * DRAWING IN THE CANVASS
 */

void changeCrayon(char c) {
    if ((c >= 48 && c <= 55 )) {
        crayon = c;
    } else {
        fprintf(stderr, "Error: incorrect value with option -p\n");
        fprintf(stderr, "%s", USAGE);
        exit(7);
    }
}

void drawHorizontalLine(char (*canvass)[width], int line) {
    if (line > height - 1 ||  line < 0) {
        fprintf(stderr, "Error: incorrect value with option -h\n");
        fprintf(stderr, "%s", USAGE);
        exit(7);
    }
    for (int i = 0; i < width; i++) {
        *(*(canvass + line) + i) = crayon;
    }
}

void drawVerticalLine(char (*canvass)[width], int column) {
    if (column > width - 1 || column < 0) {
        fprintf(stderr, "Error: incorrect value with option -v\n");
        fprintf(stderr, "%s", USAGE);
        exit(7);
    }
    for (int i = 0; i < height; i++) {
        *(*(canvass + i) + column) = crayon;
    }
}

void drawRectangle(char (*canvass)[width], char *argv[]) {

    int y, x, rectHeight, rectWidth, indexX, indexY;
    calculateCoodinates(&argv[0], &x, &y, &rectHeight, &rectWidth);
    if (rectWidth < 0 || rectHeight < 0) {
        fprintf(stderr, "Error: incorrect value with option -r\n");
        fprintf(stderr, "%s", USAGE);
        exit(7);
    }
    for(int i = 0; i < rectWidth; i++) {
        indexX = x ; indexY = y + i;
        if (validationInsideCanvass( indexX, indexY)) {
            canvass[indexX][indexY] = crayon;
        }
        indexX = x  + rectHeight -1; indexY = y + i;
        if (validationInsideCanvass(indexX, indexY)) {
            canvass[indexX][indexY] = crayon;
        }
    }
    for(int i = 0; i < rectHeight; i++) {
        indexX = x + i; indexY = y;
        if (validationInsideCanvass(indexX, indexY)) {
            canvass[indexX][indexY] = crayon;
        }
        indexX = x + i; indexY = y + rectWidth - 1;
        if (validationInsideCanvass(indexX, indexY)) {
            canvass[indexX][indexY] = crayon;
        }
    }
}

void drawLine (char (*canvass)[width], char *argv[])
{
    int x0, y0, x1, y1;
    calculateCoodinates(&argv[0], &x0, &y0, &x1, &y1);
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;){
        if (validationInsideCanvass(x0, y0)) {
            canvass[x0][y0] = crayon;
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawCircle(char (*canvass)[width], char *argv[]) {

    int xm, ym, r, x, y, err;
    calculateThreeCoordinates(&argv[0], &xm, &ym, &r);
    if (r < 0) {
        fprintf(stderr, "Error: incorrect value with option -c\n");
        fprintf(stderr, "%s", USAGE);
        exit(7);
    }
    x = -r; y = 0; err = 2-2*r;
    do {
        if (validationInsideCanvass(xm-x , ym+y)) { canvass[xm-x][ym+y] = crayon; }
        if (validationInsideCanvass(xm-y , ym-x)) { canvass[xm-y][ym-x] = crayon; }
        if (validationInsideCanvass(xm+x , ym-y)) { canvass[xm+x][ym-y] = crayon; }
        if (validationInsideCanvass(xm+y , ym+x)) { canvass[xm+y][ym+x] = crayon; }
        r = err;
        if (r >  x) err += ++x*2+1;
        if (r <= y) err += ++y*2+1;
    } while (x < 0);
}

void calculateThreeCoordinates (char  *argv[], int* x, int* y, int *r) {
    char *str = argv[0];
    char *ptr;

    char *token = strtok(str, ",");
    *x = strtol(token, &ptr, 10) ;
    token = strtok(NULL, ",");
    *y = strtol(token, &ptr, 10);
    token = strtok(NULL, ",");
    *r = strtol(token, &ptr, 10);
    token = strtok(NULL, ",");
}

/*
 * PRINTING CANVASS
 */

void printCanvass(char (*canvass)[width]) {

    for (int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            printf("%c", canvass[i][j]);
        }
        printf("\n");
    }
}

void printColoredCanvass(char (*canvass)[width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (canvass[i][j] == '0') printf(BLACK " " RESET);
            if (canvass[i][j] == '1') printf(RED " " RESET);
            if (canvass[i][j] == '2') printf(GREEN " " RESET);
            if (canvass[i][j] == '3') printf(YELLOW " " RESET);
            if (canvass[i][j] == '4') printf(BLUE " " RESET);
            if (canvass[i][j] == '5') printf(MAGENTA " " RESET);
            if (canvass[i][j] == '6') printf(CYAN " " RESET);
            if (canvass[i][j] == '7') printf(WHITE " " RESET);
            if (canvass[i][j] == '.') printf(" ");
        }
        printf("\n");
    }
}

/*
 * VALIDATIONS
 */

int validationValidCharacter (int i) {
    if (i == 46 || (i >= 48 && i <= 55 )) {
        return 1;
    } else {
        fprintf(stderr,"Error: wrong pixel value %c\n", i);
        fprintf(stderr,  "%s", USAGE);
        exit(1);
    }
}

int validationInsideCanvass (int X, int Y) {
    if (0 <= X && X < height && 0 <= Y && Y < width) {
        return 1;
    }
    return 0;
}

void validCommandLineSyntax (int argc, char *argv[]) {

    int argPosition = 1;
    while (argc > argPosition) {
        int j = (int)strlen(argv[argPosition]);
        for (int i = 0; i < j; i++){
            if (argv[argPosition][i] == '<') {
                break;
            }

            if (isalpha(argv[argPosition][i])) {
                if (i + 1< j ) {
                    if (!(isalpha(argv[argPosition][i + 1] || argv[argPosition][i] == ','))) {
                        fprintf(stderr, "Error: incorrect value with option %s\n", argv[argPosition - 1]);
                        fprintf(stderr, "%s", USAGE);
                        exit(7 );
                    }
                }
            }
        }
        argPosition++;
    }
}

void calculateCoodinates(char *argv[], int* x, int* y, int* rectHeight, int* rectWidth) {

    char *str = argv[0];
    char *ptr;

    char *token = strtok(str, ",");
    *x = strtol(token, &ptr, 10);
    token = strtok(NULL, ",");
    *y = strtol(token, &ptr, 10);
    token = strtok(NULL, ",");
    *rectHeight = strtol(token, &ptr, 10);
    token = strtok(NULL, ",");
    *rectWidth = strtol(token, &ptr, 10);
    token = strtok(NULL, ",");
}

void validCommandLineOptions (int argc, char *argv[]) {
    int argPosition = 1;
    while (argc > argPosition) {
        int j = (int)strlen(argv[argPosition]);
        for (int i = 0; i < j; i++){
            if (isalpha(argv[argPosition][i])) {
                if (argv[argPosition][0] == '-') {
                    if (argv[argPosition][1] == 'h' || argv[argPosition][1] == 'v' || argv[argPosition][1] == 'r' || argv[argPosition][1] == 'n' ||
                        argv[argPosition][1] == 'l' || argv[argPosition][1] == 'c' || argv[argPosition][1] == 'p'||  argv[argPosition][1] == 'k' || argv[argPosition][1] == 's') {
                    } else {
                        fprintf(stderr, "Error: unrecognized option %s\n", argv[argPosition]);
                        fprintf(stderr, "%s", USAGE);
                        exit(5);
                    }
                }
            }
        }
        argPosition++;
    }
}

int main(int argc, char *argv[]) {

    char *ptr;
    int nFlag = 0;

    if (argc < 2) {
        fprintf(stderr, "%s", USAGE);
        exit(0);}
    validCommandLineOptions(argc, argv);
    validCommandLineSyntax(argc, argv);
    if (strcmp("-n", argv[1]) == 0) {
        width = getWidthNFlag(argc, argv);
        height = getHeightNFlag(argc, argv);\
        if (width < 0 || width > MAXWIDTH || height < 0 || height > MAXHEIGHT) {
            fprintf(stderr,"Error: incorrect value with option -n\n");
            fprintf(stderr, "%s", USAGE);
            exit(7);
        }
    } else {
        width =  getWidthSFlag();
        height = getHeightSFlag();
    }

    char canvass[height][width];
    char (*pointerCanvass)[width] = canvass;
    int argvPosition = 1;
    if (strcmp("-n", argv[argvPosition]) == 0) {
        createBlankCanvass(canvass);
        nFlag = 1;
        argvPosition++;
    } else {
        readCanvassFile(canvass);
    }
    while (argvPosition < argc) {
        if (strcmp("-s", argv[argvPosition]) == 0 && !nFlag) {
            break;
        } else if (strcmp("-h", argv[argvPosition]) == 0) {
            drawHorizontalLine(pointerCanvass, strtol(argv[argvPosition + 1], &ptr, 10));
            argvPosition++;
        } else if (strcmp("-v", argv[argvPosition]) == 0) {
            drawVerticalLine(pointerCanvass, strtol(argv[argvPosition+1], &ptr, 10));
            argvPosition++;
        } else if (strcmp("-r", argv[argvPosition]) == 0) {
            drawRectangle(pointerCanvass, &argv[argvPosition+1]);
            argvPosition++;
        } else if (strcmp("-l", argv[argvPosition]) == 0) {
            drawLine(pointerCanvass, &argv[argvPosition + 1]);
            argvPosition++;
        } else if (strcmp("-c", argv[argvPosition]) == 0) {
            drawCircle(pointerCanvass, &argv[argvPosition + 1]);
            argvPosition++;
        } else if (strcmp("-p", argv[argvPosition]) == 0) {
            changeCrayon(argv[argvPosition +1][0]);
            argvPosition++;
        } else if (strcmp("-k", argv[argvPosition]) == 0) {
            printColoredCanvass(canvass);
        }
        argvPosition++;
    }
    printCanvass(canvass);
//    printf("\nargc = %d\n", argc);
//    for (unsigned int i = 0; i < argc; ++i) {
//        printf("argv[%d] = %s\n", i, argv[i]);
//    }

    return 0;
}

