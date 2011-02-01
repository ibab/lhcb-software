#include "merge/merge.C"
#include <cstdlib>

static int usage() {
  ::printf("Gaudi merge facility for ROOT tree based files.\n"
	   " Usage: \n"
	   "gaudi_merge -o <output-file> -i <input-file 1> [ -i <input-file 2> ...]\n\n"
	   "input- and output files may specify any legal (ROOT) file name.\n" 
	   "   -output      Specify output file name.\n"
	   "   -input       Specify input file name.\n"
	   "   -debug       Switch debug flag on.\n"
	   );
  return 1;
}

int main(int argc, char** argv) {
  bool dbg = false, fixup=true;
  vector<string> input;
  string output;
  for(int i=1; i < argc; ++i) {
    if ( *argv[i] == '-' ) {
      switch(::toupper(*(argv[i]+1))) {
      case 'N':
	fixup = false;
	break;
      case 'D':
	dbg = true;
	break;
      case 'O':
	if ( i+1 < argc ) output = argv[i+1];
	++i;
	break;
      case 'I':
	if ( i+1 < argc ) input.push_back(argv[i+1]);
	++i;
	break;
      default:
	return usage();
      }
    }
  }
  if ( input.empty() ) {
    ::printf("\nERROR: No input file(s) supplied\n\n");
    return usage();
  }
  else if ( output.empty() ) {
    ::printf("\nERROR: No output file supplied.\n\n");
    return usage();
  }
  gROOT->SetBatch(kTRUE);
  for(size_t i=0; i<input.size();++i)  {
    const string& in = input[i];
    bool do_fixup = fixup && ((i+1)==input.size());
    //::printf("+++ Target:%s\n+++ Source file:%s Fixup:%s Dbg:%s %d %d\n",
    //         output.c_str(),in.c_str(),do_fixup ? "YES" : "NO",dbg ? "YES" : "NO",i,input.size());
    int result = merge(output.c_str(),in.c_str(),do_fixup,dbg);
    if ( result == MERGE_ERROR ) {
      printf("\nERROR: File merge failed after %ld files.\n\n",long(i+1));
      return 1;
    }
  }
  return 0;
}
