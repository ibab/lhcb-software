#include "XML/lcdd/XMLLCDD.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/compact/XMLSegmentations.h"
#include "XML/compact/XMLSupports.h"
#include "XML/compact/XMLReadouts.h"
#include "XML/compact/XMLDetector.h"
#include "XML/compact/XMLCalorimeter.h"
#include "XML/compact/XMLTracker.h"
#include "XML/compact/XMLSiTrackers.h"
#include "XML/compact/XMLForwardDetector.h"
#include "DetDesc/IDDescriptor.h"

#include <iostream>

using namespace std;
using namespace DetDesc;
using DetDesc::IDDescriptor;
using namespace DetDesc::XML;

namespace DetDesc { namespace XML {
  void _toDictionary(const XMLCh* name, const XMLCh* value);
}}

template <> void XMLConverter<Constant>::operator()(const Handle_t& element)  const  {
  Constant constant(lcdd.document(),element.attr<cpXMLCh>(Attr_name));
  constant.setAttrs(element);
  _toDictionary(element.attr<cpXMLCh>(Attr_name),element.attr<cpXMLCh>(Attr_value));
  lcdd.addConstant(constant);
}

template <> void XMLConverter<Material>::operator()(const Handle_t& element)  const  {
  lcdd.addMaterial(Material(lcdd.document().clone(element,true)));
}

template <> void XMLConverter<Atom>::operator()(const Handle_t& element)  const  {
  lcdd.addMaterial(Atom(lcdd.document().clone(element,true)));
}

template <> void XMLConverter<VisAttr>::operator()(const Handle_t& element)  const  {
  lcdd.addVisAttribute(VisAttr(lcdd.document(),element.attr<cpXMLCh>(Attr_name)).fromCompact(element));
}

template <> void XMLConverter<Readout>::operator()(const Handle_t& elt)  const {
  Readout       readout(elt);
  IDDescriptor  desd(readout.idSpec().text());
  IDSpec        spec(lcdd.document(),readout.name(),desd);
  lcdd.addReadout(readout);
  lcdd.addIDSpec(spec);
}

template <> void XMLConverter<LimitSet>::operator()(const Handle_t& elt)  const {
  LimitSet compact(elt);
  Document doc(lcdd.document());
  LimitSet ls(doc,compact.refName());
  for (Collection_t c(compact,Tag_limit); c; ++c)
    ls.append(doc.clone(c,true));
  lcdd.add(ls);
}

template <> void XMLConverter<Region>::operator()(const Handle_t& elt)  const {
  lcdd.addRegion(lcdd.document().clone(elt,true));
}

template <> void XMLConverter<Subdetector>::operator()(const Handle_t& elt)  const {
  Subdetector det(0);
  SensitiveDetector sens(0);
  Document          doc = lcdd.document();
  CompactDetector   compact(elt);
  const XMLCh* xname = compact.name();
  const XMLCh* xtype = compact.type();
  string        type = _toString(xtype);
  Readout    readout = lcdd.readout(compact.readout());

  if ( type == "PolyconeSupport" )
    det = PolyconeSupport(elt).fromCompact(lcdd,sens);
  else if ( type == "TubeSegment" )
    det = TubeSegment(elt).fromCompact(lcdd,sens);
  else if ( type == "MultiLayerTracker" )  {
    if ( readout ) sens = SensitiveDetector(doc,Tag_tracker,xname);
    det = MultiLayerTracker(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "DiskTracker" )  {
    if ( readout ) sens = SensitiveDetector(doc,Tag_tracker,xname);
    det = DiskTracker(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "SiTrackerBarrel" )  {
    sens = SensitiveDetector(doc,Tag_tracker,xname);
    det = SiTrackerBarrel(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "SiTrackerEndcap" )  {
    sens = SensitiveDetector(doc,Tag_tracker,xname);
    det = SiTrackerEndcap(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "SiTrackerEndcap2" )  {
    sens = SensitiveDetector(doc,Tag_tracker,xname);
    det = SiTrackerEndcap2(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "CylindricalBarrelCalorimeter" )  {
    sens = SensitiveDetector(doc,Tag_calorimeter,xname);
    det = CylindricalBarrelCalorimeter(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "CylindricalEndcapCalorimeter" )   {
    sens = SensitiveDetector(doc,Tag_calorimeter,xname);
    det = CylindricalEndcapCalorimeter(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "EcalBarrel" )   {
    sens = SensitiveDetector(doc,Tag_calorimeter,xname);
    det = EcalBarrel(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "PolyhedraEndcapCalorimeter2" )   {
    sens = SensitiveDetector(doc,Tag_calorimeter,xname);
    det = PolyhedraEndcapCalorimeter2(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "PolyhedraBarrelCalorimeter2" )   {
    sens = SensitiveDetector(doc,Tag_calorimeter,xname);
    det = PolyhedraBarrelCalorimeter2(elt).fromCompact(lcdd,sens);
  }
  else if ( type == "ForwardDetector" )   {
    sens = SensitiveDetector(doc,Tag_calorimeter,xname);
    det = ForwardDetector(elt).fromCompact(lcdd,sens);
  }
  else  {
    cout << "UNKNOWN detector:" << _toString(xname)
      << " of type " << _toString(xtype) << endl;
  }

  if ( sens.ptr() && readout.ptr() )  {
    Segmentation compact_seg = readout.segmentation();
    if ( compact_seg )  {
      Segmentation seg = compact_seg.fromCompact(doc);
      if ( seg ) sens.setSegmentation(seg);
    }
    sens.setHitsCollection(readout);
    sens.setIDSpec(readout);
    lcdd.addSensitiveDetector(sens);
  }
  else if ( sens )  {
    cout << "Sensitive detector without Readout present for :" << _toString(xname)
      << " of type " << _toString(xtype) << endl;
  }
  if ( det ) {
  }
}

template <> void XMLConverter<Materials>::operator()(const Handle_t& materials)  const  {
  Document doc(lcdd.document());
  Collection_t(materials,Tag_element ).for_each(XMLConverter<Atom>(lcdd));
  Collection_t(materials,Tag_material).for_each(XMLConverter<Material>(lcdd));
}

template <> void XMLConverter<Compact>::operator()(const Handle_t& element)  const  {
  Element  compact(element);
  Document doc(lcdd.create());

  lcdd.addStdMaterials();
  Header(lcdd.header()).fromCompact(doc,compact.child(Tag_info),Strng_t("In memory"));
  Collection_t(compact,Tag_define   ).for_each(Tag_constant,XMLConverter<Constant>(lcdd));
  Collection_t(compact,Tag_materials).for_each(Tag_element, XMLConverter<Atom>(lcdd));
  Collection_t(compact,Tag_materials).for_each(Tag_material,XMLConverter<Material>(lcdd));
  Collection_t(compact,Tag_limits   ).for_each(Tag_limitset,XMLConverter<LimitSet>(lcdd));
  Collection_t(compact,Tag_display  ).for_each(Tag_vis,     XMLConverter<VisAttr>(lcdd));
  Collection_t(compact,Tag_readouts ).for_each(Tag_readout, XMLConverter<Readout>(lcdd));
  Collection_t(compact,Tag_detectors).for_each(Tag_detector,XMLConverter<Subdetector>(lcdd));
  lcdd.endDocument();
}

template XMLConverter<Atom>;
template XMLConverter<Compact>;
template XMLConverter<Readout>;
template XMLConverter<VisAttr>;
template XMLConverter<Constant>;
template XMLConverter<LimitSet>;
template XMLConverter<Material>;
template XMLConverter<Materials>;
template XMLConverter<Subdetector>;
