#include "AMS/amsdef.h"
#include "RTL/rtl.h"
#include <cstdio>
#include <cstdlib>

extern "C" int amsc_test(int /* ac  */, char** /* av */)  {
  const char *amsname = "testing";
  int loop;

  ::printf ("Starting test task \n");
  ::printf (" How many loops ?\n ");
  ::scanf ("%d", &loop);
  for (int i = 0; i < loop; i++)  {
    int ams_status = ::amsc_init (amsname);
    if (ams_status != AMS_SUCCESS)    {
      ::printf ("Can't initialise ams\n");
      ::exit (ams_status);
    }
    else    {
      ::printf (" opened ams loop number %d \n", i);
    }

    ams_status = ::amsc_close ();
    if (ams_status != AMS_SUCCESS)    {
      ::printf ("Can't  close  ams\n");
      ::exit (ams_status);
    }
    else    {
      ::printf (" closed ams loop number %d \n", i);

    }
  }
  return ::lib_rtl_default_return();
}
