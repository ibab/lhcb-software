#include <cctype>
#include <cstdio>
#include <vector>
#include <MBM/bmdef.h>

extern "C" int mep_install(int argc , char** argv) {
  std::vector<char*> opts;
  static char type[64] = "mbm_install";
  opts.push_back(type);
  for(size_t i=0; i<size_t(argc); ++i)  {
    opts.push_back(argv[i]);
    if ( (argv[i][0] == '-' || argv[i][0] == '/') && ::toupper(argv[i][1]) == 'C' ) {
      int sc = mbm_install(opts.size(), &opts[0]);
      if ( sc != MBM_NORMAL )  {
        ::lib_rtl_output(LIB_RTL_ERROR,"Unable to install MBM buffers...\n");
        return sc;
      }
      opts.clear();
      opts.push_back(type);
    }
  }
  if ( opts.size() > 1 )  {
    return mbm_install(opts.size(), &opts[0]);
  }
  return 1;
}
