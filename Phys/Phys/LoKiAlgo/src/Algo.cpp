// $Id: Algo.cpp,v 1.10 2006-12-01 09:24:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.10 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2006/12/01 08:51:31  ibelyaev
//  fix a bug
//
// Revision 1.8  2006/12/01 08:35:04  ibelyaev
//  fix for LHCb::RecVertex::isPrimary: to be removed later
//
// Revision 1.7  2006/08/16 17:15:16  ibelyaev
//  update for fixes in DVAlgorithm
//
// ============================================================================
// Include files 
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Print.h"
#include "LoKi/Tokens.h"
#include "LoKi/IReporter.h"
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/AlgoTypes.h"
#include "LoKi/Loop.h"
#include "LoKi/LoopObj.h"
#include "LoKi/Algo.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-14 
 */
// ============================================================================
namespace
{
  /// trivial function to extarct the formula form range list
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
  } ;
};
 

// ============================================================================
/** standard constructor 
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::Algo::Algo 
( const std::string& name , 
  ISvcLocator*       pSvc ) 
  : DVAlgorithm ( name , pSvc )
  // local storage of selected particles 
  , m_selected  ( ) 
  // local storage of selected paricles 
  , m_vselected ( ) 
  // collection of error reporters 
  , m_reporters ( )
  // the list of cut values 
  , m_cutValues ( ) 
{
  //
  declareProperty ( "Cuts"            , m_cutValues ) ;
  // 
  setProperty     ( "PropertiesPrint" , "true" ) ;
} ;
// ============================================================================
/// virtual and protected destructor 
// ============================================================================
LoKi::Algo::~Algo(){} ;
// ============================================================================

// ============================================================================
/** 'Select' the particles to be used in local storage
 *  
 *  - The Particles are selected from the desktop
 *
 *  @code
 *
 *  Range kaons = select( "Kaons" , ABSID == "K+" && P > 5 * GeV );
 *
 *  @endcode
 *
 *  - The example illustrate the 'selection'/'filtering from
 *  desktop the particles, which are @c K+ or @c K- and have a
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
  // get all particles from desktop
  const LHCb::Particle::ConstVector& particles = 
    desktop()->particles();
  // add the particles to the local storage 
  return select ( name , particles.begin() , particles.end() , cut ) ;
} ;
// ============================================================================
/** 'Select' the vertices to be used in local storage
 *  - Vertices are selected from desktop
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
  // get all PRIMARY particles from desktop
  const LHCb::RecVertex::ConstVector prims = desktop()->primaryVertices();
  //
  { /// temporary fix 
    int changed = 0 ;
    /// @todo remove this fix with NEW data
    for ( LHCb::RecVertex::ConstVector::const_iterator ipv = 
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
      Warning("LHCb::RecVertex::teqchue reset to Primary") ;
      debug() << " Number of modified vertices " <<
              << changed << "/" << prims.size() << endreq ;
    }
  }
  //
  vselect ( name , prims.begin() , prims.end() , cut ) ;
  // get all SECONDARY  particles from desktop
  const LHCb::Vertex::ConstVector&     secs  = desktop()->secondaryVertices();
  // 
  return vselect ( name , secs.begin() , secs.end() , cut ) ;
} ;
// ============================================================================
/// get the proper error reporter 
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
} ;
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const std::string&       formula  , 
  const std::string&       pid      , 
  const IParticleCombiner* combiner )
{
  const ParticleProperty* _pp = ppSvc()->find( pid ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + formula + "): invaid particle name '"+pid+"'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
};
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const std::string&       formula  , 
  const LHCb::ParticleID&  pid      , 
  const IParticleCombiner* combiner )
{
  const ParticleProperty* _pp = ppSvc()->findByStdHepID( pid.pid() ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + formula + "): invaid particle ID '" + 
            LoKi::Print::print( pid.pid() ) + "'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
};
// ============================================================================  
LoKi::Loop LoKi::Algo::loop
( const std::string&       formula  , 
  const ParticleProperty*  pid      , 
  const IParticleCombiner* combiner ) 
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
} ;
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const LoKi::Types::RangeList& formula  , 
  const std::string&            pid      , 
  const IParticleCombiner*      combiner )
{
  const ParticleProperty* _pp = ppSvc()->find( pid ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + _formu ( formula ) + "): invaid particle name '"+pid+"'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
};
// ============================================================================
LoKi::Loop LoKi::Algo::loop 
( const LoKi::Types::RangeList& formula  , 
  const LHCb::ParticleID&       pid      , 
  const IParticleCombiner*      combiner )
{
  const ParticleProperty* _pp = ppSvc()->findByStdHepID( pid.pid() ) ;
  if ( 0 == _pp ) 
  { Error ( "loop(" + _formu ( formula ) + "): invaid particle ID '" + 
            LoKi::Print::print( pid.pid() ) + "'!" ) ; }
  return loop ( formula , _pp , combiner )  ;
};
// ============================================================================  
LoKi::Loop LoKi::Algo::loop
( const LoKi::Types::RangeList& formula  , 
  const ParticleProperty*       pid      , 
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
} ;
// ============================================================================
/** save the particle into LoKi storage
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
  
  const LHCb::Particle* saved = desktop()->save( particle );
  
  if( 0 == saved ) { return Error("Particle '"+tag+"' is not saved! " ) ; }
  
  m_selected.add ( tag , saved ) ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================
LoKi::Types::Range LoKi::Algo::pattern 
( const std::string&        tag    , 
  const LoKi::Loop&         loop   ,
  const LoKi::Types::Cuts&  cut    , 
  const LoKi::Types::VCuts& vcut   , 
  const IParticleReFitter*  fitter ) 
{
  loop->backup()  ;
  for ( ; loop ; ++loop ) 
  {
    if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
    if ( !vcut ( loop ) ) { continue ; }
    if ( ! cut ( loop ) ) { continue ; }
    loop->save ( tag ) ;
  }
  loop->restore() ;
  return selected ( tag ) ;
} ;
// ============================================================================
LoKi::Types::Range LoKi::Algo::pattern
( const std::string&        tag    , 
  const LoKi::Loop&         loop   ,
  const LoKi::Types::Cuts&  cut1   , 
  const LoKi::Types::VCuts& vcut1  , 
  const IParticleReFitter*  fitter , 
  const LoKi::Types::Cuts&  cut2   , 
  const LoKi::Types::VCuts& vcut2  ) 
{
  loop->backup()  ;
  for ( ; loop ; ++loop ) 
  {
    if ( !vcut1 ( loop ) ) { continue ; }
    if ( ! cut1 ( loop ) ) { continue ; }
    if ( loop->reFit( fitter ).isFailure() ) { continue ; }
    if ( !vcut2 ( loop ) ) { continue ; }
    if ( ! cut2 ( loop ) ) { continue ; }
    loop->save ( tag ) ;
  }
  loop->restore() ;
  return selected ( tag ) ;

} ;
// ============================================================================
/// clear the internal LoKi storages 
// ============================================================================
StatusCode LoKi::Algo::clear() 
{
  m_selected    .clear () ;
  m_vselected   .clear () ;
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// initialize the algorithm 
// ============================================================================
StatusCode LoKi::Algo::initialize () 
{
  StatusCode sc = DVAlgorithm::initialize() ;
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
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// make the execution of the algorithm 
// ============================================================================
StatusCode LoKi::Algo::execute () 
{
  // reset the filter indicator  
  setFilterPassed ( false );
  // DONE in DVAlgorithm::sysExecute:
  // desktop ()->getEventInput();
  // clear all LoKi storages 
  clear() ;
  // call for actual analysis 
  StatusCode sc = analyse() ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from 'analyse()' method" , sc ) ; }
  /// save everything 
  sc = desktop()->saveDesktop();
  if ( sc.isFailure() ) 
  { return Error ( "DeskTop is not saved"          , sc ) ;}
  // clear all LoKi storages at the end 
  clear() ;
  return StatusCode::SUCCESS ;
};
// ============================================================================
/// perform the real analysis
// ============================================================================
StatusCode LoKi::Algo::analyse() 
{
  return Print ( "The default(empty) LoKi::Algo::analyse() method is invoked" , 
                 StatusCode::SUCCESS , MSG::ALWAYS );
};
// ============================================================================
/// finalize  the algorithm 
// ============================================================================
StatusCode LoKi::Algo::finalize () 
{ 
  clear() ;
  //
  if ( !m_cutValues.empty() && msgLevel ( MSG::DEBUG ) ) 
  {
    debug() << " The specific cuts used: " 
            << Gaudi::Utils::toString ( m_cutValues ) << endreq ;
  }
  //
  return DVAlgorithm::finalize () ;
} ;
// ============================================================================
/// get the helper "geometry" object
// ============================================================================
LoKi::Vertices::ImpParBase 
LoKi::Algo::geo ( const LHCb::VertexBase* vertex ) const 
{
  IGeomDispCalculator* t = geomDispCalculator() ;
  if ( 0 == t ) { Error("geo(): IGeomDispCalculator points to NULL!") ;}
  return LoKi::Vertices::ImpParBase ( vertex , t ) ;
} ;
// ============================================================================
/// get the helper "geometry" object
// ============================================================================
LoKi::Vertices::ImpParBase 
LoKi::Algo::geo ( const LoKi::Point3D& point ) const 
{
  IGeomDispCalculator* t = geomDispCalculator() ;
  if ( 0 == t ) { Error("geo(): IGeomDispCalculator points to NULL!") ;}
  return LoKi::Vertices::ImpParBase ( point , t ) ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


