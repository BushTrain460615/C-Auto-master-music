#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>

#define BUFFER_LEN 1024

float calculate_rms(float* buffer, int len) {
    float sum = 0.0f;
    for (int i = 0; i < len; i++) {
        sum += buffer[i] * buffer[i];
    }
    return sqrtf(sum / len);
}

void apply_gain(float* buffer, int len, float gain) {
    for (int i = 0; i < len; i++) {
        buffer[i] *= gain;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s input_file [output_file] [gain]\n", argv[0]);
        return 0;
    }

    char* input_file = argv[1];
    char* output_file = argc > 2 ? argv[2] : "output.wav";
    float gain = argc > 3 ? atof(argv[3]) : 1.0f;

    SF_INFO input_info;
    SNDFILE* input_file_ptr = sf_open(input_file, SFM_READ, &input_info);
    if (input_file_ptr == NULL) {
        printf("Failed to open input file '%s'\n", input_file);
        return 1;
    }

    SF_INFO output_info = input_info;
    SNDFILE* output_file_ptr = sf_open(output_file, SFM_WRITE, &output_info);
    if (output_file_ptr == NULL) {
        printf("Failed to open output file '%s'\n", output_file);
        sf_close(input_file_ptr);
        return 1;
    }

    float buffer[BUFFER_LEN];
    int read_count;

    while ((read_count = sf_read_float(input_file_ptr, buffer, BUFFER_LEN)) > 0) {
        float rms = calculate_rms(buffer, read_count);
        float target_rms = 0.5f;
        float rms_ratio = target_rms / rms;
        float target_gain = powf(rms_ratio, 0.5f);

        apply_gain(buffer, read_count, target_gain * gain);

        sf_write_float(output_file_ptr, buffer, read_count);
    }

    sf_close(input_file_ptr);
    sf_close(output_file_ptr);

    printf("Mastering complete. Output file: '%s'\n", output_file);

    return 0;
}
