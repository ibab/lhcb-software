// $Id: AlgorithmCorrelations.cpp,v 1.6 2009-04-15 09:48:14 pkoppenb Exp $
// Include files 
#include <boost/format.hpp>
#include <math.h>

// local
#include "AlgorithmCorrelations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlgorithmCorrelations
//
// 2005-04-19 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( AlgorithmCorrelations )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlgorithmCorrelations::AlgorithmCorrelations( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_conditionAlgorithms ()
    , m_algorithmsToTest ()
    , m_AlgoMatrices ()
    , m_conditionResults()
    , m_testResults()
    , m_longestName(10)
    , m_square(false)
    , m_nEvents(0)
{
  declareInterface<IAlgorithmCorrelations>(this);
  declareProperty( "Algorithms", m_algorithmsToTest ); // can be overwritten
  declareProperty( "AlgorithmsRow", m_conditionAlgorithms ); // can be overwritten
  declareProperty( "OnlyNonZero", m_minimize = true );
  declareProperty( "Decimals", m_decimals = -1 );
  declareProperty( "UseNumbers", m_useNumbers = true );

}
//=============================================================================
// Destructor
//=============================================================================
AlgorithmCorrelations::~AlgorithmCorrelations() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlgorithmCorrelations::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  if ( !m_algorithmsToTest.empty() ) return reset() ; // options are active
  else return sc;
}
//=============================================================================
// Set matrix
//=============================================================================
StatusCode AlgorithmCorrelations::reset(){
  if (msgLevel(MSG::DEBUG)) debug() << "==> Reset" << endmsg;

  if (m_algorithmsToTest.empty()){
    err() << "No algorithms defined. Use Algorithms option, or algorithms method before algorithmsRow." << endmsg;
    return StatusCode::FAILURE;
  }

  m_AlgoMatrices.clear();
  m_conditionResults.clear();
  m_testResults.clear();

  info() << "Algorithms to check correlations: " << m_algorithmsToTest << endmsg ;
  if (!testAlgos(m_algorithmsToTest)) return StatusCode::FAILURE;
  if (m_conditionAlgorithms.empty()){
    info() << "No algorithms row defined. Will make a square matrix using algorithms column." << endmsg;
    m_square = true ;
    m_conditionAlgorithms = m_algorithmsToTest ; // copy one in the other
  } else {
    info() << "Algorithms to check correlations against: " << m_conditionAlgorithms << endmsg ;
    if (!testAlgos(m_conditionAlgorithms)) return StatusCode::FAILURE;
    m_square = false ;
  }

  // minimize?
  if ( m_minimize ) warning() << "Will only print non-zero efficient algorithms in table" << endmsg ;
  
  if ( m_decimals>=0) info() << "Will print efficiencies with " << m_decimals 
                             << " decimals (percent)" << endmsg ;
  else info() << "Will print efficiencies with automatic precision" << endmsg ;

  //  int i1 = 0 ;

  for( strings::iterator ialg2 =  m_conditionAlgorithms.begin() ; 
       ialg2 != m_conditionAlgorithms.end(); ++ialg2 ){
    AlgoResult AR(*ialg2);
    m_conditionResults.push_back(AR);
  }
  
  for( strings::iterator ialg1 = m_algorithmsToTest.begin(); 
       ialg1 != m_algorithmsToTest.end(); ++ialg1 ){
    AlgoResult AR(*ialg1);
    m_testResults.push_back(AR);
    unsigned int l = ialg1->length();
    if ( l > m_longestName ) m_longestName = l ;
    for( strings::iterator ialg2 =  m_conditionAlgorithms.begin() ; 
         ialg2 != m_conditionAlgorithms.end(); ++ialg2 ){
      AlgoMatrix SRM( *(ialg1), *(ialg2) );
      m_AlgoMatrices.push_back( SRM ) ;
      if (msgLevel(MSG::DEBUG)) debug() << "Pushed back pair " << *(ialg1) << ", " << *(ialg2) << endmsg ;
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// end of event - do the work
//=============================================================================
StatusCode AlgorithmCorrelations::endEvent(){
  if (msgLevel(MSG::DEBUG)) debug() << "End of event... do the work" << endmsg ;
  ++m_nEvents;
  StatusCode sc = addResults();
  if (!sc) return sc;
  sc = resetAlgoResult(m_conditionResults);
  if (!sc) return sc;
  sc = resetAlgoResult(m_testResults);
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
  if (msgLevel(MSG::DEBUG)) debug() << "Put all results in matrices" << endmsg ;
  // fill statistics
  std::vector<AlgoMatrix>::iterator isrm = m_AlgoMatrices.begin();
  for ( std::vector<AlgoResult>::const_iterator iar1 = m_testResults.begin();
        iar1 != m_testResults.end(); ++iar1){
    for ( std::vector<AlgoResult>::const_iterator iar2 = m_conditionResults.begin();
          iar2 != m_conditionResults.end(); ++iar2){
      if (isrm == m_AlgoMatrices.end()){
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
      if (a2=="ALWAYS") {
        isrm->addConditionalResult(iar1->result(),true);
        if (msgLevel(MSG::VERBOSE)) verbose() << "A1 " << a1 << " has now " << isrm->getFullStatistics()
                                              << " in " << m_nEvents << endmsg ;
      } else if (a1=="ALWAYS") {
        isrm->addConditionalResult(true,iar2->result());
        if (msgLevel(MSG::VERBOSE)) verbose() << "A2 " << a2 << " has now " << isrm->getConditionalStatistics()
                                              << " in " << m_nEvents << endmsg ;
      } else {
        isrm->addConditionalResult(iar1->result(),iar2->result());
        verbose() << "Added result " << iar1->result() << " " << iar2->result()
                  << " for " << a1 << " " << a2 << " and have now " << 
          isrm->getConditionalFraction() << " of " << isrm->getFullStatistics() << endmsg ;
      }
      ++isrm;
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Fill a result - entry point
//=============================================================================
StatusCode AlgorithmCorrelations::fillResult(const std::string& algo, 
                                             const bool& result){
  StatusCode sc = fillResult(algo,result,m_conditionResults);
  if (!sc) return sc;
  sc = fillResult(algo,result,m_testResults);
  return sc;
}
//=============================================================================
// Fill a result
//=============================================================================
StatusCode AlgorithmCorrelations::fillResult(const std::string& algo, 
                                             const bool& result,
                                             std::vector<AlgoResult>& Results){
  if (msgLevel(MSG::DEBUG)) debug() << "fillResult " << algo << " " << result << endmsg ;
  bool found = false ;
  for ( std::vector<AlgoResult>::iterator iar = Results.begin();
        iar != Results.end(); ++iar){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Checking algorithm " << iar->algo() << endmsg ;
    if ((iar->algo() == algo ) || ("/Event/Phys/"+iar->algo() == algo)){
      if (msgLevel(MSG::DEBUG)) debug() << "Updating algorithm " << iar->algo() << endmsg ;
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
  if (found) {
    if (msgLevel(MSG::DEBUG)) debug() << "Algorithm " << algo << " has been found" << endmsg ;
  } else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Algorithm " << algo << " has not been found" << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Set algorithms
//=============================================================================
StatusCode AlgorithmCorrelations::algorithms(const strings& algos){
  m_algorithmsToTest = algos ;
  m_algorithmsToTest.insert(m_algorithmsToTest.begin(),"ALWAYS");  
  return reset();
}
//=============================================================================
StatusCode AlgorithmCorrelations::algorithmsRow(const strings& algos){
  m_conditionAlgorithms = algos ;
  m_conditionAlgorithms.insert(m_conditionAlgorithms.begin(),"ALWAYS");
  return reset();
}
//=============================================================================
//  Test
//=============================================================================
StatusCode AlgorithmCorrelations::testAlgos(const strings& algos) const{
  for ( strings::const_iterator i1 = algos.begin() ; i1!= algos.end(); ++i1){
    for ( strings::const_iterator i2 = algos.begin() ; i1!= i2; ++i2){
      if (*i1==*i2){
        err() << "Duplicate instance of ``" << *i1 << "''" << endmsg ;
        err() << "Fix you options" << endmsg ;
        return StatusCode::FAILURE ;
      }
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Print a list
//=============================================================================
StatusCode  AlgorithmCorrelations::printList() {

  info() << "Correlation list is ugly but useful with grep" << endmsg ;

  boost::format rate("%1$6.4f");
  boost::format algo("%1$-25S");

  always() << "Correlation list:\n" ;
  for( std::vector<AlgoMatrix>::iterator isrm = m_AlgoMatrices.begin() ; 
       isrm != m_AlgoMatrices.end(); ++isrm ){
    std::string a1, a2;
    isrm->getAlgorithms( a1, a2 );
    if ( isEffective( a2 )){
      double y12 = isrm->getConditionalFraction();
      always() << algo % a1  ;
      always() << " is accepted with "<< rate % y12 ;
      always() << " probability when " << algo % a2 ;
      always() << " is true\n" ;
    }  
  }
  always() << endmsg; ;
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Print a table
//=============================================================================
StatusCode AlgorithmCorrelations::printTable(void) {

  const unsigned int nalgow = happyAlgorithms() ;
  const unsigned int decimals = getDecimals();

  if (msgLevel(MSG::DEBUG)) debug() << nalgow << " columns find something " << endmsg ;

  const unsigned int nl = 4 + 9 + decimals + m_longestName + ( 6 + decimals )*nalgow ;
  const unsigned int namelength =  name().length() ;
  const double halflength = ((double)nl - namelength )/2. ;
  const unsigned int nlh = (halflength>0 ? int(halflength) : 0 );
  if (msgLevel(MSG::VERBOSE)) verbose() << name() << " namelength: " << namelength 
                                        << " decimals: " << decimals 
                                        << " longest: " << m_longestName
                                        << " nl: " << nl 
                                        << " halflength: " << halflength 
                                        << " nlh: " << nlh 
                                        << endmsg ;
  std::string halfspace = "";
  if ( nlh > 0 ) halfspace = std::string(nlh,' ');
  if (msgLevel(MSG::DEBUG)) debug() << "Will print table with " << nl << " " << nlh << endmsg ;
  
  const std::string hyphenline(nl, '-');
  const std::string equalline(nl, '=');
  const std::string crosses = std::string("  ")+std::string(4+decimals, '*') ;
  const std::string hashes = std::string("  ")+std::string(4+decimals, '#') ;

  std::string mdec = std::to_string(decimals);
  std::string mdec2 = std::to_string(4+decimals);
  std::string mdec3 = std::to_string(5+decimals);
  boost::format percent(" %1$"+mdec2+"."+mdec+"f");
  std::string mln = std::to_string(m_longestName);
  boost::format algo("%1$-"+mln+"S ");         // longest name left aligned
  boost::format number("  %1$="+mdec2+"d");    // number of decimals+4 length
  boost::format smallnumber(" %1$2i ");        // number of 2 positions
  boost::format empty("    ");                 // empty string
  boost::format shortalgo("%1$="+mdec3+"S ");  // string decimals+5 in length, centered

  always() << "Correlation Table:\n\n";
  // header
  always() << equalline << "\n"  ;
  always() << halfspace << name() << "\n" ;
  always() << equalline << "\n"  ;
  always() << "    " << algo % "Algorithm" << "   Eff.  " ;

  if (!m_useNumbers) always() << std::string(decimals,' ');    // don't ask why I need this        

  unsigned int i = 0 ;
  for( strings::const_iterator ia = m_conditionAlgorithms.begin();
       ia!= m_conditionAlgorithms.end() ; ++ia  ){
    if ( *ia == "ALWAYS" ) continue ;
    if (( isEffective(*ia) )|| (!m_minimize)) {
      if ( m_useNumbers ){
        ++i;
        always() << number % i ;
      } else {
        always() << shortalgo % (ia->size()<5+decimals ? (*ia) : ia->substr(0,5+decimals) ) ;
      }
    }
  }
  
  always() << "\n" ;
  always() << hyphenline ;
  unsigned int ia1 = 0 ;

  // the table
  bool doprint = true ;
  bool addline = false ;
  for( std::vector<AlgoMatrix>::iterator isrm = m_AlgoMatrices.begin() ;
       isrm != m_AlgoMatrices.end(); 
       ++isrm ){
    std::string a1, a2;
    isrm->getAlgorithms( a1, a2 );
    doprint = ( (!m_minimize) || isEffective( a1 ));
    if (( a1 == "ALWAYS") && ( m_square )) continue ; // no efficiency if square
    /*
     * efficiency row
     */
    if ( a2 == "ALWAYS" ) { 
      if ( addline ){
        always() << "\n" << hyphenline ;
        addline = false ;
      }
      if ( a1 == "ALWAYS" ){
        always() << "\n" << "    " << algo % "Efficiency" << crosses << " |";
        addline = true ;
      } else {
        if ( doprint ) { 
          ++ia1 ;               // increment
          always() << "\n" ;
          always() << smallnumber % ia1 << algo % a1 ;
          double y12 = isrm->getConditionalPercent();
          always() << percent % y12 << "% |";
        }
      }
    /*
     * standard
     */
    } else if ( doprint || ( a1 == "ALWAYS" )){
      if ((m_minimize) && (!isEffective( a2 ))) continue;
      if ( a1 == a2 ) always() << hashes ;
      else {
        double y12 = isrm->getConditionalPercent();
        if ( a1 == "ALWAYS" ) y12 = 100.0*isrm->getFullStatistics()/(double)m_nEvents ;
        if ( y12 < 0 ) always() << crosses ;
        else always() << percent % y12 << "%" ;
      } 
    }
  }
  always() << "\n" << equalline << "\n" ;
  if ( (!m_square) && (m_useNumbers)){
    unsigned int j = 0 ;
    always() << "Column labels are : \n" ;
    for( strings::const_iterator ia = m_conditionAlgorithms.begin();
         ia!= m_conditionAlgorithms.end() ; ++ia  ){
      if (*ia == "ALWAYS" ) continue ;
      if ((m_minimize) && !(isEffective( *ia ))) continue;
      j++ ;
      always() << smallnumber % j << algo % *ia << "\n" ;
    }
  }

  always() << endmsg ;

  if (msgLevel(MSG::VERBOSE)) {
    printList() ;
  
    for( std::vector<AlgoMatrix>::iterator isrm = m_AlgoMatrices.begin() ;
         isrm != m_AlgoMatrices.end(); 
         ++isrm ){
      std::string a1, a2;
      isrm->getAlgorithms( a1, a2 );
      verbose() << a1 << " " << a2 << " " << isrm->getConditionalStatistics() << " " << isrm->getFullStatistics()
                << " " << m_nEvents << " " << algoRate(a1) << " " << algoRate(a2) << endmsg ;
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  number of algos that did something
//=============================================================================
int AlgorithmCorrelations::happyAlgorithms(void) const {

  if ( !m_minimize ) return m_conditionAlgorithms.size() ;  
  int nalgow = 0 ;
  std::string firstalgo = "" ;
  for( strings::const_iterator ia = m_conditionAlgorithms.begin();
       ia!= m_conditionAlgorithms.end() ; ++ia  ){
     if (( isEffective(*ia) ) && (*ia != "ALWAYS" )) ++nalgow ;
     if (msgLevel(MSG::VERBOSE)) verbose() << "Algorithm " << *ia << " says: " << isEffective(*ia) << endmsg ;
  } 
  return nalgow;  
}
//=============================================================================
//  Number of significant decimals
//=============================================================================
unsigned int AlgorithmCorrelations::getDecimals(void) const {
  if (m_decimals>=0) return m_decimals ;

  int maxevt = 1;
  for( std::vector<AlgoMatrix>::const_iterator isrm = m_AlgoMatrices.begin() ; 
       isrm != m_AlgoMatrices.end(); ++isrm ){
    int sum = isrm->getFullStatistics();
    if (sum>maxevt) maxevt = sum;
    //    if (msgLevel(MSG::VERBOSE)) verbose() << sum << " -> max = " << maxevt << endmsg ;
  }
  double minerror = 100./maxevt;
  double signif = -log(minerror)/log(10.)+1;
  int IntSignif = int(signif);
  if (msgLevel(MSG::DEBUG)) debug() << "Minimal error is " << minerror << "% -> gets " << IntSignif << endmsg ;
  if (IntSignif<=0) return 0;
  else return IntSignif ;
}
//=============================================================================
//  Algo did something
//=============================================================================
bool AlgorithmCorrelations::isEffective(const std::string& name ) const {

  return (algoRate(name)>0) ;
}
//=============================================================================
//  Algo did something
//=============================================================================
double AlgorithmCorrelations::algoRate(const std::string& name ) const {

  for( std::vector<AlgoMatrix>::const_iterator isrm = m_AlgoMatrices.begin() ;
       isrm != m_AlgoMatrices.end(); 
       ++isrm ){
    std::string a1, a2;
    isrm->getAlgorithms( a1, a2 );
    if (( a1==name ) && ( a2=="ALWAYS")) return isrm->getConditionalFraction() ;   // standard efficiency
    if (( a2==name ) && ( a1=="ALWAYS")) {
      if (m_nEvents>0) return (double)isrm->getFullStatistics()/(double)m_nEvents ; // non-standard
      else return 0;
    }
  }
  Error("Could not find algorithm "+name) ;
  return -1 ;
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
void AlgorithmCorrelations::AlgoMatrix::addConditionalResult(const bool& r1,const bool& r2){
   
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
