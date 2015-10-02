#ifndef FROM_FILE_GENERATOR_HH
#define FROM_FILE_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include "Mint/IEventGenerator.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/BaseGenerator.h"

#include "Mint/DalitzEvent.h"
#include "Mint/DiskResidentEventList.h"

#include "Mint/counted_ptr.h"

#include <string>

class FromFileGenerator 
: public BaseGenerator
, virtual public MINT::IEventGenerator<IDalitzEvent>{
 protected:
  std::string _opt;
  DiskResidentEventList _dL;
  bool _listExhausted;
  unsigned int _listIndex;
  MINT::IEventGenerator<IDalitzEvent>* _gen;

  virtual MINT::counted_ptr<IDalitzEvent> newEventFromGenerator();
 public:
  FromFileGenerator(const std::string& fname = "integrationEvents.root"
		    , const std::string& ntpName = "DalitzEventList");

  FromFileGenerator(const std::string& fname
		    , MINT::IEventGenerator<IDalitzEvent>* addThisWhenFileEmpty
		    , const std::string& opt=""
		    , const std::string& ntpName = "DalitzEventList"
		    );
  // option "" defaults to "UPDATE" if addThisWhenFileEmpty != 0 and
  // "READ" otherwise (this sets the option with which the root file
  // containing the events is opened). If you want to override this behaviour
  // set the option explicitly.

  virtual MINT::counted_ptr<IDalitzEvent> newDalitzEvent();

  // this one below is required by MINT::IEventGenerator<IDalitzEvent>
  // similar to above, except it's a pointer to 
  // IDalitzEvent, not DalitzEvent.
  virtual MINT::counted_ptr<IDalitzEvent> newEvent();
  virtual bool exhausted() const;
  virtual bool ensureFreshEvents();
  virtual bool saveFile();
  virtual bool closeFile();

  DiskResidentEventList& getEventList();
  const DiskResidentEventList& getEventList() const;

};

#endif
//
