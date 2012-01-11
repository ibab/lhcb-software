#include "XML/lcdd/XMLLCDDImp.h"
#include "DetDesc/lcdd/LCDDGeoImp.h"

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

Geometry::LCDDImp* compact2geo()  {
  Geometry::LCDDImp *lcdd = new Geometry::LCDDImp;
  lcdd->fromCompact("file:../cmt/compact.xml");
  lcdd->dump();
  return lcdd;
}
#include "TRint.h"

//______________________________________________________________________________
int run_interpreter(int argc, char **argv)
{
   // Create an interactive ROOT application
   TRint *theApp = new TRint("Rint", &argc, argv);

   /* Geometry::LCDDImp *lcdd = */ compact2geo();
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
