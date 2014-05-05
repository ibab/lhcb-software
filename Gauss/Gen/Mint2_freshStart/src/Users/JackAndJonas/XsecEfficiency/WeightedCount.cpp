#include "WeightedCount.h"
#include "Mint/IDalitzPdf.h"
#include "readPhilipsNtuple.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzHistoSet.h"

#include "Mint/counted_ptr.h"

#include "Mint/IGetDalitzEvent.h"

#include <iostream>
#include "Mint/Utils.h"

using namespace std;
using namespace MINT;

WeightedCount::WeightedCount(const std::string& name
			     , IGetDalitzEvent* LHCb
			     , const std::vector<IGetDalitzEvent*>& MarkIII
			     , readPhilipsNtuple* ntp
			     )
  : _LHCbPdf(LHCb)
  , _MarkIIIPdf(MarkIII)
  , _ntpReader(ntp)
  , _name(name)
  , _counter(_name)
  , _unweightedList(_name + "_unweighteEventList.root", "RECREATE")
  , _reweightedList(_name + "_reweighteEventList.root", "RECREATE")
{

}

bool WeightedCount::OK() const{
  if(0 == _LHCbPdf) return false;
  if(0 == _MarkIIIPdf.size()) return false;
  if(0 == _ntpReader) return false;
  if(0 == _ntpReader->numEvents() ) return false;
  return true;
}

bool WeightedCount::doCount(){
  if(! OK()){
    cout << "WeightedCount::doCount, " << name() << ": I'm not OK" << endl;
    return false;
  }
  int numEvents=0;
  cout << "WeightedCount::doCount(): doing " << _ntpReader->numEvents() << " event." << endl;
  for(unsigned int i=0; i < _ntpReader->numEvents(); i++){
    counted_ptr<DalitzEvent> evtPtr = _ntpReader->readEntry(i);
    if(0 == evtPtr){
      _ntpReader->setAllModelWeights(-2);
      continue;
    }
  
    double pt = _ntpReader->mcpt();
    double y  = _ntpReader->mcycm();
    double ylab  = _ntpReader->mcylab();
    if(false && 0 == i%500){
      cout << "pt, y, ylab " << pt << ", " << y << ", " << ylab << endl;
      cout << _ntpReader->pD() << " , " << _ntpReader->mcpD() << endl;
    }

    
    //if(y < 1.5 || y > 5.5) continue;
    //if(ylab < 1.5 || ylab > 6) continue;
    // if(ylab < 1.0 || ylab > 6) {
    //  _ntpReader->setModelWeight(-1);
    //  continue;
    //}
    //if(pt < 0 || pt > 16*GeV) continue;
    //if(y < 2 || y > 4.5) continue;
    //if(pt < 0 || pt > 8*GeV) continue;
    
    numEvents++;

    DalitzEvent evt(*evtPtr);
    _unweightedList.Add(evt);

    _LHCbPdf->setEvent(&evt);
    double wL = _LHCbPdf->RealVal();

    vector<double> weightList(_MarkIIIPdf.size(), -9999.0);
    for(unsigned int j=0; j < _MarkIIIPdf.size(); j++){
      if(0 == _MarkIIIPdf[j]) continue;
      _MarkIIIPdf[j]->setEvent(&evt);
      double wM = _MarkIIIPdf[j]->RealVal();
      _MarkIIIPdf[j]->resetEventRecord();
      double w = wM/wL;
      weightList[j] = w;
      if(numEvents < 10){
	cout << "Event " << i << ", Model: " << j << ", wM, wL, w " << wM << ", " << wL << ", " << w << endl;
      }
    }
    _LHCbPdf->resetEventRecord();

    _ntpReader->setModelWeights(weightList);
    evt.setWeight(weightList[0]);

    _reweightedList.Add(evt);
    _counter.addEvent(pt, y, weightList[0]);


  }
  cout << "WeightedCount: have " 
       << _reweightedList.size() 
       << " events." << endl;
  return true;
}

DalitzHistoSet WeightedCount::getUnweightedHistos() const{
  return _unweightedList.histoSet();
}
DalitzHistoSet WeightedCount::getReweightedHistos() const{
  return _reweightedList.weightedHistoSet();
}

bool WeightedCount::drawAndSave(){

  DalitzHistoSet unwH = getUnweightedHistos();
  DalitzHistoSet rewH = getReweightedHistos();

  unwH.draw(name() + "_unweighedHistos");
  rewH.draw(name() + "_reweighedHistos");
  unwH.drawWithFit(rewH, name() + "_unwDots_rewLine");


  _unweightedList.save();
  _reweightedList.save();
  unwH.save(name() + "_unweightedHistos.root");
  rewH.save(name() + "_reweightedHistos.root");

  _counter.drawAndSave();

  return true;
}

