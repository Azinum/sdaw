// image_seq.h

#ifndef _IMAGE_SEQ_H
#define _IMAGE_SEQ_H

typedef struct image_seq {
  image* Output;
  image* Mask;
  f32 DbAverage;
} image_seq;

i32 ImageSeq(i32 argc, char** argv);

#endif
