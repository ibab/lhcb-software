// $Id: CheckLifeTimeMC.cpp,v 1.1 2008-06-24 10:07:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/select.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/MCExtract.h"
#include "LoKi/MCDecayVertex.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
// local
// ============================================================================
#include "LifeTime.h"
#include "CheckLifeTime.h"
// ============================================================================
namespace GaussMonitor 
{
  // ==========================================================================
  /** @class CheckLifeTimeMC
   *  Simple algorithm which checks the lifetime of MC particles 
   *
   *  For the description of properties 
   *  see the class GaussMonitor::CheckLifeTime
   *
   *  @code
   *
   *  SimMonitor.Members += [ "GaussMonitor::CheckLifeTimeMC/MCLifeTime" ];
   *  MCLifeTime.Frequency = 100 ; 
   *  MCLifeTime.Particles = [ "Lambda_b0" , "B+" ] ;
   *
   *  @endcode 
   *
   *  @see GaussMonitor::CheckLifeTime
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2008-06-23
   */
  class CheckLifeTimeMC : public GaussMonitor::CheckLifeTime
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<GaussMonitor::CheckLifeTimeMC> ;
    // ========================================================================
  public:
    // ========================================================================
    virtual StatusCode execute ()  ;
    // ========================================================================
  protected:
    // ========================================================================
    /** The standard constructor 
     *  @param name the algorithm instance name 
     *  @param pSvc service locator
     */
    CheckLifeTimeMC 
    ( const std::string& name ,   // algorithm instance name 
      ISvcLocator*       pSvc )  // service locator 
      : CheckLifeTime ( name , pSvc ) 
    { 
      setProperty ( "HistoTopDir" , "MC/" ) ; 
    }
    /// protected and virtual destructor 
    virtual ~CheckLifeTimeMC () {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    CheckLifeTimeMC () ;                              // no default constructor 
    /// Copy constructor is disabled 
    CheckLifeTimeMC ( const CheckLifeTimeMC& ) ;         // no copy constructor 
    /// Assignement operator is disabled 
    CheckLifeTimeMC& operator=( const CheckLifeTimeMC& ) ;    // no assignement
    // ========================================================================
  } ; 
  // ==========================================================================
} // end of namespace GaussMonitor 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// evaluate the lifetime (in mm) 
  inline double lifeTime 
  ( const LHCb::MCParticle& p , 
    const LHCb::MCVertex&   o ,
    const LHCb::MCVertex&   e ) 
  { 
    return GaussMonitor::lifeTime 
      ( p.momentum() , o.position() , e.position () ) ; }
  // ==========================================================================
  /// get the decay vertex 
  inline const LHCb::MCVertex* decayVertex 
  ( const LHCb::MCParticle* p ) 
  {
    if ( 0 == p ) { return 0 ; }
    typedef SmartRefVector<LHCb::MCVertex> MCVS ;
    const MCVS& evs = p->endVertices() ;
    MCVS::const_iterator ev = 
      std::find_if ( evs.begin() , evs.end() , LoKi::MCVertices::IsDecay ) ;
    if  ( evs.end() == ev ) { return 0 ;}
    return *ev ;
  }
  // ==========================================================================
}
// ============================================================================
// execute the algorithm 
// ============================================================================
StatusCode GaussMonitor::CheckLifeTimeMC::execute () 
{
  //
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;
  //
  const LHCb::MCParticle::Container* mc = 
    get<LHCb::MCParticle::Container> ( LHCb::MCParticleLocation::Default ) ;
  //
  for ( PIDs::const_iterator ipid = particles().begin() ; 
        particles().end() != ipid ; ++ipid ) 
  {
    // select the particles
    LHCb::MCParticle::ConstVector good ;
    // extract the particles
    MCCut cut = ( (*ipid) == MCABSID ) ;
    LoKi::select (  mc->begin() , mc->end() ,
                    std::back_inserter( good ) , cut ) ;             
    //
    // use the counter 
    const std::string   title = GaussMonitor::cntName ( *ipid ) ;
    StatEntity&         cnt   = counter               ( title ) ;
    AIDA::IHistogram1D* hist  = histo1D               ( title ) ;
    //
    for ( LHCb::MCParticle::ConstVector::const_iterator ip = good.begin() ; 
          good.end() != ip ; ++ip) 
    {
      const LHCb::MCParticle* p = *ip ;
      if ( 0 == p ) { continue ; }
      const LHCb::MCVertex*   o = p -> originVertex () ;
      if ( 0 == o ) 
      {
        Error  ( "Origin Vertex is invalid for '" + (*ipid) + "'") ;
        continue ;
      }
      const LHCb::MCVertex* e = decayVertex ( p ) ;
      if ( 0 == e ) 
      {
        Error  ( "Decay Vertex is invalid for '" + (*ipid) + "'") ;
        continue ;
      }
      // evaluate  the lifetime 
      const double ltime = ::lifeTime ( *p , *o , *e ) ;
      // use the histogram 
      if  ( 0 != hist ) { hist->fill ( ltime ) ; }
      // use the counter:
      cnt += ltime ;
    } 
  }
  //
  if ( checkEvent() ) { check ( false ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory:
DECLARE_NAMESPACE_ALGORITHM_FACTORY(GaussMonitor,CheckLifeTimeMC)
// ============================================================================
// The END 
// ============================================================================
