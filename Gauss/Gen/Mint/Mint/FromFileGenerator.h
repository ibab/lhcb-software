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
  MINT::IEventGenerator<IDalitzEvent>* _gen;

  virtual MINT::counted_ptr<IDalitzEvent> newEventFromGenerator();
 public:
  FromFileGenerator(const std::string& fname = "integrationEvents.root");

  FromFileGenerator(const std::string& fname
		    , MINT::IEventGenerator<IDalitzEvent>* addThisWhenFileEmpty
		    , const std::string& opt="UPDATE"
		    );
  // alternative option: "READ", which makes sure it only takes
  // events from the file, and does not generate new ones
  // when it runs out.

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
