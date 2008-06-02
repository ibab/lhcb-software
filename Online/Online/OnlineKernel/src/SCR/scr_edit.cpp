#include <cstdio>
#include "SCR/scr.h"
using namespace SCR;

//----------------------------------------------------------------------------
int scrc_change_rendition (Display *disp, int r1, int c1, int rows, int cols, unsigned int attr)  {
  if (!disp || !rows || !cols) return 1;
  int width = disp->cols + 2;
  int offset = r1*width + c1;
  int r2  = r1 + rows - 1;
  int c2  = c1 + cols - 1;
  attr |= disp->def_attr;
  for (int r=r1; r<=r2; r++)  {
    unsigned int *a = disp->attr + offset;
    for (int c=c1; c<=c2; c++) *a++ = attr;
    offset += width;
  }
  scrc_draw_block (disp, r1, r2, c1, c2);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_insert_line (Display *disp, const char *str, unsigned int attr, int row, int scroll)   {
  Pasteboard *pb = disp->pb;
  int h = disp->rows;
  int w = disp->cols;

  if (row < 1 || row > h) return 0;
  level_up (pb);
  if (scroll == MOVE_UP)  {
    if (row > 1)    {
      int offset = 2 * (w + 2) + 1;
      char* m = disp->map + offset;
      unsigned int* a = disp->attr + offset;
      for (int r = 1; r < row; r++, m += 2, a += 2)
        for (int c = 1; c <= w; c++)
          scrc_put_char (disp, *m++, *a++, r, c);
    }
  }
  else  {
    if (row < h)    {
      int offset = (h-1) * (w + 2) + 1;
      for (int r = h-1; r >= row; r--, offset -= w + 2)  {
        char* m = disp->map + offset;
        unsigned int* a = disp->attr + offset;
        for (int c = 1; c <= w; c++)
          scrc_put_char (disp, *m++, *a++, r+1, c);
      }
    }
  }
  scrc_put_chars (disp, str, attr, row, 1, 1);
  level_down (pb);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_insert_char (Display *disp, char ch, unsigned int attr, int row, int col)  {
  Pasteboard *pb = disp->pb;
  int h = disp->rows;
  int w = disp->cols;
  if (row < 1 || row > h || col < 1 || col > w) return 0;

  level_up (pb);
  if (row != h || col != w)  {
    int offset = h * (w + 2) + w - 1;
    char* m = disp->map + offset;
    unsigned int* a = disp->attr + offset;
    for (int r = h, c0 = 1; r >= row; r--)  {
      if (r == row) c0 = col;
      for (int c = w; c > c0; c--)
        scrc_put_char (disp, *m--, *a--, r, c);
      m -= 2;
      a -= 2;
      if (r != row) scrc_put_char (disp, *m--, *a--, r, c0);
    }
  }
  scrc_put_char (disp, ch, attr, row, col);
  level_down (pb);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_delete_line (Display *disp, int row)   {
  Pasteboard *pb = disp->pb;
  int h = disp->rows;
  int w = disp->cols;

  if (row < 1 || row > h) return 0;
  level_up (pb);
  if (row < h)  {
    int offset = (row + 1) * (w + 2) + 1;
    char* m = disp->map + offset;
    unsigned int* a = disp->attr + offset;
    for (int r = row; r < h; r++, m+=2, a+=2)
      for (int c = 1; c <= w; c++)
        scrc_put_char (disp, *m++, *a++, r, c);
  }
  scrc_erase_line (disp, h);
  level_down (pb);
  return 1;
}

//----------------------------------------------------------------------------
int scrc_delete_char (Display *disp, int row, int col)  {
  Pasteboard *pb = disp->pb;
  int h = disp->rows;
  int w = disp->cols;

  if (row < 1 || row > h) return 0;
  if (col < 1 || col > w) return 0;
  level_up (pb);
  if (row != h || col != w)  {
    int offset = row * (w + 2) + col + 1;
    char* m = disp->map + offset;
    unsigned int* a = disp->attr + offset;
    for (int c, r = row; r <= h; r++)  {
      for (c = col; c < w; c++)
        scrc_put_char (disp, *m++, *a++, r, c);
      m += 2;
      a += 2;
      if (r < h) scrc_put_char (disp, *m++, *a++, r, c);
      col = 1;
    }
  }
  scrc_put_char (disp, ' ', NORMAL, h, w);
  level_down (pb);
  return 1;
}

