#include "SCR/scrcc.h"
using namespace SCR;

//---------------------------------------------------------------------------
SCR::PasteboardContext scrcc_create_pasteboard(const char* dev) {
  PasteboardContext pb;
  pb.pb = 0;
  pb.rows = 0;
  pb.cols = 0;
  scrc_create_pasteboard(&pb.pb,dev,&pb.rows,&pb.cols);
  return pb;
}
SCR::Display* scrcc_create_display(int rows, int cols, int attr, flag border,const char* title) {
  Display* d = 0;
  scrc_create_display(&d,rows,cols,attr,border,title);
  return d;
}
