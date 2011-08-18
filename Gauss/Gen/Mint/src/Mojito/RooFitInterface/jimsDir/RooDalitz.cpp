// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#include "RooDalitz.h"
#include "RooRandom.h"

#include "RooRealVar.h"
#include "RooDataSet.h"

#include "RooNumIntConfig.h"
#include "RooNumIntFactory.h"

#include <complex>
#include <iostream>
#include <ctime>

using namespace std;

RooDalitz::RooDalitz(const char* name
		     , const char* title
		     , RooAbsReal& _Resonances
		     , RooAbsPhaseSpace& _PhaseSpace
		     , RooArgList& _allVars
		     , std::string whichIntegrator
		     )
  : RooAbsPdf(name, title)
  , allVars("allVars", "allVars", this)
  , Resonances("Resonances", "Resonances", this, _Resonances)
  , PhaseSpace("PhaseSpace", "PhaseSpace", this, _PhaseSpace)
  , phaseSpaceData(0)
  , _useRooFitsIntegrator(whichIntegrator=="useRooFitsIntegrator")
{
  cout << " RooDalitz constructor " << endl;
  phaseSpaceData = _PhaseSpace.generateDefault(1); //must happen before initInteg.
  cout << "made phaseSpaceData" << endl;

  setupRooFitsIntegrator();
  // initInteg();
  //cout << "set up RooFitsIntegrator" << endl;

  // deal with the list - hope to get dependencies from that.
  TIterator* resIter = _allVars.createIterator() ;
  RooAbsArg* resPtr;
  while((resPtr = (RooAbsArg*) resIter->Next())){
    // yes, it's an assignment
    allVars.add(*resPtr);
  }
  cout << "read the list " << endl;
  delete resIter;
  cout << "deleted iterator" << endl;
}


RooDalitz::RooDalitz(const RooDalitz& other
		     , const char* name
		     )
  : RooAbsPdf(other, name)
  , Resonances("Resonances", this, other.Resonances)
  , PhaseSpace("PhaseSpace",this, other.PhaseSpace)
  , allVars("allVars",this, other.allVars)
  , phaseSpaceData(other.phaseSpaceData)
  , _useRooFitsIntegrator(other._useRooFitsIntegrator)
{
  //  initInteg();
  cout << " " << GetName() 
       << ": I got copied. " 
       << phaseSpaceData << endl;
  setupRooFitsIntegrator();
}

RooDalitz::~RooDalitz(){
  //if(resCloneSet) delete resCloneSet;
}

void RooDalitz::setupRooFitsIntegrator(){
  return;
  bool changeDefault=true;

  cout << "getting intConfig " << endl;
  //RooNumIntConfig* intConfig 
  //  = new RooNumIntConfig(*(this->defaultIntegratorConfig()));
  RooNumIntConfig* intConfig;
  if(changeDefault){
    intConfig = this->defaultIntegratorConfig();
  }else{
    intConfig = ( RooNumIntConfig* ) 
      this->defaultIntegratorConfig()->Clone("RooDalitzOwnIntegrator");
  }

  //  RooNumIntConfig* intConfig = new RooNumIntConfig ;    
  //  RooNumIntFactory::instance().processInitializers() ;

  cout << "got it: " << endl;

  //intConfig->printToStream(cout);
    //intConfig->method1D().setLabel("RooIntegrator1D");
  intConfig->method1D().setLabel("RooMCIntegrator");
  intConfig->method2D().setLabel("RooMCIntegrator");
  //  intConfig->method1DOpen().setLabel("RooMCIntegrator");
  //  intConfig->method2DOpen().setLabel("RooMCIntegrator");

  intConfig->setEpsRel(1.0e-8);
  intConfig->setEpsAbs(1.0e-8);

  if(! changeDefault){
    if(_specIntegratorConfig){
      delete _specIntegratorConfig;
    }
    _specIntegratorConfig = intConfig;
  }
  cout << "this->getIntegratorConfig()->printToStream(cout);"
       << endl;
  
  this->getIntegratorConfig()->printToStream(cout);

  return;
  /*
  cout << " making newIntConfig. " << endl;
  const RooNumIntConfig newIntConfig = RooNumIntConfig(*intConfig);
  const RooNumIntConfig& newIntConfig(intConfig);
  cout << " setting it. " << endl;
  //this->setIntegratorConfig(newIntConfig);
  */
  
  //this is dangerous:
  //this->setIntegratorConfig(*intConfig);
}

void RooDalitz::initInteg() const{
  const RooAbsReal& res = Resonances.arg();
  std::string tmpName = GetName();
  tmpName += "_ResBranchNodeList";
  RooArgSet tmp(tmpName.c_str()) ;
  res.branchNodeServerList(&tmp) ;
  resCloneSet = (RooArgSet*) tmp.snapshot(kFALSE) ;
  
  // Find the top level RES in the snapshot list
  resClonePtr = (RooAbsReal*) resCloneSet->find(res.GetName()) ;

  // Attach RES to data set
  resClonePtr->attachDataSet(*phaseSpaceData) ;
}

void RooDalitz::finishInteg() const{
  if(resCloneSet)delete resCloneSet;
  resCloneSet=0;
}

Int_t RooDalitz::getGenerator(const RooArgSet &allVars
			      , RooArgSet &dirVars
			      , Bool_t staticInitOK
			      ) const{
  return 0;
}

void RooDalitz::generateEvent(Int_t code){
  cout << "RooDalitz::generateEvent called - that's bad."
       << endl;
}
 

Int_t RooDalitz::getAnalyticalIntegral(RooArgSet& allVars
				       , RooArgSet& analVars
				       , const char* rangeName
				       ) const{
  //  return 0;

  if(_useRooFitsIntegrator) return 0;

  RooArgSet allArgs = ((RooAbsPhaseSpace*) (&(PhaseSpace.arg())))->defaultParameterSet();
  cout << "matching .. "
       << allVars << ", " << analVars << ", " << allArgs << endl;
  if(matchArgs(allVars, analVars, allArgs)){
    cout << "RooDalitz::getAnalyticalIntegral returning 5" << endl;
    return 5;
  }
  cout << "RooDalitz::getAnalyticalIntegral returning 0" << endl;
  return 0;
}
 

void RooDalitz::updatePhaseSpaceDataSet(int Nevents) const{
  if(! phaseSpaceData){
    cout << "RooDalitz::updatePhaseSpaceDataSet generating new set. " << endl;
    phaseSpaceData = ((RooAbsPhaseSpace*) (&(PhaseSpace.arg())))->generateDefault(Nevents);
    //    resonancesClone->attachDataSet(*phaseSpaceData);
  }else{
    int missingEvents = Nevents - phaseSpaceData->numEntries();
    cout << "missing events: " << missingEvents 
	 << " = " << Nevents 
	 << " - " << phaseSpaceData->numEntries()
	 << endl;
    if(missingEvents > 0){
      RooDataSet* newData = ((RooAbsPhaseSpace*) (&(PhaseSpace.arg())))->generateDefault(missingEvents);
      phaseSpaceData->append(*newData);
      delete newData; // that OK?
      cout << "now we have " << phaseSpaceData->numEntries() 
	   << " events " << endl;
    }
  }
}

Bool_t RooDalitz::withinLimits(const RooArgSet* emms, bool debug) const{
  TIterator* ourIter = allVars.createIterator();
  RooRealVar* ourPtr;
  while((ourPtr = (RooRealVar*) ourIter->Next())){
    double val = emms->getRealValue(ourPtr->GetName());
    if(debug) cout << "withinLimits? " 
		   << ourPtr->getMin() << " < "
		   << val << " < " << ourPtr->getMax() << " ?"
		   << endl;
    if(val > ourPtr->getMax() || val < ourPtr->getMin()){
      return false;
    }
  }
  return true;
}

Double_t RooDalitz::evalInt(double& variance) const{
  if((! phaseSpaceData) || phaseSpaceData->numEntries() <= 0){
    cout << "ERROR in RooDalitz::evalInt: "
	 << "Empty dataset!" << endl;
    return 0;
  }
  initInteg();

  time_t tstart = time(0);

  double sum=0, sumsq=0;
  int N=phaseSpaceData->numEntries();
  cout << "N= " << N << endl;
  double DbleN = (double) N;
  for(int i=0; i< N; i++){
    bool debug = (! (i%(N/3)));
    const RooArgSet* row = phaseSpaceData->get(i);
    /*
    TIterator* emmIter = emms.createIterator();
    RooRealVar* emmPtr;
    while((emmPtr = (RooRealVar*) emmIter->Next())){
      *emmPtr = row->getRealValue(emmPtr->GetName());
      if(! (i%(N/3))) emmPtr->Print();
    }
    */
    double val=0;

    if(withinLimits(row)) val = resClonePtr->getVal();
    
    if(debug)cout << "val " << val << endl;
    
    sum   += val;
    sumsq += val*val;
  }
  
  double mean = sum/DbleN;
  double meanOfSq = sumsq/DbleN;
  double totalVariance = meanOfSq - mean*mean;
  variance = totalVariance/DbleN;


  double delT = difftime(time(0), tstart);

  cout << "RooDalitz::evalInt: returning " << mean << endl;
  cout << ".. this took " << delT << " s " << endl;

  finishInteg();
  return mean;
}
Double_t RooDalitz::analyticalIntegral(Int_t code
				       , const char* rangeName
				       ) const{
  
  if(code == 5){
    int Ninitial=1000;   
    double maxRelError = 1.e-2;// that will need to be tighter.
    double currentVal=-9999, variance=-9999;
    if(!phaseSpaceData || phaseSpaceData->numEntries() < Ninitial){
      updatePhaseSpaceDataSet(Ninitial);
      currentVal = evalInt(variance);
      double maxError = currentVal * maxRelError;
      double maxVariance = maxError*maxError;
      cout << "maxError = " << maxError << endl;
      cout << "maxVariance = " << maxVariance << endl;
      cout << "variance for " << Ninitial << ": " << variance << endl;
      int minEvents = Ninitial * ((int) (variance/maxVariance) + 1);
      cout << " minEvents " << minEvents << endl;
      updatePhaseSpaceDataSet(minEvents);
    }
    currentVal = evalInt(variance);
    //cout << "currentVal " << currentVal 
    //<< ", variance " << variance 
    //<< ", sigma " << sqrt(variance) 
    //<< endl;
    return currentVal;
  }else{
    cout << "ERROR in RooDalitz::analyticalIntegral:"
	 << " Got called with code "
	 << code
	 << " - don't know that code." 
	 << endl;
    return -9999;
  }

}
Double_t RooDalitz::evaluate() const{
  bool printalot=false;
  Double_t ps = PhaseSpace;
  Double_t rs = Resonances;
  Double_t integ = 1;// analyticalIntegral(5, 0);
  if(printalot)cout << " ps = " << ps << endl;
  if(printalot)cout << " Resonances " << rs << endl;
  if(printalot)cout << " integral " << integ << endl;
  Double_t returnVal = 0;
  if(ps==0 || integ==0) returnVal = 0;
  else returnVal = rs * ps;// / integ;
  if(printalot)cout << " returning " << returnVal << endl;
  return returnVal/3.14811;
}

//
