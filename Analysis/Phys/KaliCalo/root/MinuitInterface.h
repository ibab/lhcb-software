#ifndef MINUITINTERFACE_H
#define MINUITINTERFACE_H

#define CALOSHIFT 16384

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <TObject.h>
#include <TTree.h>
#include <TChain.h>
#include <TString.h>
#include <TH1F.h>
#include <math.h> 
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

const double pionmass = 134.9766;

class MinuitInterface : public TObject {
private:
  // Chain variables
  TChain*                           m_chain;
  float                             m_mass;
  int                               m_cellID1;   
  int                               m_cellID2;
  int                               m_bkg;
  float                             m_e1;
  float                             m_e2;
  float                             m_prs1;
  float                             m_prs2;  
  // Fit variables
  bool                              gDebug;
  float                             m_sigma[3];
  int                               m_sigmaCut;
  float                             m_tolerance;
  float                             m_correction;
  std::map<int,TString>             m_cellIDs;
  std::map<TString,int>             m_parameters;
  std::set<int>                     m_entries;
  unsigned int                      m_vars;
  int                               m_nEntries;
  float                             m_prsBeta[3];
  // Internal objects
  ROOT::Minuit2::Minuit2Minimizer*  fMinuit;
  ROOT::Math::IMultiGenFunction*    fFunc;  
  // Minimization functions
  virtual double weight( const double, const float );
  void configureMinuit( const int );
  virtual double fcn( const double* ) = 0;
public:
  // Constructors and configuration
  MinuitInterface() : m_chain(NULL), gDebug(false), m_sigmaCut(5), m_tolerance(0.0001), m_correction(0.0), fMinuit(NULL), fFunc(NULL) {};
  MinuitInterface( TChain* chain ) : m_chain(NULL), gDebug(false), m_sigmaCut(5), m_tolerance(0.0001), m_correction(0.0), fMinuit(NULL), fFunc(NULL) { loadTree(chain); };
  // Destructor
  ~MinuitInterface(){ if (fMinuit) delete fMinuit; if (fFunc) delete fFunc; if (m_chain) delete m_chain; }
  // File loading
  void loadFiles( std::vector<TString> );
  // Tree loading
  // void loadTree( TTree* );
  void loadTree( TChain* );  
  virtual void loadTreeVars();
  // Configuration
  void configure( int );
  // Minimization control
  int addCell( const int , const TString , const double=1.0 );
  int setFixedVar( const TString, const double );
  int setLimitedVar( const TString , const double, const double, const double );
  int minimize();
  void setDebug() { gDebug = true; };
  float setCorrection();
  float setCorrection( float correction ) { m_correction = correction; return m_correction; };
  // Other config
  void setStrategy( int value ) { if(fMinuit) fMinuit->SetStrategy(value); };
  double getVarCalib( const TString );
  double getCellCalib( const int );
  std::pair<double,double> getVarError( const TString );
  std::pair<double,double> getCellError( const int );
  void setSigma( int , float );
  void setBeta( int, float );
  void setSigmaCut( int nsigmas ) { m_sigmaCut = nsigmas; }
  void setTolerance( float tolerance ) { m_tolerance = tolerance; }  
  std::vector<int> getCellIDs();
  ClassDef( MinuitInterface , 1 );
};

#endif
