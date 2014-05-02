#include "Mint/MakePreIntegratedAmplitudes.h"

#include "Mint/SignalGenerator.h"
#include "Mint/FromFileGenerator.h"

#include "Mint/FastAmplitudeIntegrator.h"
#include "Mint/FitAmpSum.h"

#include <iostream>

using namespace std;
using namespace MINT;

MakePreIntegratedAmplitudes::
MakePreIntegratedAmplitudes( const DalitzEventPattern& pat
			     , const std::string& opt
			     , double prec
			     , const std::string& integEventsFname
			     , const std::string& generateEventsIfNeeded
			     , const std::string& fnamePrefix
			     , TRandom* rnd
			     )
  : _pat(pat)
  , _opt(opt)
  , _prec(prec)
  , _fname("noName")
  , _eventsFname(integEventsFname)
  , _generateEventsIfNeeded("DoGenerate" == generateEventsIfNeeded)
  , _rnd(rnd)
{
  makeFname(fnamePrefix);
  if("" == _eventsFname) makeEventsFname();
}

const std::string&
MakePreIntegratedAmplitudes::
makeFname(const std::string& prefix){

  if(_pat.empty()){
    _fname = prefix + "noPattern";
    return _fname;
  }
  std::string name = _pat[0].name();
  if(includeCPCon()){
    DecayTreeItem antiMum = _pat[0];
    antiMum.antiThis();
    name += "_" + antiMum.name();
  }
  name += "_to_";
  if(_pat.size() < 2){
    _fname = prefix + name;
    return _fname;
  }
  for(unsigned int i=1; i < _pat.size(); i++){
    name += _pat[i].name();
  }
  _fname = prefix + name;
  return _fname;
}

const std::string&
MakePreIntegratedAmplitudes::makeEventsFname(){
  if(_eventsFname == "") _eventsFname = defaultEventsFname();
  return _eventsFname;
}
std::string
MakePreIntegratedAmplitudes::defaultEventsFname() const{
  return "IntegEvents" + _fname + ".root";
}
bool MakePreIntegratedAmplitudes::includeCPCon()const{
  //  return false; // for now.
  return (_pat[0].props()->charge() == "0" && _pat[0].hasDistinctAnti());
}

bool MakePreIntegratedAmplitudes::doit(){

  FitAmpSum fitAmps(_pat, "", _opt);
  if(includeCPCon()){
    DalitzEventPattern antiPat = _pat;
    antiPat[0].antiThis();
    FitAmpSum antiFa(antiPat, _opt);
    fitAmps.add(antiFa);
  }
  fitAmps.setAllAmpsTo(std::complex<double>(1,0));

  counted_ptr<SignalGenerator> genSgPtr(0);
  if(_generateEventsIfNeeded){
    cout << "MakePreIntegratedAmplitudes::doit()"
	 << " will generate events if I don't find enough in file "
	 << eventsFname()
	 << " to reach precision " << _prec
	 << endl;
    counted_ptr<SignalGenerator> ptr(new SignalGenerator(_pat, &fitAmps));
    genSgPtr = ptr;
  }else{
    cout << "MakePreIntegratedAmplitudes::doit()"
	 << " will NOT generate my own events "
	 << " even if I don't find enough in file "
	 << eventsFname()
	 << " to reach precision " << _prec
	 << endl;
  }
  FromFileGenerator ffg(eventsFname(), genSgPtr.get());
  FastAmplitudeIntegrator integ(_pat, &fitAmps, &ffg, _rnd, _prec);
  integ.save(fname());

  cout << "Saved integration result for " << _pat
       << " in this file: "
       << fname()
       << endl;
  return true;
}
