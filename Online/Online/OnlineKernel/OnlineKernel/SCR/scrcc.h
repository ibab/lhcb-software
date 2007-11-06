#include "SCR/scr.h"
extern "C" SCR::PasteboardContext scrcc_create_pasteboard(const char* device);
extern "C" SCR::Display* scrcc_create_display(int rows, int cols, int attr, SCR::flag border,const char* title);
