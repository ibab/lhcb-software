#ifndef DALITZEVENPTRTLIST_HH
#define DALITZEVENPTRTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <string>

#include "EventList.h"
#include "IDalitzEvent.h"
#include "IDalitzEventList.h"
#include "DalitzEventPattern.h"
//#include "IDalitzEventAccess.h"
//#include "IEventAccess.h"


#include "IReturnReal.h"

#include "counted_ptr.h"

#include "PlotSet.h"
#include "CLHEPSystemOfUnits.h"


class TRandom;
class TH1D;
class TH2D;
class TNtupleD;

class FitAmpSum;

namespace MINT{
  template<>
    IDalitzEvent* EventList<IDalitzEvent, counted_ptr<IDalitzEvent> >::getEvent();
  
  template<>
    const IDalitzEvent* 
    EventList<IDalitzEvent, counted_ptr<IDalitzEvent> >::getEvent() const;
  
  template<>
    const IDalitzEvent* 
    EventList<IDalitzEvent, counted_ptr<IDalitzEvent> >::getREvent(unsigned int i) const;
}

class DalitzEventPtrList : 
public MINT::EventList<IDalitzEvent, MINT::counted_ptr<IDalitzEvent> >
{
  static const std::string _className;
 protected:

 public:
  DalitzEventPtrList();
  DalitzEventPtrList(const DalitzEventPtrList& other);

  const std::string& className() const{return _className;}

  int generatePhaseSpaceEvents(int NumEvents
			       , const DalitzEventPattern& pat
			       , TRandom* rnd=0
			       );
  int generateEvents(unsigned int NumEvents
		     , const DalitzEventPattern& pat
		     , MINT::IGetRealEvent<IDalitzEvent>* amps
		     , TRandom* rnd
		     );

};

#endif
//
