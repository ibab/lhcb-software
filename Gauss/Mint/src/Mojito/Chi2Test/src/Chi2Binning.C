#include "IDalitzEventList.h"

#include "Chi2Binning.h"
#include "Chi2Box.h"
#include "Chi2BoxSet.h"
#include <algorithm>
#include <iostream>

using namespace MINT;
using namespace std;

int Chi2Binning::createBinning(IDalitzEventList* events
			       , int minPerBin
			       , int maxPerBin
			       ){

  if(0 == events) return 0;
  if(0 == events->size()) return 0;
  const IDalitzEvent* evt0 = events->getREvent(0);
  if(0 == evt0) return 0;

  Chi2BoxSet boxes = splitBoxes(events, maxPerBin);

  mergeBoxes(boxes, minPerBin);

  return numBins();
}

int Chi2Binning::mergeBoxes(Chi2BoxSet& boxes, int minPerBin){
  lessByChi2BoxData sorter;
  sort(boxes.begin(), boxes.end(), sorter);
  // this way, the smalles boxes come first 
  // makes sure no empty box is left over at the end.

  Chi2BoxSet boxSet;
  for(unsigned int i=0; i < boxes.size(); i++){
    boxSet.add(boxes[i]);
    if(boxSet.nData() >= minPerBin){
      _boxSets.push_back(boxSet);
      boxSet.clear();
    }
  }
  
  return _boxSets.size();
}

Chi2BoxSet Chi2Binning::splitBoxes(IDalitzEventList* events
				   , int maxPerBin
				   ) const{
  bool dbThis=false;
  Chi2BoxSet dummy;
  if(0 == events) return dummy;
  if(0 == events->size()) return dummy;
  const IDalitzEvent* evt0 = events->getREvent(0);
  if(0 == evt0) return dummy;
  
  Chi2Box box(evt0->eventPattern());

  Chi2BoxSet boxes(box.split(0));
  bool needToSplitMore=false;
  int counter=0;
  do{
    counter++;
    boxes.resetEventCounts();
    int failedSet=0, failedBox=0;
    events->Start();
    while(events->Next()){
      bool added = boxes.addData(events->getEvent());
      if(! added) failedSet++;
      if(! box.addData(events->getEvent())){
	failedBox++;
      }
    }
    if(failedSet > 0 || dbThis){
      cout << " Chi2Binning::splitBoxes: ERROR split level " << counter 
	   << ", num boxes: " << boxes.size()
	   << ", failedSet: " << failedSet
	   << ", failedBox: " << failedBox << endl;
    }

    Chi2BoxSet newBoxes;
    needToSplitMore=false;
    for(unsigned int i=0; i < boxes.size(); i++){
      if( boxes[i].nData() > maxPerBin ){
	needToSplitMore=true;
	newBoxes.add(boxes[i].split());
      }else{
	newBoxes.add(boxes[i]);
      }
    }
    boxes = newBoxes;
  }while(needToSplitMore);

  return boxes;
}

void Chi2Binning::resetEventCounts(){
  _nData=0;
  _nMC=0;
  _totalMCWeight=0.0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    _boxSets[i].resetEventCounts();
  }
}
void Chi2Binning::fillData(IDalitzEventList* data){
  data->Start();
  while(data->Next()){
    bool foundBox=false;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].addData(data->getEvent())){
	foundBox=true;
	_nData++;
	break;
      }
    }
    if(! foundBox){
      cout << "WARNING in Chi2Binning::fillData:"
	   << "\n\t didn't find box for event " 
	   << endl;
      data->getEvent()->print();
      cout << "compare to first event: " << endl;
      data->getREvent(0)->print();
      Chi2Box box(data->getEvent()->eventPattern());
      cout << "would have fit into large? "
	   << box.addData(data->getEvent()) << endl;
    }
  }
}
void Chi2Binning::fillMC(IDalitzEventList* mc
			 , IDalitzPdf* pdf){
  bool dbThis=false;
  if(dbThis){
    cout << "Chi2Binning::fillMC called with "
	 << mc << ", " <<  pdf << endl;
  }
  mc->Start();
  pdf->setEventRecord(mc);
  int counter=0;
  int printevery=1000;
  while(mc->Next()){
    counter++;
    bool printit = (0 == counter%printevery);
    if(printit) cout << "...fillMC filling event number " << counter;
    
    IDalitzEvent* evt = mc->getEvent();
    double weight = pdf->getVal_noPs() * evt->getWeight() / 
      evt->getGeneratorPdfRelativeToPhaseSpace();

    if(printit) cout << ", with weight " << weight << endl;
    int n=0;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].addMC(evt, weight)){
	_totalMCWeight += weight;
	_nMC++;
	n++;
	break;
      }
    }
    //    cout << "event " << counter << " in " << n << " box sets" << endl;
  }
  pdf->resetEventRecord();
}

double Chi2Binning::setEventsAndPdf(IDalitzEventList* data
				    , IDalitzEventList* mc
				    , IDalitzPdf* pdf
				    , IFastAmplitudeIntegrable* fas
				    ){
  bool dbThis=false;
  if(dbThis) cout << "Chi2Binning::setEventsAndPdf" << endl;
  if(0 == data) return -9999;
  if(0 == mc) return -9999;
  if(0 == pdf) return -9999;
  //(could do this: if 0 == pdf: mc generated with pdf from fit result.
  // But for now, leave it like this for safety)

  if(0 == numBins()) createBinning(data);
  if(0 != fas) setFas(fas);
  if(dbThis) cout << "...number of chi2 bins = " << numBins() << endl;
  resetEventCounts();
  if(dbThis) cout << "...about to fill in the data " << endl;
  fillData(data);
  if(dbThis) cout << "...done that - now the MC" << endl;
  fillMC(mc, pdf);
  if(dbThis) cout << " Chi2Binning::setEventsAndPdf done" << endl;
  return 0;
}

void Chi2Binning::setFas(IFastAmplitudeIntegrable* fas){
  bool dbThis=true;

  const_counted_ptr<IIntegrationCalculator> 
    ic = fas->makeIntegrationCalculator();

  if(dbThis) {
    cout << "Chi2Binning::setFas: this is the fas:" << endl;
    fas->print(cout);
    cout << endl;
  }
  for(unsigned int i=0; i < _boxSets.size(); i++){
    counted_ptr<IIntegrationCalculator> fap(ic->clone_IIntegrationCalculator());
    _boxSets[i].setIIntegrationCalculator(fap);
  }
}
double Chi2Binning::getChi2_perBin() const{
  bool dbThis=true;
  if(dbThis) cout << "Chi2Binning::getChi2_perBin()" << endl;
  if(_nMC <= 0) return -9999;
  if(_nData <=0 ) return -9999;
  if(_totalMCWeight <= 0.0) return -9999;

  double sum=0;
  
  double sumVarData = 0;
  double sumVarMC = 0;

  double normFactor = ((double)_nData)/_totalMCWeight;
  if(dbThis) cout << "normFactor: " << normFactor << endl;
  // this was written late at night - need to check:

  int nDataCheck=0;
  double nMCCheck=0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    if(dbThis) cout << " bin " << i;
    int n_data =  _boxSets[i].nData();
    nDataCheck += n_data;

    if(dbThis) cout << "  data " << n_data;
    // double var_data = n_data;

    double weight_mc = _boxSets[i].weightedMC() * normFactor;

    nMCCheck += weight_mc;

    if(dbThis) cout << ", mcweight " << weight_mc;
    if(dbThis) cout << ", (nMC " << _boxSets[i].nMC() << ")";
    double var_mc_noNorm = _boxSets[i].rmsMC(_nMC);
    double var_mc = var_mc_noNorm* normFactor*normFactor;
    double varData_expected = weight_mc;

    double varData;
    if(0 != varData_expected) varData = varData_expected;
    else varData = n_data;

    sumVarData += varData_expected;
    sumVarMC   += var_mc;
   
    double var = varData + var_mc;
    //    if(dbThis) cout << " var " << var << ", no norm: " << var_mc_noNorm;
    if(dbThis) cout << ", sigma " << sqrt(var);
    double delta_N = n_data - weight_mc;
    double dNSq = delta_N * delta_N;
    double chi2;
    if( dNSq < var * 1.e-20) chi2=0; // catches legit. 0-entry cases
    else chi2 = dNSq / var;
    if(dbThis) cout << ", chi2 " << chi2 << endl;
    if(dbThis) _boxSets[i].printBoxInfo();
    sum += chi2;
  }

  if(dbThis){
    cout << " total data weight: " << nDataCheck
	 << " .. _nData = " << _nData
	 << " should be same as normalised total MC weight = " << nMCCheck
      	 << " (un-normalised = " << _totalMCWeight << ")"
	 << endl;
  }
  if(sumVarMC > sumVarData){
    cout << "Severe WARNING in Chi2Binning::getChi2_perBin()"
	 << "\n error on difference between data and MC dominated by MC"
	 << "\n mean data variance: " << sumVarData/_boxSets.size()
	 << ", ... mc: " << sumVarMC/_boxSets.size()
	 << ".\n Get more MC!"
	 << endl;
  }

  return sum/numBins();
}

int Chi2Binning::numBins() const{
  return _boxSets.size();
}

bool lessByChi2BoxData::operator()(const Chi2Box& a, const Chi2Box& b) const{

  return a.nData() < b.nData();
}

//
