#include "merge/extractEvt.C"
#include <cstdlib>

static int usage() {
  ::printf("Gaudi event extraction facility for ROOT tree based files.\n"
	   " Usage: \n"
	   "extract_event -o <output-file> -i <input-file> ...]\n\n"
	   "input- and output files may specify any legal (ROOT) file name.\n" 
	   "   -output      Specify output file name.\n"
	   "   -input       Specify input file name.\n"
	   "   -event       Specify the event entry number.\n"
	   );
  return 1;
}

int main(int argc, char** argv) {
  string input;
  string output;
  int evt_num = -1;
  for(int i=1; i < argc; ++i) {
    if ( *argv[i] == '-' ) {
      switch(::toupper(*(argv[i]+1))) {
      case 'E':
	if ( 1 != ::sscanf(argv[i+1],"%d",&evt_num) )  {
	  ::printf("\nERROR: No valid event identifier given.\n\n");
	  return usage();
	}
	++i;
	break;
      case 'O':
	if ( i+1 < argc ) output = argv[i+1];
	++i;
	break;
      case 'I':
	if ( i+1 < argc ) input = argv[i+1];
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
  else if ( evt_num < 0 )  { 
    ::printf("\nERROR: No valid event identifier given.\n\n");
    return usage();
  }
  gROOT->SetBatch(kTRUE);
  if ( extract_event(input.c_str(),output.c_str(),evt_num) != EXTRACT_SUCCESS ) {
    printf("\nERROR: Event extraction from file %s failed.\n",input.c_str());
    return 1;
  }
  return 0;
}
