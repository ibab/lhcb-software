#include <vector>
#include <MBM/bmdef.h>

extern "C" int mep_install(int argc , char** argv) {
  std::vector<char*> opts;
  static char* type = "mbm_install";
  opts.push_back(type);
  for(size_t i=0; i<size_t(argc); ++i)  {
    opts.push_back(argv[i]);
    if ( (argv[i][0] == '-' || argv[i][0] == '/') && ::toupper(argv[i][1]) == 'C' ) {
      mbm_install(opts.size(), &opts[0]);
      opts.clear();
      opts.push_back(type);
    }
  }
  return mbm_install(opts.size(), &opts[0]);
}
