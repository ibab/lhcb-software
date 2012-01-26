#include "../src/XML/lcdd/XMLLCDDImp.h"
#include "../src/DetDesc/lcdd/LCDDGeoImp.h"
#include "TGDMLWrite.h"
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

Geometry::LCDD& compact2geo(int argc, char **argv)  {
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
  return *lcdd;
}
#include "TRint.h"
#include "DetDesc/detector/ILDExTPC.h"
#include "DetDesc/detector/MarkusTPC.h"
#include "DetDesc/compact/Detector.h"

//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  vector<char*> args;

  for(int i=0; i<argc;++i) args.push_back((char*)argv[i]);

  // Create an interactive ROOT application
  TRint *theApp = new TRint("Rint", &argc, argv);
  
  Geometry::LCDD& lcdd = compact2geo((int)args.size(),&args[0]);
  Geometry::Document doc = lcdd.document();
  TGeoManager* mgr = doc;
  TGDMLWrite wr;
  wr.WriteGDMLfile(mgr,"ILCEx.gdml","");
  mgr->Export("ILCEx.root");

  /// Print statement with inner radius.
  Geometry::Subdetector sd = lcdd.detector("TPC");
  cout << "-----> Bugger's properties:" << sd.name() << " " << sd.type() << endl;

  /// Another way to play around:
  ILDExTPC tpc = lcdd.detector("TPC");
  cout << "-----> Weight:" << tpc.getWeight() << " Volume:" << tpc.getVolume() << endl;


  MarkusTPC mtpc = tpc;
  cout << "----> MyFeature:" << mtpc.feature() << endl;

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
