// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/Minimiser.h"
#include "Mint/IMinuitParameter.h"

#include "TGraph.h"
#include "TFile.h"

#include <iostream>

using namespace std;
using namespace MINT;

Minimiser* Minimiser::_defaultMinimiser=0;
int Minimiser::_defaultMaxCalls=100000; // 100k (a lot!)

Minimiser* Minimiser::getDefaultMinimiser(){

  if(0 == _defaultMinimiser){
    _defaultMinimiser = new Minimiser;
  }
  return _defaultMinimiser;
}


Minimiser::Minimiser(IMinimisable* fitFunction)
  : TMinuit(0)
  , ierflg(0)
  , _theFunction(fitFunction)
  , _maxCalls(_defaultMaxCalls)
  , _printLevel(3)
{
  if(0 != theFunction()){
    init();
  }
}

Minimiser::~Minimiser(){
}

void Minimiser::setMaxCalls(int maxCalls){
  _maxCalls = maxCalls;
}
int Minimiser::getMaxCalls() const{
  return _maxCalls;
}

Int_t Minimiser::Eval(Int_t        //npar
		      , Double_t*  //grad
		      , Double_t& fval
		      , Double_t* par
		      , Int_t     //flag
		      ){
  bool dbThis=false;
  if(dbThis) cout << "Eval got called " << endl;
  if(! this->OK()){
    cout << "ERROR in Minimiser::Eval (file: Minimiser.C)"
	 << " Got called although I'm not OK" << endl;
    fval = -9999;
    return -1;
  }
  this->updateFitParameters(par);
  fval = this->getFCNVal();
  return -1;
}

void Minimiser::TMinInit(){
   fStatus       = 0;
   fEmpty        = 0;
   fObjectFit    = 0;
   fMethodCall   = 0;
   fPlot         = 0;
   fGraphicsMode = kTRUE;
   SetMaxIterations();
   mninit(5,6,7);
}
bool Minimiser::MakeSpace(int needSpace){
  //return true;

  this->DeleteArrays();
  this->BuildArrays(needSpace + 2); // +2 for good luck
  TMinInit();
  return true;
}
bool Minimiser::init(){
  bool dbThis=true;
  if(dbThis) cout << "Minimiser::init(): you called me" << endl;
  if(0 == theFunction()){
    std::cout << "ERROR IN Minimiser::init():"
	      << " the function-ptr is empty."
	      << " I won't do anything for now."
	      << std::endl;
    return false;
  }
  if(dbThis) cout << " ... calling theFunction()->beginFit()" << endl;
  theFunction()->beginFit();

  _parSet = theFunction()->getParSet();
  if(0 == _parSet){
    std::cout << "ERROR IN Minimiser::init():"
	      << " theFunction()->getParSet() returned empty pointer"
	      << " I won't do anything for now."
	      << std::endl;
    return false;
  }

  MakeSpace(nPars());

  if(dbThis) cout << "... made space, now initialising variables" << endl;
  initialiseVariables();
  if(dbThis) cout << " initialised variables." << endl;

  if(dbThis) cout << "Minimiser::init(): returning true" << endl;
  return true;
}

bool Minimiser::attachFunction(IMinimisable* fcn){
  if(0==fcn) return false;
  //  detachParameters();
  _theFunction = fcn;
  return init();
}
unsigned int Minimiser::nPars() const{
  if(0 == _parSet) return 0;
  return _parSet->size();
}
IMinuitParameter* Minimiser::getParPtr(unsigned int i){
  if(0 == _parSet) return 0;
  if(i >= nPars()) return 0;
  return _parSet->getParPtr(i);
}
const IMinuitParameter* Minimiser::getParPtr(unsigned int i) const{
  if(0 == _parSet) return 0;
  if(i >= nPars()) return 0;
  return _parSet->getParPtr(i);
}

/*
bool Minimiser::detachParameters(){
  if(! parsOK()) return false;

  for(unsigned int i=0; i < nPars(); i++){
    getParPtr(i)->associate(0, i);
  }
  return true;
}
*/
bool Minimiser::initialiseVariables(){
  bool dbThis=false;
  if(! parsOK()) return false;
  
  bool success=true;
  if(dbThis) {
    cout << "Minimiser::initialiseVariables() called. " << endl;
    cout << "\n\t(declaring them to MINUIT)" << endl;
  } 

  //remove hidden MinuitParameter
  for(unsigned int i=0; i < nPars(); i++){
    if(getParPtr(i)->hidden()){
      	if(dbThis) cout << i << ")" << getParPtr(i)->name() << endl;
	parSet()->unregister(getParPtr(i));
	i--;
     }
  }

  //temporarilyQuiet();
  for(unsigned int i=0; i < nPars(); i++){
    int ierflag=0;
    //if(! getParPtr(i)->hidden()){        //hidden MinuitParamter already removed
      if(dbThis) cout << i << ")" << getParPtr(i)->name() << endl;
      double step = getParPtr(i)->stepInit();
      if(getParPtr(i)->iFixInit()) step=0;
      this->mnparm( i
		    , getParPtr(i)->name().c_str()
		    , getParPtr(i)->meanInit()
		    , step
		    , getParPtr(i)->minInit()
		    , getParPtr(i)->maxInit()
		    , ierflag);
      //if(getParPtr(i)->iFixInit()) FixParameter(i);
    //}
    //getParPtr(i)->associate(this, i);
    success &= ! ierflag;
  }
  if(dbThis){
    cout << "Minimiser::initialiseVariables():"
	 << "done declaring variables, now:"
	 << "theFunction()->parametersChanged();"
	 << endl;
  }
  theFunction()->parametersChanged();
  if(dbThis) cout << "Minimiser::initialiseVariables():"
		  << " all done - returning " << success << endl;
  //resetPrintLevel();
  return success;
}

bool Minimiser::setParametersToResult(){
  if(! parsOK())return false;

  Double_t mean, err, errN, errP;
  Double_t gcc;
  for(unsigned int i=0; i < nPars(); i++){
    //if(! getParPtr(i)->hidden()){
      this->mnerrs(i, errP, errN, err, gcc);
      this->GetParameter(i, mean, err);
      this->getParPtr(i)->setResult(mean, err, errP, errN);
    //}
  }
  theFunction()->parametersChanged();
  return true;
}

bool Minimiser::updateFitParameters(Double_t* par){
  if(! parsOK())return false;

  for(unsigned int i=0; i < nPars(); i++){
    //if(! getParPtr(i)->hidden()){
      getParPtr(i)->setCurrentFitVal(par[i]);
    //}
  }
  theFunction()->parametersChanged();
  return true;
}
bool Minimiser::endOfFit(){
  if(! parsOK())return false;
  setParametersToResult();
  theFunction()->endFit();
  return true;
}

bool Minimiser::parsOK() const{
  if(0 == _parSet) return false;
  if(0 == nPars()) return false;
  return true;
}
bool Minimiser::fcnOK() const{
  if(0 == theFunction()) return false;
  return true;
}

bool Minimiser::OK() const{
  return parsOK() && fcnOK();
}

double Minimiser::getFCNVal(){
  if(! this->OK()){
    cout << "ERROR IN Minimiser::getFCNVal()"
	 << " I'm not OK!!" << endl;
    return -9999.0;
  }
  return theFunction()->getVal();
}

bool Minimiser::setPrintLevel(int level){
  if(level >=0 )_printLevel=level;
  arglist[0] = _printLevel;
  TMinuit::mnexcm("SET PRINTOUT", arglist , 1, ierflg);
  return (! ierflg);
}
bool Minimiser::temporarilyQuiet(){
  arglist[0] = -1;
  TMinuit::mnexcm("SET PRINTOUT", arglist , 1, ierflg);
  return (! ierflg);
}
bool Minimiser::resetPrintLevel(){
  return setPrintLevel(-9999);
}
bool Minimiser::SetSomeMinuitOptions(){
  bool success = true;
  success &= setPrintLevel();
  arglist[0] = 1.;
  TMinuit::mnexcm("SET ERR", arglist , 1, ierflg);
  success &= (! ierflg);
  arglist[0] = 1;
  TMinuit::mnexcm("SET STRATEGY", arglist , 1, ierflg);
  success &= (! ierflg);
  return success;
}

bool Minimiser::CallMigrad(){
  bool dbThis=true;
  bool success=true;
  arglist[0] = _maxCalls; arglist[1] = 1.e-2;
  if(dbThis) cout << "calling MIGRAD" << endl;
  TMinuit::mnexcm("MIGRAD", arglist ,2,ierflg);
  if(dbThis) cout << "did that. How did I do? ierflg=" << ierflg << endl;
  success &= (! ierflg);
  return success;
}
bool Minimiser::CallMinos(){
  // thanks to Daniel Johnson for contributing this.
  bool dbThis=false;
  bool success=true;
  arglist[0] = _maxCalls; arglist[1] = 1.e-2;
  if(dbThis) cout << "calling MINOS" << endl;
  TMinuit::mnexcm("MINOS", arglist ,2,ierflg);
  if(dbThis) cout << "did that. How did I do? ierflg=" << ierflg << endl;
  success &= (! ierflg);
  return success;
}

bool Minimiser::CallSimplex(int maxCalls, double tolerance ){
    bool dbThis=true;
    bool success=true;
    arglist[0] = maxCalls; arglist[1] = tolerance;
    if(dbThis) cout << "calling SIMPLEX" << endl;
    TMinuit::mnexcm("SIMPLEX", arglist ,2,ierflg);
    if(dbThis) cout << "did that. How did I do? ierflg=" << ierflg << endl;
    success &= (! ierflg);
    return success;
}

bool Minimiser::CallSeek(int maxCalls, int devs){
    bool dbThis=true;
    bool success=true;
    arglist[0] = maxCalls; arglist[1] = devs; 
    if(dbThis) cout << "calling SEEK" << endl;
    TMinuit::mnexcm("SEEK", arglist ,2,ierflg);
    if(dbThis) cout << "did that. How did I do? ierflg=" << ierflg << endl;
    success &= (! ierflg);
    return success;
}

bool Minimiser::CallImprove(int maxCalls, int searches ){
    bool dbThis=true;
    bool success=true;
    arglist[0] = maxCalls; arglist[1] = searches;
    if(dbThis) cout << "calling IMPROVE" << endl;
    TMinuit::mnexcm("IMPROVE", arglist ,2,ierflg);
    if(dbThis) cout << "did that. How did I do? ierflg=" << ierflg << endl;
    success &= (! ierflg);
    return success;
}

bool Minimiser::prepFit(){
  bool dbThis=false;
  bool success=true;
  success &= SetSomeMinuitOptions();
  theFunction()->beginFit();
  success &= this->initialiseVariables();
  if(dbThis) cout << "re-initialised variables" << endl;
  return success;
}

bool Minimiser::doFit(){
  bool dbThis=false;
  bool success = true;
  if(dbThis) cout << "Minimiser::doFit() called" << endl;
  success &= prepFit();
  if(dbThis) cout << "... called prepFit " << success << endl;
  success &= CallMigrad();
  if(dbThis) cout << "... called MIGRAD" << success << endl;
  scanMarked();
  if(dbThis) cout << "... scanned" << success << endl;
  success &= this->endOfFit();
  return success;
}
bool Minimiser::doMinosFit(){
  bool dbThis=false;
  bool success = true;
  success &= prepFit();
  success &= CallMinos();
  if(dbThis) cout << "called MINOS" << endl;
  scanMarked();
  success &= this->endOfFit();
  return success;
}

bool Minimiser::doSeekFit(int maxCalls, int devs){
    bool dbThis=false;
    bool success = true;
    success &= prepFit();
    success &= CallSeek(maxCalls, devs);
    if(dbThis) cout << "called SEEK" << endl;
    //scanMarked();
    success &= this->endOfFit();
    return success;
}

bool Minimiser::doSimplexFit(int maxCalls, double tolerance){
    bool dbThis=false;
    bool success = true;
    success &= prepFit();
    success &= CallSimplex(maxCalls, tolerance);
    if(dbThis) cout << "called SIMPLEX" << endl;
    //scanMarked();
    success &= this->endOfFit();
    return success;
}


bool Minimiser::scanMarked(){
  bool sc=true;
  if(0 == nPars()) return false;
  for(unsigned int i=0; i < nPars(); i++){
    IMinuitParameter* p=getParPtr(i);
    if(0 == p || (! p->scan()) ) continue;
    sc &= (bool) scan(i, p->scanMin(), p->scanMax());
  }
  return sc;
}
bool Minimiser::scanAll(){
  bool sc=true;
  if(0 == nPars()) return false;
  for(unsigned int i=0; i < nPars(); i++){
    IMinuitParameter* p=getParPtr(i);
    if(0 == p) continue;
    sc &= (bool) scan(i, p->scanMin(), p->scanMax());
  }
  return sc;
}

TGraph* Minimiser::scan(IMinuitParameter& fp, double from, double to){
  if(fp.parSet() != this->parSet()){
    cout << "ERROR Minimiser::scanParameter: "
	 << " You are trying to scan a parameter that is"
	 << " associated to a different fit (different fit parameter set)"
	 << " will not do it."
	 << " The parameter you tried was: " << endl;
    fp.print();
    cout <<  endl;
    return 0;
  }
  return scan(fp.parSetIndex(), from, to);
}

TGraph* Minimiser::scan(int i, double from, double to){
  // note that the index i is the one in the parameter list
  // which goes from 0 to n-1 (i.e. C-style).
  // This corresponds to Minuit's parameter number i+1.
  // (so if you want Minuit's fit parameter 1, pass it 1-1=0)
  IMinuitParameter* p = getParPtr(i);
  if(0 == p) return 0;

  double points = 100;
  string fname = "scan_" + p->name() + ".root";
  
  Double_t arglist[10] = {0};
  Int_t ierflg=0;

  arglist[0]=i+1; arglist[1]=points; arglist[2]=from; arglist[3]=to;
  this->mnexcm("SCAN", arglist, 4, ierflg);
  TGraph *gr = (TGraph*) this->GetPlot();
  if(0 == gr){
    cout << " didn't get plot " << endl;
  }else{
    TFile fscan(fname.c_str(), "RECREATE");
    fscan.cd();
    gr->Write();
    fscan.Close();
  }
  return gr;
}

void Minimiser::printResultVsInput(std::ostream& os) const{
  if(0 == _parSet) return;
  _parSet->printResultVsInput(os);
}

TMatrixTSym<double> Minimiser::covMatrix() {
  // only entries for variable parameters.
  // if you want rows and columns to correspond
  // to parameter numbers in MinuitParameterSets
  // use covMatrixFull.
  unsigned int internalPars = fNpar;

  Double_t m1[internalPars][internalPars];
  for(unsigned int i=0; i < internalPars; i++){
    for(unsigned int j=0; j < internalPars; j++){
      m1[i][j]=0.0;
    }
  }
  this->mnemat(&m1[0][0], internalPars);
  TMatrixTSym<double> matrix(internalPars);
  for(unsigned int i=0; i < internalPars; i++){
    for(unsigned int j=i; j < internalPars; j++){
      matrix(i,j) = matrix(j,i) = m1[i][j];
    }
  }
  return matrix;
}

TMatrixTSym<double> Minimiser::covMatrixFull() {
  // rows and columns correspond to external parameter numbers
  // (if you have many fixed parameters, you'll get a lot of zeros)
  unsigned int internalPars = fNpar;

  Double_t m1[internalPars][internalPars];
  for(unsigned int i=0; i < internalPars; i++){
    for(unsigned int j=0; j < internalPars; j++){
      m1[i][j]=0.0;
    }
  }
  this->mnemat(&m1[0][0], internalPars);
  TMatrixTSym<double> matrix(nPars());
  for(unsigned int i=0; i < internalPars; i++){
    for(unsigned int j=i; j < internalPars; j++){
      int ex_i = fNexofi[i] -1;
      int ex_j = fNexofi[j] -1;
      matrix(ex_i,ex_j) = matrix(ex_j,ex_i) = m1[i][j];
    }
  }
  return matrix;
}
