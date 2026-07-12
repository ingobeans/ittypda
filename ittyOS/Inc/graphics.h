#include "fonts.h"
#include "rust.h"

void print(char *fmt, ...);
void print_flush(FontDef font);
int drawIBI(char *filename);
void initSPI(u32 BaudRatePrescaler);
void clear_print_buffer();
int drawIBITextOverlay(u16 x, u16 y, char *text, FontDef *font, char *filename);