// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "FromFileGenerator.h"
#include <iostream>
using namespace std;
using namespace MINT;

FromFileGenerator::FromFileGenerator(const std::string& fname)
  : BaseGenerator()
  , _opt("READ")
  , _dL(fname)
  , _listExhausted(false)
  , _gen(0)
{
  _dL.Start();
}
FromFileGenerator::FromFileGenerator(const std::string& fname
				     , IEventGenerator<IDalitzEvent>* 
				                    addThisWhenFileEmpty
				     , const std::string& opt)
  : BaseGenerator()
  , _opt(opt)
  , _dL(fname, _opt)
  , _listExhausted(false)
  , _gen(addThisWhenFileEmpty)
{
  _dL.Start();
}

counted_ptr<IDalitzEvent> FromFileGenerator::newEventFromGenerator(){
  bool dbThis=false;

  if(0 == _gen) return counted_ptr<IDalitzEvent>(0);
  counted_ptr<IDalitzEvent> evtPtr(_gen->newEvent());

  if(mothers3MomentumIsSet() && 0 != evtPtr){
    evtPtr->setMothers3Momentum(mothers3Momentum());
  }

  if(_opt != "READ"){
    if(dbThis){
      cout << "FromFileGenerator::newEventFromGenerator():"
	   << " saving event to list"
	   << " opt = " << _opt << endl;
    }
    _dL.Add(evtPtr.get());
  }
  return evtPtr;
}
counted_ptr<IDalitzEvent> FromFileGenerator::newDalitzEvent(){
  if(_listExhausted) return newEventFromGenerator();
  if(_dL.Next()){
    counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(_dL.getEvent()));
    if(mothers3MomentumIsSet() && 0 != evtPtr){
      evtPtr->setMothers3Momentum(mothers3Momentum());
    }
    return evtPtr;
  }
  else _listExhausted=true;
  return newEventFromGenerator();
}

bool FromFileGenerator::exhausted() const{
  if(! _listExhausted) return false;
  if(0 == _gen) return true;
  return _gen->exhausted();
}

bool FromFileGenerator::ensureFreshEvents(){
  _listExhausted=true;
  if(0 != _gen) _gen->ensureFreshEvents();
  return true;
}
counted_ptr<IDalitzEvent> FromFileGenerator::newEvent(){
  return counted_ptr<IDalitzEvent>(newDalitzEvent());
}
