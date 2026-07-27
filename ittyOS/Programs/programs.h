#include "home.h"
#include "notes.h"

typedef struct {
  u64 *data;
  u16 height;
} Icon;

void drawIcon(u16 x, i16 y, u16 color, u8 *buffer, u16 bufferWidth,
              u16 bufferHeight, Icon *icon);
void drawToolbar(char *name);
void updateToolbar();
