// created by renkoe@tbkoenig.de
#if defined(_WIN32)
#if defined(__BORLANDC__) || (_MSC_VER >= 1200)
#pragma hdrstop
#endif

#include <windows.h>
#include "RTL/conioex.h"

extern "C" void TBK_CONIOEXAPI clrscr(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;
  DWORD                      dwWritten;
  COORD                      coordOrg;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

  coordOrg.X = 0;
  coordOrg.Y = 0;

  FillConsoleOutputAttribute(hOutput, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coordOrg, &dwWritten);
  FillConsoleOutputCharacter(hOutput, TEXT(' '), csbi.dwSize.X * csbi.dwSize.Y, coordOrg, &dwWritten);
  SetConsoleCursorPosition(hOutput, coordOrg);
}

extern "C" void TBK_CONIOEXAPI clreol(void)   {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;
  DWORD                      dwWritten;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

  FillConsoleOutputAttribute(hOutput, csbi.wAttributes, (csbi.dwSize.X - csbi.dwCursorPosition.X), csbi.dwCursorPosition, &dwWritten);
  FillConsoleOutputCharacter(hOutput, TEXT(' '), (csbi.dwSize.X - csbi.dwCursorPosition.X), csbi.dwCursorPosition, &dwWritten);
  SetConsoleCursorPosition(hOutput, csbi.dwCursorPosition);
}

extern "C" void TBK_CONIOEXAPI gotoxy(int x, int y)   {
  COORD  coordPos;

  if(HIWORD(x) || HIWORD(y))
    return;

  coordPos.X = LOWORD(x - 1);
  coordPos.Y = LOWORD(y - 1);

  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordPos);
}

extern "C" int TBK_CONIOEXAPI wherex(void)  {
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  csbi.dwCursorPosition.X = 0;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  return(csbi.dwCursorPosition.X + 1);
}

extern "C" int TBK_CONIOEXAPI wherey(void)    {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  csbi.dwCursorPosition.Y = 0;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  return(csbi.dwCursorPosition.Y + 1);
}

extern "C" void TBK_CONIOEXAPI insline(void)  {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  SMALL_RECT                 smrcScroll;
  CHAR_INFO                  ciFill;
  HANDLE                     hOutput;
  COORD                      coordDestOrg;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

#ifdef UNICODE
  ciFill.Char.UnicodeChar = L' ';
#else
  ciFill.Char.AsciiChar   =  ' ';
#endif

  ciFill.Attributes = csbi.wAttributes;

  smrcScroll.Left   = 0;
  smrcScroll.Top    = LOWORD(csbi.dwCursorPosition.Y);
  smrcScroll.Right  = LOWORD(csbi.dwSize.X - 1);
  smrcScroll.Bottom = LOWORD(csbi.dwSize.Y - 1);

  coordDestOrg.X    = 0;
  coordDestOrg.Y    = LOWORD(csbi.dwCursorPosition.Y + 1);

  ScrollConsoleScreenBuffer(hOutput, &smrcScroll, NULL, coordDestOrg, &ciFill);
}

extern "C" void TBK_CONIOEXAPI delline(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  SMALL_RECT                 smrcScroll;
  CHAR_INFO                  ciFill;
  HANDLE                     hOutput;
  COORD                      coordDestOrg;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

#ifdef UNICODE
  ciFill.Char.UnicodeChar = L' ';
#else
  ciFill.Char.AsciiChar   =  ' ';
#endif

  ciFill.Attributes = csbi.wAttributes;

  smrcScroll.Left   = 0;
  smrcScroll.Top    = LOWORD(csbi.dwCursorPosition.Y + 1);
  smrcScroll.Right  = LOWORD(csbi.dwSize.X - 1);
  smrcScroll.Bottom = LOWORD(csbi.dwSize.Y - 1);

  coordDestOrg.X    = 0;
  coordDestOrg.Y    = LOWORD(csbi.dwCursorPosition.Y);

  ScrollConsoleScreenBuffer(hOutput, &smrcScroll, NULL, coordDestOrg, &ciFill);
}

static void _addTextAttr(WORD attr)  {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;
  // Though Windows' console supports COMMON_LVB_REVERSE_VIDEO,	
  // it seems to be buggy.  So we must simulate it.	
  if (attr & COMMON_LVB_REVERSE_VIDEO)	{
    WORD at = csbi.wAttributes;
    attr = (attr & ~COMMON_LVB_REVERSE_VIDEO) | ((at & 0xF0) >> 4) | ((at & 0x0F) << 4);
    SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0xFF) | attr));
    return;
  }
  SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes | attr));
}

static void _delTextAttr(WORD attr)  {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

  if (attr & COMMON_LVB_REVERSE_VIDEO)	{
    WORD at = csbi.wAttributes;
    attr = (attr & ~COMMON_LVB_REVERSE_VIDEO) | ((at & 0xF0) >> 4) | ((at & 0x0F) << 4);
    SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0xFF) | attr));
    return;
  }
  SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes & ~attr));
}

// Scroll console up or down
extern "C" bool TBK_CONIOEXAPI consolescroll(bool Up) {
  HANDLE hStdout;
  CONSOLE_SCREEN_BUFFER_INFO scrbuf;
  SMALL_RECT srect;
  short diff;

  if(INVALID_HANDLE_VALUE == (hStdout = GetStdHandle(STD_OUTPUT_HANDLE)))
    return false;
  else if(!GetConsoleScreenBufferInfo(hStdout, &scrbuf) ) {	
    return false;
  }
  diff = scrbuf.srWindow.Bottom -scrbuf.srWindow.Top+1 ;
  if (Up)		diff = -diff;
  if ((scrbuf.srWindow.Top + diff > 0) && (scrbuf.srWindow.Bottom + diff < scrbuf.dwSize.Y)) {
    srect.Top = diff;
    srect.Bottom = diff;
    srect.Left = 0;
    srect.Right = 0;
    if (! SetConsoleWindowInfo( hStdout, FALSE, &srect)) {
      return false;
    }
  }
  return true;
}

extern "C" void TBK_CONIOEXAPI textcolor(int newcolor)    {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  else if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

  SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0x0F) | ((WORD)(newcolor & 0x0F))));
}

extern "C" void TBK_CONIOEXAPI textbackground(int newcolor)   {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

  SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0xF0) | ((WORD)((newcolor & 0x0F) << 4))));
}

extern "C" void TBK_CONIOEXAPI textattr(int newattr)  {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  else if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;

  SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0xFF) | (WORD)((newattr & 0xFF))));
}

extern "C" void TBK_CONIOEXAPI highvideo(void)    {
  _addTextAttr(FOREGROUND_INTENSITY);
}

extern "C" void TBK_CONIOEXAPI lowvideo(void)   {
  _delTextAttr(FOREGROUND_INTENSITY);
}

extern "C" void TBK_CONIOEXAPI inversevideo(void)   {
  _addTextAttr(COMMON_LVB_REVERSE_VIDEO);
}

extern "C" void TBK_CONIOEXAPI noinversevideo(void)   {
  _delTextAttr(COMMON_LVB_REVERSE_VIDEO);
}

extern "C" void TBK_CONIOEXAPI underlinevideo(void)   {
  _addTextAttr(COMMON_LVB_UNDERSCORE);
}

extern "C" void TBK_CONIOEXAPI nounderlinevideo(void) {
  _delTextAttr(COMMON_LVB_UNDERSCORE);
}

extern "C" void TBK_CONIOEXAPI blinkvideo(void)   {
  _addTextAttr(BACKGROUND_INTENSITY);
}

extern "C" void TBK_CONIOEXAPI noblinkvideo(void) {
  _delTextAttr(BACKGROUND_INTENSITY);
}

extern "C" void TBK_CONIOEXAPI normalvideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;
  WORD                       attr;
  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
    return;
  else if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return;
  attr = (WORD)(csbi.wAttributes & ~COMMON_LVB_REVERSE_VIDEO);
  attr = (attr & ~COMMON_LVB_UNDERSCORE);
  attr = (attr & ~BACKGROUND_INTENSITY);
  SetConsoleTextAttribute(hOutput, attr);
}

// returns the console size
extern "C" void TBK_CONIOEXAPI consolesize(int* rows, int* cols)      {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;
  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))  {
    *rows = 25;
    *cols = 80;
  }
  else if(!GetConsoleScreenBufferInfo(hOutput, &csbi))  {
    *rows = 25;
    *cols = 80;
  }
  else {
    *rows = csbi.srWindow.Bottom-csbi.srWindow.Top;
    *cols = csbi.srWindow.Right-csbi.srWindow.Left;
  }
}

extern "C" void TBK_CONIOEXAPI _setcursortype(int cur_t)
{
  CONSOLE_CURSOR_INFO cci;

  if((cur_t == _NOCURSOR) || (cur_t == _NORMALCURSOR))
    cci.dwSize = 25;
  else if(cur_t == _SOLIDCURSOR)
    cci.dwSize = 100;
  else
    return;

  cci.bVisible = (cur_t == _NOCURSOR) ? FALSE : TRUE;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);

  return;
}
#endif
