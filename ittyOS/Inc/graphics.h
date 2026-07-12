#include "fonts.h"
#include "rust.h"

typedef struct {
  /// Configure crop start X. 0 counts as no cropping on X axis.
  u16 cropX;
  /// Configure crop start Y. 0 counts as no cropping on U axis.
  u16 cropY;
  /// Configure crop width. 0 counts as no width bounds.
  u16 cropWidth;
  /// Configure crop height. 0 counts as no height bounds.
  u16 cropHeight;
  /// Whether the image should be drawn centered around the position gived to
  /// the draw function
  u8 centered;
} DRAW_IBI_CONFIG;

extern DRAW_IBI_CONFIG DEFAULT_DRAW_IBI_CONFIG;

void print(char *fmt, ...);
void print_flush(FontDef font);
int drawIBIFullscreen(char *filename);
void initSPI(u32 BaudRatePrescaler);
void clear_print_buffer();
int drawIBITextOverlay(u16 x, u16 y, char *text, FontDef *font, char *filename);
int drawIBI(char *filename, u16 x, u16 y, DRAW_IBI_CONFIG drawIBIConfig);