// $Id: AlgorithmCorrelations.cpp,v 1.1 2007-03-02 08:49:26 cattanem Exp $
// Include files 
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <math.h>
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "AlgorithmCorrelations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlgorithmCorrelations
//
// 2005-04-19 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( AlgorithmCorrelations );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlgorithmCorrelations::AlgorithmCorrelations( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_algorithmsRow ()
    , m_algorithmsColumn ()
    , m_SelResultMatrices ()
    , m_rowResults()
    , m_columnResults()
    , m_longestName(10)
{
  declareInterface<IAlgorithmCorrelations>(this);
  declareProperty( "Algorithms", m_algorithmsColumn ); // can be overwritten
  declareProperty( "AlgorithmsRow", m_algorithmsRow ); // can be overwritten
  declareProperty( "OnlyNonZero", m_minimize = true );
  declareProperty( "Decimals", m_decimals = -1 );

}
//=============================================================================
// Destructor
//=============================================================================
AlgorithmCorrelations::~AlgorithmCorrelations() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlgorithmCorrelations::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  if ( !m_algorithmsColumn.empty() ) return reset() ; // options are active
  else return sc;
}
//=============================================================================
// Set matrix
//=============================================================================
StatusCode AlgorithmCorrelations::reset(){
  debug() << "==> Reset" << endmsg;
  if (m_algorithmsColumn.empty()){
    err() << "No algorithms defined. Use Algorithms option." << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "Algorithms to check correlations:" << m_algorithmsColumn << endmsg ;
  if (m_algorithmsRow.empty()){
    info() << "No algorithms row defined. Will make a square matrix using algorithms column." << endmsg;
  } else info() << "Algorithms to check correlations against:" << m_algorithmsRow << endmsg ;

  // square or not ?
  if ( m_algorithmsRow.empty() ){
    m_algorithmsRow = m_algorithmsColumn ; // copy one in the other
  }
  m_algorithmsRow.insert(m_algorithmsRow.begin(),"ALWAYS");

  // minimize?
  if ( m_minimize ) warning() << "Will only print non-zero efficient algorithms in table" << endmsg ;
  
  if ( m_decimals>=0) info() << "Will print efficiencies with " << m_decimals 
                             << " decimals (percent)" << endmsg ;
  else info() << "Will print efficiencies with automatic precision" << endmsg ;
  

  //  int i1 = 0 ;

  for( std::vector<std::string>::iterator ialg2 =  m_algorithmsRow.begin() ; 
       ialg2 != m_algorithmsRow.end(); ++ialg2 ){
    int l = ialg2->length();
    if ( l > m_longestName )  m_longestName = l ;
    AlgoResult AR(*ialg2);
    m_rowResults.push_back(AR);
  }
  
  for( std::vector<std::string>::iterator ialg1 = m_algorithmsColumn.begin(); 
       ialg1 != m_algorithmsColumn.end(); ++ialg1 ){
    AlgoResult AR(*ialg1);
    m_columnResults.push_back(AR);
    for( std::vector<std::string>::iterator ialg2 =  m_algorithmsRow.begin() ; 
         ialg2 != m_algorithmsRow.end(); ++ialg2 ){
      //    ++i1 ;
      //    int i2 = 0 ;
      //      ++i2 ;
      //      if ( ( !m_square ) || (i1<i2)) {
      AlgoMatrix SRM( *(ialg1), *(ialg2) );
      m_SelResultMatrices.push_back( SRM ) ;
      debug() << "Pushed back pair " << *(ialg1) << ", " << *(ialg2) << endmsg ;
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// end of event - do the work
//=============================================================================
StatusCode AlgorithmCorrelations::endEvent(){
  debug() << "End of event... do the work" << endmsg ;
  StatusCode sc = addResults();
  if (!sc) return sc;
  sc = resetAlgoResult(m_rowResults);
  if (!sc) return sc;
  sc = resetAlgoResult(m_columnResults);
  return sc ;
}
//=============================================================================
// Clear for next event
//=============================================================================
StatusCode AlgorithmCorrelations::resetAlgoResult(std::vector<AlgoResult>& Results){
  for (std::vector<AlgoResult>::iterator ir = Results.begin();
       ir != Results.end(); ++ir){
    ir->reset();
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Add all results to matrix elements
//=============================================================================
StatusCode AlgorithmCorrelations::addResults(){
  debug() << "Put all results in matrices" << endmsg ;
  // fill statistics
  std::vector<AlgoMatrix>::iterator isrm = m_SelResultMatrices.begin();
  for ( std::vector<AlgoResult>::const_iterator iar1 = m_columnResults.begin();
        iar1 != m_columnResults.end(); ++iar1){
    for ( std::vector<AlgoResult>::const_iterator iar2 = m_rowResults.begin();
          iar2 != m_rowResults.end(); ++iar2){
      if (isrm == m_SelResultMatrices.end()){
        err() << "Reached end of matrices" << endmsg;
         return StatusCode::FAILURE;      
      }
      std::string a1, a2;
      isrm->getAlgorithms(a1,a2);
      if ( ( a1 != iar1->algo() ) || ( a2 != iar2->algo() )){
        err() << "Confused! I see " << a1 << " and " << a2 << " and expect " 
              << iar1->algo() << " and " << iar2->algo() << endmsg ;
        return StatusCode::FAILURE;
      }
      if (a2=="ALWAYS") isrm->addResult(iar1->result(),true);
      else isrm->addResult(iar1->result(),iar2->result());
      ++isrm;
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Fill a result
//=============================================================================
StatusCode AlgorithmCorrelations::fillResult(const std::string& algo, 
                                             const bool& result){
  StatusCode sc = fillResult(algo,result,m_rowResults);
  if (!sc) return sc;
  sc = fillResult(algo,result,m_columnResults);
  return sc;
}
//=============================================================================
// Fill a result
//=============================================================================
StatusCode AlgorithmCorrelations::fillResult(const std::string& algo, 
                                             const bool& result,
                                             std::vector<AlgoResult>& Results){
  debug() << "FillResult " << algo << " " << result << endmsg ;
  bool found = false ;
  for ( std::vector<AlgoResult>::iterator iar = Results.begin();
        iar != Results.end(); ++iar){
    verbose() << "Checking algorithm " << iar->algo() << endmsg ;
    if ((iar->algo() == algo ) || ("/Event/Phys/"+iar->algo() == algo)){
      debug() << "Updating algorithm " << iar->algo() << endmsg ;
      StatusCode sc = iar->setResult(result);
      if (!sc){
        err() << "Resetting result for algorithm " << iar->algo() 
              << " in same event. Are you missing a m_AlgorithmCorrelations->endEvent()?" 
              << endmsg ;
        return StatusCode::FAILURE;
      }
      found = true ;
      break ;
    }
  }
  if (found) debug() << "Algorithm " << algo << " has been found" << endmsg ;
  else verbose() << "Algorithm " << algo << " has not been found" << endmsg ;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Set algorithms
//=============================================================================
StatusCode AlgorithmCorrelations::algorithms(const std::vector<std::string>& algos){
  m_algorithmsColumn = algos ;
  return reset();
}
//=============================================================================
StatusCode AlgorithmCorrelations::algorithmsRow(const std::vector<std::string>& algos){
  m_algorithmsRow = algos ;
  return reset();
}
//=============================================================================
//  Print a list
//=============================================================================
StatusCode  AlgorithmCorrelations::printList() {

  info() << "Correlation list is ugly but useful with grep" << endmsg ;

  boost::format rate("%1$6.4f");
  boost::format algo("%1$-25S");

  always() << "Correlation list:\n" ;
  for( std::vector<AlgoMatrix>::iterator isrm = m_SelResultMatrices.begin() ; 
       isrm != m_SelResultMatrices.end(); ++isrm ){
    std::string a1, a2;
    isrm->getAlgorithms( a1, a2 );
    double y12 = isrm->getFraction();
    always() << algo % a1  ;
    always() << " is accepted with "<< rate % y12 ;
    always()  << " probability when " << algo % a2 ;
    always()  << " is true\n" ;
  }  
  always() << endmsg; ;
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Print a table
//=============================================================================
StatusCode AlgorithmCorrelations::printTable(void) {

  const int nalgow = happyAlgorithms() ;
  const int decimals = getDecimals();

  debug() << nalgow << " algorithms find something " << endmsg ;

  const int nl = 4+9+decimals+m_longestName+(6+decimals)*nalgow ;
  const int namelength =  name().length() ;
  const double halflength = ((double)nl - namelength )/2. ;
  const int nlh = int(halflength);
  debug() << name() << " " << name().length() << " " << namelength << " " << halflength << endmsg ;
  std::string halfspace = "";
  if ( nlh > 0 ) halfspace = std::string(nlh,' ');
  debug() << "Will print table with " << nl << " " << nlh << endmsg ;
  
  const std::string hyphenline(nl, '-');
  const std::string equalline(nl, '=');
  const std::string crosses = std::string("  ")+std::string(4+decimals, '*') ;
  const std::string hashes = std::string("  ")+std::string(4+decimals, '#') ;

  std::string mdec = boost::lexical_cast<std::string>(decimals);
  std::string mdec2 = boost::lexical_cast<std::string>(4+decimals);
  boost::format percent(" %1$"+mdec2+"."+mdec+"f");
  std::string mln = boost::lexical_cast<std::string>(m_longestName);
  boost::format algo("%1$-"+mln+"S ");
  boost::format number("  %1$="+mdec2+"d");
  boost::format smallnumber(" %1$2i ");
  boost::format empty("    ");

  always() << "Correlation Table:\n\n";
  // header
  always() << equalline << "\n"  ;
  always() << halfspace << name() << "\n" ;
  always() << equalline << "\n"  ;
  always() << "    " << algo % "Algorithm" << "   Eff.  " ;

  int i = 0 ;
  for( std::vector<std::string>::const_iterator ia = m_algorithmsRow.begin();
       ia!= m_algorithmsRow.end() ; ++ia  ){
    if ( *ia == "ALWAYS" ) continue ;
    ++i;
    if (( isEffective(*ia) )|| (!m_minimize))  always() << number % i ;
  }
  always() << "\n" ;
  always() << hyphenline ;
  std::string presentalgo = "none" ;
  int ia1 = 0 ;

  // the table
  bool doprint = true ;
  for( std::vector<AlgoMatrix>::iterator isrm = m_SelResultMatrices.begin() ;
       isrm != m_SelResultMatrices.end(); 
       ++isrm ){
    std::string a1, a2;
    isrm->getAlgorithms( a1, a2 );
    doprint = ( (!m_minimize) || isEffective( a1 ));
    if ( a2 == "ALWAYS" ) {
      presentalgo = a1 ;
      ++ia1 ;
      if ( doprint ) { 
        always() << "\n" ;
        always() << smallnumber % ia1 << algo % a1 ;
        double y12 = isrm->getPercent();
        always() << percent % y12 << "% |";
      }
    } else if ( doprint ){
      if ((m_minimize) && !(isEffective( a2 ))) continue;
      if ( a1 == a2 ) always() << hashes ;
      else {
        double y12 = isrm->getPercent();
        if ( y12 < 0 ) always() << crosses ;
        else always() << percent % y12 << "%" ;
      } 
    }
  }
  always() << "\n" << equalline << "\n" ;
  always() << endmsg ;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  number of algos that did something
//=============================================================================
int AlgorithmCorrelations::happyAlgorithms(void) {

  if ( !m_minimize ) return m_algorithmsRow.size() ;  
  int nalgow = 0 ;
  std::string firstalgo = "" ;
  for( std::vector<std::string>::const_iterator ia = m_algorithmsRow.begin();
       ia!= m_algorithmsRow.end() ; ++ia  ){
     if (( isEffective(*ia) ) && (*ia != "ALWAYS" )) ++nalgow ;
     verbose() << "Algorithm " << *ia << " says: " << isEffective(*ia) << endmsg ;
  } 
  return nalgow;  
}
//=============================================================================
//  Number of significant decimals
//=============================================================================
int AlgorithmCorrelations::getDecimals(void) {
  if (m_decimals>=0) return m_decimals ;

  int maxevt = 1;
  for( std::vector<AlgoMatrix>::iterator isrm = m_SelResultMatrices.begin() ; 
       isrm != m_SelResultMatrices.end(); ++isrm ){
    int sum = isrm->getStatistics();
    if (sum>maxevt) maxevt = sum;
    //    verbose() << sum << " -> max = " << maxevt << endmsg ;
  }
  double minerror = 100./maxevt;
  double signif = -log(minerror)/log(10.)+1;
  int IntSignif = int(signif);
  debug() << "Minimal error is " << minerror << "% -> gets " << IntSignif << endmsg ;
  if (IntSignif<=0) return 0;
  else return IntSignif ;
}
//=============================================================================
//  Algo did something
//=============================================================================
bool AlgorithmCorrelations::isEffective(const std::string& name ) {

  for( std::vector<AlgoMatrix>::iterator isrm = m_SelResultMatrices.begin() ;
       isrm != m_SelResultMatrices.end(); 
       ++isrm ){
    std::string a1, a2;
    isrm->getAlgorithms( a1, a2 );
    if ( a2==name ){
      int sum = isrm->getStatistics();
      return ( sum>0 );
    }
  }
  err() << "Could not find algorithm " << name << endmsg ;
  return false ;
}


//#############################################################################
// AlgoMatrix implementation
//#############################################################################
//=============================================================================
/// Standard constructor
//=============================================================================
AlgorithmCorrelations::AlgoMatrix::AlgoMatrix(){
  m_bothpassed = 0 ;
  m_alg2passed = 0 ;
  m_algorithm1.clear();
  m_algorithm2.clear();  
  return ;
}  
//=============================================================================
/// Useful constructor
//=============================================================================
AlgorithmCorrelations::AlgoMatrix::AlgoMatrix( const std::string a1, const std::string a2 ){
  m_bothpassed = 0 ;
  m_alg2passed = 0 ;
  m_algorithm1 = a1;
  m_algorithm2 = a2;  
  return ;
}
  
//=============================================================================
/// Add result
//=============================================================================
void AlgorithmCorrelations::AlgoMatrix::addResult(const bool& r1,const bool& r2){
   
  if (!r2) return ;
  ++m_alg2passed;
  if (r1) ++m_bothpassed;
    
  return ;
}  
//=============================================================================
/// Return algorithm names
//=============================================================================
void AlgorithmCorrelations::AlgoMatrix::getAlgorithms( std::string& a1, std::string& a2) const{
  a1 = m_algorithm1 ;
  a2 = m_algorithm2 ;
  return  ;
}
