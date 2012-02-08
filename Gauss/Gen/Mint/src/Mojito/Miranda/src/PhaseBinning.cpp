#include "Mint/IDalitzEventList.h"

#include "Mint/PhaseBinning.h"
#include "Mint/ScpBinning.h"
#include "Mint/ScpBox.h"
#include "Mint/ScpBoxSet.h"

#include "Mint/DalitzHistoStackSet.h"

#include "TCanvas.h"
#include "TH1D.h"
#include <stdio.h>
#include <math.h>

#include <algorithm>
#include <iostream>

using namespace MINT;
using namespace std;

int PhaseBinning::createBinning(IDalitzEventList* events ){

  if(0 == events) return 0;
  if(0 == events->size()) return 0;
  const IDalitzEvent* evt0 = events->getREvent(0);
  if(0 == evt0) return 0;

  IDalitzEvent* evt1 = evt0->clone();
//  IDalitzEvent* evt1 = evt0;
  std::complex<double> amp = _fas->getVal(evt1);
//  std::cout << "Real " << amp.real() << std::endl;
  
  ScpBoxSet boxSet;
    for(unsigned int i=0; i < _nbins; i++){
    	ScpBox box(evt0->eventPattern());
  		boxSet.add(box);
  		_boxSets.push_back(boxSet);
  		boxSet.clear();
    }
  return numBins();

}


void PhaseBinning::fillData(IDalitzEventList* data){
  data->Start();
  while(data->Next()){
    bool foundBox=false;
//    IDalitzEvent* evt1 = evt0->clone();
  //  IDalitzEvent* evt1 = evt0;
    std::complex<double> amp = _fas->getVal(data->getEvent());
    double real =  amp.real();
       double im = amp.imag();
       double phase = 0;
       
       phase = atan2(im,real);
   
       if (phase < 0)
       {
    	   phase = phase + 2*3.14159265;
       }
       
       for (int i = 0; i < _nbins; i++)
       {
       	if (phase > (2*3.14159265/(_nbins))*i && phase < (2*3.14159265/(_nbins))*(i+1))
       	{
       		_boxSets[i].addData(data->getEvent());
       		_nData++;
       		foundBox=true;
       	}
       }
    
    if(! foundBox){
      cout << "WARNING in PhaseBinning::fillData:"
	   << "\n\t didn't find box for event " 
	   << endl;
//      data->getEvent()->print();
      cout << "compare to first event: " << endl;
//      data->getREvent(0)->print();
      ScpBox box(data->getEvent()->eventPattern());
      cout << "would have fit into large? "
	   << box.addData(data->getEvent()) << endl;
    }
  }
}

void PhaseBinning::fillDataCC(IDalitzEventList* data){
  data->Start();
  double real =  0;
  double im = 0;
  long double phase = 0;
  while(data->Next()){
    bool foundBox=false;
    std::complex<double> amp = _fas->getVal(data->getEvent());

    real =  amp.real();
    im = amp.imag();
    
    
    phase = atan2(im,real);
    
    if (phase < 0)
        {
     	   phase = phase + 2*3.14159265;
        }
           
    for (int i = 0; i < _nbins; i++)
    {
    	if (phase > (2*3.14159265/(_nbins))*i && phase < (2*3.14159265/(_nbins))*(i+1))
    	{
    		_boxSets[i].addMC((data->getEvent()),1.0);
    		_nDataCC++;
    		foundBox=true;
    	}
    }

  }
}

double PhaseBinning::setEvents(IDalitzEventList* data
				    , IDalitzEventList* dataCC
				    ){
  bool dbThis=false;
  if(dbThis) cout << "PhaseBinning::setEventsAndPdf" << endl;
  if(0 == data) return -9999;
  if(0 == dataCC) return -9999;
 
  if(0 == numBins()) createBinning(data);
  if(dbThis) cout << "...number of scp bins = " << numBins() << endl;
  resetEventCounts();
  if(dbThis) cout << "...about to fill in the data " << endl;
  fillData(data);
  if(dbThis) cout << "...done that - now the MC" << endl;
  fillDataCC(dataCC);
  if(dbThis) cout << "... fillMC done, now setting norm factors" << endl;
  setBoxesNormFactors();
  if(dbThis) cout << " done the norm factors, now sorting by chi2" << endl;
  sortByScp();
  if(dbThis) cout << " PhaseBinning::setEventsAndPdf done" << endl;
  return 0;
}

std::ostream& operator<<(std::ostream& os, const PhaseBinning& c2b){
  c2b.print(os);
  return os;
}
//
