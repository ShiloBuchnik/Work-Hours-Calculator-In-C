#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <windows.h>

void flushBuffer(void)
{
    while (getchar() != '\n');
}

/* This function takes a pointer to a char and 2 chars, and verifies that the user input one of the two given chars
Note that the verification is case-insensitive */
void charVerification(char* ptr, char char0, char char1)
{
    while (1)
    {
        scanf("%c", ptr);
        *ptr = toupper(*ptr);
        char0 = toupper(char0);
        char1 = toupper(char1);

        if (*ptr != char0 && *ptr != char1)
        {
            printf("Invalid input, try again: ");
            flushBuffer();
        }
        else
        {
            flushBuffer();
            break;
        }
    }
}

void hourVerification(int* ptrh, int* ptrm)
{
    while (scanf("%d : %d", ptrh, ptrm) != 2)
    {
        printf("Invalid input, try again: ");
        flushBuffer();
    }
}

int enableColor(void) // Enable the use of color in the program
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // We need to get the windows handle of our stdout
    if (hOut == INVALID_HANDLE_VALUE) return -1; // If something went wrong - return -1

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return -1; // checking the output mode, idk, anyway if something went wrong - return -1

    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING macro, this is what we tell to setConsoleMode() to enable ansi escape sequences
    if (!SetConsoleMode(hOut, dwMode)) return -1;
    // setting the console mode to enable ansi escape sequences, if something went wrong - return -1

    return 1; // Return 1 on success
}

void printManual(void)
{
    printf("Hello wage slave! This program lets you store your work hours in a save file, and calculate your hourly sum\n");
    printf("You can enter the hours daily and calculate at the end of the month, or enter it altogether and calculate instantly\n");
    printf("\x1b[31mWhen you wish to stop, simply enter 00:00\n\n\x1b[0m");

    printf("But hold on buckaroo, lets clarify some things first:\n");
    printf("1) Use military time (hh:mm format). Typos or deviations will alter your result\n");
    printf("2) The program will ask you if you want to deduce a 30 minute break in a work day longer than 6 hours\n");
    printf("3) Every hour after 8 hours net is worth 125%% of itself\n");
    printf("4) Every hour after 10 hours net is worth 150%% of itself\n");
    printf("5) \x1b[94mSimple sum\x1b[0m is the net hours you've worked. \x1b[91mWeighted sum\x1b[0m is the net hours with factoring of \x1b[95movertime\x1b[0m\n");
    printf("6) The program rounds the shown minutes, but the \x1b[32msalary\x1b[0m calculation is accurate on the dot\n");
    printf("7) You can import your own .txt (save) file, named \"Hours\"; but you have to adhere to the following syntax:\n");
    printf("<h1:m1> <h2:m2>\n");
    printf("8) enjoy! ^~^\n\n");
}

// Check if file exists, and asks if append or write
void fileHandling(FILE** fptr)
{
    char AW, YN;
    *fptr = fopen("Hours.txt", "r");
    if (*fptr == NULL) printf(">System does not recognize previous save file, creating new save...\n");
    else
    {
        printf(">System recognizes previous save file\n");
        printf("Would you like to read file's content? [\x1b[92mY\x1b[0m\\\x1b[31mN\x1b[0m]");
        charVerification(&YN, 'Y', 'N');
        if (YN == 'Y')
        {
            printf("\n");
            int h1, m1, h2, m2, i;
            for (i = 1; fscanf(*fptr, "%d : %d  %d : %d", &h1, &m1, &h2, &m2) != -1; i++)
            {
                printf("%d) %02d:%02d %02d:%02d\n", i, h1, m1, h2, m2);
            }
            if (i == 1) printf("<Null file>\n");
            fclose(*fptr);
        }

        printf("\nWould you like to \x1b[92mA\x1b[0mppend to existing save or \x1b[31mW\x1b[0mrite a new one? [\x1b[92mA\x1b[0m\\\x1b[31mW\x1b[0m] ");
        charVerification(&AW, 'A', 'W');

        if (AW == 'W') // Resetting the file
        {
            *fptr = fopen("Hours.txt", "w");
            fclose(*fptr);
        }
    }

    *fptr = fopen("Hours.txt", "a"); // 'a' mode can also create a file
    printf("\n");
}

// getting differences and printing them into file
void getDifferences(int* h1, int* m1, int* h2, int* m2, FILE** fptr)
{
    while (1)
    {
        printf("Enter start hour: ");
        hourVerification(h1, m1);
        if (*h1 == 0 && *m1 == 0) break;

        printf("Enter end hour: ");
        hourVerification(h2, m2);
        if (*h2 == 0 && *m2 == 0) break;

        if (*h2 < *h1 || (*h2 == *h1 && *m2 < *m1)) // Making sure the time difference is positive
        {
            printf("Invalid input, try again.\n\n");
            continue;
        }

        fprintf(*fptr, "%d:%d ", *h1, *m1);
        fprintf(*fptr, "%d:%d\n", *h2, *m2);
        printf("\n");
    }
    fclose(*fptr);
}

// Scans file and sum hours
void scanAndSum(FILE** fptr, int* h1, int* m1, int* h2, int* m2, int* minuteSum, int* overtime125, int* overtime150)
{
    int start, end, difference;
    *fptr = fopen("hours.txt", "r");

    char YN;
    printf("Would you like to deduce a 30 minute break after 6 hours? [\x1b[92mY\x1b[0m\\\x1b[31mN\x1b[0m]");
    charVerification(&YN, 'Y', 'N');

    // When 'fscanf' gets to EOF, it returns -1. That's when we want to break out
    while (fscanf(*fptr, "%d : %d", h1, m1) != -1)
    {
        fscanf(*fptr, "%d : %d", h2, m2);

        start = 60 * *h1 + *m1;
        end = 60 * *h2 + *m2;
        difference = end - start;
        *minuteSum += difference; // This variable stores the entire time duration, including overtime, in *unweighted* way

        int start125 = (YN == 'Y' ? 510 : 480);
        int end125 = (YN == 'Y' ? 630 : 600);

        if (YN == 'Y' && 6*60 <= difference) *minuteSum -= 30; // Deducing 30 minutes break if user asked for it

        if (start125 < difference && difference <= end125) *overtime125 = difference - start125;
        if (difference > end125)
        {
            *overtime125 = 2*60;
            *overtime150 = difference - end125;
        }
    }
    fclose(*fptr);
}

// calculating sum, weighted sum and salary
void salaryCalc(int minuteSum, int overtime125, int overtime150)
{
    int minute = minuteSum % 60;
    int hour = (minuteSum - minute) / 60;
    printf("\x1b[94mThe simple sum is: %d hours and %d minutes\n\x1b[0m", hour, minute);

    int minute125 = overtime125 % 60;
    int hour125 = (overtime125 - minute125) / 60;
    int minute150 = overtime150 % 60;
    int hour150 = (overtime150 - minute150) / 60;
    printf("Out of this sum:\n");
    printf("\x1b[95mTotal 125%% overtime is is: %d hours and %d minutes\x1b[0m\n", hour125, minute125);
    printf("\x1b[95mTotal 150%% overtime is is: %d hours and %d minutes\x1b[0m\n\n", hour150, minute150);

    double weightedSum = minuteSum + 0.25 * overtime125 + 0.5 * overtime150;
    hour = (int) weightedSum / 60;
    minute = (int) round((weightedSum / 60 - hour) * 60);
    printf("\x1b[91mThe weighted sum is: %d hours and %d minutes\x1b[0m\n\n", hour, minute);

    double hourlyRate;
    printf("Enter your hourly rate: ");
    while (scanf("%lf", &hourlyRate) != 1)
    {
        printf("Invalid input, try again: ");
        flushBuffer();
    }

    double salary = hourlyRate * (weightedSum / 60);

    printf("Your salary is: \x1b[32m%lf\x1b[0m\n\n", salary);
}