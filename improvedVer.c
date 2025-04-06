//
// Created by JSW on 25. 4. 4.
//
#include "improvedVer.h"
int compareTempImproved(const void *a, const void *b) {
    return (*(Poly**)b)->exp-(*(Poly**)a)->exp;
}
void removeDupPolyImproved(Poly **poly, int start, int end) {
    for (int i = start; i < end+1; i++) {
        if ((*(poly+i))->coef == 0) continue;
        for (int j = i+1; j < end+1; j++) {
            if ((*(poly+j))->exp == (*(poly+i))->exp) {
                (*(poly+i))->coef += (*(poly+j))->coef;
                (*(poly+j))->coef = 0;
            }
        }
    }
}
void writePolyImproved(FILE *output, Poly** poly, int startPos, int endPos) {
    int writen = 0;
    for (int i = startPos; i < endPos; i++) {
        if ((*(poly+i))->coef == 0) continue;
        if ((*(poly+i))->exp != 0) fprintf(output, "%dx^%d + ", (*(poly+i))->coef, (*(poly+i))->exp);
        else fprintf(output, "%d + ", (*(poly+i))->coef);
        writen+=1;
    }
    if (writen != 0) {
        fseek(output, -3, SEEK_CUR);
        fwrite("", 0, 3, output);
    }
    fprintf(output, "\n");
}
double runImproved(FILE *input, FILE *output, int noWrite){
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    fseek(input, 0, SEEK_SET);
    int lens[2] = {0, 0};
    fscanf(input, "%d %d", lens, lens+1);
    int polyStructLen = (*lens+*(lens+1))*2+1;
    Poly** poly = (Poly**)malloc(polyStructLen * sizeof(Poly*));
    if (poly == NULL) return -1;
    for (int i = 0; i < polyStructLen; i++) {
        *(poly+i) = (Poly*)malloc(sizeof(Poly));
        if (*(poly+i) == NULL) {
            for (int j = 0; j < i; j++) free(*(poly+j));
            free(poly);
            return -1;
        }
        *(*(poly+i)) = (Poly){0, 0};
    }
    for (int i=0;i<(polyStructLen-1)/2;i++)
        fscanf(input, "%d %d", &(*(poly+i))->coef, &(*(poly+i))->exp);
    int startA = 0, startB = *lens, startC = *lens+*(lens+1);
    int endA = *lens-1, endB = *lens+*(lens+1)-1, endC = startC;
    qsort(poly, *lens, sizeof(Poly*), compareTempImproved);
    qsort(poly+*lens, *(lens+1), sizeof(Poly*), compareTempImproved);
    removeDupPolyImproved(poly, startA, endA);
    removeDupPolyImproved(poly, startB, endB);
    if (!noWrite) writePolyImproved(output, poly, startA, endA+1);
    if (!noWrite) writePolyImproved(output, poly, startB, endB+1);
    while (startA <= endA && startB <= endB) {
        if ((*(poly+startA))->exp > (*(poly+startB))->exp) {
            (*(poly+endC))->coef = (*(poly+startA))->coef;
            (*(poly+endC))->exp = (*(poly+startA))->exp;
            endC++; startA++;
        } else if ((*(poly+startA))->exp < (*(poly+startB))->exp) {
            (*(poly+endC))->coef = (*(poly+startB))->coef;
            (*(poly+endC))->exp = (*(poly+startB))->exp;
            endC++; startB++;
        } else {
            (*(poly+endC))->coef = (*(poly+startA))->coef + (*(poly+startB))->coef;
            (*(poly+endC))->exp = (*(poly+startA))->exp;
            endC++; startA++; startB++;
        }
    }
    if (startA <= endA) {
        for (int i = startA; i <= endA; i++) {
            (*(poly+endC))->coef = (*(poly+i))->coef;
            (*(poly+endC))->exp = (*(poly+i))->exp;
            endC++;
        }
    }
    if (startB <= endB) {
        for (int i = startB; i <= endB; i++) {
            (*(poly+endC))->coef = (*(poly+i))->coef;
            (*(poly+endC))->exp = (*(poly+i))->exp;
            endC++;
        }
    }
    if (!noWrite) writePolyImproved(output, poly, startC, endC);
    for (int i = 0; i < polyStructLen; i++) free(*(poly+i));
    free(poly);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
}