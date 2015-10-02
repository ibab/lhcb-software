// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/FromFileGenerator.h"
#include <iostream>
using namespace std;
using namespace MINT;

FromFileGenerator::FromFileGenerator(const std::string& fname
				     , const std::string& ntpName)
  : BaseGenerator()
  , _opt("READ")
  , _dL(fname,1,ntpName)
  , _listExhausted(false)
  , _listIndex(0)
  , _gen(0)
{
  cout << "FromFileGenerator::FromFileGenerator(" << fname << ")" << endl;
  std::cout << "Got Intergrator events " << _dL.size() << std::endl;
}
FromFileGenerator::FromFileGenerator(const std::string& fname
				     , IEventGenerator<IDalitzEvent>* 
				     addThisWhenFileEmpty
				     , const std::string& opt
				     , const std::string& ntpName
				     )
  : BaseGenerator()
  , _opt(  ("" != opt ? opt : ( 0 == addThisWhenFileEmpty ? "READ" : "UPDATE") ) ) 
  , _dL(fname, 1, ntpName, _opt)
  , _listExhausted(false)
  , _listIndex(0)
  , _gen(addThisWhenFileEmpty)
{
  cout << " FromFileGenerator::FromFileGenerator(" << fname
       << ", " << addThisWhenFileEmpty
       << ", " << opt << ")" << endl;

  std::cout << "Got Intergrator events " << _dL.size() << std::endl;
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
    _listIndex++;
  }
  return evtPtr;
}
counted_ptr<IDalitzEvent> FromFileGenerator::newDalitzEvent(){
  if(_listExhausted) return newEventFromGenerator();
  if(_listIndex < _dL.size()){
    
    counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(_dL.getEvent(_listIndex)));
    if(mothers3MomentumIsSet() && 0 != evtPtr){
      evtPtr->setMothers3Momentum(mothers3Momentum());
    }
    _listIndex++;
    return evtPtr;
  }else{
    _listExhausted=true;
  }
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

bool FromFileGenerator::saveFile(){
  return _dL.save();
}
bool FromFileGenerator::closeFile(){
  return _dL.Close();
}

DiskResidentEventList& FromFileGenerator::getEventList(){
  return _dL;
}
const DiskResidentEventList& FromFileGenerator::getEventList() const{
  return _dL;
}


//======
//
