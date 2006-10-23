#include "MBMDump/MBMDump.h"

#define FMT_HEX08    " %08X "
#define FMT_DEC10      "%10d"

MBMDump::Format::Format()  {
  words_per_line  = 4;
  column_one_flag = 1;
  ascii_flag      = true;
  // default all words to hex....in case of dud string
  for(int i=0;i<MAX_WORDS_PER_LINE;i++)
    fmt[i] = FMT_HEX08;
}
