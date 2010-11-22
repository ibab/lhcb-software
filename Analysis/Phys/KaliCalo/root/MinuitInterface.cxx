#include "MinuitInterface.h"

#if !defined( __CINT__ )
  ClassImp( MinuitInterface );
#endif

void MinuitInterface::loadFiles( std::vector<TString> fileList ){
  if (m_chain) delete m_chain;
  m_chain = new TChain("KaliPi0/Pi0-Tuple");
  for (std::vector<TString>::iterator ifile = fileList.begin(); ifile != fileList.end(); ifile++){
    m_chain->AddFile(*ifile);
  }
  loadTreeVars();
}

void MinuitInterface::loadTree( TChain* chain ){
  // if ( m_chain ) delete m_chain;
  m_chain = chain;
  loadTreeVars();
  int n = 0;
  while ( m_chain->GetEntry( n ) ) {
    n++;
  }
  m_nEntries = n;
}

void MinuitInterface::loadTreeVars(){
  m_chain->SetBranchAddress( "m12"  , &m_mass );
  m_chain->SetBranchAddress( "ind1" , &m_cellID1 );
  m_chain->SetBranchAddress( "ind2" , &m_cellID2 );
  m_chain->SetBranchAddress( "bkg"  , &m_bkg);  
  m_chain->SetBranchAddress( "prs1" , &m_prs1 );
  m_chain->SetBranchAddress( "prs2" , &m_prs2 );    
  m_chain->SetBranchAddress( "g1E"  , &m_e1 );
  m_chain->SetBranchAddress( "g2E"  , &m_e2 );
}

void MinuitInterface::configure( int nVars ) {
  // Instantiation of minuit object
  configureMinuit( nVars );
  // Clear parameter vector
  m_parameters.clear();
  m_parameters[ "" ] = -1;
  m_cellIDs.clear();
  m_entries.clear();
}

void MinuitInterface::configureMinuit( const int nVars ){
  fMinuit = new ROOT::Minuit2::Minuit2Minimizer( ROOT::Minuit2::kSimplex );
  fFunc = new ROOT::Math::Functor( this , &MinuitInterface::fcn , nVars );
  m_vars = nVars;
  fMinuit->SetFunction( *fFunc );
  // Printing options
  if ( gDebug ) fMinuit->SetPrintLevel(3);
  else fMinuit->SetPrintLevel(-1);
  // Setting value of UP parameter
  fMinuit->SetErrorDef(1.0);
  fMinuit->SetMaxFunctionCalls(10000000);
  fMinuit->SetMaxIterations(1000000);
  fMinuit->SetTolerance(m_tolerance);
}

double MinuitInterface::weight( const double massdiff, const float sigma ){
  return exp( -0.5*pow( massdiff , 2 ) / pow ( sigma , 2 ) );
}

int MinuitInterface::addCell( const int cellID , const TString varName , const double initialValue ){
  if ( !fMinuit ){
    std::cout << "Minuit not configured" << std::endl;
    return 0;
  }
  if ( m_parameters.size() - 1  > m_vars ) {
    std::cout << "Max number of parameters reached: " << m_vars << std::endl;
    return 0;
  }
  if ( m_cellIDs.insert( std::pair<int,TString>( cellID , varName ) ).second == false ) {
    std::cout << "CellID already known: " << cellID << std::endl;
    return 0;
  }
  int varNum = m_parameters.size() - 1;
  if ( m_parameters.insert( std::pair<TString,int>( varName , varNum ) ).second == true ){
    if (gDebug) std::cout << "Setting variable " << varName << " to initial value " << initialValue << std::endl;
    fMinuit->SetFixedVariable( varNum , varName.Data() , initialValue );
  }
  return 1;
}

int MinuitInterface::setFixedVar( const TString varName, const double value ){
  if ( !fMinuit ){
    std::cout << "Minuit not configured" << std::endl;
    return 0;
  }
  if ( m_parameters.count(varName) > 0 ){
    // std::cout << "Fixing variable " << varName << std::endl;
    const int varNum = m_parameters[varName];
    // double value = fMinuit->X()[varNum];
    if (gDebug) std::cout << "Setting fixed var " << varName << " to value " << value << std::endl;    
    fMinuit->SetFixedVariable(varNum, varName.Data(), value);
    return 1;    
  }
  return 0;
}

int MinuitInterface::setLimitedVar( const TString varName, const double value, const double minValue, const double maxValue ){
  if ( !fMinuit ){
    std::cout << "Minuit not configured" << std::endl;
    return 0;
  }
  if ( m_parameters.count(varName) > 0 ){
    const int varNum = m_parameters[varName];
    // double value = fMinuit->X()[varNum];
    if (gDebug) std::cout << "Setting limited var " << varName << " to value " << value << std::endl;
    fMinuit->SetLimitedVariable(varNum, varName.Data(), value,  0.01 , minValue , maxValue );
    return 1;
  }
  return 0;
} 

void MinuitInterface::setSigma( int area, float sigma ){
  m_sigma[area] = sigma;
}

void MinuitInterface::setBeta( int area, float beta ){
  m_prsBeta[area] = beta;
}

float MinuitInterface::setCorrection(){
  float nsig = m_chain->GetEntries("(bkg==0)*(m12>200 && m12<250)");
  float nbkg = m_chain->GetEntries("(bkg==1)*(m12>200 && m12<250)");
  float correction = 1 - nsig/nbkg;
  return setCorrection(correction);
}

int MinuitInterface::minimize(){
  if ( m_parameters.size() - 1 < m_vars ) {
    std::cout << "Warning: There are still non-defined parameters!" << std::endl;
    if ( gDebug ) {
      std::cout << "  Number of initialized parameters: " << m_parameters.size() - 1 << std::endl;
      std::cout << "  Max number of parameters: " << m_vars << std::endl;
    }
  }
  time_t tstart, tend;
  tstart = time(0);
  fMinuit->Minimize();
  tend = time(0);
  std::cout << "The calibration took " << difftime( tend, tstart ) << " seconds" << std::endl;
  return fMinuit->Status();
}

double MinuitInterface::getVarCalib( const TString var ){
  if ( !fMinuit ) return -1.0;
  const double* m_result = fMinuit->X();
  return m_result[m_parameters[var]];
}

double MinuitInterface::getCellCalib( const int cellID ) {
  return getVarCalib(m_cellIDs[cellID]);
};

std::pair<double,double> MinuitInterface::getVarError( const TString var ) {
  if ( !fMinuit ) return std::make_pair(0.0, 0.0);
  double errLow, errUp;
  const int parnumber = m_parameters[var];
  bool sc = fMinuit->GetMinosError(parnumber, errLow, errUp);
  if (!sc) return std::make_pair(0.0, 0.0);
  return std::make_pair(errLow, errUp);
};

std::pair<double,double> MinuitInterface::getCellError( const int cellID ) {
  return getVarError(m_cellIDs[cellID]);
};

std::vector<int> MinuitInterface::getCellIDs() {
  std::vector<int> cells;
  for ( std::map<int,TString>::iterator iter = m_cellIDs.begin(); iter != m_cellIDs.end(); iter++ ){
    cells.push_back(iter->first);
  }
  return cells;
}
