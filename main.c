#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Origin Version Functions
int calcLeadExpOrigin(FILE *input, int count);
void inputPolyOrigin(FILE *input, int* poly, int len, int size);
void removeOrigin(int* poly);
void writePolyOrigin(FILE *output, int* poly, int polyLen);
void attachExtraOrigin(int* result, int *poly, int len);
void sumPolyOrigin(int* result, int* polyA, int* polyB);
double runOrigin(FILE *input, FILE *output, int noWrite);
// Improved Version Struct and Functions
typedef struct Poly{
    int coef;
    int exp;
}Poly;
int compareTempImproved(const void *a, const void *b);
void removeDupPolyImproved(Poly **poly, int start, int end);
void writePolyImproved(FILE *output, Poly** poly, int startPos, int endPos);
double runImproved(FILE *input, FILE *output, int noWrite);
// Linked Version Struct and Functions
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
int main(int argc, char *argv[]) {
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    if (input == NULL || output == NULL) return 0;
    runLinked(input, output, 0);
    double timing1 = runOrigin(input, output, 1);
    double timing2 = runImproved(input, output, 1);
    double timing3 = runLinked(input, output, 1);
    fprintf(output, "%.10lf\t%.10lf\t%.10lf\n", timing1*1000000, timing2*1000000, timing3*1000000);
    fclose(input);
    return 0;
}
// Origin Version
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
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    sumPolyOrigin(result, polyA, polyB);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    if (!noWrite) writePolyOrigin(output, result, *result+2);
    free(polyA);
    free(polyB);
    free(result);
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
}
// Improved Version
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
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
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
    clock_gettime(CLOCK_MONOTONIC, &tend);
    if (!noWrite) writePolyImproved(output, poly, startC, endC);
    for (int i = 0; i < polyStructLen; i++) free(*(poly+i));
    free(poly);
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
}
// Linked Version
int compareTempLinked(const void *a, const void *b){return ((LinkedPoly*)b)->exp-((LinkedPoly*)a)->exp;}
void inputLinked(FILE *input, LinkedPoly **head, LinkedPoly **now, int count){
    for (int i=0;i<count;i++) {
        int temp[2]; fscanf(input, "%d %d", temp, temp+1);
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly));
        newNode->coef = *temp; newNode->exp = *(temp+1); newNode->next = NULL;
        if (*head == NULL) {
            *head = newNode;
            *now = *head;
        }else {
            *now = *head;
            while ((*now) != NULL) {
                if ((*now)->exp > newNode->exp) {
                    if ((*now)->next == NULL) {
                        (*now)->next = newNode;
                        break;
                    }
                    *now = (*now)->next;
                    continue;
                }
                if ((*now)->exp == newNode->exp) {
                    (*now)->coef += newNode->coef;
                    free(newNode);
                    break;
                }
                if ((*now)->exp < newNode->exp) {
                    if ((*now) == *head) {
                        newNode->next = *head;
                        *head = newNode;
                        break;
                    }
                    LinkedPoly *t = *head;
                    while (t->next != (*now)) t = t->next;
                    newNode->next = (*now);
                    t->next = newNode;
                    break;
                }
            }
        }
    }
}
void writeLinked(FILE *output, LinkedPoly *linked){
    int writen = 0;
    while (linked != NULL) {
        if (linked->coef == 0) {linked=linked->next;continue;}
        if (linked->exp != 0) fprintf(output, "%dx^%d + ", linked->coef, linked->exp);
        else fprintf(output, "%d + ", linked->coef);
        linked = linked->next;
        writen+=1;
    }
    if (writen != 0) {
        fseek(output, -3, SEEK_CUR);
        fwrite("", 0, 3, output);
    }
    fprintf(output, "\n");
}
void freeLinked(LinkedPoly **linked) {
    while (*linked != NULL) {
        LinkedPoly *temp = *linked;
        *linked = (*linked)->next;
        free(temp);
    }
}
double runLinked(FILE *input, FILE *output, int noWrite){
    fseek(input, 0, SEEK_SET);
    int lens[2];
    fscanf(input, "%d %d", lens, lens+1);
    LinkedPoly *Ahead = NULL, *Bhead = NULL, *Chead = NULL;
    LinkedPoly *Anow = NULL, *Bnow = NULL, *Cnow = NULL;
    inputLinked(input, &Ahead, &Anow, *lens);
    inputLinked(input, &Bhead, &Bnow, *(lens+1));
    Anow = Ahead; Bnow = Bhead;
    if (!noWrite) writeLinked(output, Anow);
    if (!noWrite) writeLinked(output, Bnow);
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    Anow = Ahead; Bnow = Bhead;
    while (Anow != NULL && Bnow != NULL) {
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly));
        if (Anow->exp > Bnow->exp) {
            newNode->coef = Anow->coef; newNode->exp = Anow->exp; newNode->next = NULL;
            if (Chead == NULL) {
                Chead = newNode;
                Cnow = Chead;
            }else {
                Cnow->next = newNode;
                Cnow = newNode;
            }
            Anow = Anow->next;
        }else if (Anow->exp == Bnow->exp) {
            newNode->coef = Anow->coef+Bnow->coef;
            newNode->exp = Anow->exp;
            newNode->next = NULL;
            if (Chead == NULL) {
                Chead = newNode;
                Cnow = Chead;
            }else {
                Cnow->next = newNode;
                Cnow = newNode;
            }
            Anow = Anow->next; Bnow = Bnow->next;
        }else {
            newNode->coef = Bnow->coef; newNode->exp = Bnow->exp; newNode->next = NULL;
            if (Chead == NULL) {
                Chead = newNode;
                Cnow = Chead;
            }else {
                Cnow->next = newNode;
                Cnow = newNode;
            }
            Bnow = Bnow->next;
        }
    }
    while (Anow != NULL) {
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly));
        newNode->coef = Anow->coef; newNode->exp = Anow->exp; newNode->next = NULL;
        if (Chead == NULL) {
            Chead = newNode;
            Cnow = Chead;
        }else {
            Cnow->next = newNode;
            Cnow = newNode;
        }
        Anow = Anow->next;
    }
    while (Bnow != NULL) {
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly));
        newNode->coef = Bnow->coef; newNode->exp = Bnow->exp; newNode->next = NULL;
        if (Chead == NULL) {
            Chead = newNode;
            Cnow = Chead;
        }else {
            Cnow->next = newNode;
            Cnow = newNode;
        }
        Bnow = Bnow->next;
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);
    if (!noWrite) writeLinked(output, Chead);
    freeLinked(&Ahead);
    freeLinked(&Bhead);
    freeLinked(&Chead);
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
}