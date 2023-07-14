#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "units.h"

    char* PLAYER_ONE_EXE_STRING = ".\\playerOne.exe";
    char* PLAYER_TWO_EXE_STRING = ".\\playerTwo.exe";

    struct PlayerStatus {
        struct Unit* playerUnits;

    };


int main() {
    int counter = 0;
    // assert globals
    char winAsserted = '0'; // if no winner give it 0, if winner one - 1, winner two - 2, if draw - 3
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    int playerNumberID = 1;

    char* playerExeString = PLAYER_ONE_EXE_STRING;

    while(winAsserted == '0' || counter < 2000)
    {
 // Run Program 1
    printf("Running Program %d...\n", playerNumberID);
    

    char commandLineArgs[100];
    snprintf(commandLineArgs, sizeof(commandLineArgs), "%s %s %s %s", playerExeString, "mapa.txt", "status.txt", "rozkazy.txt");
    
    if (!CreateProcess(NULL, (LPSTR) commandLineArgs , NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Error creating process: %lu\n", GetLastError());
        return 1;
    }

    // Start the timer
    DWORD timeout = 5000; // Timeout value in milliseconds
    DWORD start = GetTickCount();

    // Wait for the program to finish or until timeout occurs
    DWORD waitResult = WaitForSingleObject(pi.hProcess, timeout);

    // Check the wait result
    if (waitResult == WAIT_TIMEOUT) {
        // Timeout occurred
        printf("Warning: Program %d timed out after 5 seconds.\n", playerNumberID);
        printf("Player %d wins!!.\n", playerNumberID%2 + 1);
        // Terminate the process
        TerminateProcess(pi.hProcess, 0);

        // Clean up the process handle
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return 1;
    } else {
        // continue with second player
        playerExeString = playerNumberID == 1 ? PLAYER_TWO_EXE_STRING : PLAYER_ONE_EXE_STRING;
        playerNumberID = playerNumberID == 1 ? 2 : 1;
        
        counter++;

         // Clean up the process handle
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }


    // Continue with the rest of the code

    }
   
    return 0;
}