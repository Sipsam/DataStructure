//
// Created by JSW on 25. 4. 4.
//
#include "originVer.h"
int calcLeadExpOrigin(FILE *input, int count) {
    int lead = -1;
    int start = ftell(input);
    for (int i=0;i<count;i++) {
        int temp;
        fscanf(input, "%d\t%d", &temp, &temp);
        if (temp > lead) lead = temp;
    }
    fseek(input, start, SEEK_SET);
    return lead;
}
void inputPolyOrigin(FILE *input, int* poly, int len, int size) {
    for (int i = 0; i < len; i++) {
        int temp[2];
        fscanf(input, "%d %d", temp, temp+1);
        *(poly+abs(*(temp+1)-size)+1) += *temp;
    }
    *poly = size;
}
void removeOrigin(int* poly) {
    for (int i=1;i<*poly+1;i++)
        *(poly+i) = *(poly+i+1);
    *(poly+*poly+1) = 0;
}
void writePolyOrigin(FILE *output, int* poly, int polyLen) {
    int writen = 0;
    for (int i = 1; i < polyLen; i++) {
        if (*(poly+i) != 0) {
            if (polyLen-i-1 != 0) {
                fprintf(output, "%dx^%d + ", *(poly+i), polyLen-i-1);
            }else {
                fprintf(output, "%d + ", *(poly+i));
            }
            writen+=1;
        }
    }
    if (writen != 0) {
        fseek(output, -3, SEEK_CUR);
        fwrite("", 0, 3, output);
    }
    fprintf(output, "\n");
}
void attachExtraOrigin(int* result, int *poly, int len) {
    if (*poly >= 0) {
        const int n = *poly+2;
        for (int i=1;i<n;i++) {
            *(result+len-1-*poly) += *(poly+1);
            removeOrigin(poly);
            *poly -= 1;
        }
    }
}
void sumPolyOrigin(int* result, int* polyA, int* polyB) {
    int resultLen = *polyA > *polyB ? *polyA+2 : *polyB+2;
    *result = resultLen-2;
    while (*polyA >= 0 && *polyB >= 0) {
        if (*polyA > *polyB) {
            *(result+resultLen-1-*polyA) += *(polyA+1);
            removeOrigin(polyA);
            *polyA -= 1;
        }else if (*polyA < *polyB) {
            *(result+resultLen-1-*polyB) += *(polyB+1);
            removeOrigin(polyB);
            *polyB -= 1;
        }else {
            *(result+resultLen-1-*polyA) += *(polyA+1)+*(polyB+1);
            removeOrigin(polyA);
            removeOrigin(polyB);
            *polyA -= 1; *polyB -= 1;
        }
    }
    if (polyA >= 0) attachExtraOrigin(result, polyA, resultLen);
    if (polyB >= 0) attachExtraOrigin(result, polyB, resultLen);
}
double runOrigin(FILE *input, FILE *output, int noWrite) {
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    fseek(input, 0, SEEK_SET);
    int lens[2];
    fscanf(input, "%d %d", lens, lens+1);
    int leadA = calcLeadExpOrigin(input, *lens);
    int *polyA = calloc(leadA+2, sizeof(int));
    if (polyA == NULL) return -1;
    inputPolyOrigin(input, polyA, *lens, leadA);
    if (!noWrite) writePolyOrigin(output, polyA, *polyA+2);
    int leadB = calcLeadExpOrigin(input, *(lens+1));
    int *polyB = calloc(leadB+2, sizeof(int));
    if (polyB == NULL) return -1;
    inputPolyOrigin(input, polyB, *(lens+1), leadB);
    if (!noWrite) writePolyOrigin(output, polyB, *polyB+2);
    int leadResult = leadA > leadB ? leadA : leadB;
    int *result = calloc(leadResult+2, sizeof(int));
    if (result == NULL) return -1;
    sumPolyOrigin(result, polyA, polyB);
    if (!noWrite) writePolyOrigin(output, result, *result+2);
    free(polyA);
    free(polyB);
    free(result);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
}