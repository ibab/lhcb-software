#include "DetDesc/detector/ILDExTPC.h"
#include "DetDesc/Internals.h"
#include <iostream>

using namespace std;

namespace DetDesc {

  using namespace Geometry;

  ILDExTPC::ILDExTPC(const LCDD&, const string& name, const string& type, int id)
  {
    Value<TNamed,Object>* p = new Value<TNamed,Object>();
    assign(p,name, type);
    p->pressure = 12345.0;
    setAttr(Attr_id,id);
  }

  double ILDExTPC::getVolume() const {
    return 999.;
  }
  double ILDExTPC::getWeight() const {
    return 888.;
  }

  void ILDExTPC::setInnerWall(RefElement obj) {
    setAttr(outerWall,obj);
  }

  void ILDExTPC::setOuterWall(RefElement obj) {
    setAttr(innerWall,obj);
  }

  void ILDExTPC::setGasVolume(RefElement obj) {
    setAttr(gas,obj);
  }

}
