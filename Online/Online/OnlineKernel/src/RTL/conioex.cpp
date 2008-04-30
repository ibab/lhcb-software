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

  return;
}

extern "C" void TBK_CONIOEXAPI clreol(void)
{
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

  return;
}

extern "C" void TBK_CONIOEXAPI gotoxy(int x, int y)
{
  COORD  coordPos;

    if(HIWORD(x) || HIWORD(y))
        return;

    coordPos.X = LOWORD(x - 1);
    coordPos.Y = LOWORD(y - 1);

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordPos);

  return;
}

extern "C" int TBK_CONIOEXAPI wherex(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

    csbi.dwCursorPosition.X = 0;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  return(csbi.dwCursorPosition.X + 1);
}

extern "C" int TBK_CONIOEXAPI wherey(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

    csbi.dwCursorPosition.Y = 0;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  return(csbi.dwCursorPosition.Y + 1);
}

extern "C" void TBK_CONIOEXAPI insline(void)
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
    smrcScroll.Top    = LOWORD(csbi.dwCursorPosition.Y);
    smrcScroll.Right  = LOWORD(csbi.dwSize.X - 1);
    smrcScroll.Bottom = LOWORD(csbi.dwSize.Y - 1);

    coordDestOrg.X    = 0;
    coordDestOrg.Y    = LOWORD(csbi.dwCursorPosition.Y + 1);

    ScrollConsoleScreenBuffer(hOutput, &smrcScroll, NULL, coordDestOrg, &ciFill);
  return;
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
  return;
}

extern "C" void TBK_CONIOEXAPI textcolor(int newcolor)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0x0F) | ((WORD)(newcolor & 0x0F))));
  return;
}

extern "C" void TBK_CONIOEXAPI textbackground(int newcolor)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0xF0) | ((WORD)((newcolor & 0x0F) << 4))));
  return;
}

extern "C" void TBK_CONIOEXAPI textattr(int newattr)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)((csbi.wAttributes & ~0xFF) | (WORD)((newattr & 0xFF))));
  return;
}

extern "C" void TBK_CONIOEXAPI highvideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes | FOREGROUND_INTENSITY));
  return;
}

extern "C" void TBK_CONIOEXAPI lowvideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes & ~FOREGROUND_INTENSITY));
  return;
}

extern "C" void TBK_CONIOEXAPI inversevideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes | COMMON_LVB_REVERSE_VIDEO));
  return;
}

extern "C" void TBK_CONIOEXAPI underlinevideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes | COMMON_LVB_UNDERSCORE));
  return;
}

extern "C" void TBK_CONIOEXAPI blinkvideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;

    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;

    SetConsoleTextAttribute(hOutput, (WORD)(csbi.wAttributes | BACKGROUND_INTENSITY));
  return;
}

extern "C" void TBK_CONIOEXAPI normalvideo(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;
  WORD                       attr;
    if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))
        return;
    if(!GetConsoleScreenBufferInfo(hOutput, &csbi))
        return;
    attr = (WORD)(csbi.wAttributes & ~COMMON_LVB_REVERSE_VIDEO);
    attr = (attr & ~COMMON_LVB_UNDERSCORE);
    attr = (attr & ~BACKGROUND_INTENSITY);
    SetConsoleTextAttribute(hOutput, attr);
  return;
}

// returns the console size
extern "C" void TBK_CONIOEXAPI consolesize(int* rows, int* cols)  
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE                     hOutput;
  if(INVALID_HANDLE_VALUE == (hOutput = GetStdHandle(STD_OUTPUT_HANDLE)))  {
    *rows = 25;
    *cols = 80;
    return;
  }
  if(!GetConsoleScreenBufferInfo(hOutput, &csbi))  {
    *rows = 25;
    *cols = 80;
    return;
  }
  *rows = csbi.srWindow.Bottom-csbi.srWindow.Top;
  *cols = csbi.srWindow.Right-csbi.srWindow.Left;
  return;
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
