#ifndef LINKEDVER_H
#define LINKEDVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef struct Linked{
    int coef;
    int exp;
    struct Linked *next;
}LinkedPoly;
int compareTempLinked(const void *a, const void *b);
void inputLinked(FILE *input, LinkedPoly **head, LinkedPoly **now, int count);
void writeLinked(FILE *output, LinkedPoly *linked);
void freeLinked(LinkedPoly **linked);
double runLinked(FILE *input, FILE *output, int noWrite);
#endif //LINKEDVER_H
