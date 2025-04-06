#ifndef IMPROVEDVER_H
#define IMPROVEDVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef struct Poly{
    int coef;
    int exp;
}Poly;
int compareTempImproved(const void *a, const void *b);
void removeDupPolyImproved(Poly **poly, int start, int end);
void writePolyImproved(FILE *output, Poly** poly, int startPos, int endPos);
double runImproved(FILE *input, FILE *output, int noWrite);
#endif //IMPROVEDVER_H
