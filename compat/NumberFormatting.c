#include <Carbon/Carbon.h>

static void reverse_u8(void *start, size_t size)
{
  unsigned char *lo = start;
  unsigned char *hi = start + size - 1;
  unsigned char swap;
  while (lo < hi) {
    swap = *lo;
    *lo++ = *hi;
    *hi-- = swap;
  }
}

void NumToString(
    long theNum,
    Str255 theString)
{
  if (theString == NULL) { return; }

  uint8_t *i = &theString[0];
  uint8_t *s = &theString[1];

  *i = 0;
  if (theNum == 0) {
    s[*i] = '0'; (*i)++;
    return;
  }

  bool is_neg = theNum < 0;
  if (is_neg) {
    s[*i] = '-'; (*i)++;
    theNum = -theNum;
  }

  while (theNum > 0) {
    uint8_t rem = theNum % 10;
    s[*i] = rem + '0'; (*i)++;
    theNum = theNum / 10;
  }

  s = &theString[1];
  uint8_t offset = is_neg ? +1 : 0;
  reverse_u8(s + offset, *i - offset);
}
