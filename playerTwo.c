#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include "units.h"
#include <windows.h>

Unit createKnight() {
    Unit knight;
    knight.durability = 70;
    knight.speed = 5;
    knight.cost = 400;
    knight.attackRange = 1;
    knight.buildTime = 5;
    return knight;
}

Unit createSwordsman() {
    Unit swordsman;
    swordsman.durability = 60;
    swordsman.speed = 2;
    swordsman.cost = 250;
    swordsman.attackRange = 1;
    swordsman.buildTime = 3;
    return swordsman;
}

Unit createArcher() {
    Unit archer;
    archer.durability = 40;
    archer.speed = 2;
    archer.cost = 250;
    archer.attackRange = 5;
    archer.buildTime = 3;
    return archer;
}

Unit createPikeman() {
    Unit pikeman;
    pikeman.durability = 50;
    pikeman.speed = 2;
    pikeman.cost = 200;
    pikeman.attackRange = 2;
    pikeman.buildTime = 3;
    return pikeman;
}

Unit createRam() {
    Unit ram;
    ram.durability = 90;
    ram.speed = 2;
    ram.cost = 500;
    ram.attackRange = 1;
    ram.buildTime = 4;
    return ram;
}

Unit createCatapult() {
    Unit catapult;
    catapult.durability = 50;
    catapult.speed = 2;
    catapult.cost = 800;
    catapult.attackRange = 7;
    catapult.buildTime = 6;
    return catapult;
}

Unit createWorker() {
    Unit worker;
    worker.durability = 20;
    worker.speed = 2;
    worker.cost = 100;
    worker.attackRange = 1;
    worker.buildTime = 2;
    return worker;
}

Unit createBase() {
    Unit base;
    base.durability = 200;
    base.speed = 0;
    base.cost = 0;  // Not applicable for a base
    base.attackRange = 0;  // Not applicable for a base
    base.buildTime = -1;  // Not applicable for a base
    return base;
}

struct PlayerStatus {
    long gold;
};
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
    char* mapFileString;
    char* statusFileString;
    char* ordersFileString;
    char* timeoutInSeconds;
    if(argc >= 4) {
    mapFileString = argv[1];
    statusFileString = argv[2];
    ordersFileString = argv[3];
        if(argc == 5) {
        timeoutInSeconds = argv[4];
         }
    } else  if (argc > 5 || argc <= 1){
        printf("Wrong arguments passed, Program Two quits");
        return 0;
    }

    printf("Player Two is making a move...\n");

    // Generate a random number between 1 and 6000
    // int randomNumber = rdtsc() % 7000 + 1;
    // printf("%d\n", randomNumber);
     Sleep(3000);

    printf("Writing command into a file\n");
    char* commandOne = "BBB WD";
    writeCommandIntoFile(commandOne, ordersFileString);

    printf("End of turn\n");
    return 0;
}
