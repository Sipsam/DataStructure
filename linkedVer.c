//
// Created by JSW on 25. 4. 4.
//
#include "linkedVer.h"
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
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
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
    if (!noWrite) writeLinked(output, Chead);
    freeLinked(&Ahead);
    freeLinked(&Bhead);
    freeLinked(&Chead);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
}