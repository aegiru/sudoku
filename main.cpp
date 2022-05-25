#include <iostream>
#include <chrono>
#include <cmath>
#include <conio.h>
#include <Windows.h>

using namespace std;

const int globalSize = 9;
const int globalBoxSize = sqrt(globalSize);
const int displaySize = 3;

struct difficulty {
    int maxEmptyValues;
    int minEmptyValues;
    char diffName[10];
};

struct settingsStruct {
    char thickHorizontal = '=';
    char thickVertical = (char) 186;
    char pointerChar = '*';
    char emptySpace = ' ';
    char wrongSpace = '!';
    char safeSpace = '.';
    char crossSection = '+';
    char horizontalLine = '-';
    char verticalLine = '|';
    bool remainingNumbersEnabled = true;
    bool showWrongNumbers = true;
    int offsetY = 5;
    int offsetX = 5;
    int defaultColor = 15;
    int gridColor = 11;
    int safeNumberColor = 14;
    int wrongNumberColor = 12;
    int pointerColor = 13;
    int goodColor = 10;
    int spaceBetweenGridAndRemainingNumbers = 5;
};

void hidecursor() {
    CONSOLE_CURSOR_INFO info;
    info.bVisible = false;
    info.dwSize = 100;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void gotoxy(int x, int y) {
    COORD coordinate;

    coordinate.X = x;
    coordinate.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
}

void setColor(int n) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n);
}

void nextLine() {
    cout << endl;
}

void shuffle(int (*possibilities), int length) {
    int help = 0;
    for (int i = 0; i < length; i++) {
        int toSwap = rand() % length;
        help = possibilities[toSwap];
        possibilities[toSwap] = possibilities[i];
        possibilities[i] = help;
    }
}

bool findEmptySlot(int(**grid), int &y, int &x, int gridSize) {
    for (y = 0; y < gridSize; y++) {
        for (x = 0; x < gridSize; x++) {
            if (grid[y][x] == 0) {
                return true;
            }
        }
    }

    return false;
}

bool usedOnY(int(**grid), int y, int i, int gridSize) {
    for (int x = 0; x < gridSize; x++) {
        if (grid[y][x] == i) {
            return true;
        }
    }

    return false;
}

bool usedOnX(int(**grid), int x, int i, int gridSize) {
    for (int y = 0; y < gridSize; y++) {
        if (grid[y][x] == i) {
            return true;
        }
    }

    return false;
}

bool usedInBox(int(**grid), int y, int x, int k, int gridSize, int boxSize) {
    int xS = (x / boxSize) * (boxSize);
    int yS = (y / boxSize) * (boxSize);
    for (int i = 0; i < (gridSize / boxSize); i++) {
        for (int j = 0; j < (gridSize / boxSize); j++) {
            if (grid[i + yS][j + xS] == k) {
                return true;
            }
        }
    }

    return false;
}

bool isSafe(int(**grid), int y, int x, int i, int gridSize, int boxSize) {
    return !usedOnY(grid, y, i, gridSize) && !usedOnX(grid, x, i, gridSize) && !usedInBox(grid, y, x, i, gridSize, boxSize);
}

bool solveGrid(int(**grid), int (*possibilities), int gridSize, int boxSize) {
    int y, x;

    if (!findEmptySlot(grid, y, x, gridSize)) {
        return true;
    }

    for (int i = 0; i < gridSize; i++) {
        if (isSafe(grid, y, x, possibilities[i], gridSize, boxSize)) {
            grid[y][x] = possibilities[i];

            if (solveGrid(grid, possibilities, gridSize, boxSize)) {
                return true;
            }

            grid[y][x] = 0;
        }
    }

    return false;
}

void countSolutions(int(**grid), int (*possibilities), int &i, int gridSize, int boxSize) {
    int y, x;

    if (!findEmptySlot(grid, y, x, gridSize)) {
        i++;
        return;
    }

    for (int j = 0; j < gridSize && i < 2; j++) {
        if (isSafe(grid, y, x, possibilities[j], gridSize, boxSize)) {
            grid[y][x] = possibilities[j];
            countSolutions(grid, possibilities, i, gridSize, boxSize);
        }

        grid[y][x] = 0;
    }
}

void generatePuzzle(int(**grid), int(*possibilities), const int(*positions), int gridSize, int boxSize, int goalEmptyPlaces) {
    int currentEmptyPlaces = 0;
    for (int i = 0; i < (gridSize * gridSize); i++) {
        int x = (positions[i]) / gridSize;
        int y = (positions[i]) % gridSize;

        int temp = grid[y][x];
        grid[y][x] = 0;

        int check = 0;
        countSolutions(grid, possibilities, check, gridSize, boxSize);
        currentEmptyPlaces++;
        if (check != 1) {
            grid[y][x] = temp;
            currentEmptyPlaces--;
        }
        if (goalEmptyPlaces == currentEmptyPlaces) {
            return;
        }
    }
}

void printWin() {
    nextLine();
    nextLine();
    cout << "Congration, you done it." << endl;
}

void fillRandomPosition(int(**inputGrid), int(**compareGrid), const int(*positions), int (*remainingNumbers), int gridSize) {
    for (int i = 0; i < (gridSize * gridSize); i++) {
        int x = (positions[i]) / gridSize;
        int y = (positions[i]) % gridSize;

        if (inputGrid[y][x] == 0) {
            inputGrid[y][x] = compareGrid[y][x];
            remainingNumbers[compareGrid[y][x] - 1]--;
            return;
        }
    }
}

void drawLine(int gridSize, int displayBoxSize, int constantCoordinate, settingsStruct settings, int additionalOffsetX, int additionalOffsetY, bool trueIfVerticalFalseIfHorizontal, bool clear, bool doThickLines) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j <= (displayBoxSize + 1); j++) {
            int movingCoordinate = (i * (displayBoxSize + 1)) + j;
            if (trueIfVerticalFalseIfHorizontal) {
                gotoxy(constantCoordinate + settings.offsetX + additionalOffsetX, movingCoordinate + settings.offsetY + additionalOffsetY);
            } else {
                gotoxy(movingCoordinate + settings.offsetX + additionalOffsetX, constantCoordinate + settings.offsetY + additionalOffsetY);
            }
            if (j % (displayBoxSize + 1) == 0) {
                if (clear) {
                    cout << settings.emptySpace;
                } else {
                    setColor(settings.gridColor);
                    cout << settings.crossSection;
                }
            } else {
                if (trueIfVerticalFalseIfHorizontal) {
                    if (clear) {
                        cout << settings.emptySpace;
                    } else {
                        setColor(settings.gridColor);
                        if (doThickLines) {
                            cout << settings.thickVertical;
                        } else {
                            cout << settings.verticalLine;
                        }

                    }
                } else {
                    if (clear) {
                        cout << settings.emptySpace;
                    } else {
                        setColor(settings.gridColor);
                        if (doThickLines) {
                            cout << settings.thickHorizontal;
                        } else {
                            cout << settings.horizontalLine;
                        }
                    }
                }
            }
        }
    }
}

void emptyGrid(int gridSize, int displayBoxSize, settingsStruct settings, bool clear) {
    for (int y = 0; y <= gridSize; y++) {
        bool thickLines = false;
        int coordY = y * (displayBoxSize + 1);
        if (y % 3 == 0) {
            thickLines = true;
        }
        drawLine(gridSize, displayBoxSize, coordY, settings, 0, 0, false, clear, thickLines);
    }
    for (int x = 0; x <= gridSize; x++) {
        bool thickLines = false;
        int coordX = x * (displayBoxSize + 1);
        if (x % 3 == 0) {
            thickLines = true;
        }
        drawLine(gridSize, displayBoxSize, coordX, settings, 0, 0, true, clear, thickLines);
    }
}

void printNumbers(int(**inputGrid), int (**compareGrid), int (**safeGrid), int gridSize, int displayBoxSize, settingsStruct settings, bool clear) {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            for (int yPlace = 0; yPlace < (displayBoxSize); yPlace++) {
                for (int xPlace = 0; xPlace < displayBoxSize; xPlace++) {
                    if (yPlace == (displayBoxSize / 2) + 1 && xPlace == (displayBoxSize / 2) + 1) {
                        gotoxy((x * (displayBoxSize + 1)) + xPlace + settings.offsetX, (y * (displayBoxSize + 1)) + yPlace + settings.offsetY);
                        if (clear) {
                            cout << settings.emptySpace;
                        } else {
                            if (inputGrid[y][x] != 0) {
                                if (safeGrid[y][x] == 1) {
                                    setColor(settings.safeNumberColor);
                                } else if (settings.showWrongNumbers && (inputGrid[y][x] != compareGrid[y][x])) {
                                    setColor(settings.wrongNumberColor);
                                } else {
                                    setColor(settings.goodColor);
                                }
                                cout << inputGrid[y][x];
                                setColor(settings.defaultColor);
                            } else {
                                cout << settings.emptySpace;
                            }
                        }
                    }
                }
            }
        }
    }
}

void printDefault(int(**safeGrid), int gridSize, int displayBoxSize, settingsStruct settings, bool clear) {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            for (int yPlace = 1; yPlace <= (displayBoxSize); yPlace++) {
                for (int xPlace = 1; xPlace <= displayBoxSize; xPlace++) {
                    if ((yPlace == (displayBoxSize / 2) + 1 || xPlace == (displayBoxSize / 2) + 1) && (yPlace != xPlace)) {
                        gotoxy((x * (displayBoxSize + 1)) + xPlace + settings.offsetX, (y * (displayBoxSize + 1)) + yPlace + settings.offsetY);
                        if (clear) {
                            cout << settings.emptySpace;
                        } else {
                            if (safeGrid[y][x] != 0) {
                                setColor(settings.safeNumberColor);
                                cout << settings.safeSpace;
                            }
                        }
                    }
                }
            }
        }
    }
}

void printPointer(int pointerX, int pointerY, int gridSize, int displayBoxSize, settingsStruct settings, bool clear) {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            for (int yPlace = 1; yPlace <= (displayBoxSize); yPlace++) {
                for (int xPlace = 1; xPlace <= displayBoxSize; xPlace++) {
                    if ((yPlace != (displayBoxSize / 2) + 1 && xPlace != (displayBoxSize / 2) + 1)) {
                        gotoxy((x * (displayBoxSize + 1)) + xPlace + settings.offsetX, (y * (displayBoxSize + 1)) + yPlace + settings.offsetY);
                        if (clear) {
                            cout << settings.emptySpace;
                        } else {
                            if (pointerY == y && pointerX == x) {
                                setColor(settings.pointerColor);
                                cout << settings.pointerChar;
                            } else {
                                cout << settings.emptySpace;
                            }
                        }
                    }
                }
            }
        }
    }
}

void printWrongYourOpinionIsDong(int(**inputGrid), int(**compareGrid), int(**safeGrid), int gridSize, int displayBoxSize, settingsStruct settings, bool clear) {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            for (int yPlace = 1; yPlace <= (displayBoxSize); yPlace++) {
                for (int xPlace = 1; xPlace <= displayBoxSize; xPlace++) {
                    if (safeGrid[y][x] != 1) {
                        if ((yPlace == (displayBoxSize / 2) + 1 || xPlace == (displayBoxSize / 2) + 1) && (yPlace != xPlace)) {
                            gotoxy((x * (displayBoxSize + 1)) + xPlace + settings.offsetX, (y * (displayBoxSize + 1)) + yPlace + settings.offsetY);
                            if (clear) {
                                cout << settings.emptySpace;
                            } else {
                                if (inputGrid[y][x] != compareGrid[y][x]) {
                                    if (inputGrid[y][x] != 0) {
                                        setColor(settings.wrongNumberColor);
                                        cout << settings.wrongSpace;
                                    }
                                } else {
                                    cout << settings.emptySpace;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void printRemainingNumberGrid(int gridSize, int displayBoxSize, settingsStruct settings) {
    int offsetX = settings.spaceBetweenGridAndRemainingNumbers + (gridSize * (displayBoxSize + 1));
    for (int y = 0; y <= 9; y++) {
        int coordY = y * (displayBoxSize + 1);
        drawLine(2, displayBoxSize, coordY, settings, offsetX, 0, false, false, false);
    }

    for (int x = 0; x <= 2; x++) {
        int coordX = x * (displayBoxSize + 1) + offsetX;
        drawLine(gridSize, displayBoxSize, coordX, settings, 0, 0, true, false, false);
    }
}

void printRemainingNumbers(int(*remainingNumbers), int gridSize, int displayBoxSize, settingsStruct settings) {
    int offsetX = settings.spaceBetweenGridAndRemainingNumbers + (gridSize * (displayBoxSize + 1));
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < 2; x++) {
            for (int yPlace = 0; yPlace < (displayBoxSize); yPlace++) {
                for (int xPlace = 0; xPlace < displayBoxSize; xPlace++) {
                    if (yPlace == (displayBoxSize / 2) + 1 && xPlace == (displayBoxSize / 2) + 1) {
                        gotoxy((x * (displayBoxSize + 1)) + xPlace + settings.offsetX + offsetX, (y * (displayBoxSize + 1)) + yPlace + settings.offsetY);
                        if (x == 0) {
                            setColor(settings.safeNumberColor);
                            cout << y + 1;
                        } else {
                            setColor(settings.goodColor);
                            if (remainingNumbers[y] >= 0) {
                                cout << remainingNumbers[y];
                            } else {
                                cout << 0;
                            }

                        }
                    }
                }
            }
        }
    }
}

void fullPrintRemainingNumbers(int(*remainingNumbers), int gridSize, int displayBoxSize, settingsStruct settings) {
    printRemainingNumberGrid(gridSize, displayBoxSize, settings);

    printRemainingNumbers(remainingNumbers, gridSize, displayBoxSize, settings);
}

void initializeGrid(int gridSize, int displayBoxSize, settingsStruct settings, bool clear) {
    emptyGrid(gridSize, displayBoxSize, settings, clear);
}

void printGrid(int(**inputGrid), int(**compareGrid), int(**safeGrid), int(*remainingNumbers), int gridSize, int displayBoxSize, int pointerX, int pointerY, settingsStruct settings, bool clear) {
    printNumbers(inputGrid, compareGrid, safeGrid, gridSize, displayBoxSize, settings, clear);
    printDefault(safeGrid, gridSize, displayBoxSize, settings, clear);
    printPointer(pointerX, pointerY, gridSize, displayBoxSize, settings, clear);
    if (settings.showWrongNumbers) {
        printWrongYourOpinionIsDong(inputGrid, compareGrid, safeGrid, gridSize, displayBoxSize, settings, clear);
    }
    if (settings.remainingNumbersEnabled) {
        fullPrintRemainingNumbers(remainingNumbers, gridSize, displayBoxSize, settings);
    }
}

bool checkWin(int (**inputGrid), int (**compareGrid), int gridSize) {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (inputGrid[y][x] != compareGrid[y][x]) {
                return false;
            }
        }
    }
    return true;
}

int menuSelector(difficulty (*difficulties), int numberOfDifficulties) {
    int selection = 0;
    int offsetX = 5;
    int offsetY = 3;
    gotoxy(offsetX, offsetY - 1);
    cout << "Difficulty Name | Number Of Empty Spaces";
    while (true) {
        for (int i = 0; i < numberOfDifficulties; i++) {
            if (i == selection) {
                setColor(10);
            } else {
                setColor(15);
            }
            gotoxy(offsetX, i + offsetY);
            cout << i + 1 << ". " << difficulties[i].diffName << " (" << difficulties[i].minEmptyValues << " - " << difficulties[i].maxEmptyValues << ")" << endl;
        }
        char ch = _getch();
        switch(ch) {
            case 'w':
                if (selection != 0) {
                    selection--;
                }
                break;
            case 's':
                if (selection != numberOfDifficulties - 1) {
                    selection++;
                }
                break;
            case 13:
                return selection;
        }
    }
}

int seedMenu() {
    int selection = 0;
    int offsetX = 5;
    int offsetY = 3;
    gotoxy(offsetX, offsetY - 1);
    cout << "Would you like to use a random seed or choose a specific one?";
    while (true) {
        for (int i = 0; i < 2; i++) {
            if (i == selection) {
                setColor(10);
            } else {
                setColor(15);
            }
            gotoxy(offsetX, i + offsetY);
            if (i == 0) {
                cout << i + 1 << ". Use a randomised seed.";
            } else {
                cout << i + 1 << ". Choose a specific seed.";
            }
        }
        char ch = _getch();
        switch(ch) {
            case 'w':
                if (selection != 0) {
                    selection--;
                }
                break;
            case 's':
                if (selection != 1) {
                    selection++;
                }
                break;
            case 13:
                if (selection == 0) {
                    return time(nullptr);
                } else {
                    system("cls");
                    gotoxy(offsetX, offsetY - 1);
                    cout << "Input your desired seed: ";
                    setColor(15);
                    int seed = 0;
                    cin >> seed;
                    return seed;
                }
        }
    }
}

int main() {
    hidecursor();

    int seed = seedMenu();
    srand(seed);

    const int numberOfDifficulties = 5;
    difficulty difficulties[numberOfDifficulties];
    strcpy_s(difficulties[0].diffName, "Easy");
    difficulties[0].maxEmptyValues = 35;
    difficulties[0].minEmptyValues = 22;
    strcpy_s(difficulties[1].diffName, "Medium");
    difficulties[1].maxEmptyValues = 47;
    difficulties[1].minEmptyValues = 36;
    strcpy_s(difficulties[2].diffName, "Hard");
    difficulties[2].maxEmptyValues = 49;
    difficulties[2].minEmptyValues = 46;
    strcpy_s(difficulties[3].diffName, "Difficult");
    difficulties[3].maxEmptyValues = 53;
    difficulties[3].minEmptyValues = 50;
    strcpy_s(difficulties[4].diffName, "Evil");
    difficulties[4].maxEmptyValues = 64;
    difficulties[4].minEmptyValues = 54;

    int difficultyNumber = menuSelector(difficulties, numberOfDifficulties);
    system("cls");

    int numberOfPossibleSolutions = (rand() % (difficulties[difficultyNumber].maxEmptyValues + 1 - difficulties[difficultyNumber].minEmptyValues)) + difficulties[difficultyNumber].minEmptyValues;

    int gridSize = globalSize;
    int boxSize = globalBoxSize;
    int displayBoxSize = displaySize;
    auto settings = new settingsStruct;
    auto start = chrono::high_resolution_clock::now();
    auto solvedGrid = (int **) malloc(gridSize * sizeof(int *));
    auto readyGrid = (int **) malloc(gridSize * sizeof(int *));
    auto savedGrid = (int **) malloc(gridSize * sizeof(int *));
    auto possibilities = (int *) malloc(gridSize * sizeof(int));
    auto randomisedPositions = (int *) malloc((gridSize * gridSize) * sizeof(int));
    auto remainingNumbers = (int *) malloc(gridSize * sizeof(int));
    int pointerY = gridSize / 2;
    int pointerX = gridSize / 2;
    bool toContinue = true;



    for (int i = 0; i < gridSize; i++) {
        solvedGrid[i] = (int *) malloc(gridSize * sizeof(int));
        readyGrid[i] = (int *) malloc(gridSize * sizeof(int));
        savedGrid[i] = (int *) malloc(gridSize * sizeof(int));
        possibilities[i] = i + 1;
        remainingNumbers[i] = 9;
        for (int j = 0; j < gridSize; j++) {
            solvedGrid[i][j] = 0;
            readyGrid[i][j] = 0;
            savedGrid[i][j] = 0;
            randomisedPositions[(i * gridSize) + j] = (i * gridSize) + j;
        }
    }

    shuffle(possibilities, gridSize);
    solveGrid(readyGrid, possibilities, gridSize, boxSize);

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            solvedGrid[i][j] = readyGrid[i][j];
        }
    }

    shuffle(randomisedPositions, (gridSize * gridSize));

    generatePuzzle(readyGrid, possibilities, randomisedPositions, gridSize, boxSize, numberOfPossibleSolutions);

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (readyGrid[i][j] != 0) {
                remainingNumbers[solvedGrid[i][j] - 1]--;
                savedGrid[i][j] = 1;
            }
        }
    }

    shuffle(randomisedPositions, (gridSize * gridSize));

    initializeGrid(gridSize, displayBoxSize, *settings, false);

    while (toContinue) {
        printGrid(readyGrid, solvedGrid, savedGrid, remainingNumbers, gridSize, displayBoxSize, pointerX, pointerY, *settings, false);
        if (!checkWin(readyGrid, solvedGrid, gridSize)) {
            char ch = _getch();
            switch (ch) {
                case 'q':
                    toContinue = false;
                    break;
                case 'w':
                    if (pointerY != 0) {
                        pointerY--;
                    }
                    break;
                case 's':
                    if (pointerY != 8) {
                        pointerY++;
                    }
                    break;
                case 'a':
                    if (pointerX != 0) {
                        pointerX--;
                    }
                    break;
                case 'd':
                    if (pointerX != 8) {
                        pointerX++;
                    }
                    break;
                case 'r':
                    if (savedGrid[pointerY][pointerX] == 0) {
                        fillRandomPosition(readyGrid, solvedGrid, randomisedPositions, remainingNumbers, gridSize);
                    }
                    break;
                default:
                    if (ch >= '1' && ch <= '9') {
                        if (savedGrid[pointerY][pointerX] == 0) {
                            int number = (int) ch - 48;
                            if (readyGrid[pointerY][pointerX] != 0) {
                                remainingNumbers[readyGrid[pointerY][pointerX] - 1]++;
                            }
                            readyGrid[pointerY][pointerX] = number;
                            remainingNumbers[number - 1]--;
                        }
                    }
                    break;
            }
        } else {
            printWin();
            toContinue = false;
        }
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << endl << duration.count() << endl;

    system("pause");

    for (int i = 0; i < gridSize; i++) {
        delete[] solvedGrid[i];
        delete[] readyGrid[i];
    }
    delete[] solvedGrid;
    delete[] readyGrid;
    delete[] possibilities;
    delete[] randomisedPositions;
    delete[] remainingNumbers;

    return 0;
}