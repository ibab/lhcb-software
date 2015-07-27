#ifndef MINIMISER_HH
#define MINIMISER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "TMinuit.h"
#include "TMatrixTSym.h"

#include "Mint/IMinimisable.h"

#include <iostream>

class TGraph;
namespace MINT{

class IMinuitParameter;

class Minimiser : public TMinuit{
  mutable Double_t arglist[10];
  mutable Int_t ierflg;
 protected:
  static Minimiser* _defaultMinimiser;
  static int _defaultMaxCalls;

  MinuitParameterSet* _parSet;
  //  MinuitParameterSet _floating;

  IMinimisable* _theFunction;
  int _maxCalls;

  int _printLevel;

  bool init();  
  bool MakeSpace(int needSpace);
  void TMinInit();

  bool updateFitParameters(Double_t* p);
  bool setParametersToResult();
  bool endOfFit();
  //  bool detachParameters();

  Int_t Eval(Int_t npar
	     , Double_t *grad
	     , Double_t &fval
	     , Double_t *par
	     , Int_t flag
	     );
 public:
  static Minimiser* getDefaultMinimiser();

  Minimiser(IMinimisable* fitFunction=0);
  virtual ~Minimiser();
  bool attachFunction(IMinimisable* fcn);
  IMinimisable* theFunction(){ return _theFunction;}
  const IMinimisable* theFunction() const{ return _theFunction;}

  const MinuitParameterSet* parSet()const{ return _parSet;}
  MinuitParameterSet* parSet(){ return _parSet;}

  unsigned int nPars() const;
  IMinuitParameter* getParPtr(unsigned int i);
  const IMinuitParameter* getParPtr(unsigned int i) const;

  bool OK() const;
  bool parsOK() const;
  bool fcnOK() const;

  double getFCNVal();

  bool initialiseVariables();

  bool setPrintLevel(int level=-1);
  bool temporarilyQuiet();
  bool resetPrintLevel();

  bool SetSomeMinuitOptions();
  bool CallMigrad();
  bool CallMinos();
  bool CallSeek(int maxCalls = 100, int devs = 5);
  bool CallSimplex(int maxCalls = 300, double tolerance = 1. );
  bool CallImprove(int maxCalls= 1500, int searches= 5);

  bool prepFit();
  bool doFit();
  bool doMinosFit();
  bool doSeekFit();
  bool doSimplexFit();

  bool scanMarked();
  bool scanAll();
  TGraph* scan(int i, double from=0, double to=0);
  // note that the index i is the one in the parameter list
  // which goes from 0 to n-1 (i.e. C-style).
  // This corresponds to Minuit's parameter number i+1.
  // (so if you want Minuit's fit parameter 1, pass it 1-1=0)
  TGraph* scan(IMinuitParameter& fp, double from=0, double to=0);


  void setMaxCalls(int maxCalls);
  int getMaxCalls() const;

  void printResultVsInput(std::ostream& os = std::cout) const;

  // get number of free parameters from TMinuit call:
  // GetNumFreePars()

  TMatrixTSym<double> covMatrix();
  TMatrixTSym<double> covMatrixFull();

};
}//namespace MINT
#endif
//
