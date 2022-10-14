#include <stdio.h>
#include <stdlib.h>
#include "funcs.h"

int main() {
    int h1, m1, h2, m2;
    int minuteSum = 0, overtime125 = 0, overtime150 = 0;
    char YN;
    FILE *fpointer;

    enableColor();
    printManual();

    fileHandling(&fpointer);
    getDifferences(&h1, &m1, &h2, &m2, &fpointer);

    flushBuffer();
    printf("\nWould you like to proceed and calculate the sum of hours in save file? [\x1b[92mY\x1b[0m\\\x1b[31mN\x1b[0m] ");
    charVerification(&YN, 'Y', 'N');

    if (YN == 'N')
    {
        printf("\n");
        printf("Shutting down...\n");
        system("pause");
        exit(0);
    }

    scanAndSum(&fpointer, &h1, &m1, &h2, &m2, &minuteSum, &overtime125, &overtime150);
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    salaryCalc(minuteSum, overtime125, overtime150);

    system("pause");

    return 0;
}
