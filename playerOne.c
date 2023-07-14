#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <Windows.h>
#include "units.h"
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#define UNIT_NAME_LETTERS {'K', 'S', 'A', 'P', 'C', 'R', 'W'}
#define UNIT_NAME_LETTERS_COUNT 7
#define MAX_COMMAND_NUMBER 100

UnitData getKnightData() {
    UnitData knight;
    knight.durability = 70;
    knight.speed = 5;
    knight.cost = 400;
    knight.attackRange = 1;
    knight.buildTime = 5;
    return knight;
}

UnitData getSwordsmanData() {
    UnitData swordsman;
    swordsman.durability = 60;
    swordsman.speed = 2;
    swordsman.cost = 250;
    swordsman.attackRange = 1;
    swordsman.buildTime = 3;
    return swordsman;
}

UnitData getArcherData() {
    UnitData archer;
    archer.durability = 40;
    archer.speed = 2;
    archer.cost = 250;
    archer.attackRange = 5;
    archer.buildTime = 3;
    return archer;
}

UnitData getPikemanData() {
    UnitData pikeman;
    pikeman.durability = 50;
    pikeman.speed = 2;
    pikeman.cost = 200;
    pikeman.attackRange = 2;
    pikeman.buildTime = 3;
    return pikeman;
}

UnitData getRamData() {
    UnitData ram;
    ram.durability = 90;
    ram.speed = 2;
    ram.cost = 500;
    ram.attackRange = 1;
    ram.buildTime = 4;
    return ram;
}

UnitData getCatapultData() {
    UnitData catapult;
    catapult.durability = 50;
    catapult.speed = 2;
    catapult.cost = 800;
    catapult.attackRange = 7;
    catapult.buildTime = 6;
    return catapult;
}

UnitData getWorkerData() {
    UnitData worker;
    worker.durability = 20;
    worker.speed = 2;
    worker.cost = 100;
    worker.attackRange = 1;
    worker.buildTime = 2;
    return worker;
}

UnitData getBaseData() {
    UnitData base;
    base.durability = 200;
    base.speed = 0;
    base.cost = 0;  // Not applicable for a base
    base.attackRange = 0;  // Not applicable for a base
    base.buildTime = -1;  // Not applicable for a base
    return base;
}



#define MAX_UNITS 1000

typedef struct {
    char affiliation;
    char type;
    int ID;
    int Xcord;
    int Ycord;
    int health;
    char producedUnit;
} Unit;

struct GameStatus {
    long gold;
    Unit* units;
    int numUnits;
};

Unit convertTextFileRowToUnit(char* unitRow) {
  Unit unit;

    // Split the input string into separate tokens
    char* token = strtok(unitRow, " ");
    int count = 0;

    // Process each token and assign values to the struct members
    while (token != NULL) {
        switch (count) {
            case 0:
                unit.affiliation = token[0];
                break;
            case 1:
                unit.type = token[0];
                break;
            case 2:
                unit.ID = atoi(token);
                break;
            case 3:
                unit.Xcord = atoi(token);
                break;
            case 4:
                unit.Ycord = atoi(token);
                break;
            case 5:
                unit.health = atoi(token);
                break;
            case 6:
                unit.producedUnit = token[0]; // assign token when Unit is a base, or assign 0 when it's a unit
                break;
        }
        token = strtok(NULL, " ");
        count++;
    };
    printf("Player One unit IDs: %d\n", unit.ID);

    return unit;
}


// mape liczymy od 0 (w przypadku przykladu od 0 do 31)

int** readDataFromMapFile(const char* filename, int* numRows, int* numCols) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return NULL;
    }

    // Count the number of lines and characters in the file
    int lines = 0;
    int chars = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        } else {
            chars++;
        }
    }

    // Calculate the dimensions of the data
    *numRows = lines + 1;  // Add 1 to include the last line without a newline
    *numCols = chars / *numRows;  // Assumes all lines have the same number of characters

    // Allocate memory for the two-dimensional array
    int** dataArray = (int**)malloc(*numRows * sizeof(int*));
    for (int i = 0; i < *numRows; i++) {
        dataArray[i] = (int*)malloc(*numCols * sizeof(int));
    }

    // Rewind the file pointer to the beginning
    rewind(file);

    // Read the data from the file and store it in the array
    int row = 0, col = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            row++;
            col = 0;
        } else if (isdigit(ch)) {
            dataArray[row][col] = ch - '0';  // Store the character value directly
            col++;
        }
    }

    fclose(file);
    return dataArray;
}

void freeDataArray(int** dataArray, int numRows) {
    for (int i = 0; i < numRows; i++) {
        free(dataArray[i]);
    }
    free(dataArray);
}

struct GameStatus readGameStatus(char* statusFileName) {
    struct GameStatus gameStatus;
    
    // open file for read
    FILE* file = fopen(statusFileName, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return gameStatus;
    }

    // Count number of rows for array-size allocation
    char c;
    int count = 0;

    while(c != EOF){
        c = getc(file);
        if (c == '\n') {
            count++; // Increment count if this character is newline
        }
    }
    count++; // account for the EOF character

    printf("%s File has %d rows\n", statusFileName, count);

    if (count - 1 <= 0) {
        printf("No units found in the file.\n");
        fclose(file);
        return gameStatus;
    }

    char rowFromFile[50];
    char arrOfRows[count][50];
    count = 0;

    rewind(file);
    while(fgets(rowFromFile, sizeof(rowFromFile), file)) {
        strcpy(arrOfRows[count], rowFromFile);
        count++;
        printf("%s\n", rowFromFile);
    }
    fclose(file);

    long gold = (long) atoi(arrOfRows[0]); // Gold taking up first row in the file

    Unit* units = malloc(sizeof(Unit) * (count - 1));

    for (int i = 1; i < count; i++) {
        units[i - 1] = convertTextFileRowToUnit(arrOfRows[i]);
    }

    // Assign gathered data
    gameStatus.gold = gold;
    gameStatus.numUnits = count - 1;
    gameStatus.units = units;

    return gameStatus;
}

int freeAllocatedMapMemory(int** dataArray, int numRows, int numCols) {
     if (dataArray != NULL) {
        // Print the data array
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                printf("%d ", dataArray[i][j]);
            }
            printf("\n");
        }
        
        // Free the allocated memory
        freeDataArray(dataArray, numRows);
    }
}

void freeAllocatedGameStatusMemory(struct GameStatus* gameStatus) {
    free(gameStatus->units);
}

typedef struct {
    int x;
    int y;
} Point;

typedef struct Node {
    int x;
    int y;
    int f, g, h;
    bool visited;
    struct Node* parent;
} Node;


bool isValid(int x, int y, int numRows, int numCols) {
    return (x >= 0 && x < numRows && y >= 0 && y < numCols);
}

bool isObstacle(int** grid, int x, int y) {
    return (grid[x][y] == 9 || grid[x][y] == 6);
}

bool isDestination(int** grid, int x, int y) {
    return (grid[x][y] == 2);
}

int calculateH(int x, int y, int destX, int destY) {
    return abs(destX - x) + abs(destY - y);
}

void findShortestPath(int** grid, Point start, Point dest, int numRows, int numCols, Point** path, int* pathLength) {
    int startX = start.x;
    int startY = start.y;
    int destX = dest.x;
    int destY = dest.y;

    if (!isValid(startX, startY, numRows, numCols) || !isValid(destX, destY, numRows, numCols)) {
        printf("Invalid start or destination point.\n");
        return;
    }

    if (isObstacle(grid, startX, startY) || isObstacle(grid, destX, destY)) {
        printf("Start or destination point is an obstacle.\n");
        return;
    }

    if (isDestination(grid, startX, startY)) {
        printf("You are already at the destination.\n");
        return;
    }

    bool closedList[numRows][numCols];
    memset(closedList, false, sizeof(closedList));

    Node nodes[numRows][numCols];
    memset(nodes, 0, sizeof(nodes));

    int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    Node* startNode = &nodes[startX][startY];
    startNode->x = startX;
    startNode->y = startY;
    startNode->f = 0;
    startNode->g = 0;
    startNode->h = 0;
    startNode->visited = true;
    startNode->parent = NULL;

    Node* currentNode;

    while (true) {
        int minF = INT_MAX;

        // Find the node with the lowest f value
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (nodes[i][j].visited && !closedList[i][j] && nodes[i][j].f < minF) {
                    minF = nodes[i][j].f;
                    currentNode = &nodes[i][j];
                }
            }
        }

        if (minF == INT_MAX) {
            printf("Path not found.\n");
            return;
        }

        // Mark the current node as visited
        closedList[currentNode->x][currentNode->y] = true;

        if (currentNode->x == destX && currentNode->y == destY) {
            // Path found
            printf("Path found.\n");

            // Count the number of nodes in the path
            int count = 0;
            Node* tempNode = currentNode;
            while (tempNode != NULL) {
                count++;
                tempNode = tempNode->parent;
            }

            // Allocate memory for the path array
            *path = (Point*)malloc(count * sizeof(Point));
            *pathLength = count;

            // Store the path nodes in reverse order
            int index = count - 1;
            tempNode = currentNode;
            while (tempNode != NULL) {
                (*path)[index].x = tempNode->x;
                (*path)[index].y = tempNode->y;
                index--;
                tempNode = tempNode->parent;
            }

            return;
        }

        // Explore neighbors
        for (int i = 0; i < 8; i++) {
            int newX = currentNode->x + dx[i];
            int newY = currentNode->y + dy[i];

            if (isValid(newX, newY, numRows, numCols) && !isObstacle(grid, newX, newY) && !closedList[newX][newY]) {
                Node* neighbor = &nodes[newX][newY];

                int newG = currentNode->g + 1;
                int newH = calculateH(newX, newY, destX, destY);
                int newF = newG + newH;

                if (!neighbor->visited || newF < neighbor->f) {
                    neighbor->x = newX;
                    neighbor->y = newY;
                    neighbor->f = newF;
                    neighbor->g = newG;
                    neighbor->h = newH;
                    neighbor->visited = true;
                    neighbor->parent = currentNode;
                }
            }
        }
    }
}


Unit getAllyBaseData(struct GameStatus gameStatus) {
    Unit base;
    base.affiliation = '0'; // Set affiliation to an invalid value as a fallback

    for (int i = 0; i < gameStatus.numUnits; i++) {
        if (gameStatus.units[i].type == 'B' && gameStatus.units[i].affiliation == 'P') {
            return gameStatus.units[i];
        }
    }

    printf("Couldn't find ally base.\n");
    return base;
}

Unit getEnemyBaseData(struct GameStatus gameStatus) {

Unit base; 

    for(int i=0; i<gameStatus.numUnits; i++) {
        if(gameStatus.units[i].type == 'B' && gameStatus.units[i].affiliation == 'E') {
            return gameStatus.units[i];
        }
    }

printf("Couldn't find enemy base.");
return base;

}

void buildUnits() {
    
}

void freeAllocatedCommandStringsMemory(char** stringArray, int arraySize) {
    // Free up the memory for each string in the array
    for (int i = 0; i < arraySize; i++) {
        free(stringArray[i]);
    }
    
    // Free up the memory for the array itself
    free(stringArray);
}

bool allyBaseIsProducingUnit(Unit allyBase) {
    
    if(allyBase.producedUnit == '0') {
        printf("The base is not building anything\n");
    }
}



int generateRandomNumberFromZeroTo(int n) {
        // Seed the random number generator with the current time
    srand(time(NULL));
    
    // Generate a random number between 0 and 7
    int randomNumber = rand() % n;
    
    // Print the randomly generated number
    printf("Random number: %d\n", randomNumber);

    return randomNumber;
}


char* generateCommandBasedOnFile(char* statusFileName, char* mapFileName, char** commandsStrings, int comStringsNum) {


    struct GameStatus gameStatus = readGameStatus(statusFileName);
    
    int numRows, numCols;
    int** dataArray = readDataFromMapFile(mapFileName, &numRows, &numCols);
    Unit allyBase = getAllyBaseData(gameStatus);
    Unit enemyBase = getEnemyBaseData(gameStatus);

    printf("ally base cord: %d %d\n", allyBase.Xcord, allyBase.Ycord);
    printf("enemy base cord: %d %d\n", enemyBase.Xcord, enemyBase.Ycord);
    Point start;
    start.x = allyBase.Xcord;
    start.y = allyBase.Ycord;

    Point target;
    target.x = enemyBase.Xcord;
    target.y = enemyBase.Ycord;

    Point* path; // array of result Points that you can take along the way to destination (base)
    int pathLength;

    findShortestPath(dataArray, start, target, numRows, numCols, &path, &pathLength);

    for(int i=0; i<pathLength; i++) {
        printf("x: %d y: %d\n", path[i].x, path[i].y);
    }
    
    if(!allyBaseIsProducingUnit(allyBase)) { // if no units are being produced, produce a unit
        if(gameStatus.numUnits == 0){
        printf("There are: %d units/n", gameStatus.numUnits);
        } else {

            // Start producing a Unit
            char unitNameLetters[UNIT_NAME_LETTERS_COUNT] = UNIT_NAME_LETTERS;
            
            char randomUnitLetter = unitNameLetters[generateRandomNumberFromZeroTo(6)];
            
                int stringLength = 6;
                char* resultString = malloc(stringLength * sizeof(char));
                sprintf(resultString, "%d %c %c", allyBase.ID, allyBase.type, randomUnitLetter);
    
                // Print the dynamically created string
                printf("Result string: %s\n", resultString);

                commandsStrings[comStringsNum] = resultString;
                comStringsNum++;
        }
    }



    freeAllocatedMapMemory(dataArray, numRows, numCols);
    freeAllocatedGameStatusMemory(&gameStatus);
    
    // open file in read mode

    return commandsStrings[0];

}

int writeCommandIntoFile(char* commandString, char* fileString){

    FILE* file = fopen("rozkazy.txt", "w");  // Open file in write mode

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    // Write text to the file
    fprintf(file, commandString);

    // Close the file
    fclose(file);

    return 0;

}

unsigned long long rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

int main(int argc, char* argv[]) {
    // assign arguments from terminal
    char* mapFileString;
    char* statusFileString;
    char* ordersFileString;
    char* timeoutInSeconds;
    printf("received data: %d %s\n", argc, argv[1]);
    if(argc >= 4) {
    mapFileString = argv[1];
    statusFileString = argv[2];
    ordersFileString = argv[3];
    printf("Received name of file %s\n", argv[3]);
        if(argc == 5) {
        timeoutInSeconds = argv[4];
         }
    } else  if (argc > 5 || argc <= 1){
        printf("Wrong arguments passed, Program One quits");
        return 0;
    }

    printf("Player One is making a move...\n");

   // Sleep(3000);
    printf("Player One is generating a command...\n");
    char** commandsStrings = malloc(MAX_COMMAND_NUMBER * sizeof(char*));
    int comStringsNum = 0;
    char* commandPlayerOne = generateCommandBasedOnFile(statusFileString, mapFileString, commandsStrings, comStringsNum);



    printf("Writing command into a file...\n");
    printf("COMMAND: %s\n", commandPlayerOne);
    
    writeCommandIntoFile(commandPlayerOne, ordersFileString);

        // free allocated space for commands
    freeAllocatedCommandStringsMemory(commandsStrings, comStringsNum); 
    printf("End of turn\n");
    return 0;
}
