// BitmapTexture information

#include <windows.h>

const int TF_NONE = 0;
const int TF_BILINEAR = 1;

void LoadBitmapTexture(int id);
void SetTextureParameters();
void SetCurrentTexture(int tex);
void DestroyBitmaps();

// type of smoothing for texture mapping
extern int Filter;

extern HINSTANCE theInstance;
