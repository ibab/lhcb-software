// $Id: LoKiJets2JetsAlg.cpp,v 1.2 2010-04-16 14:49:10 jpalac Exp $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/Algo.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IJets2Jets.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================

namespace LoKi 
{
  // ==========================================================================
  /** @class Jets2JetsAlg
   *  @date  2009-10-29 
   *  @author Victor Coco  victor.coco@cern.ch
   *  
   */
  class Jets2JetsAlg : public LoKi::Algo  
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<LoKi::Jets2JetsAlg> ;
    // ========================================================================
  protected:  
    // ========================================================================    
    /** Standard constructor
     *  @param name instance name 
     *  @param pSvc pointer to Service Locator 
     */
    Jets2JetsAlg
    ( const std::string& name ,
      ISvcLocator*       pSvc ) 
      : LoKi::Algo ( name , pSvc )
      // 
      , m_jetMatcherName (   )
      , m_jetALocation (   )
      , m_jetBLocation (   )
      , m_output       (   )
      , m_jetMatcher     ( 0   )
    { 
      // 
      declareProperty    ( "Jets2Jets"  ,    m_jetMatcherName ,    "Type type/name of jet-jetMatcher tool (IJets2Jets interface)") ;  
      declareProperty    ( "JetsALocation"  ,    m_jetALocation ,    "Location of the first set of jet") ;  
      declareProperty    ( "JetsBLocation"  ,    m_jetBLocation ,    "Location of the second set of jet") ; 
      declareProperty    ( "OutputTable"    ,    m_output       ,    "Location of the output table") ;  
      //
    }
    /// destructor
    virtual ~Jets2JetsAlg( ){}
    // ========================================================================    
  public:
    // ========================================================================    
    /** standard execution of the algorithm 
     *  @see LoKi::Algo 
     *  @return status code 
     */
    virtual StatusCode initialize   () ;
    virtual StatusCode analyse   () ;
    // ========================================================================    
  private:
    // ========================================================================    
    // the default constructor is disabled 
    Jets2JetsAlg () ;
    // the copy constructor is disabled 
    Jets2JetsAlg ( const Jets2JetsAlg& )  ;
    // the assignement operator is disabled 
    Jets2JetsAlg& operator=( const  Jets2JetsAlg& )  ;
    // ========================================================================    
  private:  
    // ========================================================================    
    /// jetMatcher name
    std::string      m_jetMatcherName ; ///< jet jetMatcher name  
    std::string      m_jetALocation ; ///< Location of the first set of jet
    std::string      m_jetBLocation ; ///< Location of the second set of jet
    std::string      m_output  ; ///< Location of the second set of jet
    /// jetMatcher
    const IJets2Jets* m_jetMatcher     ; ///< jet jetMatcher to be used 
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Jets2JetsAlg
 *  Client for implementation of IJets2Jets, furnishing the sets of jets out of
 *  transiant event store location and recording the resulting output table
 *
 *  @date  2009-10-29 
 *  @author Victor COCO  victor.coco@cern.ch
 */


StatusCode LoKi::Jets2JetsAlg::initialize   ()
{
  if( m_jetALocation.empty() || m_jetBLocation.empty() )return Error("No input locations is specified") ;
  if( m_jetMatcherName.empty() || m_jetBLocation.empty() )return Error("No jet matching tool specified") ;
  if (m_output.empty())return Error ( "No OutputTable is specified" ) ;  
  return  StatusCode::SUCCESS ;
}

StatusCode LoKi::Jets2JetsAlg::analyse   () 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  
  const LHCb::Particle::Range PartsA    = get<LHCb::Particle::Range>(m_jetALocation);
  const LHCb::Particle::Range PartsB    = get<LHCb::Particle::Range>(m_jetBLocation);

  if (PartsA.size()==0 || PartsB.size()==0 ){
   Warning("No jets to match in the event") ;
    setFilterPassed ( false ); 
    return StatusCode::SUCCESS ;
  }

  IJets2Jets::Jets* JetsA(0);
  IJets2Jets::Jets* JetsB(0);

  for (LHCb::Particle::Range::const_iterator ip = PartsA.begin() ; PartsA.end() != ip ; ip++ ) JetsA->push_back((*ip)->clone());
  for (LHCb::Particle::Range::const_iterator ip = PartsB.begin() ; PartsB.end() != ip ; ip++ ) JetsB->push_back((*ip)->clone());
  
  if ( 0 == m_jetMatcher ) m_jetMatcher = tool<IJets2Jets> ( m_jetMatcherName ,m_jetMatcherName, this ) ;
  
  // call the tool
  IJets2Jets::Table* table = new IJets2Jets::Table(m_jetMatcher->makeRelation(*JetsA, *JetsB));
  
  put ( table , m_output ) ; 

  // total number of established links 
  const size_t links = table->relations().size() ;
  // make a statistics 
  if ( printStat() || msgLevel( MSG::VERBOSE ) ) { counter ( "# "+m_jetALocation+"->"+m_jetBLocation ) += links ; }
  //
  if (table->relations().empty()) { 
    Warning("The relation table is empty!") ;
    setFilterPassed ( false ) ;
  }
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,Jets2JetsAlg)
// ============================================================================
// The END 
// ============================================================================
