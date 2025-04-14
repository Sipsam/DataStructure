// Include header files using in this code
#include <stdio.h> // For input and output
#include <stdlib.h> // For memory allocation
#include <string.h> // For string manipulation
#include <time.h> // For time measurement
// Define Origin Version Functions
int calcLeadExpOrigin(FILE *input, int count);
void inputPolyOrigin(FILE *input, int* poly, int len, int size);
void removeOrigin(int* poly);
void writePolyOrigin(FILE *output, int* poly, int polyLen);
void attachExtraOrigin(int* result, int *poly, int len);
void sumPolyOrigin(int* result, int* polyA, int* polyB);
double runOrigin(FILE *input, FILE *output, int noWrite);
// Define Improved Version Struct and Functions
typedef struct Poly{
    int coef;
    int exp;
}Poly; // Polynomial structure
int compareTempImproved(const void *a, const void *b);
void removeDupPolyImproved(Poly **poly, int start, int end);
void writePolyImproved(FILE *output, Poly** poly, int startPos, int endPos);
double runImproved(FILE *input, FILE *output, int noWrite);
// Define Linked Version Struct and Functions
typedef struct Linked{
    int coef;
    int exp;
    struct Linked *next;
}LinkedPoly; // Linked list node structure
int compareTempLinked(const void *a, const void *b);
void inputLinked(FILE *input, LinkedPoly **head, LinkedPoly **now, int count);
void writeLinked(FILE *output, LinkedPoly *linked);
void freeLinked(LinkedPoly **linked);
double runLinked(FILE *input, FILE *output, int noWrite);
// Execute the program
int main(int argc, char *argv[]) {
    // Get the input and output file names from command line arguments
    FILE *input = fopen(argv[1], "r"); // Open input file for reading
    FILE *output = fopen(argv[2], "w"); // Open output file for writing
    if (input == NULL || output == NULL) return 0; // Check if files opened successfully
    double timing1 = runOrigin(input, output, 0); // Run the original version, and write the timing
    double timing2 = runImproved(input, output, 0); // Run the improved version, and write the timing
    double timing3 = runLinked(input, output, 0); // Run the linked version, and write the timing
    fprintf(output, "%.10lf\t%.10lf\t%.10lf\n", timing1, timing2, timing3);
    /*
     * print the timing result at the end of output
     * the timing result in the output file will be displayed in seconds
     * the first column is the original version, the second column is the improved version, and the third column is the linked version
     */
    fclose(input); // Close input file
    fclose(output); // Close output file
    return 0;
}
/****************************************************************************
 *  ____  ____  _  _____ _  _        _     _____ ____  ____  _  ____  _
 * /  _ \/  __\/ \/  __// \/ \  /|  / \ |\/  __//  __\/ ___\/ \/  _ \/ \  /|
 * | / \||  \/|| || |  _| || |\ ||  | | //|  \  |  \/||    \| || / \|| |\ ||
 * | \_/||    /| || |_//| || | \||  | \// |  /_ |    /\___ || || \_/|| | \||
 * \____/\_/\_\\_/\____\\_/\_/  \|  \__/  \____\\_/\_\\____/\_/\____/\_/  \|
 * **************************************************************************
 * This asciiArt created by https://wepplication.github.io/tools/asciiArtGen/
 * **************************************************************************/
// Implement Origin Version
// calcLeadExpOrigin calculates the maximum exponent of the polynomial
// Get the input file and the number of terms as parameters
int calcLeadExpOrigin(FILE *input, int count) {
    int lead = -1; // Initialize lead to -1, lead will save the max exponent
    int start = ftell(input); // Mark the current position in the file to go back
    for (int i=0;i<count;i++) { // Loop through the number of terms
        int temp; // Temporary variable to store the exponent
        fscanf(input, "%d\t%d", &temp, &temp); // Read the coefficient and exponent
        if (temp > lead) lead = temp; // Update lead if the temporary exponent is greater than the current lead
    }
    fseek(input, start, SEEK_SET); // Go back to the marked position in the file
    return lead; // return the lead exponent
}
// inputPolyOrigin reads the polynomial from the input file and stores it in an array
// Get the input file, the polynomial array, the number of terms, and the size of the polynomial as parameters
void inputPolyOrigin(FILE *input, int* poly, int len, int size) {
    for (int i = 0; i < len; i++) { // Loop through the number of terms
        int temp[2]; // Temporary array to store the coefficient and exponent
        fscanf(input, "%d %d", temp, temp+1); // Read the coefficient and exponent from the input file
        *(poly+abs(*(temp+1)-size)+1) += *temp; // Store the coefficient in the polynomial array
        /*
         * abs(*(temp+1)-size)+1 is used to calculate the index in the polynomial array
         * The +1 is used to skip the first element of the array, which is used to store the size of the polynomial
         * The abs function is used to ensure that the index is always positive
         * The size of the polynomial is used to determine the maximum exponent
         * The index in the polynomial array is calculated by subtracting the exponent from the size
         * This ensures that the polynomial is stored in descending order of exponents
         */
    }
    *poly = size; // Set the first element of the polynomial array to the size of the polynomial same as the lead exponent
}
// removeOrigin removes the coefficient of the lead exponent from the polynomial array
// Get the polynomial array as a parameter
void removeOrigin(int* poly) {
    for (int i=1;i<*poly+1;i++) // Loop through the polynomial array
        *(poly+i) = *(poly+i+1); // Replace the former one to the next one
    *(poly+*poly+1) = 0; // The last one is set to 0 to erase the last element
}
// writePolyOrigin writes the polynomial to the output file
// Get the output file, the polynomial array, and the length of the polynomial as parameters
void writePolyOrigin(FILE *output, int* poly, int polyLen) {
    int writen = 0; // Initialize writen to 0, writen will save the number of terms written
    for (int i = 1; i < polyLen; i++) { // Loop through the polynomial array
        if (*(poly+i) != 0) { // Check if the coefficient is not zero
            fprintf(output, "%dx^%d + ", *(poly+i), polyLen-i-1);
            // Write the coefficient and exponent to the output file in coef^exp format
            writen+=1;
        }
    }
    if (writen != 0) { // Check if write nothing, erase the last " + "
        fseek(output, -3, SEEK_CUR);
        fwrite("", 0, 3, output);
    }
    fprintf(output, "\n");
}
// attachExtraOrigin attaches the remaining coefficients to the result polynomial
// Get the result polynomial, the polynomial array, and the length of the result as parameters
void attachExtraOrigin(int* result, int *poly, int len) {
    if (*poly >= 0) { // Check if the polynomial has remaining coefficients
        const int n = *poly+2; // Calculate the number of remaining coefficients
        for (int i=1;i<n;i++) { // Loop through the remaining coefficients
            *(result+abs(len-1-*poly)) += *(poly+1); // Add the remaining coefficient to the result polynomial
            /*
             * abs(len-1-*poly) is used to calculate the index in the result polynomial
             * For more detail, check line 70-77
             */
            removeOrigin(poly); // Remove the coefficient from the polynomial array added to result
            *poly -= 1; // Decrement the exponent of the polynomial
        }
    }
}
// sumPolyOrigin sums two polynomials and stores the result in the result array
// Get the result polynomial, the two polynomial arrays as parameters
void sumPolyOrigin(int* result, int* polyA, int* polyB) {
    int resultLen = *polyA > *polyB ? *polyA+2 : *polyB+2; // Calculate the length of the result polynomial
    *result = resultLen-2; // Set the first element of the result polynomial to the lead exponent
    while (*polyA >= 0 && *polyB >= 0) { // Loop until one of the polynomials is empty
        if (*polyA > *polyB) { // Check if polyA has a greater exponent
            *(result+abs(resultLen-1-*polyA)) += *(polyA+1); // Add the coefficient to the result polynomial
            /*
             * abs(resultLen-1-*polyA) is used to calculate the index in the result polynomial
             * For more detail, check line 70-77
             */
            removeOrigin(polyA); // Remove the coefficient from the polynomial array
            *polyA -= 1; // Decrement the exponent of polyA
        }else if (*polyA < *polyB) { // Check if polyB has a greater exponent
            *(result+abs(resultLen-1-*polyB)) += *(polyB+1); // Add the coefficient to the result polynomial
            /*
             * abs(resultLen-1-*polyB) is used to calculate the index in the result polynomial
             * For more detail, check line 70-77
             */
            removeOrigin(polyB); // Remove the coefficient from the polynomial array
            *polyB -= 1; // Decrement the exponent of polyB
        }else { // Check if both polynomials have the same exponent
            *(result+abs(resultLen-1-*polyA)) += *(polyA+1)+*(polyB+1); // Add the coefficients to the result polynomial
            /*
             * abs(resultLen-1-*polyA) is used to calculate the index in the result polynomial
             * For more detail, check line 70-77
             */
            removeOrigin(polyA); // Remove the coefficient from the polynomial array
            removeOrigin(polyB); // Remove the coefficient from the polynomial array
            *polyA -= 1; *polyB -= 1; // Decrement the exponents of polyA and polyB
        }
    }
    if (polyA >= 0) attachExtraOrigin(result, polyA, resultLen); // Check if polyA has remaining coefficients
    if (polyB >= 0) attachExtraOrigin(result, polyB, resultLen); // Check if polyB has remaining coefficients
}
// runOrigin runs the original version of the polynomial addition
// Get the input file, output file, and noWrite flag as parameters
double runOrigin(FILE *input, FILE *output, int noWrite) {
    // Under two lines are used to measure the time of the program
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    fseek(input, 0, SEEK_SET); // Go to the beginning of the input file
    int lens[2]; // Array to store the number of terms in each polynomial
    fscanf(input, "%d %d", lens, lens+1); // Read the number of terms from the input file
    int leadA = calcLeadExpOrigin(input, *lens); // Calculate the lead exponent of polyA
    int *polyA = calloc(leadA+2, sizeof(int)); // Allocate memory for polyA
    if (polyA == NULL) return -1; // Check if memory allocation was successful
    inputPolyOrigin(input, polyA, *lens, leadA); // Read polyA from the input file
    if (!noWrite) writePolyOrigin(output, polyA, *polyA+2); // Write polyA to the output file
    int leadB = calcLeadExpOrigin(input, *(lens+1)); // Calculate the lead exponent of polyB
    int *polyB = calloc(leadB+2, sizeof(int)); // Allocate memory for polyB
    if (polyB == NULL) return -1; // Check if memory allocation was successful
    inputPolyOrigin(input, polyB, *(lens+1), leadB); // Read polyB from the input file
    if (!noWrite) writePolyOrigin(output, polyB, *polyB+2); // Write polyB to the output file
    int leadResult = leadA > leadB ? leadA : leadB; // Calculate the lead exponent of the result polynomial
    int *result = calloc(leadResult+2, sizeof(int)); // Allocate memory for the result polynomial
    if (result == NULL) return -1; // Check if memory allocation was successful
    sumPolyOrigin(result, polyA, polyB); // Sum polyA and polyB and store the result in the result polynomial
    if (!noWrite) writePolyOrigin(output, result, *result+2); // Write the result polynomial to the output file
    free(polyA); // Free the memory allocated for polyA
    free(polyB); // Free the memory allocated for polyB
    free(result); // Free the memory allocated for the result polynomial
    clock_gettime(CLOCK_MONOTONIC, &tend); // Get the end time
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec); // Calculate the elapsed time
}
/******************************************************************************************
 *  _  _      ____  ____  ____  _     _____ ____    _     _____ ____  ____  _  ____  _
 * / \/ \__/|/  __\/  __\/  _ \/ \ |\/  __//  _ \  / \ |\/  __//  __\/ ___\/ \/  _ \/ \  /|
 * | || |\/|||  \/||  \/|| / \|| | //|  \  | | \|  | | //|  \  |  \/||    \| || / \|| |\ ||
 * | || |  |||  __/|    /| \_/|| \// |  /_ | |_/|  | \// |  /_ |    /\___ || || \_/|| | \||
 * \_/\_/  \|\_/   \_/\_\\____/\__/  \____\\____/  \__/  \____\\_/\_\\____/\_/\____/\_/  \|
 * ****************************************************************************************
 * This asciiArt created by https://wepplication.github.io/tools/asciiArtGen/
 * ****************************************************************************************/
// Implement Improved Version
// compareTempImproved compares two polynomial structures based on their exponents
// Get two polynomial structures as parameters
int compareTempImproved(const void *a, const void *b) {
    return (*(Poly**)b)->exp-(*(Poly**)a)->exp; // return the difference of the exponents
}
// removeDupPolyImproved removes duplicate terms from the polynomial array
// Get the polynomial array, the start index, and the end index as parameters
void removeDupPolyImproved(Poly **poly, int start, int end) {
    for (int i = start; i < end+1; i++) { // Loop through the polynomial array
        if ((*(poly+i))->coef == 0) continue; // Check if the coefficient is zero
        for (int j = i+1; j < end+1; j++) { // Loop through the polynomial array again
            if ((*(poly+j))->exp == (*(poly+i))->exp) { // Check if the exponents are equal
                (*(poly+i))->coef += (*(poly+j))->coef; // Add the coefficients
                (*(poly+j))->coef = 0; // Set the coefficient to zero
            }
        }
    }
}
// writePolyImproved writes the polynomial to the output file
// Get the output file, the polynomial array, the start index, and the end index as parameters
void writePolyImproved(FILE *output, Poly** poly, int startPos, int endPos) {
    int writen = 0; // Initialize writen to 0, writen will save the number of terms written
    for (int i = startPos; i < endPos; i++) { // Loop through the polynomial array
        if ((*(poly+i))->coef == 0) continue; // Check if the coefficient is zero
        fprintf(output, "%dx^%d + ", (*(poly+i))->coef, (*(poly+i))->exp); // Write the coefficient and exponent to the output file
        writen+=1; // Increment writen
    }
    if (writen != 0) { // Check if write nothing, erase the last " + "
        fseek(output, -3, SEEK_CUR); // Go back to the last position
        fwrite("", 0, 3, output); // Erase the last " + "
    }
    fprintf(output, "\n");
}
// runImproved runs the improved version of the polynomial addition
// Get the input file, output file, and noWrite flag as parameters
double runImproved(FILE *input, FILE *output, int noWrite){
    // Under two lines are used to measure the time of the program
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    fseek(input, 0, SEEK_SET); // Go to the beginning of the input file
    int lens[2] = {0, 0}; // Array to store the number of terms in each polynomial
    fscanf(input, "%d %d", lens, lens+1); // Read the number of terms from the input file
    int polyStructLen = (*lens+*(lens+1))*2+1; // Calculate the length of the polynomial structure
    Poly** poly = (Poly**)malloc(polyStructLen * sizeof(Poly*)); // Allocate memory for the polynomial structure array
    if (poly == NULL) return -1; // Check if memory allocation was successful
    for (int i = 0; i < polyStructLen; i++) { // Loop through the polynomial structure
        *(poly+i) = (Poly*)malloc(sizeof(Poly)); // Allocate memory for each polynomial structure
        if (*(poly+i) == NULL) { // Check if memory allocation was successful
            for (int j = 0; j < i; j++) free(*(poly+j)); // Free the memory allocated for the previous polynomial structures
            free(poly); // Free the memory allocated for the polynomial structure array
            return -1; // Return -1 to indicate failure
        }
        *(*(poly+i)) = (Poly){0, 0}; // Initialize each polynomial structures
    }
    for (int i=0;i<(polyStructLen-1)/2;i++) // Loop through the polynomial structure
        fscanf(input, "%d %d", &(*(poly+i))->coef, &(*(poly+i))->exp); // Read the coefficient and exponent from the input file
    int startA = 0, startB = *lens, startC = *lens+*(lens+1); // Initialize the start indices for polyA, polyB, and polyC
    int endA = *lens-1, endB = *lens+*(lens+1)-1, endC = startC; // Initialize the end indices for polyA, polyB, and polyC
    qsort(poly, *lens, sizeof(Poly*), compareTempImproved); // Sort polyA in descending order of exponents
    qsort(poly+*lens, *(lens+1), sizeof(Poly*), compareTempImproved); // Sort polyB in descending order of exponents
    removeDupPolyImproved(poly, startA, endA); // Remove duplicate terms from polyA
    removeDupPolyImproved(poly, startB, endB); // Remove duplicate terms from polyB
    if (!noWrite) writePolyImproved(output, poly, startA, endA+1); // Write polyA to the output file
    if (!noWrite) writePolyImproved(output, poly, startB, endB+1); // Write polyB to the output file
    while (startA <= endA && startB <= endB) { // Loop until one of the polynomials is empty
        if ((*(poly+startA))->exp > (*(poly+startB))->exp) { // Check if polyA has a greater exponent
            (*(poly+endC))->coef = (*(poly+startA))->coef; // Add the coefficient to polyC
            (*(poly+endC))->exp = (*(poly+startA))->exp; // Add the exponent to polyC
            endC++; startA++; // Increment the end index of polyC and the start index of polyA
        } else if ((*(poly+startA))->exp < (*(poly+startB))->exp) { // Check if polyB has a greater exponent
            (*(poly+endC))->coef = (*(poly+startB))->coef; // Add the coefficient to polyC
            (*(poly+endC))->exp = (*(poly+startB))->exp; // Add the exponent to polyC
            endC++; startB++; // Increment the end index of polyC and the start index of polyB
        } else { // Check if both polynomials have the same exponent
            (*(poly+endC))->coef = (*(poly+startA))->coef + (*(poly+startB))->coef; // Add the coefficients to polyC
            (*(poly+endC))->exp = (*(poly+startA))->exp; // Add the exponent to polyC
            endC++; startA++; startB++; // Increment the end index of polyC and the start indices of polyA and polyB
        }
    }
    if (startA <= endA) { // Check if polyA has remaining coefficients
        for (int i = startA; i <= endA; i++) { // Loop through polyA
            (*(poly+endC))->coef = (*(poly+i))->coef; // Add the coefficient to polyC
            (*(poly+endC))->exp = (*(poly+i))->exp; // Add the exponent to polyC
            endC++; // Increment the end index of polyC
        }
    }
    if (startB <= endB) { // Check if polyB has remaining coefficients
        for (int i = startB; i <= endB; i++) { // Loop through polyB
            (*(poly+endC))->coef = (*(poly+i))->coef; // Add the coefficient to polyC
            (*(poly+endC))->exp = (*(poly+i))->exp; // Add the exponent to polyC
            endC++; // Increment the end index of polyC
        }
    }
    if (!noWrite) writePolyImproved(output, poly, startC, endC); // Write polyC to the output file
    for (int i = 0; i < polyStructLen; i++) free(*(poly+i)); // Free the memory allocated for each polynomial structure
    free(poly); // Free the memory allocated for the polynomial structure array
    clock_gettime(CLOCK_MONOTONIC, &tend); // Get the end time
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec); // Calculate the elapsed time
}
/******************************************************************************
 *  _     _  _      _  __ _____ ____    _     _____ ____  ____  _  ____  _
 * / \   / \/ \  /|/ |/ //  __//  _ \  / \ |\/  __//  __\/ ___\/ \/  _ \/ \  /|
 * | |   | || |\ |||   / |  \  | | \|  | | //|  \  |  \/||    \| || / \|| |\ ||
 * | |_/\| || | \|||   \ |  /_ | |_/|  | \// |  /_ |    /\___ || || \_/|| | \||
 * \____/\_/\_/  \|\_|\_\\____\\____/  \__/  \____\\_/\_\\____/\_/\____/\_/  \|
 * ****************************************************************************
 * This asciiArt created by https://wepplication.github.io/tools/asciiArtGen/
 * ****************************************************************************/
// Implement Linked Version
// compareTempLinked compares two linked polynomial structures based on their exponents
// Get two linked polynomial structures as parameters
int compareTempLinked(const void *a, const void *b){return ((LinkedPoly*)b)->exp-((LinkedPoly*)a)->exp;}
// inputLinked reads the polynomial from the input file and stores it in a linked list
// Get the input file, the head of the linked list, the current node, and the number of terms as parameters
void inputLinked(FILE *input, LinkedPoly **head, LinkedPoly **now, int count){
    for (int i=0;i<count;i++) { // Loop through the number of terms
        int temp[2]; fscanf(input, "%d %d", temp, temp+1); // Read the coefficient and exponent from the input file
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly)); // Allocate memory for the new node
        newNode->coef = *temp; newNode->exp = *(temp+1); newNode->next = NULL; // Initialize the new node
        if (*head == NULL) { // Check if the linked list is empty
            *head = newNode; // Set the head to the new node
            *now = *head; // Set the current node to the head
        }else { // Check if the linked list is not empty
            *now = *head; // Set the current node to the head
            while ((*now) != NULL) { // Loop through the linked list
                if ((*now)->exp > newNode->exp) { // Check if the current node has a greater exponent
                    if ((*now)->next == NULL) { // Check if the current node is the last node
                        (*now)->next = newNode; // Add the new node to the end of the linked list
                        break; // Exit the loop
                    }
                    *now = (*now)->next; // Move to the next node
                    continue; // Continue to the next iteration
                }
                if ((*now)->exp == newNode->exp) { // Check if the current node has the same exponent
                    (*now)->coef += newNode->coef; // Add the coefficients
                    free(newNode); // Free the new node
                    break; // Exit the loop
                }
                if ((*now)->exp < newNode->exp) { // Check if the current node has a smaller exponent
                    if ((*now) == *head) { // Check if the current node is the head
                        newNode->next = *head; // Set the next of the new node to the head
                        *head = newNode; // Set the head to the new node
                        break; // Exit the loop
                    }
                    LinkedPoly *t = *head; // Set t to the head
                    while (t->next != (*now)) t = t->next; // Loop through the linked list to find the previous node
                    newNode->next = (*now); // Set the next of the new node to the current node
                    t->next = newNode; // Set the next of the previous node to the new node
                    break; // Exit the loop
                }
            }
        }
    }
}
// writeLinked writes the polynomial to the output file
// Get the output file and the linked list as parameters
void writeLinked(FILE *output, LinkedPoly *linked){
    int writen = 0; // Initialize writen to 0, writen will save the number of terms written
    while (linked != NULL) { // Loop through the linked list
        if (linked->coef == 0) {linked=linked->next;continue;} // Check if the coefficient is zero
        fprintf(output, "%dx^%d + ", linked->coef, linked->exp); // Write the coefficient and exponent to the output file
        linked = linked->next; // Move to the next node
        writen+=1; // Increment writen
    }
    if (writen != 0) { // Check if write nothing, erase the last " + "
        fseek(output, -3, SEEK_CUR); // Go back to the last position before " + "
        fwrite("", 0, 3, output); // erase the last " + "
    }
    fprintf(output, "\n");
}
// freeLinked is free the linked nodes
// Get the head of the linked list as a parameter
void freeLinked(LinkedPoly **linked) {
    while (*linked != NULL) { // Loop through the linked list
        LinkedPoly *temp = *linked; // Set temp to the current node
        *linked = (*linked)->next; // Move to the next node
        free(temp); // Free the current node
    }
}
// runLinked runs the linked version of the polynomial addition
// Get the input file, output file, and noWrite flag as parameters
double runLinked(FILE *input, FILE *output, int noWrite){
    // Under two lines are used to measure the time of the program
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    fseek(input, 0, SEEK_SET); // Go to the beginning of the input file
    int lens[2]; // Array to store the number of terms in each polynomial
    fscanf(input, "%d %d", lens, lens+1); // Read the number of terms from the input file
    LinkedPoly *Ahead = NULL, *Bhead = NULL, *Chead = NULL; // Initialize the linked list heads
    LinkedPoly *Anow = NULL, *Bnow = NULL, *Cnow = NULL; // Initialize the linked list current nodes
    inputLinked(input, &Ahead, &Anow, *lens); // Read polyA from the input file
    inputLinked(input, &Bhead, &Bnow, *(lens+1)); // Read polyB from the input file
    Anow = Ahead; Bnow = Bhead; // Set the current nodes to the heads
    if (!noWrite) writeLinked(output, Anow); // Write polyA to the output file
    if (!noWrite) writeLinked(output, Bnow); // Write polyB to the output file
    Anow = Ahead; Bnow = Bhead; // Set the current nodes to the heads again
    while (Anow != NULL && Bnow != NULL) { // Loop until one of the polynomials is empty
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly)); // Allocate memory for the new node
        if (Anow->exp > Bnow->exp) { // Check if polyA has a greater exponent
            newNode->coef = Anow->coef; newNode->exp = Anow->exp; newNode->next = NULL; // Initialize the new node
            if (Chead == NULL) { // Check if the linked list is empty
                Chead = newNode; // Set the head to the new node
                Cnow = Chead; // Set the current node to the head
            }else { // Check if the linked list is not empty
                Cnow->next = newNode; // Add the new node to the end of the linked list
                Cnow = newNode; // Set the current node to the new node
            }
            Anow = Anow->next; // Move to the next node in polyA
        }else if (Anow->exp == Bnow->exp) { // Check if both polynomials have the same exponent
            newNode->coef = Anow->coef+Bnow->coef; // Add the coefficients
            newNode->exp = Anow->exp; // Add the exponent to the new node
            newNode->next = NULL; // Set the next of the new node to NULL
            if (Chead == NULL) { // Check if the linked list is empty
                Chead = newNode; // Set the head to the new node
                Cnow = Chead; // Set the current node to the head
            }else { // Check if the linked list is not empty
                Cnow->next = newNode; // Add the new node to the end of the linked list
                Cnow = newNode; // Set the current node to the new node
            }
            Anow = Anow->next; Bnow = Bnow->next; // Move to the next nodes in polyA and polyB
        }else { // Check if polyB has a greater exponent
            newNode->coef = Bnow->coef; newNode->exp = Bnow->exp; newNode->next = NULL; // Initialize the new node
            if (Chead == NULL) { // Check if the linked list is empty
                Chead = newNode; // Set the head to the new node
                Cnow = Chead; // Set the current node to the head
            }else { // Check if the linked list is not empty
                Cnow->next = newNode; // Add the new node to the end of the linked list
                Cnow = newNode; // Set the current node to the new node
            }
            Bnow = Bnow->next; // Move to the next node in polyB
        }
    }
    while (Anow != NULL) { // Loop through the remaining nodes in polyA
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly)); // Allocate memory for the new node
        newNode->coef = Anow->coef; newNode->exp = Anow->exp; newNode->next = NULL; // Initialize the new node
        if (Chead == NULL) { // Check if the linked list is empty
            Chead = newNode; // Set the head to the new node
            Cnow = Chead; // Set the current node to the head
        }else { // Check if the linked list is not empty
            Cnow->next = newNode; // Add the new node to the end of the linked list
            Cnow = newNode; // Set the current node to the new node
        }
        Anow = Anow->next; // Move to the next node in polyA
    }
    while (Bnow != NULL) { // Loop through the remaining nodes in polyB
        LinkedPoly *newNode = (LinkedPoly*)malloc(sizeof(LinkedPoly)); // Allocate memory for the new node
        newNode->coef = Bnow->coef; newNode->exp = Bnow->exp; newNode->next = NULL; // Initialize the new node
        if (Chead == NULL) { // Check if the linked list is empty
            Chead = newNode; // Set the head to the new node
            Cnow = Chead; // Set the current node to the head
        }else { // Check if the linked list is not empty
            Cnow->next = newNode; // Add the new node to the end of the linked list
            Cnow = newNode; // Set the current node to the new node
        }
        Bnow = Bnow->next; // Move to the next node in polyB
    }
    if (!noWrite) writeLinked(output, Chead); // Write polyC to the output file
    freeLinked(&Ahead); // Free the memory allocated for polyA
    freeLinked(&Bhead); // Free the memory allocated for polyB
    freeLinked(&Chead); // Free the memory allocated for polyC
    clock_gettime(CLOCK_MONOTONIC, &tend); // Get the end time
    return (double)tend.tv_sec + 1.0e-9*tend.tv_nsec-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec); // Calculate the elapsed time
}