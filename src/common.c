// common.c

inline float Lerp(float V0, float V1, float T) {
  return (1.0f - T) * V0 + T * V1;
}
