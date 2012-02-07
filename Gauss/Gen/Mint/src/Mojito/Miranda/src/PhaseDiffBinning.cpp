#include "Mint/IDalitzEventList.h"

#include "Mint/PhaseBinning.h"
#include "Mint/PhaseDiffBinning.h"

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
//
//int PhaseBinning::createBinning(IDalitzEventList* events
//			       , int minPerBin
//			       , int maxPerBin
//			       ){
//	
//	
//	
//	  std::cout << "Create Phase Binning " << std::endl;
//
//	
//
//  if(0 == events) return 0;
//  if(0 == events->size()) return 0;
//  const IDalitzEvent* evt0 = events->getREvent(0);
//  if(0 == evt0) return 0;
//
//  IDalitzEvent* evt1 = evt0->clone();
////  IDalitzEvent* evt1 = evt0;
//  std::complex<double> amp = _fas->getVal(evt1);
//  std::cout << "Real " << amp.real() << std::endl;
//  
//  ScpBoxSet boxSet;
//    for(unsigned int i=0; i < 9; i++){
//    	ScpBox box(evt0->eventPattern());
//  		boxSet.add(box);
//  		_boxSets.push_back(boxSet);
//  		boxSet.clear();
//    }
//  return numBins();
//
//}


int PhaseDiffBinning::createBinning(IDalitzEventList* events
			       , int minPerBin
			       , int maxPerBin
			       ){
	
	
	
	  std::cout << "Create Phase Binning " << std::endl;

	

  if(0 == events) return 0;
  if(0 == events->size()) return 0;
  const IDalitzEvent* evt0 = events->getREvent(0);
  if(0 == evt0) return 0;

  IDalitzEvent* evt1 = evt0->clone();
//  IDalitzEvent* evt1 = evt0;
  std::complex<double> amp = _fas->getVal(evt1);
  std::cout << "Real " << amp.real() << std::endl;
  
  ScpBoxSet boxSet;
    for(unsigned int i=0; i < 9; i++){
    	ScpBox box(evt0->eventPattern());
  		boxSet.add(box);
  		_boxSets.push_back(boxSet);
  		boxSet.clear();
    }

	  std::cout << "Created Phase Binning " << std::endl;

  return numBins();

}


void PhaseDiffBinning::fillData(IDalitzEventList* data){
  data->Start();
  
  std::cout << "Fill data" <<std::endl;
  
  double real = 0;
  double im = 0;
  double phase = 0;
  
  double realCC = 0;
  double imCC = 0;
  double phaseCC = 0;
  
  double phaseDiff= 0;

  FitAmpSum* fasCC = new FitAmpSum(_fas);
  fasCC->CPConjugateSameFitParameters();

  while(data->Next()){
    bool foundBox=false;
//    IDalitzEvent* evt1 = evt0->clone();
  //  IDalitzEvent* evt1 = evt0;
    std::complex<double> amp = _fas->getVal(data->getEvent());
//    MINT::counted_ptr<FitAmpList> fasCC = _fas->GetCloneSameFitParameters();
    
    
//    fasCC->CPConjugateSameFitParameters();
    std::complex<double> ampCC = fasCC->getVal(data->getEvent());
    
//    std::complex<double> ampCC;
//    ampCC.real() = 10;
//    ampCC.imag() = 10;
    real =  amp.real();
    im = amp.imag();
    
    realCC =  ampCC.real();
    imCC = ampCC.imag();
       
    phase = atan2(im,real);
    phaseCC = atan2(imCC,realCC);
    
    if (phase < 0)
    {
    	phase = phase + 2*3.14159265;
    }	
    if (phaseCC < 0)
     {
     	phaseCC = phaseCC + 2*3.14159265;
     }	
   	   
     phaseDiff = phase - phaseCC;
     if (phaseDiff < 0)
     {
    	 phaseDiff = phaseDiff + 2*3.14159265;
     }	
      
     std::cout << " PhaseDiff: " << phaseDiff << std::endl;
       
    for (int i = 0; i < 9; i++)
    {
    	if (phaseDiff > (2*3.14159265/(9))*i && phaseDiff < (2*3.14159265/(9))*(i+1))
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

void PhaseDiffBinning::fillDataCC(IDalitzEventList* data){
	
	  std::cout << "Fill dataCC " <<std::endl;

  data->Start();
  double real =  0;
  double im = 0;
  long double phase = 0;
  
  double realCC = 0;
  double imCC = 0;
  double phaseCC = 0;
  
  double phaseDiff= 0;
  FitAmpSum* fasCC = new FitAmpSum(_fas);
  fasCC->CPConjugateSameFitParameters();

  while(data->Next()){
    bool foundBox=false;
    std::complex<double> amp = _fas->getVal(data->getEvent());
//    std::cout << "Real " << amp.real() << std::endl;
//    std::cout << "Imag " << amp.imag() << std::endl;

//    MINT::counted_ptr<FitAmpList> fasCC = _fas->GetCloneSameFitParameters();

//    fasCC->CPConjugateSameFitParameters();
    std::complex<double> ampCC = fasCC->getVal(data->getEvent());
        std::cout << "Real " << amp.real() << std::endl;
        std::cout << "Imag " << amp.imag() << std::endl;
        std::cout << "RealCC " << ampCC.real() << std::endl;
        std::cout << "ImagCC " << ampCC.imag() << std::endl;

    
    real =  amp.real();
    im = amp.imag();
    
//    std::complex<double> ampCC;
//    ampCC.real() = 10;
//    ampCC.imag() = 10;

    
    realCC =  ampCC.real();
    imCC = ampCC.imag();
       
    phase = atan2(im,real);
    phaseCC = atan2(imCC,realCC);
    
    if (phase < 0)
    {
    	phase = phase + 2*3.14159265;
    }	
    if (phaseCC < 0)
     {
     	phaseCC = phaseCC + 2*3.14159265;
     }	
   	   
     phaseDiff = phase - phaseCC;
     if (phaseDiff < 0)
     {
    	 phaseDiff = phaseDiff + 2*3.14159265;
     }	
      
     std::cout << " PhaseDiff: " << phaseDiff << std::endl;

           
    for (int i = 0; i < 9; i++)
    {
    	if (phaseDiff > (2*3.14159265/(9))*i && phaseDiff < (2*3.14159265/(9))*(i+1))
    	{
    		_boxSets[i].addMC((data->getEvent()),1.0);
    		_nDataCC++;
    		foundBox=true;
    	}
    }

  }
}

double PhaseDiffBinning::setEvents(IDalitzEventList* data
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

