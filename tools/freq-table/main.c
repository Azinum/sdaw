// main.c
// generates a table of frequencies
//
// gcc main.c -o build/freq -lm
//
// Usage:
// ./build/freq > freq_table.c

#include <math.h>
#include <stdio.h>

#define STANDARD_PITCH 55
#define FREQ_TABLE_SIZE 128
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2, SEMI_TONE / 12.0f))

int main(int argc, char** argv) {
  FILE* File = stdout;
  fprintf(File, "// freq_table.c\n\n");

  const int Width = 7;
  const char* TableName = "FreqTable";

  fprintf(File, "#define FREQ_TABLE_SIZE %i\n\n", FREQ_TABLE_SIZE);
  fprintf(File, "const float %s[FREQ_TABLE_SIZE] = {\n  ", TableName);
  for (int Index = 0; Index < FREQ_TABLE_SIZE; ++Index) {
    float Freq = NOTE_FREQ(Index);
    fprintf(File, "%.2ff, ", Freq);
    if ((Index & Width) == Width) {
      fprintf(File, "\n");
      if (Index + 1 < FREQ_TABLE_SIZE)
        fprintf(File, "  ");
    }
  }
  fprintf(File, "};\n");
  return 0;
}
