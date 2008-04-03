// $Id: LoopObj.cpp,v 1.12 2008-04-03 09:59:23 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ParticleProperties.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/LoopObj.h"
#include "LoKi/LoopObj.icpp"
#include "LoKi/Algo.h"
#include "LoKi/CompareParticles.h"
// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::LoopObj
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
 *  @date 2006-03-19
 */
// ============================================================================
//  default constructor 
// ============================================================================
LoKi::LoopObj::LoopObj
( const std::string&       name     , 
  const LoKi::IReporter*   reporter , 
  const LoKi::Algo*        algo     , 
  const IParticleCombiner* combiner , 
  const IParticleReFitter* fitter   )
  : LoKi::Base ( name , reporter  )
  , m_algo     ( algo      )
  , m_comb     ( combiner  )
  , m_reFit    ( fitter    )
  //
  , m_formula       (         ) 
  , m_combiner      (         )
  , m_combination   (         )
  ///
  , m_valid         ( false   ) 
  , m_status        ( StatusCode::SUCCESS , true ) 
  ///
  , m_particle      ( 0       )
  , m_vertex        ( 0       )
  , m_pOwner        ( false   ) 
  , m_vOwner        ( false   ) 
  ///
  , m_pid           (         ) 
  , m_pp            ( 0       )
  , m_pidname       (         ) 
  //
  , m_pv            ( 0       )
  //
  , m_clones        (         )
{} 
// ============================================================================
//  destructor
// ============================================================================
LoKi::LoopObj::~LoopObj()
{
  // check status code 
  m_status.ignore() ;
  // delete particle 
  if ( m_pOwner && 0 != m_particle ) { delete m_particle ; m_particle = 0 ; }
  // delete vertex
  if ( m_vOwner && 0 != m_vertex   ) { delete m_vertex   ; m_vertex   = 0 ; }
} 
// ============================================================================
//  advance to the next VALID combination 
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::next      ()
{
  // advance the combiner at least once 
  ++m_combiner ;
  // combiner is over ? 
  if (  m_combiner.end    () ) 
  { resetCache() ; m_valid = false ; return *this ; }
  // combiner is still valid? 
  if ( !m_combiner.valid  () ) { return next () ; }
  // unique heap combination ? 
  if ( !m_combiner.unique () ) { return next () ; }
  // check for clones 
  if ( !clonesOrdered     () ) { return next () ; }
  // local check 
  if ( !isValid()            ) { return next () ; }
  //
  // clear all temporaries 
  resetCache();
  // set validity flag
  m_valid        = true ;
  m_combination  = LHCb::Particle::ConstVector( current().size() ) ;
  // fill the combination from the combiner 
  m_combiner.current ( m_combination.begin() ) ;
  // return 
  return *this ;
}
// ============================================================================
//  backup the current state of the loop 
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::backup  () { m_combiner.backup(); return *this ; }
// ============================================================================
//  restore the loop from the last saved/backup state
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::restore () 
{
  // restore combiner 
  m_combiner.restore();
  // reset all caches 
  resetCache();
  // check the local validity
  m_valid = isValid();
  //
  return *this ;
}
// ============================================================================
//  estimate the validity of current combination
// ============================================================================
bool LoKi::LoopObj::isValid() 
{    
  m_valid = false ;
  // valid heap ?
  if( !m_combiner.valid  () ) { return valid () ; }
  if( !m_combiner.unique () ) { return valid () ; }
  if( !clonesOrdered     () ) { return valid () ; }
  
  //                                                             FIX ME ASAP! 
  // check for overlap
  //   const bool ok = 
  //     LoKi::Unique::nonOverlapped( auxdau.begin() , auxdau.end() ) ;
  //   if( !ok )                   { return valid() ; }           // RETURN 
  
  //
  m_valid = true ;
  //
  return valid() ;
}
// ============================================================================
//  reset all temporary caches 
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::resetCache() 
{
  //  delete the particle 
  if ( 0 != m_particle && m_pOwner ) { delete m_particle ; }
  m_particle = 0 ;
  //  delete the vertex 
  if ( 0 != m_vertex   && m_vOwner ) { delete m_vertex   ; }
  m_vertex   = 0 ;
  // clear primary vertex 
  m_pv       = 0 ;
  // clear the combination 
  m_combination.clear() ;
  //
  m_status = StatusCode ( StatusCode::SUCCESS , true ) ;
  ///
  return *this ;
}
// ============================================================================
//  find clone particles..
// ============================================================================
void LoKi::LoopObj::findClones() 
{
  // clear existing clones 
  m_clones.clear();
  // find clone particles.. 
  for ( Formula::const_iterator item = formula().begin() ; 
        formula().end() != item ; ++item ) 
  { 
    m_clones[*item].push_back( item-formula().begin() );
  };
  Clones::iterator iclone = m_clones.begin() ;
  // hmmm. I would like to know how it works....  
  while ( m_clones.end() != iclone ) 
  {
    if ( 2 > iclone->second.size() ) 
    { 
      m_clones.erase( iclone ) ;
      iclone = m_clones.begin() ;
      continue ;
    }
    else { ++iclone ; }
  }
}
// ============================================================================
//  check the correct order of clones  to avoid the multiple count
// ============================================================================
bool LoKi::LoopObj::clonesOrdered () const 
{
  // HOW IT WORKS ??? I am not able to understand  these lines :-((( 
  
  // the comparison criteria: pid+kinematics
  const LoKi::Particles::FullCompare compare = LoKi::Particles::FullCompare() ;
  
  // check for clones 
  for ( Clones::const_iterator iclone = m_clones.begin() ; 
        m_clones.end() != iclone ; ++iclone ) 
  { 
    const CloneList& clones = iclone->second ;
    if ( 2 > clones.size() ) 
    { Warning(" isValid(): invalid clone's size") ;            return false ; }
    // check that 'clones' are ordered
    for ( CloneList::const_iterator clone1 = clones.begin() ; 
          clones.end() != clone1 ; ++clone1 ) 
    {
      const size_t index1 = *clone1 ;
      if ( index1 >= dim() ) 
      { Error ( "clonesOrdered(): invalid index    " ) ;       return false ; }
      const LHCb::Particle* p1 = *(current()[index1]) ;
      if ( 0 == p1 )
      { Error ( "clonesOrdered(): invalid particle " ) ;       return false ; }
      //
      for ( CloneList::const_iterator clone2 = clone1 + 1 ; 
            clones.end() != clone2 ; ++clone2 ) 
      {
        const size_t index2 = *clone2 ;
        if ( index2 >= dim() ) 
        { Error ( "clonesOrdered(): invalid index    " ) ;     return false ; }
        const LHCb::Particle* p2 = *(current()[index2]);
        if ( 0 == p2 )
        { Error ( "clonesOrdered(): invalid particle " ) ;     return false ; }
        if ( !compare ( p1 ,p2 ) )   {                         return false ; }
      }
    }
  }
  return true ;
}
// ============================================================================
//  make 'effective' particle from the current configuration
// ============================================================================
StatusCode LoKi::LoopObj::make ( const IParticleCombiner* comb ) const 
{
  // delete
  if ( m_pOwner && 0 != m_particle ) { delete m_particle ; }
  m_particle = 0 ;
  if ( m_vOwner && 0 != m_vertex   ) { delete m_vertex   ; }
  m_vertex   = 0 ; 
  // valid combination ?
  if ( !valid() ) 
  {
    m_status = StatusCode ( StatusCode::FAILURE , true )  ;
    return Error("make(): invalid combination" , m_status ); 
  }
  // the most trivial case 
  if ( 1 == dim() ) 
  {
    m_particle = const_cast<LHCb::Particle*>( particle(1) ) ;
    m_pOwner   = false ;
    if ( 0 == m_particle ) 
    { 
      m_status = StatusCode ( StatusCode::FAILURE , true ) ;
      return Error ( "make(): the first particle is invalid " ) ;
    }
    m_vertex   = m_particle->endVertex() ;
    m_vOwner   = false ;
    if ( 0 == m_vertex ) 
    { Warning ( "make(): the vertex for the first particle is NULL" ) ; }
    //
    return StatusCode::SUCCESS ;
  }
  // do we have some tools? 
  LoKi::Interface<IParticleCombiner> combiner ( comb ) ;   // as argument 
  if ( !combiner.validPointer() ) { combiner = m_comb ; }  // form constructor 
  if ( !combiner.validPointer() && m_algo.validPointer() ) // from algorithm
  { combiner = m_algo->particleCombiner() ; }
  if ( !combiner.validPointer() ) 
  { 
    m_status = StatusCode ( StatusCode::FAILURE , true );
    return Error ( "make: no valid IParticlefCombiner tool is available " );
  } 
  // create new particle 
  m_particle = new LHCb::Particle ()   ;
  m_pOwner   = true ;
  m_vertex   = new LHCb::Vertex   ()   ;
  m_vOwner   = true ;
  // pre-fill the particle
  {
    Assert ( 0 != m_pp , "ParticleProperty* points to NULL!" );
    m_particle -> setParticleID    ( m_pid         ) ;
    m_particle -> setMeasuredMass  ( m_pp->mass () ) ;
  }
  // use the tool!
  m_status   = combiner -> combine
    ( m_combination , 
      *m_particle   ,
      *m_vertex     ) ;
  if ( m_status.isFailure() )
  {
    // delete the particle 
    delete m_particle ; m_particle = 0 ;  // ATTENTION!
    // delete the particle  
    delete m_vertex   ; m_vertex   = 0 ;  // ATTENTION!
    //
    return Error ( "Error from IParticleCombiner" , m_status ) ;
  }
  return StatusCode::SUCCESS  ;
}
// ============================================================================
//  make 'effective' particle from the current configuration
// ============================================================================
StatusCode LoKi::LoopObj::make ( const std::string& nick ) const 
{
  // delete
  if ( m_pOwner && 0 != m_particle ) { delete m_particle ; }
  m_particle = 0 ;
  if ( m_vOwner && 0 != m_vertex   ) { delete m_vertex   ; }
  m_vertex   = 0 ; 
  //
  if ( m_algo.validPointer() ) 
  {
    IParticleCombiner* combiner = m_algo->particleCombiner( nick ) ;
    return make ( combiner ) ;
  }
  m_status = StatusCode ( StatusCode::FAILURE , true ) ;
  return Error ( "make('" + nick + "'): no valid IParticleCombiner is available " );
} 
// ============================================================================
//  set the particle ID for the effective particle of the loop 
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::setPID ( const LHCb::ParticleID& pid ) 
{
  m_pid     = pid ;
  m_pp      = LoKi::Particles::ppFromPID( pid ) ;
  Assert ( 0 != m_pp , "Invalid Particle ID is set!" ) ;
  m_pidname = m_pp->particle() ;
  return adaptToPID() ;
}
// ============================================================================
//  set the particle ID for the effectiev particle of the loop 
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::setPID ( const std::string&      pid ) 
{
  m_pidname = pid ;
  m_pp = LoKi::Particles::ppFromName ( pid ) ;
  Assert ( 0 != m_pp , "Invalid Particle name is set!" );
  m_pid = LHCb::ParticleID ( m_pp->jetsetID() ) ;
  return  adaptToPID() ;
}
// ============================================================================
//  set the particle ID for the effectiev particle of the loop 
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::setPID ( const ParticleProperty* pid ) 
{
  m_pp = pid ;
  Assert ( 0 != m_pp , "Invalid Particle ID is set!" ) ;
  m_pid = LHCb::ParticleID( m_pp->jetsetID() ) ;
  m_pidname = m_pp->particle() ;
  return adaptToPID() ;
} 
// ============================================================================
//  make an adaptation for new PID
// ============================================================================
LoKi::LoopObj&  LoKi::LoopObj::adaptToPID() 
{
  // nothing to do 
  if ( !valid() || 0 == m_particle   ) {                       return *this ; }
  // ID is correct
  else if ( m_pid == m_particle->particleID() ) {              return *this ; }
  // particle <--> antiparticle
  else if ( int(m_pid.abspid()) == m_particle->particleID().pid() )
  { m_particle->setParticleID( m_pid ) ;                       return *this ; } 
  //  the particle need to be remade
  StatusCode sc = make() ;
  if ( sc.isFailure() ) { Error("adaptToPID, error from make" , sc ) ; }
  return *this ;
}
// ============================================================================
//  refit the particle using IParticleReFitter tool
// ============================================================================
StatusCode LoKi::LoopObj::reFit 
( const IParticleReFitter* fit ) const 
{
  if ( !valid() ) { return Error( "reFit(): ivalid combination!" ) ; }
  if ( 0 == m_particle || 0 == m_vertex )  
  { 
    m_status = make() ; 
    if ( m_status.isFailure()  ) 
    { return Error("reFit(): error    from make()", m_status ) ;}
    if ( 0 == m_particle || 0 == m_vertex ) 
    { 
      m_status = StatusCode ( StatusCode::FAILURE, true )  ; 
      return Error("reFit(): invalid  from make()" , m_status ) ;
    }
  }
  //
  LoKi::Interface<IParticleReFitter> fitter ( fit ) ;
  if ( !fitter.validPointer() ) { fitter = m_reFit ; }
  if ( !fitter.validPointer() && m_algo.validPointer() )
  { fitter = m_algo->particleReFitter() ; }
  if ( !fitter.validPointer() ) 
  { 
    m_status = StatusCode ( StatusCode::FAILURE, true )  ; 
    return Error ( "reFit: no valid IParticleReFitter tool is available " );
  }
  //
  if ( 0 == m_particle->endVertex() ) 
  { m_particle->setEndVertex( m_vertex ) ; }
  //
  StatusCode sc = fitter->reFit( *m_particle ) ;
  if ( sc.isFailure() ) 
  {
    // delete particle and vertex ???
    return Error ( "reFit(): error from IParticleReFitter", sc );
  }
  return StatusCode::SUCCESS ;
} 
// ============================================================================
//  refit the particle using IParticleReFitter tool
// ============================================================================
StatusCode LoKi::LoopObj::reFit 
( const std::string& nick ) const 
{
  if ( m_algo.validPointer() ) 
  {
    IParticleReFitter* fitter = m_algo->particleReFitter( nick ) ;
    return reFit( fitter  ) ;
  }
  m_status = StatusCode ( StatusCode::FAILURE , true ) ;
  return Error("reFit('" + nick + "'): no valid IParticleReFitter is available " );
}
// ============================================================================
//  add the component to the Loop obejcts
// ============================================================================
LoKi::LoopObj& LoKi::LoopObj::addComponent 
( const std::string&                    name  , 
  const LoKi::LoopObj::Combiner::Range& range ) 
{
  m_formula  .push_back ( name  ) ;
  m_combiner .add       ( range ) ;
  // invalidate the state
  m_valid = false ;
  //
  resetCache() ;
  //
  findClones() ;
  //
  // find valid combination 
  while ( m_combiner.valid() && !isValid() ) { next(); }
  //
  m_valid = isValid() ;
  if ( valid() ) 
  {
    m_combination  = LHCb::Particle::ConstVector( current().size() ) ;
    m_combiner.current ( m_combination.begin() ) ;
  }
  // backup the state of combiner 
  m_combiner.backup();
  m_status = StatusCode ( StatusCode::SUCCESS , true ) ;
  return *this ;
}
// ============================================================================
//  save the particle into LoKi storage
// ============================================================================
StatusCode LoKi::LoopObj::save ( const std::string& tag ) const 
{
  LHCb::Particle* p = particle();
  if ( 0 == p               )
  { return Error ( "save('"+tag+"'): Particle* points to NULL") ; }
  if ( status().isFailure() ) 
  { return Error ( "save('"+tag+"'): Invalid status ", status() ) ; }
  // save !
  m_status = m_algo->save( tag , p );
  if ( m_status.isFailure () ) 
  { return Error ( "save('"+tag+"'): Particle is not saved " , status() ) ; }
  // few trivial and obvious checks 
  LoKi::Types::Range ps  = m_algo->selected( tag );
  if ( ps.empty     () ) 
  { return Error ( "save('"+tag+"'): Saved Particle is not found!"  ) ; }
  LHCb::Particle* saved = const_cast<LHCb::Particle*>( ps.back() ) ;
  if ( 0 == saved      ) 
  { return Error ( "save('"+tag+"'): Saved Particle is invalid"     ) ; }
  // replace the existing particle and vertex 
  if ( p != saved )  
  {
    if ( m_pOwner && 0 != m_particle ) { delete m_particle ; m_particle = 0 ; }
    if ( m_vOwner && 0 != m_vertex   ) { delete m_vertex   ; m_vertex   = 0 ; }
    m_particle = saved ;
    m_vertex   = m_particle -> endVertex() ; // attention! 
    m_pOwner   = false ;
    m_vOwner   = false ;
  }
  //
  if ( 0 == m_particle || 0 == m_vertex ) 
  {
    m_status = StatusCode ( StatusCode::FAILURE , true ) ;
    if ( 0 == m_particle ) 
    { Error ( "save('"+tag+"'): Particle* is NULL", status () ) ; }
    if ( 0 == m_vertex   ) 
    { Error ( "save('"+tag+"'): Vertex*   is NULL", status ()  ) ; }
  };
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
