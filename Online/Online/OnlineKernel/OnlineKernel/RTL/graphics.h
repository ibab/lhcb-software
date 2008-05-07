// $Id: graphics.h,v 1.1 2008-05-07 18:12:32 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : OnlineKernel
//
//  Description: Simple screen display graphics
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineKernel/OnlineKernel/RTL/graphics.h,v 1.1 2008-05-07 18:12:32 frankb Exp $
#ifndef ONLINEKERNEL_GRAPHICS_H
#define ONLINEKERNEL_GRAPHICS_H

#include <cstdio>

namespace graphics {
  void  consolesize(size_t* rows, size_t* cols);
  void  gotoxy(int x,int y);
  void  bold();
  void  nobold();
  void  dimmed();
  void  nodimmed();
  void  inverse();
  void  noinverse();
  void  underline();
  void  nounderline();
  void  flash();
  void  noflash();
  void  normal();

  void  clear_screen();
  void  plain();
  void  narrow_screen();
  void  wide_screen();
  void  cursor_on();
  void  cursor_off();

  void  red();
  void  yellow();
  void  white();
  void  green();
  void  blue();
  void  cyan();
  void  black();
  void  magenta();

  void  bg_red();
  void  bg_yellow();
  void  bg_white();
  void  bg_green();
  void  bg_blue();
  void  bg_cyan();
  void  bg_black();
  void  bg_magenta();
}
#endif /* ONLINEKERNEL_GRAPHICS_H */
