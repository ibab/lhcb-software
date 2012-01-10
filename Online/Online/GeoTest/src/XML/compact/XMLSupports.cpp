#include "XML/compact/XMLSupports.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/lcdd/XMLShapes.h"
#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLLCDD.h"

using namespace std;
using namespace DetDesc::XML;


Subdetector& PolyconeSupport::fromCompact(LCDD& lcdd, const Handle_t& sens)  {
  Document   doc     = lcdd.document();
  Handle_t   compact = handle();
  Tag_t      support = name();
  Material   mat     = lcdd.material(material());
  Polycone   cone    (doc,support+"_envelope_polycone");
  Volume     volume  (doc,support+"_envelope_volume", cone, mat);
  PhysVol    physvol (doc,volume);

  cone.addZPlanes(compact);
  lcdd.add(cone);
  lcdd.add(volume);
  setVisAttributes(lcdd, compact, volume);
  Volume(lcdd.pickMotherVolume(*this)).addPhysVol(physvol);
  return *this;
}

Subdetector& TubeSegment::fromCompact(LCDD& lcdd, const Handle_t& sens)   {
  Document   doc     = lcdd.document();
  Handle_t   compact = handle();
  Tag_t      support = name();
  Material   mat     = lcdd.material(material());
  Volume     mother  = isInsideTrackingVolume() ? lcdd.trackingVolume() : lcdd.worldVolume();
  Tube       tube    (doc,support+"_tube");
  Position   pos     (doc,support+"_position");
  Rotation   rot     (doc,support+"_rotation");
  Volume     vol     (doc,support,tube,mat);
  PhysVol    physvol (doc,vol);

  tube.fromCompact(compact.child(Tag_tubs));
  pos.fromXML(compact.child(Tag_position,false));
  rot.fromXML(compact.child(Tag_rotation,false));  
  lcdd.add(tube);
  lcdd.add(pos);
  lcdd.add(rot);
  lcdd.add(vol);
  setVisAttributes(lcdd, compact, vol);
  physvol.setRotation(rot).setPosition(pos).addPhysVolID(Attr_id,id());
  mother.addPhysVol(physvol);
  return *this;
}
