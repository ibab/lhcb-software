// created by renkoe@tbkoenig.de
#if defined(_WIN32)
#if defined(__BORLANDC__) || (_MSC_VER >= 1200)
  #pragma hdrstop
#endif

#include <windows.h>
#include "RTL/conioex.h"

void TBK_CONIOEXAPI clrscr(void)
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

void TBK_CONIOEXAPI clreol(void)
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

void TBK_CONIOEXAPI gotoxy(int x, int y)
{
  COORD  coordPos;

    if(HIWORD(x) || HIWORD(y))
        return;

    coordPos.X = LOWORD(x - 1);
    coordPos.Y = LOWORD(y - 1);

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordPos);

  return;
}

int TBK_CONIOEXAPI wherex(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

    csbi.dwCursorPosition.X = 0;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  return(csbi.dwCursorPosition.X + 1);
}

int TBK_CONIOEXAPI wherey(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;

    csbi.dwCursorPosition.Y = 0;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

  return(csbi.dwCursorPosition.Y + 1);
}

void TBK_CONIOEXAPI insline(void)
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

void TBK_CONIOEXAPI delline(void)
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

void TBK_CONIOEXAPI textcolor(int newcolor)
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

void TBK_CONIOEXAPI textbackground(int newcolor)
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

void TBK_CONIOEXAPI textattr(int newattr)
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

void TBK_CONIOEXAPI highvideo(void)
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

void TBK_CONIOEXAPI lowvideo(void)
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

void TBK_CONIOEXAPI _setcursortype(int cur_t)
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
