#ifndef ORIGINVER_H
#define ORIGINVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int calcLeadExpOrigin(FILE *input, int count);
void inputPolyOrigin(FILE *input, int* poly, int len, int size);
void removeOrigin(int* poly);
void writePolyOrigin(FILE *output, int* poly, int polyLen);
void attachExtraOrigin(int* result, int *poly, int len);
void sumPolyOrigin(int* result, int* polyA, int* polyB);
double runOrigin(FILE *input, FILE *output, int noWrite);
#endif //ORIGINVER_H