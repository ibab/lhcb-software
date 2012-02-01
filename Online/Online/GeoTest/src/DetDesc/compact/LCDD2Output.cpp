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
    os << "++ Constant: " << val->GetName() << "  \"" << val->GetTitle() << "\"  Value:" << _toDouble(val->GetTitle()) << endl;
  }

  template <> void Printer<Material>::operator()(const Material& mat)  const  {
    RefElement_type<TGeoMedium>  val(mat);
    os << "++ Medium:" << val->GetName() << " " << val->GetTitle() << " id:" << hex << val->GetId() 
       << " Pointer:" << val->GetPointerName() << dec << endl;
    os << "|  ";
    val->Print();
  }

  template <> void Printer<VisAttr>::operator()(const VisAttr& val)  const  {
    VisAttr::Object* obj = val.data<VisAttr::Object>();
    TColor* col = gROOT->GetColor(obj->color);
    char text[256];
    ::sprintf(text," RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s",
	      col->AsHexString(), obj->color, col->GetAlpha(), int(obj->drawingStyle), int(obj->lineStyle),
	      obj->showDaughters ? "YES" : "NO", obj->visible ? "YES" : "NO");
    os << "++ VisAttr:  " << setw(32) << left << val->GetName() << text << endl;
  }

  template <> void Printer<Readout>::operator()(const Readout& val)  const  {
    val->Print();
  }

  template <> void Printer<Region>::operator()(const Region& val)  const  {
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

  template <> void Printer<Subdetector>::operator()(const Subdetector& val)  const  {
    Subdetector::Object* obj = val.data<Subdetector::Object>();
    if ( obj )  {
      char text[256];
      const Subdetector& sd = val;
      bool vis = sd.visAttr().isValid();
      bool rdo = sd.readout().isValid();
      bool env = sd.envelope().isValid();
      bool mat = sd.material().isValid();
      ::sprintf(text,"ID:%-3d Combine Hits:%3s Readout:%s Material:%s Envelope:%s VisAttr:%s",
		sd.id(), yes_no(sd.combineHits()), 
		rdo ? sd.readout().name()  : yes_no(rdo),
		mat ? sd.material().name() : yes_no(mat),
		env ? sd.envelope().name() : yes_no(env),
		yes_no(vis)
		);
      os << prefix << "+= Subdetector: " << val.name() << " " << val.type() << endl;
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
		  vol.name(), s.isValid() ? s.name() : "Unknonw", m.isValid() ? m.name() : "Unknown"
		  );
	os << prefix << "+-------------  " << text << endl;
      }
      const Subdetector::Children& ch = sd.children();
      for(Subdetector::Children::const_iterator i=ch.begin(); i!=ch.end(); ++i)
	Printer<Subdetector>(lcdd,os,prefix+"| ")((*i).second);
      return;
    }
  }

  template <class T> void PrintMap<T>::operator()()  const {
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
    PrintMap<Subdetector>(lcdd,os,lcdd.detectors(),    "List of Subdetectors")();
#endif
    //PrintMap<Subdetector>(lcdd,os,lcdd.detectors(),    "List of Subdetectors")();
    PrintMap<VisAttr   > (lcdd,os,lcdd.visAttributes(),"List of Visualization attributes")();
  }

}}
