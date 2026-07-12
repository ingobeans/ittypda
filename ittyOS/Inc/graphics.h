#include "fonts.h"
#include "rust.h"

void print(char *fmt, ...);
void print_flush(FontDef font);
int drawIbiImage(char *filename);
void initSPI(u32 BaudRatePrescaler);
void clear_print_buffer();