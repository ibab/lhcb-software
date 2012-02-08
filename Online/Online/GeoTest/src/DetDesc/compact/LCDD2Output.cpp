// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "Conversions.h"
#include "DetDesc/lcdd/LCDD.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/IDDescriptor.h"

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include <iostream>
#include <iomanip>

using namespace std;

namespace DetDesc { namespace Geometry {

  template <> void Printer<Constant>::operator()(const Constant& val)  const  {
    os << "++ Constant:" << val.toString() << endl;
  }

  template <> void Printer<Material>::operator()(const Material& mat)  const  {
    os << "++ Medium:  " << mat.toString() << "|  " << endl;
    mat->Print();
  }

  template <> void Printer<VisAttr>::operator()(const VisAttr& val)  const  {
    os << "++ VisAttr: " << val.toString() << endl;
  }

  template <> void Printer<Readout>::operator()(const Readout& val)  const  {
    os << "++ Readout: ";
    val->Print();
  }

  template <> void Printer<Region>::operator()(const Region& val)  const  {
    os << "++ Region:  ";
    val->Print();
  }

  template <> void Printer<Rotation>::operator()(const Rotation& val)  const  {
    os << "++ Rotation: ";
    val->Print();
  }

  template <> void Printer<Position>::operator()(const Position& val)  const  {
    os << "++ Position: ";
    val->Print();
  }

  template <> void Printer<LimitSet>::operator()(const LimitSet& val)  const  {
    TMap*   m = dynamic_cast<TMap*>(val.ptr());
    os << "++ LimitSet: ";
    val->TNamed::Print();
    m->TMap::Print();
  }

  template <> void Printer<DetElement>::operator()(const DetElement& val)  const  {
    DetElement::Object* obj = val.data<DetElement::Object>();
    if ( obj )  {
      char text[256];
      const DetElement& sd = val;
      bool vis = sd.visAttr().isValid();
      bool rdo = sd.readout().isValid();
      bool env = sd.envelope().isValid();
      bool mat = sd.material().isValid();
      ::sprintf(text,"ID:%-3d Combine Hits:%3s Readout:%s Material:%s Envelope:%s VisAttr:%s",
		sd.id(), yes_no(sd.combineHits()), 
		rdo ? sd.readout()->GetName()  : yes_no(rdo),
		mat ? sd.material()->GetName() : yes_no(mat),
		env ? sd.envelope()->GetName() : yes_no(env),
		yes_no(vis)
		);
      os << prefix << "+= DetElement: " << val->GetName() << " " << val.type() << endl;
      os << prefix << "|               " << text << endl;

      if ( vis )   {
	VisAttr attr  = sd.visAttr();
	VisAttr::Object* v = attr.data<VisAttr::Object>();
	TColor* col = gROOT->GetColor(v->color);
	char text[256];
	::sprintf(text," RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s",
		  col->AsHexString(), v->color, col->GetAlpha(), int(v->drawingStyle), int(v->lineStyle),
		  v->showDaughters ? "YES" : "NO", v->visible ? "YES" : "NO");
	os << prefix << "|               VisAttr:  " << setw(32) << left << attr.name() << text << endl;
      }
      if ( sd.volume().isValid() )  {
	Volume vol = sd.volume();
	Solid    s = vol.solid();
	Material m = vol.material();
	::sprintf(text,"Volume:%s Shape:%s Material:%s",
		  vol->GetName(), s.isValid() ? s.name() : "Unknonw", m.isValid() ? m->GetName() : "Unknown"
		  );
	os << prefix << "+-------------  " << text << endl;
      }
      const DetElement::Children& ch = sd.children();
      for(DetElement::Children::const_iterator i=ch.begin(); i!=ch.end(); ++i)
	Printer<DetElement>(lcdd,os,prefix+"| ")((*i).second);
      return;
    }
  }

  template <typename T> void PrintMap<T>::operator()()  const {
    Printer<T> p(lcdd,os);
    os << "++" << endl << "++          " << text << endl << "++" << endl;
    for (LCDD::HandleMap::const_iterator i=cont.begin(); i != cont.end(); ++i) 
      p((*i).second);
  }

  template <> void Printer<const LCDD*>::operator()(const LCDD*const&)  const  {
    //Header(lcdd.header()).fromCompact(doc,compact.child(Tag_info),Strng_t("In memory"));
#if 0
    PrintMap<Constant  > (lcdd,os,lcdd.constants(),    "List of Constants")();
    PrintMap<Material  > (lcdd,os,lcdd.materials(),    "List of Materials")();
    PrintMap<VisAttr   > (lcdd,os,lcdd.visAttributes(),"List of Visualization attributes")();
    PrintMap<Position  > (lcdd,os,lcdd.positions(),    "List of Positions")();
    PrintMap<Rotation  > (lcdd,os,lcdd.rotations(),    "List of Rotations")();
    PrintMap<LimitSet  > (lcdd,os,lcdd.readouts(),     "List of Readouts")();
    PrintMap<Region    > (lcdd,os,lcdd.regions(),      "List of Regions")();
    PrintMap<DetElement> (lcdd,os,lcdd.detectors(),    "List of DetElements")();
#endif
    //PrintMap<DetElement>(lcdd,os,lcdd.detectors(),   "List of DetElements")();
    PrintMap<VisAttr   > (lcdd,os,lcdd.visAttributes(),"List of Visualization attributes")();
  }

}}
