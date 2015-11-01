// $Id: Jets2JetsAlg.cpp,v 1.2 2010-04-16 14:49:10 jpalac Exp $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IRelateJets.h"
// ===========================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================

// ==========================================================================
/** @class MCJets2JetsAlg
 *  @date  2009-10-29 
 *  @author Victor Coco  victor.coco@cern.ch
 *  
 */
class MCJets2JetsAlg : public GaudiAlgorithm 
{
  // ========================================================================
  /// the friend factory for instantiation 
  // ========================================================================
public:  
  // ========================================================================    
  /** Standard constructor
   *  @param name instance name 
   *  @param pSvc pointer to Service Locator 
   */
  MCJets2JetsAlg
  ( const std::string& name ,
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc )
  // 
    , m_jetMatcherName (   )
    , m_jetALocation (   )
    , m_jetBLocation (   )
    , m_output       (   )
    , m_output2       (   )
    , m_jetMatcher     ( 0   )
  { 
    // 
    declareProperty    ( "Jets2Jets"             ,    m_jetMatcherName ,    "Type type/name of jet-jetMatcher tool (IRelateJets interface)") ;  
    declareProperty    ( "JetsALocation"         ,    m_jetALocation ,    "Location of the first set of jet") ;  
    declareProperty    ( "JetsBLocation"         ,    m_jetBLocation ,    "Location of the second set of jet") ; 
    declareProperty    ( "OutputTable"           ,    m_output       ,    "Location of the output table") ;  
    declareProperty    ( "OutputInverseTable"    ,    m_output2       ,    "Location of the output table for opposit match") ;  
    //
  }
  /// destructor
  virtual ~MCJets2JetsAlg( ){}
  // ========================================================================    
public:
  // ========================================================================    
  /** standard execution of the algorithm 
   *  @see LoKi::Algo 
   *  @return status code 
   */
  virtual StatusCode initialize   () ;
  virtual StatusCode execute  () ;
  // ========================================================================    

  // ========================================================================    
private:  
  // ========================================================================    
  /// jetMatcher name
  std::string      m_jetMatcherName ; ///< jet jetMatcher name  
  std::string      m_jetALocation ; ///< Location of the first set of jet
  std::string      m_jetBLocation ; ///< Location of the second set of jet
  std::string      m_output  ; ///< Location of the second set of jet
  std::string      m_output2  ; ///< Location of the second set of jet
  /// jetMatcher
  const IRelateJets* m_jetMatcher     ; ///< jet jetMatcher to be used 
  // ========================================================================    
};
// ==========================================================================
// end of namespace LoKi 
// ============================================================================
/** @file 
 *  Implementation file for class MCJets2JetsAlg
 *  Client for implementation of IRelateJets, furnishing the sets of jets out of
 *  transiant event store location and recording the resulting output table
 *
 *  @date  2009-10-29 
 *  @author Victor COCO  victor.coco@cern.ch
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCJets2JetsAlg )

StatusCode MCJets2JetsAlg::initialize   ()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  if( m_jetALocation.empty() || m_jetBLocation.empty() )return Error("No input locations is specified") ;
  if( m_jetMatcherName.empty() || m_jetBLocation.empty() )return Error("No jet matching tool specified") ;
  info()<<"Matching "<<m_jetALocation<<" with "<<m_jetBLocation<<endmsg;
  if (m_output.empty())return Error ( "No OutputTable is specified" ) ;
  if (m_output2.empty())return Error ( "No OutputInverseTable is specified" ) ;
  return sc;
}

StatusCode MCJets2JetsAlg::execute   () 
{
  // Get the jets
  const LHCb::Particles* PartsA    = get<LHCb::Particles>(m_jetALocation);
  const LHCb::Particles* PartsB    = get<LHCb::Particles>(m_jetBLocation);
  
  if ( PartsA->empty() || PartsB->empty() ){
    Warning("No jets to match in the event") ;
    setFilterPassed ( false ); 
    return StatusCode::SUCCESS ;
  }
  IRelateJets::Jets* JetsA = new IRelateJets::Jets();
  IRelateJets::Jets* JetsB = new IRelateJets::Jets();
  for (LHCb::Particles::const_iterator ip = PartsA->begin() ; PartsA->end() != ip ; ip++ ) JetsA->push_back(*ip);
  for (LHCb::Particles::const_iterator ip = PartsB->begin() ; PartsB->end() != ip ; ip++ ) JetsB->push_back(*ip);
  if ( 0 == m_jetMatcher ) m_jetMatcher = tool<IRelateJets> ( m_jetMatcherName ,m_jetMatcherName, this ) ;

  // create the relation table
  IRelateJets::Table* table122 = new IRelateJets::Table();
  put ( table122 , m_output ) ; 
  //always()<<"table122"<<endmsg;
  IRelateJets::Table* table221 = new IRelateJets::Table();
  put ( table221 , m_output2 ) ; 
  //always()<<"table221"<<endmsg;

  // call the tool
  m_jetMatcher->makeRelation(*JetsA, *JetsB, *table122, *table221);
  
  //always()<<"end"<<endmsg;
  // total number of established links 
  const size_t links122 = table122->relations().size() ;
  const size_t links221 = table221->relations().size() ;

  // make a statistics 
  if ( printStat() || msgLevel( MSG::VERBOSE ) ) { counter ( "# "+m_jetALocation+"->"+m_jetBLocation ) += links122 ; counter ( "# "+m_jetBLocation+"->"+m_jetALocation ) += links221; }
  //
  if (table122->relations().empty()) { 
    Warning("The relation table is empty!") ;
    setFilterPassed ( false ) ;
  }
  if (table221->relations().empty()) { 
    Warning("The relation table is empty!") ;
    setFilterPassed ( false ) ;
  }
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
}

// ===========================================================================
/// The factory
// ============================================================================
// The END 
// ============================================================================
