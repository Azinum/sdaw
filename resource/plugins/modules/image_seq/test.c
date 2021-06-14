// test.c
// test module for generating images

#include <sdaw/sdaw.h>

extern void Init(image_seq* Seq);
extern color_rgba Process(image_seq* Seq, i32 X, i32 Y, float AnimationTime);
extern void Destroy(image_seq* Seq);

void Init(image_seq* Seq) {

}

color_rgba Process(image_seq* Seq, i32 X, i32 Y, float AnimationTime) {
  color_rgba Color = {};

  Color = ColorRGBA(255 * (0.5 * sin(AnimationTime) + 1), 0, 255 * (0.5f * cos(AnimationTime) + 1), 255);

  if (Seq->Mask) {
    v2 MaskScaling = V2(
      (float)Seq->Mask->Width / Seq->Output->Width,
      (float)Seq->Mask->Height / Seq->Output->Height
    );
    v2 TexelSample = V2(
      X * MaskScaling.Y,
      Y * MaskScaling.Y
    );
    color_rgba* MaskPixel = (color_rgba*)FetchPixel(Seq->Mask, TexelSample.X, TexelSample.Y);
    if (MaskPixel) {
      Color.R += MaskPixel->R;
      Color.G += MaskPixel->G;
      Color.B += MaskPixel->B;
    }
  }

  return Color;
}

void Destroy(image_seq* Seq) {

}
