#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"
#include "TAN/TanInterface.h"

extern "C" int amsc_qmtest(int /* ac  */, char** /* av */)  {
  ::putenv((char*)"TAN_PORT=YES");
  ::system("test.exe  tan_nameserver -a -d&");
  ::lib_rtl_sleep(2000);
  ::system("test.exe  amsc_bounce -l=555 -n=READER_0 -t=5000&");
  ::lib_rtl_sleep(2000);
  ::system("test.exe  amsc_bounce -l=555 -s=READER_0 -n=SENDER_0 -t=5000");
  ::tan_shutdown(0);
  ::lib_rtl_sleep(2000);
  return 0;
}
