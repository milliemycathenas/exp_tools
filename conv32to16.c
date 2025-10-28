// Convert 4-byte per channel Radware .spn/.m4b matrix to 2-byte .mat format
// Usage: ./convert_spn_to_mat input.spn output.mat

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MATRIX_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input.spn output.mat\n", argv[0]);
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

    int32_t value;
    int16_t svalue;
    size_t total = 0;

    for (int row = 0; row < MATRIX_SIZE; ++row) {
        for (int col = 0; col < MATRIX_SIZE; ++col) {
            if (fread(&value, sizeof(int32_t), 1, fin) != 1) {
                fprintf(stderr, "\nError reading matrix at [%d,%d]\n", row, col);
                fclose(fin);
                fclose(fout);
                return 1;
            }

            // Truncate if out of range for int16
            if (value > 32767) {
                svalue = 32767;
            } else if (value < -32768) {
                svalue = -32768;
            } else {
                svalue = (int16_t)value;
            }

            fwrite(&svalue, sizeof(int16_t), 1, fout);
            ++total;
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Successfully converted %zu values (4096x4096 matrix).\n", total);
    return 0;
}
