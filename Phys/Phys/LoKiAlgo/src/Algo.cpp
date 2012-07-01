// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IToolSvc.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Tokens.h"
#include "LoKi/IReporter.h"
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/Decay.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/AlgoTypes.h"
#include "LoKi/Loop.h"
#include "LoKi/LoopObj.h"
#include "LoKi/Algo.h"
#include "LoKi/Objects.h"
// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Algo
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-14 
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// trivial function to extract the formula from range list
  inline std::string _formu ( const LoKi::Types::RangeList& ranges )
  {
    std::string result = "";
    for ( LoKi::Types::RangeList::const_iterator ir = ranges.begin() ; 
          ranges.end() != ir ; ++ir ) 
    {
      if ( ranges.begin() != ir ) { result += " " ; }
      result += ir->name() ;
    }
    return result ;
  } 
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
LoKi::Algo::Lock::Lock ( LoKi::Algo* algo )
{
  // keep the old current algorithm:
  m_old = LoKi::Algo::currentAlgo() ;
  // set a new current algorithm 
  LoKi::Algo::setCurrentAlgo( algo ) ;
} 
// ============================================================================
LoKi::Algo::Lock::~Lock() 
{
  // restore the old current algorithm 
  LoKi::Algo::setCurrentAlgo( m_old ) ;
} 
// =============================================================================
// initialize the static pointer 
// =============================================================================
LoKi::Algo* LoKi::Algo::s_currentAlgo = 0 ;
// =============================================================================
LoKi::Algo* LoKi::Algo::currentAlgo()  { return s_currentAlgo ; }
// =============================================================================
LoKi::Algo* LoKi::Algo::setCurrentAlgo ( LoKi::Algo* value ) 
{
  s_currentAlgo = value ;
  return s_currentAlgo ;
} 
// ============================================================================
/*  standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::Algo::Algo 
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : DaVinciTupleAlgorithm ( name , pSvc )
// local storage of selected particles 
  , m_selected  ( ) 
// local storage of selected paricles 
  , m_vselected ( ) 
// collection of error reporters 
  , m_reporters ( )
// the list of cut values 
  , m_cutValues ( ) 
//
  , m_decay     ( ) 
{
  //
  declareProperty 
    ( "Cuts"  , m_cutValues , 
      "The map of 'named-cuts': { 'cut' : value } " ) ;
  // 
  setProperty ( "HistoPrint"      , true ) . ignore () ;
  setProperty ( "NTuplePrint"     , true ) . ignore () ;
  setProperty ( "PropertiesPrint" , true ) . ignore () ;
} 
// ============================================================================
// virtual and protected destructor 
// ============================================================================
LoKi::Algo::~Algo(){} 
// ============================================================================
/* 'Select' the particles to be used in local storage
 *  
 *  - The Particles are selected from the local storage
 *
 *  @code
 *
 *  Range kaons = select( "Kaons" , ABSID == "K+" && P > 5 * GeV );
 *
 *  @endcode
 *
 *  - The example illustrate the 'selection'/'filtering from
 *  local storage the particles, which are @c K+ or @c K- and have a
 *  momentum in excess of 5GeV/c
 *  - The selected particles
 *  are stored inside local LoKi storage under the tag @c "Kaons"
 *  and returned as a sequence of particles @c kaons .
 *
 *  @see LoKi::Types::Range 
 *  @see LoKi::Types::Cuts 
 *  @see LoKi::Cuts::ABSID 
 *  @see LoKi::Cuts::P
 *  @param name name/tag assigned to the selected particles
 *  @param cut  cut to be applied
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select    
( const std::string&        name ,
  const LoKi::Types::Cuts&  cut  ) 
{
  // get all particles
  LHCb::Particle::Range particles = this->particles();
  // add the particles to the local storage 
  return select ( name , particles.begin() , particles.end() , cut ) ;
} 
// ============================================================================
/* 'Select' the vertices to be used in local storage
 *  - Vertices are selected from DaVinciTupleAlgorithm local storage.
 *  @param name name/tag assigned to the selected vertices
 *  @param cut  cut to be applied
 *  @return selected range of vertices
 */
// ============================================================================
LoKi::Types::VRange         
LoKi::Algo::vselect   
( const std::string&         name  ,
  const LoKi::Types::VCuts&  cut   ) 
{
  // get all PRIMARY particles from IDValgorithm
  const LHCb::RecVertex::Range prims = this->primaryVertices();
  //
  if ( prims.empty() ) { Warning("No Primary vertices").ignore() ; }
  else 
  { /// temporary fix 
    int changed = 0 ;
    /// @todo remove this fix with NEW data
    for ( LHCb::RecVertex::Range::const_iterator ipv = 
            prims.begin() ; prims.end() != ipv ; ++ipv ) 
    {
      const LHCb::RecVertex* _rv = *ipv ;
      if ( 0 == _rv         ) { continue ; }
      if ( _rv->isPrimary() ) { continue ; }
      if ( LHCb::RecVertex::Unknown == _rv->technique() ) 
      {
        // ATTENTION!!!
        LHCb::RecVertex* rv = const_cast<LHCb::RecVertex*>( _rv ) ;
        rv->setTechnique( LHCb::RecVertex::Primary ) ;
        ++changed ;
      }
    }
    if ( 0 < changed ) 
    {
      Warning("LHCb::RecVertex::technique reset to Primary") ;
      debug() << " Number of modified vertices " 
              << changed << "/" << prims.size() << endreq ;
    }
    //
    vselect ( name , prims.begin() , prims.end() , cut ) ;
  }
  //
  Warning ( "vselect: The extraction of secondary vertices is disabled" , 
            StatusCode::SUCCESS , 1 ).ignore() ;
  
  return vselected ( name ) ;
} 
// ============================================================================
// get the proper error reporter 
// ============================================================================
const LoKi::IReporter* 
LoKi::Algo::reporter( const std::string& name ) const 
{
  const LoKi::IReporter* rep = m_reporters [ name ];
  if ( 0 == rep ) 
  { 
    rep = tool<LoKi::IReporter>( "LoKi::Reporter" , name , this ) ;
    m_reporters[ name ] = rep ;
  }
  if ( 0 == rep ) 
  { Warning ( "LoKi::IReporter*('"+name+"') points to NULL" ) ; }
  // 
  return rep ;
} 
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const std::string&       formula  , 
  const std::string&       pid      , 
  const IParticleCombiner* combiner )
{
  const LHCb::ParticleProperty* _pp = ppSvc()->find( pid ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + formula + "): invaid particle name '"+pid+"'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
}
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const std::string&       formula  , 
  const LHCb::ParticleID&  pid      , 
  const IParticleCombiner* combiner )
{
  const LHCb::ParticleProperty* _pp = ppSvc()->find ( pid ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + formula + "): invaid particle ID '" + 
            Gaudi::Utils::toString( pid.pid() ) + "'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
}
// ============================================================================  
LoKi::Loop LoKi::Algo::loop
( const std::string&             formula  , 
  const LHCb::ParticleProperty*  pid      , 
  const IParticleCombiner*       combiner ) 
{
  // tokenize the formula 
  LoKi::Tokens tokens;
  LoKi::tokenize( formula , tokens , " ,;" ) ;
  
  if ( 0 == pid ) 
  { Warning ( "loop(" + formula + "): invalid particle ID !" , 
              StatusCode::SUCCESS , 1 ) ; }
  const std::string _name = "Loop:" + formula ;
  // get the proper error reporter
  const LoKi::IReporter* rep = reporter( _name ) ;
  // create the loop object
  LoKi::LoopObj* object = new LoKi::LoopObj( _name , rep , this , combiner ) ;
  // configure the loop object
  object->setPID ( pid ) ;
  // feed the data 
  for ( LoKi::Tokens::const_iterator t = tokens.begin() ; tokens.end() != t ; ++t )
  {
    object->addComponent ( *t , m_selected( *t ) ) ;
  }
  // return valid object 
  return LoKi::Loop( object ) ;
} 
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const LoKi::Types::RangeList& formula  , 
  const std::string&            pid      , 
  const IParticleCombiner*      combiner )
{
  const LHCb::ParticleProperty* _pp = ppSvc()->find( pid ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + _formu ( formula ) + "): invaid particle name '"+pid+"'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
}
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const LoKi::Types::RangeList& formula  , 
  const LHCb::ParticleID&       pid      , 
  const IParticleCombiner*      combiner )
{
  const LHCb::ParticleProperty* _pp = ppSvc()->find ( pid ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + _formu ( formula ) + "): invaid particle ID '" + 
            Gaudi::Utils::toString ( pid.pid() ) + "'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
}
// ============================================================================  
LoKi::Loop LoKi::Algo::loop
( const LoKi::Types::RangeList& formula  , 
  const LHCb::ParticleProperty* pid      , 
  const IParticleCombiner*      combiner ) 
{
  if ( 0 == pid ) 
  { Warning ( "loop(" + _formu ( formula ) + "): invalid particle ID !" , 
              StatusCode::SUCCESS , 1 ) ; }
  std::string _name = "Loop:" + _formu ( formula ) ;
  // get the proper error reporter
  const LoKi::IReporter* rep = reporter( _name ) ;
  // create the loop object
  LoKi::LoopObj* object = new LoKi::LoopObj( _name , rep , this , combiner ) ;
  // configure the loop object
  object->setPID ( pid ) ;
  // feed the data 
  for ( LoKi::Types::RangeList::const_iterator r = formula.begin() ; 
        formula.end() != r ; ++r )
  {
    object->addComponent ( r->name() , *r ) ;
  }
  // return valid object 
  return LoKi::Loop( object ) ;
} 
// ============================================================================
/*  Create the loop object from "decay"
 *  @see Decays::Decay
 *  @param decay the decay desctrptor
 *  @param combined the combiner
 *  @return the valid looping-object
 */
LoKi::Loop LoKi::Algo::loop 
( const Decays::Decay&     decay    , 
  const IParticleCombiner* combiner ) 
{
  // verify the decay descriptor
  StatusCode sc = decay.validate ( ppSvc() ) ;
  // get the string:
  const std::string& d = decay.toString() ;
  Assert ( sc.isFailure() , "loop(" + d + "): invalid decay descriptor " ) ;
  // 
  const std::string _nam = "Loop:" + d ;
  // get the proper error reporter 
  const LoKi::IReporter* rep = reporter ( _nam ) ;
  // create the loop object
  LoKi::LoopObj* object = new LoKi::LoopObj( _nam , rep , this , combiner ) ;
  // configure the loop object
  object -> setPID ( decay.mother().pp() ) ;
  // feed the data
  const Decays::Decay::Items& children = decay.children() ;
  for ( Decays::Decay::Items::const_iterator ic = 
          children.begin() ; children.end() != ic ; ++ic ) 
  {
    const std::string& c = ic->name()  ;
    object -> addComponent ( c , m_selected ( c ) ) ;
  }
  // return valid object 
  return LoKi::Loop( object ) ;
}
// ============================================================================
/*  save the particle into LoKi storage
 *  @param  tag particle tag (only for LoKi)
 *  @param  particle particle to be saved 
 *  @return status code  
 */
// ============================================================================
StatusCode LoKi::Algo::save 
( const std::string& tag      , 
  LHCb::Particle*    particle ) 
{
  if( 0 == particle ) 
  { return Error("save('"+tag+"'): invalid particle could not be saved!") ; }
  
  const LHCb::Particle* saved = this->markTree( particle );
  
  if( 0 == saved ) { return Error("Particle '"+tag+"' is not kept! " ) ; }
  
  m_selected.add ( tag , saved ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  shortcut for the following symbolic expression:
 * 
 *  @code 
 * 
 * {
 *  loop->backup()  ;
 *  for ( ; loop ; ++loop ) 
 *   {
 *     // get the mass from the sum of LorentzVectors 
 *     const double mass = loop->mass() ;
 *     // apply a mass window 
 *     if ( low > mass || high > mass ) { continue ; } 
 *     // use the explicit refitter (if needed) 
 *     if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
 *     // apply the vertex cuts 
 *     if ( !vcut ( loop ) ) { continue ; }
 *     // apply other cuts cuts 
 *     if ( ! cut ( loop ) ) { continue ; }
 *     loop->save ( tag ) ;
 *   }
 *  loop->restore() ;
 *  return selected ( tag ) ;
 * } ;
 * 
 *  @endcode 
 *
 *  @param tag  the symbolic unique tag 
 *  @param loop the looping object itself 
 *  @param low  low edge for mass window 
 *  @param high high edge for mass window 
 *  @param cut  cut to be used for filtering 
 *  @param vcut vertex cut to be used for filtering 
 *  @param fitter refitter to be applied before cuts 
 *  @return the selected range of particles 
 */
// ============================================================================
LoKi::Types::Range LoKi::Algo::pattern
( const std::string&        tag    , 
  const LoKi::Loop&         loop   ,
  const double              low    , 
  const double              high   ,
  const LoKi::Types::Cuts&  cut    , 
  const LoKi::Types::VCuts& vcut   , 
  const IParticleReFitter*  fitter ) 
{
  if ( !loop.valid() ) { return selected( tag ) ; }
  // back-up! 
  loop->backup  () ; 
  for ( ; loop ; ++loop ) 
  {
    // get the mass from the sum of LorentzVectors 
    const double mass = loop->mass() ;
    // apply a mass window 
    if ( low > mass || high > mass ) { continue ; } 
    // use the explicit refitter (if needed) 
    if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
    // apply the vertex cuts 
    if ( !vcut ( loop ) ) { continue ; }
    // apply other cuts cuts 
    if ( ! cut ( loop ) ) { continue ; }
    loop->save ( tag ) ;   
  }
  // restore the loop 
  loop->restore () ; 
  // return selected/saved candidates
  return selected ( tag ) ;
}     
// ============================================================================
/*  shortcut for the following symbolic expression:
 * 
 *  @code 
 * 
 * {
 *  loop->backup()  ;
 *  for ( ; loop ; ++loop ) 
 *   {
 *     // get the mass from the sum of LorentzVectors 
 *     const double mass = loop->mass() ;
 *     // apply a mass window 
 *     if (  abs( mass - nominal ) > window ) { continue ; } 
 *     // use the explicit refitter (if needed) 
 *     if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
 *     // apply the vertex cuts 
 *     if ( !vcut ( loop ) ) { continue ; }
 *     // apply other cuts cuts 
 *     if ( ! cut ( loop ) ) { continue ; }
 *     loop->save ( tag ) ;
 *   }
 *  loop->restore() ;
 *  return selected ( tag ) ;
 * } ;
 * 
 *  @endcode 
 *
 *  @param tag  the symbolic unique tag 
 *  @param loop the looping object itself 
 *  @param window the width of the mass window 
 *  @param cut  cut to be used for filtering 
 *  @param vcut vertex cut to be used for filtering 
 *  @param fitter refitter to be applied before cuts 
 *  @return the selected range of particles 
 */
// ============================================================================
LoKi::Types::Range LoKi::Algo::pattern
( const std::string&        tag    , 
  const LoKi::Loop&         loop   ,
  const double              window , 
  const LoKi::Types::Cuts&  cut    , 
  const LoKi::Types::VCuts& vcut   , 
  const IParticleReFitter*  fitter ) 
{
  if ( !loop.valid() ) { return selected( tag ) ; }  // RETURN 
  // backup! 
  loop->backup() ; 
  // 
  const LHCb::ParticleProperty* _pp = loop->pp ();
  if ( 0 == _pp ) 
  {
    Error("pattern(..,window,...): LHCb::ParticleProperty* is invalid! select NOTHING!");
    return selected(tag);
  }
  const double nominal = _pp->mass() ;
  // make a loop 
  for ( ; loop ; ++loop ) 
  { 
    // get the mass from the sum of LorentzVectors 
    const double mass = loop->mass() ;
    // apply a mass window 
    if (  ::fabs( mass - nominal ) > window ) { continue ; } 
    // use the explicit refitter (if needed) 
    if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
    // apply the vertex cuts 
    if ( !vcut ( loop ) ) { continue ; }
    // apply other cuts cuts 
    if ( ! cut ( loop ) ) { continue ; }
    loop->save ( tag ) ;
  }
  // restore 
  loop->restore() ; 
  // return the selected/saved combinations 
  return selected ( tag ) ;
} 
// ============================================================================
/*  shortcut for the following expression:
 *
 *  @code 
 *
 * { 
 *  loop->backup()  ;
 *  for ( ; loop ; ++loop ) 
 *   { 
 *     // get the mass from the sum of LorentzVectors 
 *     const double mass = loop->mass() ;
 *     // apply a mass window 
 *     if ( low > mass || high > mass ) { continue ; } 
 *     // apply vertex selection cuts  
 *     if ( !vcut1 ( loop ) ) { continue ; }
 *     // apply selection cuts 
 *     if ( ! cut1 ( loop ) ) { continue ; }
 *     // refit 
 *     if ( loop->reFit( fitter ).isFailure() ) { continue ; }
 *     // apply vertex selection cuts  
 *     if ( !vcut2 ( loop ) ) { continue ; }
 *     // apply selection cuts 
 *     if ( ! cut2 ( loop ) ) { continue ; }
 *     loop->save ( tag ) ;
 *   }
 *  loop->restore() ;
 *  return selected ( tag ) ;
 * }
 *
 *  @endcode 
 *
 *  @param tag   the symbolic unique tag 
 *  @param loop  the looping object itself 
 *  @param low   low edge for mass window 
 *  @param high  high edge for mass window 
 *  @param cut1  cut to be used for filtering before refit
 *  @param vcut1 vertex cut to be used for filtering before refitt
 *  @param fitter refitter to be applied before cuts 
 *  @param cut2  cut to be used for filtering after refit
 *  @param vcut2 vertex cut to be used for filtering after refitt
 *  @return the selected range of particles 
 */
// ============================================================================
LoKi::Types::Range LoKi::Algo::pattern
( const std::string&        tag    , 
  const LoKi::Loop&         loop   ,
  const double              low    , 
  const double              high   , 
  const LoKi::Types::Cuts&  cut1   , 
  const LoKi::Types::VCuts& vcut1  , 
  const IParticleReFitter*  fitter , 
  const LoKi::Types::Cuts&  cut2   , 
  const LoKi::Types::VCuts& vcut2  ) 
{
  if ( !loop.valid()  ) { return selected( tag ) ; } // RETURN 
  // backup!
  loop->backup()  ; 
  // make a loop 
  for ( ; loop ; ++loop ) 
  { 
    // get the mass from the sum of LorentzVectors 
    const double mass = loop->mass() ;
    // apply a mass window 
    if ( low > mass || high > mass ) { continue ; } 
    // apply vertex selection cuts  
    if ( !vcut1 ( loop ) ) { continue ; }
    // apply selection cuts 
    if ( ! cut1 ( loop ) ) { continue ; }
    // refit 
    if ( loop->reFit( fitter ).isFailure() ) { continue ; }
    // apply vertex selection cuts  
    if ( !vcut2 ( loop ) ) { continue ; }
    // apply selection cuts 
    if ( ! cut2 ( loop ) ) { continue ; }
    loop->save ( tag ) ;
  }
  // retore the status 
  loop->restore() ; 
  // return the selected/saved objects 
  return selected ( tag ) ;
} 
// ============================================================================
/*  shortcut for the following expression:
 *
 *  @code 
 *
 * { 
 *  loop->backup()  ;
 *  for ( ; loop ; ++loop ) 
 *   { 
 *     // get the mass from the sum of LorentzVectors 
 *     const double mass = loop->mass() ;
 *     // apply a mass window 
 *     if ( ::fabs( mass - nominal ) > window ) { continue ; } 
 *     // apply vertex selection cuts  
 *     if ( !vcut1 ( loop ) ) { continue ; }
 *     // apply selection cuts 
 *     if ( ! cut1 ( loop ) ) { continue ; }
 *     // refit 
 *     if ( loop->reFit( fitter ).isFailure() ) { continue ; }
 *     // apply vertex selection cuts  
 *     if ( !vcut2 ( loop ) ) { continue ; }
 *     // apply selection cuts 
 *     if ( ! cut2 ( loop ) ) { continue ; }
 *     loop->save ( tag ) ;
 *   }
 *  loop->restore() ;
 *  return selected ( tag ) ;
 * }
 *
 *  @endcode 
 *
 *  @param tag   the symbolic unique tag 
 *  @param loop  the looping object itself 
 *  @param window the window around the nominal mass 
 *  @param high  high edge for mass window 
 *  @param cut1  cut to be used for filtering before refit
 *  @param vcut1 vertex cut to be used for filtering before refitt
 *  @param fitter refitter to be applied before cuts 
 *  @param cut2  cut to be used for filtering after refit
 *  @param vcut2 vertex cut to be used for filtering after refitt
 *  @return the selected range of particles 
 */
// ============================================================================
LoKi::Types::Range LoKi::Algo::pattern
( const std::string&        tag    , 
  const LoKi::Loop&         loop   ,
  const double              window , 
  const LoKi::Types::Cuts&  cut1   , 
  const LoKi::Types::VCuts& vcut1  , 
  const IParticleReFitter*  fitter , 
  const LoKi::Types::Cuts&  cut2   , 
  const LoKi::Types::VCuts& vcut2  ) 
{
  if ( !loop.valid()  ) { return selected( tag ) ; } // RETURN 
  // backup!
  loop->backup()  ; 
  // get the nominal mass 
  const LHCb::ParticleProperty* _pp = loop->pp ();
  if ( 0 == _pp ) 
  {
    Error("pattern(..,window,...): LHCb::ParticleProperty* is invalid! select NOTHING!");
    return selected(tag);
  }
  const double nominal = _pp->mass() ;
  // make a loop 
  for ( ; loop ; ++loop ) 
  { 
    // get the mass from the sum of LorentzVectors 
    const double mass = loop->mass() ;
    // apply a mass window 
    if ( ::fabs( mass - nominal ) > window ) { continue ; }
    // apply vertex selection cuts  
    if ( !vcut1 ( loop ) ) { continue ; }
    // apply selection cuts 
    if ( ! cut1 ( loop ) ) { continue ; }
    // refit 
    if ( loop->reFit( fitter ).isFailure() ) { continue ; }
    // apply vertex selection cuts  
    if ( !vcut2 ( loop ) ) { continue ; }
    // apply selection cuts 
    if ( ! cut2 ( loop ) ) { continue ; }
    loop->save ( tag ) ;
  }
  // retore the status 
  loop->restore() ; 
  // return the selected/saved objects 
  return selected ( tag ) ;
} 
// ============================================================================
// clear the internal LoKi storages 
// ============================================================================
StatusCode LoKi::Algo::clear() 
{
  //
  for ( LoKi::PhysTypes::Selected::map_iterator i = 
          m_selected.begin() ; m_selected.end() != i ; ++i ) 
  { counter ( "#" + i ->first ) = i->second.size() ; }
  m_selected    .clear () ;
  //
  for ( LoKi::PhysTypes::VSelected::map_iterator iv = 
          m_vselected.begin() ; m_vselected.end() != iv ; ++iv ) 
  { counter ( "#" + iv->first ) = iv->second.size() ; }
  m_vselected   .clear () ;
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// initialize the algorithm 
// ============================================================================
StatusCode LoKi::Algo::initialize () 
{
  const StatusCode sc = DaVinciTupleAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  /// locate LoKi service 
  svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
  /// get one global reporter
  if ( 0 == m_reporters[""] ) 
  {
    LoKi::IReporter* rep = tool<LoKi::IReporter>("LoKi::Reporter") ;
    m_reporters[""] = rep ;
  }
  //
  return sc ;
} 
// ============================================================================
// make the execution of the algorithm 
// ============================================================================
StatusCode LoKi::Algo::execute () 
{
  // lock & set the proper environment for "current" algorithm 
  Lock lock ( this ) ;
  // reset the filter indicator  
  setFilterPassed ( false );
  // DONE in DaVinciTupleAlgorithm::sysExecute:
  // desktop ()->getEventInput();
  // clear all LoKi storages 
  clear().ignore() ;
  // call for actual analysis 
  StatusCode sc = analyse() ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from 'analyse()' method" , sc ) ; }

  // clear all LoKi storages at the end 
  clear().ignore () ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
// perform the real analysis
// ============================================================================
StatusCode LoKi::Algo::analyse() 
{
  return Print ( "The default(empty) LoKi::Algo::analyse() method is invoked" , 
                 StatusCode::SUCCESS , MSG::ALWAYS );
}
// ============================================================================
// finalize  the algorithm 
// ============================================================================
StatusCode LoKi::Algo::finalize () 
{ 
  clear().ignore() ;
  //
  if ( !m_cutValues.empty() && msgLevel ( MSG::DEBUG ) ) 
  {
    debug() << " The specific cuts used: " 
            << Gaudi::Utils::toString ( m_cutValues ) << endreq ;
  }
  //
  m_decay.release() ;
  //
  return DaVinciTupleAlgorithm::finalize () ;
}
// ============================================================================
// get the helper "geometry" object
// ============================================================================
LoKi::Vertices::ImpParBase LoKi::Algo::geo 
( const LHCb::VertexBase* vertex , 
  const std::string&      nick   ) const 
{
  const IDistanceCalculator* t = distanceCalculator ( nick ) ;
  if ( 0 == t ) 
  { Error("geo(): IDistanceCalculator('"+nick+"') points to NULL!") ;}
  return LoKi::Vertices::ImpParBase ( vertex , t ) ;
} 
// ============================================================================
// get the helper "geometry" object
// ============================================================================
LoKi::Vertices::ImpParBase LoKi::Algo::geo 
( const LoKi::Point3D& point ,
  const std::string&   nick  ) const 
{
  const IDistanceCalculator* t = distanceCalculator ( nick ) ;
  if ( 0 == t ) 
  { Error("geo(): IDistanceCalculator'"+nick+"' points to NULL!") ;}
  return LoKi::Vertices::ImpParBase ( point , t ) ;
} 
// ============================================================================
/*  'Select' the particles using decay finder 
 * 
 *  @code
 *
 *  const Decays::IDecay::Finder& finder = ... ;
 *  
 *  Range d0 = select( "D0" , finder ) ;
 *
 *  @endcode
 *
 *  @see Decays::IDecay::Finder  
 *  @param name   (INPUT)  name/tag assigned to the selected particles
 *  @param finder (INPUT) the decay finder 
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select    
( const std::string&            name   ,
  const Decays::IDecay::Finder& finder ) 
{
  // get all particles from IDVAlgorihtm 
  LHCb::Particle::Range parts = this->particles () ;
  // add the particles to the local storage 
  return select ( name , parts , finder )  ;
} 
// ============================================================================
/* 'Select' the particles using decay tree
 * 
 *  @code
 *
 *  const Decays::IDecay::iTree& tree = ... ;
 *  
 *  Range d0 = select( "D0" , tree ) ;
 *
 *  @endcode
 *
 *  @see Decays::IDecay::iTree  
 *  @param name   (INPUT)  name/tag assigned to the selected particles
 *  @param tree   (INPUT) the decay tree 
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select 
( const std::string&            tag    ,
  const Decays::IDecay::iTree&  tree   ) 
{
  // get all particles from IDVAlgorihtm 
  LHCb::Particle::Range parts = this->particles () ;
  // add the particles to the local storage 
  return select ( tag , parts , tree  )  ;
}
// ============================================================================
/*  'Select' the particles using decay descriptor 
 * 
 *  @code
 *
 *  Range d0 = select( "D0" , "[ D0 -> K- pi+ ]CC") ;
 *
 *  @endcode
 *
 *  @param name       (INPUT)  name/tag assigned to the selected particles
 *  @param descriptor (INPUT) the decay descriptor 
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select 
( const std::string&            tag        ,
  const std::string&            descriptor ) 
{
  // get all particles from IDVAlgorihtm 
  LHCb::Particle::Range parts = this->particles () ;
  // add the particles to the local storage 
  return select ( tag , parts , descriptor )  ;
}
// ============================================================================
/*  'Sub-select' the particles using decay finder 
 * 
 *  @code
 *
 *  const Decays::IDecay::Finder& finder = ... ;
 *  const Range input = ... ;
 * 
 *  Range d0 = select( "D0" , input , finder ) ;
 *
 *  @endcode
 *
 *  @see Decays::IDecay::Finder  
 *  @param name   (INPUT) name/tag assigned to the selected particles
 *  @param input  (INPUT) the input particles 
 *  @param finder (INPUT) the decay finder 
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select 
( const std::string&            tag    ,
  const LoKi::Types::Range&     range  ,
  const Decays::IDecay::Finder& finder ) 
{
  // check the validity of finder 
  if ( !finder ) 
  {
    StatusCode sc = finder.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Invalid decay finder: '" + finder.tree().toString() + "'") ;
      return LoKi::Types::Range () ;
    }
  }
  //
  LHCb::Particle::ConstVector results ;
  results.reserve  ( 100 ) ;
  finder.findDecay ( range.begin () , range.end   () , results ) ;
  //
  return this->select ( tag              , 
                        results.begin () , 
                        results.end   () , 
                        LoKi::Objects::_ALL_ ) ;
}
// ============================================================================
/*  'Select' the particles using decay tree
 * 
 *  @code
 *
 *  const Decays::IDecay::iTree& tree = ... ;
 *  const Range  input = ... ;
 *
 *  Range d0 = select( "D0" , input , tree ) ;
 *
 *  @endcode
 *
 *  @see Decays::IDecay::iTree  
 *  @param name   (INPUT) name/tag assigned to the selected particles
 *  @param input  (INPUT) the input particles 
 *  @param tree   (INPUT) the decay tree 
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select 
( const std::string&            tag    ,
  const LoKi::Types::Range&     range  ,
  const Decays::IDecay::iTree&  tree   ) 
{
  //
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "select: Unable to validate tree '" + tree.toString() + "'" ) ;
      return LoKi::Types::Range() ;
    }  
  }
  //
  return select ( tag , range , Decays::IDecay::Finder ( tree ) ) ;
}
// ============================================================================
/*  'Select' the particles using decay descriptor 
 * 
 *  @code
 *
 *  const Range  input = ... ;
 *  Range d0 = select( "D0" , input , "[ D0 -> K- pi+ ]CC") ;
 *
 *  @endcode
 *
 *  @param name       (INPUT)  name/tag assigned to the selected particles
 *  @param input      (INPUT) the input particles 
 *  @param descriptor (INPUT) the decay descriptor 
 *  @return selected range of particles
 */
// ============================================================================
LoKi::Types::Range 
LoKi::Algo::select 
( const std::string&            tag        ,
  const LoKi::Types::Range&     range      ,
  const std::string&            descriptor ) 
{
  // 
  if ( !m_decay ) 
  { m_decay = tool<Decays::IDecay>  ( "LoKi::Decay/Decay" ,this ) ; }
  //
  Assert ( !(!m_decay) , "Decays::IDecay* poitns to NULL!" ) ;
  //
  Decays::IDecay::Tree tree = m_decay->tree ( descriptor ) ;
  if ( !tree ) 
  {
    Error ( "select: Unable to create decay tree from descriptor '" + 
            descriptor + "'" ) ;
    return LoKi::Types::Range () ;
  }
  //
  return select ( tag, range , Decays::IDecay::Finder ( tree ) ) ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================


