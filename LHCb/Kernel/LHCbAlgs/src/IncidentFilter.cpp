// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
/** file
 *  Simple algorithm to filter events according occurance of 
 *  the certains incidents
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-12-16
 */
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  /** @class IncidentFilter 
   *
   *  Trivial algorithm to filter events according to occurance 
   *  of some incidents 
   *
   *  @code
   *
   *  from Configurables import Gaudi__IncidentFilter and IncFltr 
   *
   *  fltr = IncFltr ( Incidents = [ 'GoodEvent' ] ) 
   *
   *  @endcode
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-05
   */
  class IncidentFilter 
    : public            GaudiAlgorithm 
    , public virtual IIncidentListener 
  {
    // ========================================================================
    friend class AlgFactory<Gaudi::IncidentFilter> ;
    // ========================================================================
  public:
    // ========================================================================
    /// standard initialization 
    virtual StatusCode initialize () ;
    /// standard finalization  
    virtual StatusCode finalize   () ;
    /// the only one essential method 
    virtual StatusCode execute () ; // the only one essential method 
    // ========================================================================
  public:
    // ========================================================================
    // handle the incident 
    void handle ( const Incident& inc ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    IncidentFilter
    ( const std::string& name ,
      ISvcLocator*       pSvc ) ;                       // standard constructor 
    /// virtual destructor 
    virtual ~IncidentFilter ()  ;                       //   virtual destructor 
    // ========================================================================
  private: 
    // ========================================================================
    /// the default constructor is disabled 
    IncidentFilter  () ;                              // no default constructor
    /// the copy constructor is disabled 
    IncidentFilter  ( const IncidentFilter& ) ;         //  no copy constructor
    /// the assignement operator is disabled
    IncidentFilter& operator=( const IncidentFilter& ); //       no assignement
    // ========================================================================
  protected:
    // ========================================================================
    void setVeto ( bool value ) { m_veto = value ; }
    // ========================================================================
  private:
    // ========================================================================
    /// update handler for "What"
    void handler_1 ( Property&  p ) ;            // update handler for "What"
    // ========================================================================
  private:
    // ========================================================================
    void   subscribe () ;
    void unsubscribe () ;
    // ========================================================================
  private:
    // ========================================================================
    bool                     m_veto          ;
    bool                     m_decision      ;
    //
    typedef std::vector<std::string> LIST    ;
    LIST m_incidents     ;
    LIST m_old_incidents ;
    // ========================================================================
  } ; //                                            end of class IncidentFilter 
  // ==========================================================================
  /** @class IncidentVeto
   *
   *  Trivial algorithm to veto events according to occurance 
   *  of some incidents 
   *
   *  @code
   *
   *  from Configurables import Gaudi__IncidentVeto and IncVeto
   *
   *  fltr = IncVeto ( Incidents = [ 'BadEvent' ] ) 
   *
   *  @endcode
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-10-05
   */
  class IncidentVeto : public IncidentFilter 
  {
    // ========================================================================
    friend class AlgFactory<Gaudi::IncidentVeto> ;
    // ========================================================================
  public:
    // ========================================================================
    /// standard constructor 
    IncidentVeto
    ( const std::string& name ,
      ISvcLocator*       pSvc ) ;                       // standard constructor 
    /// virtual destructor 
    virtual ~IncidentVeto ()  ;                         //   virtual destructor 
    // ========================================================================
  private: 
    // ========================================================================
    /// the default constructor is disabled 
    IncidentVeto  () ;                              // no default constructor
    /// the copy constructor is disabled 
    IncidentVeto  ( const IncidentVeto& ) ;         //  no copy constructor
    /// the assignement operator is disabled
    IncidentVeto& operator=( const IncidentVeto& ); //       no assignement
    // ========================================================================
  } ; //                                            end of class IncidentFilter 
  // ==========================================================================
} //                                                     end of namespace Gaudi 
// ============================================================================
// Filter 
// ============================================================================
// standard constructor 
// ============================================================================
Gaudi::IncidentFilter::IncidentFilter
( const std::string& name ,
  ISvcLocator*       pSvc )                        // standard constructor 
  : GaudiAlgorithm  ( name , pSvc ) 
//
  , m_veto          ( false )
  , m_decision      ( false )
  , m_incidents     ( ) 
  , m_old_incidents ( ) 
//
{
  // ==========================================================================
  declareProperty 
    ( "Incidents" , 
      m_incidents , 
      "Incidents to be handled" ) -> 
    declareUpdateHandler ( &Gaudi::IncidentFilter::handler_1 , this ) ;
  // ==========================================================================
}
// ============================================================================
// virtual destructor 
// ============================================================================
Gaudi::IncidentFilter::~IncidentFilter(){}
// ============================================================================
// standard initialization 
// ============================================================================
StatusCode Gaudi::IncidentFilter::initialize () 
{
  //
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  subscribe () ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalize 
// ============================================================================
StatusCode Gaudi::IncidentFilter::finalize () 
{
  //
  unsubscribe () ;
  //
  return GaudiAlgorithm::finalize () ;
}
// ============================================================================
// subscribe incidents 
// ============================================================================
void Gaudi::IncidentFilter::subscribe () 
{
  //
  m_old_incidents.clear () ;
  //
  if ( m_incidents.empty() ) 
  { Warning ( "Empty list of incidents", StatusCode::SUCCESS ).ignore() ; return ; }
  //                  
  IIncidentSvc* isvc = svc<IIncidentSvc>( "IncidentSvc" ) ;
  //
  const LIST& tmp = m_incidents ;
  for ( LIST::const_iterator item = tmp.begin() ; tmp.end() != item ; ++item )
  { isvc->addListener ( this , *item ) ; } 
  //
  if ( m_incidents.end() == std::find ( m_incidents.begin       () , 
                                        m_incidents.end         () , 
                                        IncidentType::BeginEvent   ) ) 
  { isvc->addListener ( this ,  IncidentType::BeginEvent  ) ; }
  //
  m_old_incidents = m_incidents ;
  //
}
// ============================================================================
// subscribe incidents 
// ============================================================================
void Gaudi::IncidentFilter::unsubscribe () 
{
  //
  m_old_incidents.clear () ;
  //
  IIncidentSvc* isvc = svc<IIncidentSvc>( "IncidentSvc" ) ;
  //
  const LIST& tmp = m_old_incidents ;
  for ( LIST::const_iterator item = tmp.begin() ; tmp.end() != item ; ++item )
  { isvc->removeListener ( this , *item ) ; } 
  //
  m_old_incidents = m_incidents ;
  //
}
// ============================================================================
// update handler for property "What"
// ============================================================================
void Gaudi::IncidentFilter::handler_1 ( Property&  /* p */ ) 
{
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  if ( m_old_incidents == m_incidents                ) { return ; }
  //
  // unsibscribe old incidents 
  IIncidentSvc* isvc = svc<IIncidentSvc>( "IncidentSvc" ) ;
  //
  const LIST& tmp = m_old_incidents ;
  for ( LIST::const_iterator item = tmp.begin() ; tmp.end() != item ; ++item )
  { isvc->removeListener ( this , *item ) ; } 
  //
  // subscribe new incidents 
  subscribe() ;
}
// ============================================================================
// handle the incident
// ============================================================================
void Gaudi::IncidentFilter::handle ( const Incident& inc ) 
{
  //
  // reset at the begin-event
  if ( IncidentType::BeginEvent == inc.type() ) { m_decision = m_veto ; }
  //
  // look in the list of incidents:
  if ( m_incidents.end() != std::find ( m_incidents.begin () , 
                                        m_incidents.end   () , 
                                        inc.type          () ) ) 
  { m_decision = !m_veto ; }
  //    
}
// ============================================================================
// execute
// ============================================================================
StatusCode Gaudi::IncidentFilter::execute ()
{
  //
  setFilterPassed ( m_decision ) ; 
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================



// ============================================================================
// Veto
// ============================================================================
// standard constructor 
// ============================================================================
Gaudi::IncidentVeto::IncidentVeto
( const std::string& name ,
  ISvcLocator*       pSvc )                        // standard constructor 
  : Gaudi::IncidentFilter ( name , pSvc ) 
{
  setVeto ( true ) ;
}
// ============================================================================
// virtual destructor 
// ============================================================================
Gaudi::IncidentVeto::~IncidentVeto(){}
// ============================================================================


// ============================================================================
// Factories 
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Gaudi,IncidentFilter)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Gaudi,IncidentVeto)

// ============================================================================
//                                                                      The END 
// ============================================================================
