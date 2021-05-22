// lutgen.c

#include <math.h>
#include <stdio.h>

#define PI32 3.14159265359f
const int Width = 7;

static void GenSineTable(FILE* File, const char* TableName, int Size);
static void GenFreqTable(FILE* File, const char* TableName, int Size);

int main(int argc, char** argv) {
  (void)GenSineTable;
  // GenSineTable(stdout, "SineTable", 256);
  GenFreqTable(stdout, "FreqTable", 128);
  return 0;
}

void GenSineTable(FILE* File, const char* TableName, int Size) {
  fprintf(File, "#define %sSize %i\n", TableName, Size);
  fprintf(File, "static const float %s[%sSize] = {\n  ", TableName, TableName);
  for (int Index = 0; Index < Size; ++Index) {
    float Value = sinf((float)Index);
    fprintf(File, "%.3ff, ", Value);
    if ((Index & Width) == Width) {
      fprintf(File, "\n");
      if (Index + 1 < Size)
        fprintf(File, "  ");
    }
  }
  fprintf(File, "};\n");
}

#define STANDARD_PITCH 55.0f
#define NOTE_FREQ(SEMI_TONE) (STANDARD_PITCH * powf(2.0f, SEMI_TONE / 12.0f))

void GenFreqTable(FILE* File, const char* TableName, int Size) {
  fprintf(File, "#define %sSize %i\n", TableName, Size);
  fprintf(File, "static const float %s[%sSize] = {\n  ", TableName, TableName);
  for (int Index = -9; Index < Size; ++Index) {
    float Value = NOTE_FREQ(Index);
    fprintf(File, "%ff, ", Value);
    if ((Index & Width) == Width) {
      fprintf(File, "\n");
      if (Index + 1 < Size)
        fprintf(File, "  ");
    }
  }
  fprintf(File, "};\n");
}
