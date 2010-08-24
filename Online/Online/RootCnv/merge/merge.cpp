#include "merge/merge.C"
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

static int usage() {
  cout << "POOL merge facility for ROOT tree based files.\n"
    " Usage: \n"
    "mergePool -o <output-file> -i <input-file 1> [ -i <input-file 2> ...]\n\n"
    "input- and output files may consist of any legal file name.\n" 
    " -debug       Switch debug flag on.\n"
       << endl;
  return pool::ERROR;
}

int main(int argc, char** argv) {
  bool dbg = false;
  vector<string> input;
  string output;
  for(int i=1; i < argc; ++i) {
    if ( *argv[i] == '-' ) {
      switch(::toupper(*(argv[i]+1))) {
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
    cout << "No input files supplied" << endl;
    return usage();
  }
  else if ( output.empty() ) {
    cout << "No output file supplied" << endl;
    return usage();
  }
  gROOT->SetBatch(kTRUE);
  for(size_t i=0; i<input.size();++i)  {
    const string& in = input[i];
    bool fixup = ((i+1)==input.size());
    int result = merge(output.c_str(),in.c_str(),fixup,dbg);
    if ( result == pool::ERROR ) {
      cout << "File merge failed after " << i+1 << " files.";
      return pool::ERROR;
    }
  }
  return pool::SUCCESS;
}
