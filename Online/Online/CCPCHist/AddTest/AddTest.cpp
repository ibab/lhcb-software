#include "HistAdder.h"
#include "CounterAdder.h"

main (int argc, char *argv[])
{
  HistAdder *histadd, *EORadd;
  CounterAdder *cntadd, *cntEORadd;
  char *tsk, *prefix;
  switch(argc)
  {
  case 1:
    {
      tsk = "shit";
      prefix = "ADDER";
      break;
    }
  case 2:
    {
      tsk = argv[1];
      prefix = "";
      break;
    }
  case 3:
    {
      tsk = argv[1];
      prefix = argv[2];
      break;
    }
  }
  histadd = new HistAdder(tsk ,prefix,"/Hist",false);
  EORadd = new HistAdder(tsk,prefix,"/HistEOR");
  cntadd = new CounterAdder(tsk,prefix,"/Counter",true);
  cntEORadd = new CounterAdder(tsk,prefix,"/CounterEOR");
  sleep(~0);
}