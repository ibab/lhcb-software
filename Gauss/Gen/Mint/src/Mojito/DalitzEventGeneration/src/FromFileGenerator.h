#ifndef FROM_FILE_GENERATOR_HH
#define FROM_FILE_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include "IEventGenerator.h"
#include "IDalitzEvent.h"

#include "BaseGenerator.h"

#include "DalitzEvent.h"
#include "DiskResidentEventList.h"

#include "counted_ptr.h"

#include <string>

class FromFileGenerator 
: public BaseGenerator
, virtual public MINT::IEventGenerator<IDalitzEvent>{
 protected:
  std::string _opt;
  DiskResidentEventList _dL;
  bool _listExhausted;
  MINT::IEventGenerator<IDalitzEvent>* _gen;

  virtual MINT::counted_ptr<IDalitzEvent> newEventFromGenerator();
 public:
  FromFileGenerator(const std::string& fname = "integrationEvents.root");

  FromFileGenerator(const std::string& fname
		    , MINT::IEventGenerator<IDalitzEvent>* addThisWhenFileEmpty
		    , const std::string& opt="UPDATE"
		    );
  
  virtual MINT::counted_ptr<IDalitzEvent> newDalitzEvent();

  // this one below is required by MINT::IEventGenerator<IDalitzEvent>
  // similar to above, except it's a pointer to 
  // IDalitzEvent, not DalitzEvent.
  virtual MINT::counted_ptr<IDalitzEvent> newEvent();
  virtual bool exhausted() const;
  virtual bool ensureFreshEvents();
  virtual bool saveFile();
  virtual bool closeFile();
};

#endif
//
