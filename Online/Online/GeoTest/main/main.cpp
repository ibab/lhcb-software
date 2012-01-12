#include "XML/lcdd/XMLLCDDImp.h"
#include "DetDesc/lcdd/LCDDGeoImp.h"
#include <iostream>
#include <vector>
#include <string>


using namespace std;
using namespace DetDesc;

#if 0
#include "XML/CompactReader.h"
static int read_compact(int, char**)  {
  XML::CompactReader rdr("file:../cmt/compact.xml","lccdd");
  rdr.open();
  rdr.dump();
  return 0x1;
}
#endif

int compact2lcdd()  {
  XML::LCDDImp lcdd;
  lcdd.fromCompact("file:../cmt/compact.xml");
  dumpTree(lcdd.document());
  return 0x1;
}

Geometry::LCDDImp* compact2geo(int argc, char **argv)  {
  string input;
#ifdef _WIN32
  input = "file:../cmt/compact.xml";
#else
  //input = "file:/afs/cern.ch/user/f/frankb/scratch0/ONLINE/ONLINE_HEAD/Online/GeoTest/cmt/compact.xml";

  input = "compact.xml";
  //input = "file://../cmt/compact.xml";
  //input = "http://www.cern.ch/frankm/compact.xml";

#endif
  if ( argc>1 ) {
    input = argv[1];
  }
  Geometry::LCDDImp *lcdd = new Geometry::LCDDImp;
  cout << argc << " Input file : " << input << endl;
  lcdd->fromCompact(input);
  lcdd->dump();
  return lcdd;
}
#include "TRint.h"

//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  vector<char*> args;

  for(int i=0; i<argc;++i) args.push_back((char*)argv[i]);

  // Create an interactive ROOT application
  TRint *theApp = new TRint("Rint", &argc, argv);
  
  /* Geometry::LCDDImp *lcdd = */ compact2geo((int)args.size(),&args[0]);
  //delete lcdd->document().operator->();
  
  // and enter the event loop...
  theApp->Run();
  delete theApp;
  return 0;
}

int main(int argc,char** argv)  {
  //return read_compact();
  //return compact2lcdd();
  return run_interpreter(argc,argv);
}
