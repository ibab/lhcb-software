#include "XML/lcdd/XMLLCDDImp.h"
#include "DetDesc/lcdd/LCDDGeoImp.h"

using namespace DetDesc;


int compact2lcdd()  {
  XML::LCDDImp lcdd;
  lcdd.fromCompact("file:../cmt/compact.xml");
  dumpTree(lcdd.document());
  return 0x1;
}

int compact2geo()  {
  Geometry::LCDDImp lcdd;
  lcdd.fromCompact("file:../cmt/compact.xml");
  lcdd.dump();
  return 0x1;
}

struct compact2geo_test  {
  compact2geo_test()  {
    //compact2lcdd();
    compact2geo();
  }
};

static compact2geo_test obj;
