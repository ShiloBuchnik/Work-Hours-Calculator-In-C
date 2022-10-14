#include <stdio.h>

void flushBuffer(void);
void charVerification(char* ptr, char char0, char char1);
void hourVerification(int* ptrh, int* ptrm);
int enableColor (void);
void printManual(void);
void fileHandling(FILE** fptr);
void getDifferences(int* h1, int* m1, int* h2, int* m2, FILE** fptr);
void scanAndSum(FILE** fptr, int* h1, int* m1, int* h2, int* m2, int* minuteSum, int* overtime125, int* overtime150);
void salaryCalc(int minuteSum, int overtime125, int overtime150);