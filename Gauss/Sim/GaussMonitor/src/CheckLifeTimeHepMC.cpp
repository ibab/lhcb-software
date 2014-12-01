// $Id: CheckLifeTimeHepMC.cpp,v 1.1 2008-06-24 10:07:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/GenExtract.h"
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
  /** @class CheckLifeTimeHepMC
   *  Simple algorithm which checks the lifetime of HepMC particles 
   *
   *  For the description of properties 
   *  see the class GaussMonitor::CheckLifeTime
   *
   *  @code
   *
   *  GenMonitor.Members += [ "GaussMonitor::CheckLifeTimeHepMC/HepMCLifeTime" ];
   *  HepMCLifeTime.Frequency = 100 ; 
   *  HepMCLifeTime.Particles = [ "Lambda_b0" , "B+" ] ;
   *
   *  @endcode 
   * 
   *  @attention: The lifetime of oscillated particles (which 
   *     is zero according to LHCb/EvtGen conventions) has not taken 
   *     into account for counters (they are skept form the counters), but 
   *     it is taken into account into the histograms (one should see 
   *     the peak at zero lifetime for "B0" and "B_s0" particles).
   *     As a resutl the mean lifetime value extracted from the 
   *     historgams is different from the mean lifetiem valeu extracted 
   *     from the counter of the hisgorams for these particles. Moreover 
   *     the number of entries are different also!
   *
   *  @see GaussMonitor::CheckLifeTime
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2008-06-23
   */
  class CheckLifeTimeHepMC : public GaussMonitor::CheckLifeTime
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<GaussMonitor::CheckLifeTimeHepMC> ;
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
    CheckLifeTimeHepMC 
    ( const std::string& name ,   // algorithm instance name 
      ISvcLocator*       pSvc )  // service locator 
      : CheckLifeTime ( name , pSvc ) 
    { 
      setProperty ( "HistoTopDir" , "Gen/" ) ; 
    }
    /// protected and virtual destructor 
    virtual ~CheckLifeTimeHepMC () {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    CheckLifeTimeHepMC () ;                        // no default constructor
    /// Copy constructor is disabled 
    CheckLifeTimeHepMC ( const CheckLifeTimeHepMC& ) ;   // no copy constructor
    /// Assignement operator is disabled 
    CheckLifeTimeHepMC& operator=( const CheckLifeTimeHepMC& ) ; // no =
    // ========================================================================
  } ; 
  // ==========================================================================
} // end of namespace GaussMonitor 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// evaluate the lifetime (c*tau) 
  inline double lifeTime 
  ( const HepMC::GenParticle& p , 
    const HepMC::GenVertex&   o ,
    const HepMC::GenVertex&   e ) 
  {
    return GaussMonitor::lifeTime 
      ( Gaudi::LorentzVector ( p.momentum () ) , 
        Gaudi::XYZPoint      ( o.position () ) , 
        Gaudi::XYZPoint      ( e.position () ) ) ;
  }
  /// check the oscilaltion for the particle 
  bool oscillated ( const HepMC::GenParticle* p )
  {
    if ( 0 == p  ) { return false ; }
    const HepMC::GenVertex* pv = p->production_vertex () ;
    if ( 0 == pv ) { return false ; }
    if ( 1 != pv->particles_in_size() ) { return false ; }
    const HepMC::GenParticle* m = *(pv->particles_in_const_begin()) ;
    if ( 0 == m  ) { return false ; }
    // oscillated 
    return ( p->pdg_id() == -m->pdg_id() ) ;
  }
  // ==========================================================================
}
// ============================================================================
// execute the algorithm 
// ============================================================================
StatusCode GaussMonitor::CheckLifeTimeHepMC::execute () 
{
  //
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;
  //
  const LHCb::HepMCEvent::Container* events = 
    get<LHCb::HepMCEvent::Container> ( LHCb::HepMCEventLocation::Default ) ;
  //
  for ( PIDs::const_iterator ipid = particles().begin() ; 
        particles().end() != ipid ; ++ipid ) 
  {
    // select the particles
    LoKi::GenTypes::GenContainer good ;
    // extract the particles
    GCut cut = ( (*ipid) == GABSID ) && ( 3 != GSTATUS ) ;
    LoKi::Extract::genParticles 
      ( events , std::back_inserter( good ) , cut ) ;             
    //
    // use the counter 
    const std::string   title = GaussMonitor::cntName ( *ipid ) ;
    StatEntity&         cnt   = counter               ( title ) ;
    AIDA::IHistogram1D* hist  = histo1D               ( title ) ;
    //
    for ( LoKi::GenTypes::GenContainer::const_iterator ip = good.begin() ; 
          good.end() != ip ; ++ip) 
    {
      const HepMC::GenParticle* p = *ip ;
      if ( 0 == p ) { continue ; }
      const HepMC::GenVertex* o = p -> production_vertex () ;
      if ( 0 == o ) 
      {
        Error  ( "Origin Vertex is invalid for '" + (*ipid) + "'") ;
        continue ;
      }
      const HepMC::GenVertex* e = p -> end_vertex        () ;
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
      // skip oscillating particles 
      if  ( oscillated ( p ) ) { continue ; }
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
DECLARE_NAMESPACE_ALGORITHM_FACTORY(GaussMonitor,CheckLifeTimeHepMC)
// ============================================================================
// The END 
// ============================================================================
