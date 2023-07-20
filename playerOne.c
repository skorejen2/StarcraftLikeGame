#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#define UNIT_NAME_LETTERS_COUNT 7
#define MAX_COMMAND_NUMBER 100

#define UNIT_NAME_LETTERS                 \
    {                                     \
        'K', 'S', 'A', 'P', 'C', 'R', 'W' \
    }

typedef struct
{
    int durability;  // Wytrzymałość
    int speed;       // Prędkość
    int cost;        // Koszt zakupu
    int attackRange; // Zasięg ataku
    int buildTime;   // Czas budowania
} UnitData;

UnitData getKnightData()
{
    UnitData knight;
    knight.durability = 70;
    knight.speed = 5;
    knight.cost = 400;
    knight.attackRange = 1;
    knight.buildTime = 5;
    return knight;
}

UnitData getSwordsmanData()
{
    UnitData swordsman;
    swordsman.durability = 60;
    swordsman.speed = 2;
    swordsman.cost = 250;
    swordsman.attackRange = 1;
    swordsman.buildTime = 3;
    return swordsman;
}

UnitData getArcherData()
{
    UnitData archer;
    archer.durability = 40;
    archer.speed = 2;
    archer.cost = 250;
    archer.attackRange = 5;
    archer.buildTime = 3;
    return archer;
}

UnitData getPikemanData()
{
    UnitData pikeman;
    pikeman.durability = 50;
    pikeman.speed = 2;
    pikeman.cost = 200;
    pikeman.attackRange = 2;
    pikeman.buildTime = 3;
    return pikeman;
}

UnitData getRamData()
{
    UnitData ram;
    ram.durability = 90;
    ram.speed = 2;
    ram.cost = 500;
    ram.attackRange = 1;
    ram.buildTime = 4;
    return ram;
}

UnitData getCatapultData()
{
    UnitData catapult;
    catapult.durability = 50;
    catapult.speed = 2;
    catapult.cost = 800;
    catapult.attackRange = 7;
    catapult.buildTime = 6;
    return catapult;
}

UnitData getWorkerData()
{
    UnitData worker;
    worker.durability = 20;
    worker.speed = 2;
    worker.cost = 100;
    worker.attackRange = 1;
    worker.buildTime = 2;
    return worker;
}

UnitData getBaseData()
{
    UnitData base;
    base.durability = 200;
    base.speed = 0;
    base.cost = 0;        // Not applicable for a base
    base.attackRange = 0; // Not applicable for a base
    base.buildTime = -1;  // Not applicable for a base
    return base;
}

// Function to get unit data based on a letter
UnitData getUnitData(char letter)
{
    switch (letter)
    {
    case 'K':
        return getKnightData();
    case 'S':
        return getSwordsmanData();
    case 'A':
        return getArcherData();
    case 'P':
        return getPikemanData();
    case 'C':
        return getCatapultData();
    case 'R':
        return getRamData();
    case 'W':
        return getWorkerData();
    default:
        printf("Invalid unit letter: %c\n", letter);
        UnitData invalidData = {0, 0, 0, 0, 0}; // Default invalid data
        return invalidData;
    }
}

#define MAX_UNITS 1000

typedef struct
{
    char affiliation;
    char type;
    int ID;
    int Xcord;
    int Ycord;
    int health;
    char producedUnit;
} Unit;

struct GameStatus
{
    long gold;
    Unit *units;
    int numUnits;
};

// Function to convert one line from a file into a Unit
Unit convertTextFileRowToUnit(char *unitRow)
{
    Unit unit;

    // Split the input string into separate tokens
    char *token = strtok(unitRow, " ");
    int count = 0;

    // Process each token and assign values to the struct members
    while (token != NULL)
    {
        switch (count)
        {
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
    // printf("Player One unit IDs: %d\n", unit.ID);

    return unit;
}

// mape liczymy od 0 (w przypadku przykladu od 0 do 31)

// Function to read the map and turn it into a 2D array
int **readDataFromMapFile(const char *filename, int *numRows, int *numCols)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file.\n");
        return NULL;
    }

    // Count the number of lines and characters in the file
    int lines = 0;
    int chars = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            lines++;
        }
        else
        {
            chars++;
        }
    }

    // Calculate the dimensions of the data
    *numRows = lines + 1;        // Add 1 to include the last line without a newline
    *numCols = chars / *numRows; // Assumes all lines have the same number of characters

    // Allocate memory for the two-dimensional array
    int **dataArray = (int **)malloc(*numRows * sizeof(int *));
    for (int i = 0; i < *numRows; i++)
    {
        dataArray[i] = (int *)malloc(*numCols * sizeof(int));
    }

    // Rewind the file pointer to the beginning
    rewind(file);

    // Read the data from the file and store it in the array
    int row = 0, col = 0;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            row++;
            col = 0;
        }
        else if (isdigit(ch))
        {
            dataArray[row][col] = ch - '0'; // Store the character value directly
            col++;
        }
    }

    fclose(file);
    return dataArray;
}

void freeDataArray(int **dataArray, int numRows)
{
    for (int i = 0; i < numRows; i++)
    {
        free(dataArray[i]);
    }
    free(dataArray);
}

// Function to read data from status.txt file
// units in gameStatus include all units and bases
struct GameStatus readGameStatus(char *statusFileName)
{
    struct GameStatus gameStatus;

    // open file for read
    FILE *file = fopen(statusFileName, "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return gameStatus;
    }

    // Count number of rows for array-size allocation
    char c;
    int count = 0;

    while (c != EOF)
    {
        c = getc(file);
        if (c == '\n')
        {
            count++; // Increment count if this character is newline
        }
    }
    count++; // account for the EOF character

    if (count - 1 <= 0)
    {
        printf("No units found in the file.\n");
        fclose(file);
        return gameStatus;
    }

    char rowFromFile[50];
    char arrOfRows[count][50];
    count = 0;

    rewind(file);
    while (fgets(rowFromFile, sizeof(rowFromFile), file))
    {
        strcpy(arrOfRows[count], rowFromFile);
        count++;
    }
    fclose(file);

    long gold = (long)atoi(arrOfRows[0]); // Gold taking up first row in the file

    Unit *units = malloc(sizeof(Unit) * (count - 1));

    for (int i = 1; i < count; i++)
    {
        units[i - 1] = convertTextFileRowToUnit(arrOfRows[i]);
    }

    // Assign gathered data
    gameStatus.gold = gold;
    gameStatus.numUnits = count - 1;
    gameStatus.units = units;

    return gameStatus;
}

int freeAllocatedMapMemory(int **dataArray, int numRows)
{
    if (dataArray != NULL)
    {
        freeDataArray(dataArray, numRows);
    }
    return 0; // Return a value indicating success or error status
}

void freeAllocatedGameStatusMemory(struct GameStatus *gameStatus)
{
    free(gameStatus->units);
}

typedef struct
{
    int x;
    int y;
} Point;

typedef struct Node
{
    int x;
    int y;
    int f, g, h;
    bool visited;
    struct Node *parent;
} Node;

// Validating the new point
bool isValid(int x, int y, int numRows, int numCols)
{
    return (x >= 0 && x < numRows && y >= 0 && y < numCols);
}

// Checking if a point is an obstacle (a 9)
bool isObstacle(int **grid, int x, int y)
{
    return (grid[x][y] == 9);
}

// Check if the point is the destination
bool isDestination(int **grid, int x, int y)
{
    return (grid[x][y] == 2);
}

// Calculate variant H of A* algorithm
int calculateH(int x, int y, int destX, int destY)
{
    return abs(destX - x) + abs(destY - y);
}

// Function for finding the shortest path from Point start to Point dest (destination), implementing A* algorithm
void findShortestPath(int **grid, Point start, Point dest, int numRows, int numCols, Point **path, int *pathLength)
{
    int startX = start.x;
    int startY = start.y;
    int destX = dest.x;
    int destY = dest.y;

    if (!isValid(startX, startY, numRows, numCols) || !isValid(destX, destY, numRows, numCols))
    {
        printf("Invalid start or destination point.\n");
        return;
    }

    if (isObstacle(grid, startX, startY) || isObstacle(grid, destX, destY))
    {
        printf("Start or destination point is an obstacle.\n");
        return;
    }

    if (isDestination(grid, startX, startY))
    {
        printf("You are already at the destination.\n");
        return;
    }

    bool closedList[numRows][numCols];
    memset(closedList, false, sizeof(closedList));

    Node nodes[numRows][numCols];
    memset(nodes, 0, sizeof(nodes));

    int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    Node *startNode = &nodes[startX][startY];
    startNode->x = startX;
    startNode->y = startY;
    startNode->f = 0;
    startNode->g = 0;
    startNode->h = 0;
    startNode->visited = true;
    startNode->parent = NULL;

    Node *currentNode;

    while (true)
    {
        int minF = INT_MAX;

        // Find the node with the lowest f value
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numCols; j++)
            {
                if (nodes[i][j].visited && !closedList[i][j] && nodes[i][j].f < minF)
                {
                    minF = nodes[i][j].f;
                    currentNode = &nodes[i][j];
                }
            }
        }

        if (minF == INT_MAX)
        {
            printf("Path not found.\n");
            return;
        }

        // Mark the current node as visited
        closedList[currentNode->x][currentNode->y] = true;

        if (currentNode->x == destX && currentNode->y == destY)
        {
            // Path found
            // printf("Path found.\n");

            // Count the number of nodes in the path
            int count = 0;
            Node *tempNode = currentNode;
            while (tempNode != NULL)
            {
                count++;
                tempNode = tempNode->parent;
            }

            // Allocate memory for the path array
            *path = (Point *)malloc(count * sizeof(Point));
            *pathLength = count;

            // Store the path nodes in reverse order
            int index = count - 1;
            tempNode = currentNode;
            while (tempNode != NULL)
            {
                (*path)[index].x = tempNode->x;
                (*path)[index].y = tempNode->y;
                index--;
                tempNode = tempNode->parent;
            }

            return;
        }

        // Explore neighbors
        for (int i = 0; i < 8; i++)
        {
            int newX = currentNode->x + dx[i];
            int newY = currentNode->y + dy[i];

            if (isValid(newX, newY, numRows, numCols) && !isObstacle(grid, newX, newY) && !closedList[newX][newY])
            {
                Node *neighbor = &nodes[newX][newY];

                int newG = currentNode->g + 1;
                int newH = calculateH(newX, newY, destX, destY);
                int newF = newG + newH;

                if (!neighbor->visited || newF < neighbor->f)
                {
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

// Function for retrieving data of an ally base
Unit getAllyBaseData(struct GameStatus gameStatus)
{
    Unit base;
    base.affiliation = '0'; // Set affiliation to an invalid value as a fallback

    for (int i = 0; i < gameStatus.numUnits; i++)
    {
        if (gameStatus.units[i].type == 'B' && gameStatus.units[i].affiliation == 'P')
        {
            return gameStatus.units[i];
        }
    }

    printf("Couldn't find ally base.\n");
    return base;
}

// Function for retrieving data of an enemy Base
Unit getEnemyBaseData(struct GameStatus gameStatus)
{

    Unit base;

    for (int i = 0; i < gameStatus.numUnits; i++)
    {
        if (gameStatus.units[i].type == 'B' && gameStatus.units[i].affiliation == 'E')
        {
            return gameStatus.units[i];
        }
    }

    printf("Couldn't find enemy base.");
    return base;
}

void freeAllocatedCommandStringsMemory(char **stringArray, int arraySize)
{
    // Free up the memory for each string in the array
    for (int i = 0; i < arraySize; i++)
    {
        free(stringArray[i]);
    }

    // Free up the memory for the array itself
    free(stringArray);
}

// Function for checking if an ally base is producing any units
bool allyBaseIsProducingUnit(Unit allyBase)
{

    if (allyBase.producedUnit == '0')
    {
        printf("The base is not producing anything\n");
        return false;
    }

    return true;
}

// Function for random number generation
int generateRandomNumberFromZeroTo(int n)
{
    // Seed the random number generator with the current time
    srand(time(NULL));

    // Generate a random number between 0 and 7
    int randomNumber = rand() % n;

    // Print the randomly generated number
    printf("Random number: %d\n", randomNumber);

    return randomNumber;
}

// Function to determine the affordable unit based on gold
char getAffordableUnitLetter(int gold)
{
    srand(time(NULL)); // Initialize random seed

    if (gold >= getRamData().cost)
    {
        return 'R';
    }
    else if (gold >= getCatapultData().cost)
    {
        return 'C';
    }
    else if (gold >= getKnightData().cost)
    {
        return 'K';
    }
    else if (gold >= getSwordsmanData().cost || gold >= getArcherData().cost)
    {
        // Randomly choose between 'S' and 'A'
        int randomIndex = rand() % 2;
        if (randomIndex == 0)
        {
            return 'S';
        }
        else
        {
            return 'A';
        }
    }
    else if (gold >= getPikemanData().cost)
    {
        return 'P';
    }
    else if (gold >= getWorkerData().cost)
    {
        return 'W';
    }
    else
    {
        return '0';
    }
}

// Function for checking if unit2 is in attack range of unit1
// Unit 1 is the one attacking
// According to paper attack range between units is counted as D = |x1 - x2| + |y1 - y2|
bool isInAttackRange(Unit unit1, Unit unit2)
{

    int distance = abs(unit1.Xcord - unit2.Xcord) + abs(unit1.Ycord - unit2.Ycord);
    UnitData unit1Data = getUnitData(unit1.type);

    // Compare the squared distances to avoid using square root
    if (distance <= unit1Data.attackRange)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Function which checks if after an attack, the unit gets defeated
bool isUnitDefeated(Unit attackingUnit, Unit defendingUnit)
{
    char attackingUnitType = attackingUnit.type;
    char defendingUnitType = defendingUnit.type;
    // Damage table
    int damageTable[8][8] = {
        {35, 35, 35, 35, 35, 50, 35, 35},
        {30, 30, 30, 20, 20, 30, 30, 30},
        {15, 15, 15, 15, 10, 10, 15, 15},
        {35, 15, 15, 15, 15, 10, 15, 10},
        {40, 40, 40, 40, 40, 40, 40, 50},
        {10, 10, 10, 10, 10, 10, 10, 50},
        {5, 5, 5, 5, 5, 5, 5, 1}};

    // Convert unit types to indices
    int attackingIndex, defendingIndex;
    switch (attackingUnitType)
    {
    case 'K':
        attackingIndex = 0;
        break;
    case 'S':
        attackingIndex = 1;
        break;
    case 'A':
        attackingIndex = 2;
        break;
    case 'P':
        attackingIndex = 3;
        break;
    case 'C':
        attackingIndex = 4;
        break;
    case 'R':
        attackingIndex = 5;
        break;
    case 'W':
        attackingIndex = 6;
        break;
    default:
        return false; // Invalid attacking unit type
    }

    switch (defendingUnitType)
    {
    case 'K':
        defendingIndex = 0;
        break;
    case 'S':
        defendingIndex = 1;
        break;
    case 'A':
        defendingIndex = 2;
        break;
    case 'P':
        defendingIndex = 3;
        break;
    case 'C':
        defendingIndex = 4;
        break;
    case 'R':
        defendingIndex = 5;
        break;
    case 'W':
        defendingIndex = 6;
        break;
    case 'B':
        defendingIndex = 7;
        break;
    default:
        return false; // Invalid defending unit type
    }

    // Compare the attacking unit's damage with the defending unit's health
    int damage = damageTable[attackingIndex][defendingIndex];
    if (damage >= 0 && damage >= defendingUnit.health)
    {
        return true; // Unit is killed
    }
    else
    {
        return false; // Unit survives
    }
}

// In this function we're picking a Point from a path array which is presumably closest to the possible speed limit of the unit,
// which should be at that index in the path array (e.g. path[speed]). That is not always the case because of how we
// calculate distance though, that's why we're decrementing count (same as speed) variable to match the furthest possible point

// we also need to validate the Point here because of the count > speed, otherwise there is no possible move towards enemy base

int getFurthestPossibleMovePointOnPathIndex(Point *path, int pathLength, int speed)
{
    // Check if the path is shorter than the unit's speed
    if (pathLength <= speed)
    {
        // Return the last point on the path
        return pathLength - 1;
    }
    else
    {
        // Calculate the index of the move point based on the unit's speed
        Point startPoint = path[0];
        Point assumedPoint = path[speed];
        // Use formula inserted in the paper which is D = |x1 - x2| + |y1 - y2|
        int rangeBetweenPoints = abs(startPoint.x - assumedPoint.x) + abs(startPoint.y - assumedPoint.y);

        int count = speed;
        while (rangeBetweenPoints > speed)
        {
            count--;
            assumedPoint = path[count];
            rangeBetweenPoints = abs(startPoint.x - assumedPoint.x) + abs(startPoint.y - assumedPoint.y);
        }

        // Return the move point on the path
        return count;
    }
}

// Function for validating the furthest possible point for a unit on certain path, based on colliding enemy units
Point validateFurthestPossiblePointWithEnemyUnits(Point *path, Unit *enemyUnits, int *enemyUnitsNum, int furthestPossiblePointIndex)
{
    // check if the point is not occupied by enemy's unit, if it is count--
    for (int i = 0; i < (*enemyUnitsNum); i++)
    {
        if (path[furthestPossiblePointIndex].x == enemyUnits[i].Xcord && path[furthestPossiblePointIndex].y == enemyUnits[i].Ycord)
        {
            furthestPossiblePointIndex--;
        }
    }

    // if the path towards enemy base is occupied, return default Point (no possible move)
    if (furthestPossiblePointIndex <= 0)
    {
        Point defaultPoint;
        return defaultPoint;
    }

    return path[furthestPossiblePointIndex];
}

#define MOVE_COMMAND 'M'
#define ATTACK_COMMAND 'A'

// Function responsible for generating a string of one command
// Some of these arguments are optional for different COMMANDS, therefore some of these can take values of 0 or NULL
char *generateUnitCommand(char commandType, int unitID, int attackedUnitID, int cordX, int cordY)
{
    char *resultString = NULL;
    int stringLength = 0;

    if (commandType == 'M')
    {
        stringLength = snprintf(NULL, 0, "%d %c %d %d", unitID, MOVE_COMMAND, cordX, cordY) + 1;
    }
    else if (commandType == 'A')
    {
        stringLength = snprintf(NULL, 0, "%d %c %d", unitID, ATTACK_COMMAND, attackedUnitID) + 1;
    }

    resultString = malloc(stringLength * sizeof(char));
    if (resultString != NULL)
    {
        if (commandType == 'M')
        {
            snprintf(resultString, stringLength, "%d %c %d %d", unitID, MOVE_COMMAND, cordX, cordY);
        }
        else if (commandType == 'A')
        {
            snprintf(resultString, stringLength, "%d %c %d", unitID, ATTACK_COMMAND, attackedUnitID);
        }
    }

    return resultString;
}

// Function returning an index of a enemy unit in range, otherwise return -1
int checkIfUnitInRangeOfEnemy(Unit *enemyUnits, int *enemyUnitsNum, Unit unit)
{
    for (int i = 0; i < *enemyUnitsNum; i++)
    {
        if (isInAttackRange(unit, enemyUnits[i]))
        {
            return i;
        }
    }
    return -1;
}

// Function for returning furthest possible point based on unit's speed, obstacles and enemies
Point generateValidatedMovePointForUnit(Unit unit, Unit *enemyUnits, int *enemyUnitsNum, Point destination, int **mapArray, int numRows, int numCols)
{
    //, otherwise move the unit towards enemy base
    Point start;
    start.x = unit.Xcord;
    start.y = unit.Ycord;
    Point *path; // array of result Points that you can take along the way to destination (base)
    int pathLength;
    findShortestPath(mapArray, start, destination, numRows, numCols, &path, &pathLength);

    // write a command to move the unit towards enemy base
    int unitSpeed = getUnitData(unit.type).speed;
    int furthestPossiblePointIndexSpeedValidated = getFurthestPossibleMovePointOnPathIndex(path, pathLength, unitSpeed);

    Point furthestPossiblePoint = validateFurthestPossiblePointWithEnemyUnits(path, enemyUnits, enemyUnitsNum, furthestPossiblePointIndexSpeedValidated);

    return furthestPossiblePoint;
}

// Fucntion for removing enemy from a list of enemies (after defeating one)
void removeUnitFromEnemyListAtIndex(Unit *enemyUnits, int *enemyUnitsNum, int indexOfUnit)
{
    if (indexOfUnit < 0 || indexOfUnit >= *enemyUnitsNum)
    {
        printf("Invalid index\n");
        return;
    }

    // Shift elements to fill the gap
    for (int i = indexOfUnit; i < *enemyUnitsNum - 1; i++)
    {
        enemyUnits[i] = enemyUnits[i + 1];
    }

    (*enemyUnitsNum)--; // Update the count of enemy units
}

// Function for retrieving data about the closest Mine to send the worker to
Point getClosestMinePoint(Unit allyBase, int **mapArray, int numRows, int numCols)
{
    Point closestMinePoint;
    int minDistance = INT_MAX;
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            if (mapArray[i][j] == 6)
            {
                int distance = abs(allyBase.Xcord - i) + abs(allyBase.Ycord - 1);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closestMinePoint.x = i;
                    closestMinePoint.y = j;
                }
            }
        }
    }
    return closestMinePoint;
}

// make Unit Decisions based on provided data and add those to commandsStrings
void generateUnitDecisions(Unit allyBase, Unit unit, Unit *enemyUnits, int *enemyUnitsNum, char **commandsStrings, int *comStringsNum, Point enemyBaseDest, int **mapArray, int numRows, int numCols)
{

    bool attacked = false;

    if (unit.type != 'B')
    {
        if (unit.type != 'W')
        {
            int indexOfUnitInRange = checkIfUnitInRangeOfEnemy(enemyUnits, enemyUnitsNum, unit);
            // If enemy unit is in attack range, stay where you are and attack him
            if (indexOfUnitInRange != -1)
            {
                printf("Unit in attack range at %d %d\n", enemyUnits[indexOfUnitInRange].Xcord, enemyUnits[indexOfUnitInRange].Ycord);
                commandsStrings[*comStringsNum] = generateUnitCommand(ATTACK_COMMAND, unit.ID, enemyUnits[indexOfUnitInRange].ID, 0, 0);
                (*comStringsNum)++;
                attacked = true;

                // check if enemy unit gets killed
                if (isUnitDefeated(unit, enemyUnits[indexOfUnitInRange]))
                {

                    // if yes then check if any other unit is in the range, if yes end round for that unit
                    int unitInRangeIndex = checkIfUnitInRangeOfEnemy(enemyUnits, enemyUnitsNum, unit);
                    if (unitInRangeIndex == -1)
                    {
                        return;
                    }

                    // otherwise move towards enemy base

                    Point furthestPossiblePoint = generateValidatedMovePointForUnit(unit, enemyUnits, enemyUnitsNum, enemyBaseDest, mapArray, numRows, numCols);
                    printf("Unit type: %c and ID: %d, defeated unit type: %c and ID: %d\n", unit.type, unit.ID, enemyUnits[indexOfUnitInRange].type, enemyUnits[indexOfUnitInRange].ID);

                    commandsStrings[*comStringsNum] = generateUnitCommand(MOVE_COMMAND, unit.ID, 0, furthestPossiblePoint.x, furthestPossiblePoint.y);
                    (*comStringsNum)++;

                    removeUnitFromEnemyListAtIndex(enemyUnits, enemyUnitsNum, indexOfUnitInRange); // if unit is defeated also remove it from enemy list

                    return;
                }
            }

            // if you attacked end the turn
            if (attacked)
                return;

            // otherwise, if no unit is in range move the unit and then try to attack

            Point furthestPossiblePoint = generateValidatedMovePointForUnit(unit, enemyUnits, enemyUnitsNum, enemyBaseDest, mapArray, numRows, numCols);

            commandsStrings[*comStringsNum] = generateUnitCommand(MOVE_COMMAND, unit.ID, 0, furthestPossiblePoint.x, furthestPossiblePoint.y);
            (*comStringsNum)++;

            // update coordinates after move
            unit.Xcord = furthestPossiblePoint.x;
            unit.Ycord = furthestPossiblePoint.y;

            // then check if in range of attack
            int enemyUnitInRangeIndex = checkIfUnitInRangeOfEnemy(enemyUnits, enemyUnitsNum, unit);
            if (enemyUnitInRangeIndex != -1)
            {
                // if yes attack

                commandsStrings[*comStringsNum] = generateUnitCommand(ATTACK_COMMAND, unit.ID, enemyUnits[enemyUnitInRangeIndex].ID, 0, 0);
                (*comStringsNum)++;
                if (isUnitDefeated(unit, enemyUnits[enemyUnitInRangeIndex]))
                { // if unit gets defeated delete it from unit list
                    removeUnitFromEnemyListAtIndex(enemyUnits, enemyUnitsNum, enemyUnitInRangeIndex);
                }
                return;
            }

            // If you happen to be in range to attack enemy unit - attack
            //  if u kill a unit, also make a move
            //  if not, return
            // If not assert closest path to enemy base and move
            // Check for possible enemy units to not stand on them
            // If you happen to collide with enemy unit, move one path unit closer
            // If you haven't attacked before, check for attack again and attack

            // do something if not worker
        }
        else
        {
            // if worker send yourself to the closest mine, don't try to fight enemy units
            Point closestMinePoint = getClosestMinePoint(allyBase, mapArray, numRows, numCols);

            printf("Closest mine point is at %d %d\n", closestMinePoint.x, closestMinePoint.y);

            Point furthestPossiblePoint = generateValidatedMovePointForUnit(unit, enemyUnits, enemyUnitsNum, closestMinePoint, mapArray, numRows, numCols);
            commandsStrings[*comStringsNum] = generateUnitCommand(MOVE_COMMAND, unit.ID, 0, furthestPossiblePoint.x, furthestPossiblePoint.y);
            (*comStringsNum)++;
        }
    }
}

// Function returns an array of structs of all enemy units (including base)
Unit *getEnemyUnits(Unit *units, int unitsNum, int *enemyUnitsNum)
{

    Unit *enemyUnits = (Unit *)malloc(*enemyUnitsNum * sizeof(Unit));
    int index = 0;
    for (int i = 0; i < unitsNum; i++)
    {
        if (units[i].affiliation == 'E')
        {
            enemyUnits[index] = units[i];
            index++;
        }
    }
    return enemyUnits;
}

// Function returns number of enemy units alive
int countEnemyUnits(Unit *units, int unitsNum)
{

    int count = 0;
    for (int i = 0; i < unitsNum; i++)
    {
        if (units[i].affiliation == 'E')
        {
            count++;
        }
    }

    if (count == 0)
    {
        printf("No enemy units found.\n");
        return 0;
    }
    return count;
}

// Function returns number of ally units alive
int countAllyUnits(Unit *units, int unitsNum)
{
    int count = 0;
    for (int i = 0; i < unitsNum; i++)
    {
        if (units[i].affiliation == 'P')
        {
            count++;
        }
    }

    if (count == 0)
    {
        printf("No ally units found.\n");
        return 0;
    }

    return count;
}

// Function returns an array of ally Units (including base)
Unit *getAllyUnits(Unit *units, int unitsNum, int allyUnitsNum)
{
    Unit *allyUnits = (Unit *)malloc(allyUnitsNum * sizeof(Unit));
    int index = 0;
    for (int i = 0; i < unitsNum; i++)
    {
        if (units[i].affiliation == 'P')
        {
            allyUnits[index] = units[i];
            index++;
        }
    }

    return allyUnits;
}

// Function for generating Commands for all units in the game
char *generateCommandBasedOnFile(char *statusFileName, char *mapFileName, char **commandsStrings, int *comStringsNum)
{

    struct GameStatus gameStatus = readGameStatus(statusFileName);
    int enemyUnitsNum = countEnemyUnits(gameStatus.units, gameStatus.numUnits);
    Unit *enemyUnits = getEnemyUnits(gameStatus.units, gameStatus.numUnits, &enemyUnitsNum);
    int allyUnitsNum = countAllyUnits(gameStatus.units, gameStatus.numUnits);
    Unit *allyUnits = getAllyUnits(gameStatus.units, gameStatus.numUnits, allyUnitsNum);
    Unit allyBase = getAllyBaseData(gameStatus);
    Unit enemyBase = getEnemyBaseData(gameStatus);

    Point enemyBaseDest;
    enemyBaseDest.x = enemyBase.Xcord;
    enemyBaseDest.y = enemyBase.Ycord;

    printf("enemy units number: %d\n", enemyUnitsNum);

    int numRows, numCols;
    int **mapArray = readDataFromMapFile(mapFileName, &numRows, &numCols);

    // generate command for each units
    for (int i = 0; i < allyUnitsNum; i++)
    {
        generateUnitDecisions(allyBase, allyUnits[i], enemyUnits, &enemyUnitsNum, commandsStrings, comStringsNum, enemyBaseDest, mapArray, numRows, numCols);
    }

    if (!allyBaseIsProducingUnit(allyBase))
    { // if no units are being produced, produce a unit
        if (gameStatus.numUnits == 0)
        {
            printf("There are: %d units/n", gameStatus.numUnits);
        }
        else
        {
            // always try to build the most expensive unit u can afford
            char affordableUnitLetter = getAffordableUnitLetter(gameStatus.gold);

            // if units < 3 build 3 knights
            if (gameStatus.gold >= 250 && allyUnitsNum < 3)
            {
                // build swordsman if u have less than 3 units
                affordableUnitLetter = 'S';
            }
            else if (gameStatus.gold >= 100 && gameStatus.gold < 300 && allyUnitsNum > 6)
            {
                // build workers when lack of gold but already have a few units
                affordableUnitLetter = 'W';
            }

            int stringLength = 6;
            char *resultString = malloc(stringLength * sizeof(char));
            sprintf(resultString, "%d %c %c", allyBase.ID, allyBase.type, affordableUnitLetter);

            commandsStrings[*comStringsNum] = resultString;
            (*comStringsNum)++;
        }
    }

    // makeUnitDecisions(enemyUnits, enemyUnitsNum, numRows, numCols, gameStatus, commandsStrings, comStringsNum);

    freeAllocatedMapMemory(mapArray, numRows);
    freeAllocatedGameStatusMemory(&gameStatus);
    free(enemyUnits);
    // open file in read mode

    return commandsStrings[0];
}

// Function for writing generated commands into a file (should be called "presumably rozkazy.txt")
int writeCommandsIntoFile(char **commandString, int *comStringsNum, char *fileString)
{

    FILE *file = fopen(fileString, "w"); // Open file in write mode
    // Write an empty string to clear the file

    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return 1;
    }

    for (int i = 0; i < *comStringsNum; i++)
    {
        fprintf(file, "%s\n", commandString[i]);
    }

    // Close the file
    fclose(file);

    return 0;
}

int main(int argc, char *argv[])
{
    // assign arguments from terminal
    char *mapFileString;
    char *statusFileString;
    char *ordersFileString;
    printf("received arguments in number: %d \n", argc);
    if (argc >= 4)
    {
        mapFileString = argv[1];
        statusFileString = argv[2];
        ordersFileString = argv[3];
        printf("Received name of file %s\n", argv[3]);
    }
    else if (argc > 5 || argc <= 1)
    {
        printf("Wrong arguments passed, Program One quits");
        return 0;
    }

    printf("Player One is making a move...\n");

    // Sleep(3000);
    printf("Player One is generating a command...\n");
    char **commandsStrings = malloc(MAX_COMMAND_NUMBER * sizeof(char *));
    int comStringsNum = 0;
    generateCommandBasedOnFile(statusFileString, mapFileString, commandsStrings, &comStringsNum);

    printf("Writing commands into a file...\n");

    writeCommandsIntoFile(commandsStrings, &comStringsNum, ordersFileString);

    // free allocated space for commands
    // freeAllocatedCommandStringsMemory(commandsStrings, comStringsNum);
    printf("End of turn\n");
    return 0;
}
