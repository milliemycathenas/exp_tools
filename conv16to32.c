// Convert 2-byte Radware .mat matrix to 4-byte .spn or .m4b matrix
// Usage: ./convert_mat_to_spn input.mat output.spn

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MATRIX_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input.mat output.spn\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *fin = fopen(input_file, "rb");
    if (!fin) {
        perror("Error opening input file");
        return 1;
    }

    FILE *fout = fopen(output_file, "wb");
    if (!fout) {
        perror("Error opening output file");
        fclose(fin);
        return 1;
    }

    int16_t svalue;
    int32_t ivalue;
    size_t total = 0;

    for (int row = 0; row < MATRIX_SIZE; ++row) {
        for (int col = 0; col < MATRIX_SIZE; ++col) {
            if (fread(&svalue, sizeof(int16_t), 1, fin) != 1) {
                fprintf(stderr, "\nError reading .mat file at [%d,%d]\n", row, col);
                fclose(fin);
                fclose(fout);
                return 1;
            }

            ivalue = (int32_t)svalue;
            fwrite(&ivalue, sizeof(int32_t), 1, fout);
            ++total;
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Successfully converted %zu values (4096x4096 matrix).\n", total);
    return 0;
}
