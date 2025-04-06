#include "originVer.h"
#include "improvedVer.h"
#include "linkedVer.h"
int main(int argc, char *argv[]) {
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    if (input == NULL || output == NULL) return 0;
    runLinked(input, output, 0);
    double timing1 = runOrigin(input, output, 1);
    double timing2 = runImproved(input, output, 1);
    double timing3 = runLinked(input, output, 1);
    fprintf(output, "%.10lf\t%.10lf\t%.10lf\n", timing1, timing2, timing3);
    fclose(input);
    return 0;
}
