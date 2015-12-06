// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// local
// ============================================================================
#include "LoKi/HltTool.h"
#include "LoKi/HltCandidates.h"
#include "LoKi/HltStages.h"
#include "LoKi/Constants.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/Hlt1Wrappers.h"
// ============================================================================
/** @file
 *  Implementation of Hlt-Candidates functors
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 */
// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Candidates::TotalStages::~TotalStages () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::TotalStages*
LoKi::Candidates::TotalStages::clone() const
{ return new LoKi::Candidates::TotalStages( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::TotalStages::result_type
LoKi::Candidates::TotalStages::operator()
  ( LoKi::Candidates::TotalStages::argument a ) const
{
  if ( !a )
  {
    Error("Hlt::Candidate* points to NULL, return -1") ;
    return -1 ;
  }
  return a->stages().size();
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::TotalStages::fillStream ( std::ostream& s ) const
{ return s << "TC_TOTSTAGES" ; }


// ============================================================================
// constructor
// ============================================================================
LoKi::Candidates::NStages::NStages
( const  LoKi::TriggerTypes::TS_Cuts& cut )
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Function ()
  , m_cut ( cut )
{}
// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Candidates::NStages::~NStages () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::NStages*
LoKi::Candidates::NStages::clone() const
{ return new LoKi::Candidates::NStages( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::NStages::result_type
LoKi::Candidates::NStages::operator()
  ( LoKi::Candidates::NStages::argument a ) const
{
  if ( !a )
  {
    Error("Hlt::Candidate* points to NULL, return -1") ;
    return -1 ;
  }
  const SmartRefVector<Hlt::Stage>& stages = a->stages() ;
  return std::count_if( stages.begin(), stages.end(), m_cut );
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::NStages::fillStream ( std::ostream& s ) const
{ return s << " TC_NSTAGES(" << m_cut << ") " ; }


// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Candidates::Branch::~Branch (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::Branch*
LoKi::Candidates::Branch::clone() const
{ return new LoKi::Candidates::Branch( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::Branch::result_type
LoKi::Candidates::Branch::operator()
  ( LoKi::Candidates::Branch::argument a ) const
{
    if ( !a ) {
        Error( "Hlt::Candidate* points to NULL, return false" );
        return false;
    }
    //
    const Hlt::Stage* s = a->initiatorStage();
    if ( !s ) {
        Error( "Hlt::Stage* points to NULL, return false" );
        return false;
    }
    //
    return s->is<Hlt::Stage>();
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::Branch::fillStream ( std::ostream& s ) const
{ return s << " TC_BRANCH " ; }

// ============================================================================
// constructor
// ============================================================================
LoKi::Candidates::StageFun::StageFun
( const LoKi::TriggerTypes::TS_Func& cut  ,
  const int                          slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_cut( cut )
  , m_slot( slot )
{}
// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Candidates::StageFun::~StageFun()
{
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::StageFun*
LoKi::Candidates::StageFun::clone() const
{
    return new LoKi::Candidates::StageFun( *this );
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::StageFun::result_type LoKi::Candidates::StageFun::
operator()( LoKi::Candidates::StageFun::argument a ) const
{
    if ( !a ) {
        Error( "Hlt::Candidate* points to NULL, return NegativeInfinity" );
        return LoKi::Constants::NegativeInfinity;
    }
    //
    const Hlt::Stage* stage = a->get<Hlt::Stage>( m_slot );
    if ( !stage ) {
        Error( "Hlt::Stage* points to NULL, return NegativeInfinity" );
        return LoKi::Constants::NegativeInfinity;
    }
    //
    return m_cut( stage );
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::StageFun::fillStream ( std::ostream& s ) const
{
  s << " TC_StFUN(" << m_cut ;
  if ( 0 != m_slot ) { s << "," << m_slot ; }
  return s << ") ";
}




// ============================================================================
// constructor
// ============================================================================
LoKi::Candidates::StageCut::StageCut
( const LoKi::TriggerTypes::TS_Cuts& cut  ,
  const int                          slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( cut )
  , m_slot( slot )
{}
// ============================================================================
// MANDATORY: virtual desctructor
// ============================================================================
LoKi::Candidates::StageCut::~StageCut () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::StageCut*
LoKi::Candidates::StageCut::clone() const
{
    return new LoKi::Candidates::StageCut{ *this };
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::StageCut::result_type LoKi::Candidates::StageCut::
operator()( LoKi::Candidates::StageCut::argument a ) const
{
    if ( !a ) {
        Error( "Hlt::Candidate* points to NULL, return false" );
        return false;
    }
    //
    const Hlt::Stage* stage = a->get<Hlt::Stage>( m_slot );
    if ( !stage ) {
        Error( "Hlt::Stage* points to NULL, return false" );
        return false;
    }
    //
    return m_cut( stage );
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::StageCut::fillStream( std::ostream& s ) const
{
  s << " TC_StCUT(" << m_cut ;
  if ( 0 != m_slot ) { s << "," << m_slot ; }
  return s << ") ";
}



// ============================================================================
namespace
{
  // ==========================================================================
  /// the fake parameter
  const int s_FAKE = -100 ; // the fake parameter
  // ==========================================================================
}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut,
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut,
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Predicate& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut, const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate& cut,
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotCut::SlotCut
( const LoKi::BasicFunctors<const LHCb::Track*>::CutVal& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate()
  , m_cut( LoKi::Stages::cut_( cut, s_FAKE ) )
  , m_slot( slot )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Candidates::SlotCut::~SlotCut() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::SlotCut*
LoKi::Candidates::SlotCut::clone() const
{ return new LoKi::Candidates::SlotCut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::SlotCut::result_type LoKi::Candidates::SlotCut::
operator()( LoKi::Candidates::SlotCut::argument a ) const
{
  //
  if ( !a )
  {
    Error ("Hlt::Candidate* points to NULL, return false") ;
    return false ;                                                    // RETURN
  }
  // get the slot
  const Hlt::Stage* stage = a->get<Hlt::Stage>( m_slot ) ;
  if ( !stage )
  {
    Error ("Invalid slot, return false") ;
    return false ;                                                    // RETURN
  }
  // use the functor
  return m_cut ( stage )  ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::SlotCut::fillStream( std::ostream& s ) const
{ return s << "TC_CUT(" << m_cut << "," << m_slot << ")" ; }
// ============================================================================


// ============================================================================
/*  constructor
 *  @param fun the function
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun, 
  const int    slot,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( fun, bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
/*  constructor
 *  @param fun the function
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function& fun, 
  const int   slot ,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( fun, bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
/*  constructor
 *  @param fun the function
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun, 
  const int   slot ,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( fun, bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
/*  constructor
 *  @param fun the function
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun,
  const int   slot , 
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( fun, bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::BasicFunctors<const Hlt::Stage*>::Function& fun, 
  const int    slot ,
  const double bad  )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( fun, bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::BasicFunctors<const LHCb::Track*>::FunVal& fun, 
  const int    slot,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( fun, bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
/*  constructor
 *  @param fun the function
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFun::SlotFun
( const LoKi::Hlt1::TrackFunction& fun,
  const int    slot , 
  const double bad  )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Function()
  , m_fun( LoKi::Stages::fun_( LoKi::Tracks::TrFunction( fun ), bad ) )
  , m_slot( slot )
  , m_bad( bad )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Candidates::SlotFun::~SlotFun() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::SlotFun* LoKi::Candidates::SlotFun::clone() const
{
    return new LoKi::Candidates::SlotFun( *this );
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::SlotFun::result_type LoKi::Candidates::SlotFun::
operator()( LoKi::Candidates::SlotFun::argument a ) const
{
  //
  if ( !a )
  {
    Error ("Hlt::Candidate* points to NULL, return 'bad'") ;
    return m_bad ;                                                    // RETURN
  }
  // get the slot
  const Hlt::Stage* stage = a->get<Hlt::Stage>( m_slot ) ;
  if ( !stage )
  {
    Error ("Invalid slot, return 'bad'") ;
    return m_bad ;                                                    // RETURN
  }
  // use the functor
  return m_fun ( stage )  ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::SlotFun::fillStream( std::ostream& s ) const
{
  s << "TC_FUN(" << m_fun  << "," << m_slot ;
  if ( 0 != m_bad ) { s << "," << m_bad ; }
  return s << ")" ;
}

// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut ,
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut  (SlotCut( cut , slot ))
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut,
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut( SlotCut( cut, slot ) )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut, const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut( SlotCut( cut, slot ) )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut( SlotCut( cut, slot ) )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Predicate& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut( SlotCut( cut, slot ) )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate& cut,
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut( SlotCut( cut, slot ) )
{}
// ============================================================================
/*  constructor
 *  @param fun the predicate
 *  @param slot the slot:
 *     - 0 corresponds to current stage ,
 *     - negative value corresponds to initiator stage
 *     - positive value corresponds to step-back in history
 */
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter
( const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate& cut, 
  const int slot )
  : LoKi::AuxFunBase ( std::tie ( cut , slot ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe()
  , m_cut( SlotCut( cut, slot ) )
{}
// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotFilter::SlotFilter( const LoKi::Candidates::SlotCut& cut )
  : LoKi::AuxFunBase ( std::tie ( cut ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe(), m_cut( cut )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Candidates::SlotFilter::~SlotFilter() {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::SlotFilter* LoKi::Candidates::SlotFilter::clone() const
{
    return new LoKi::Candidates::SlotFilter( *this );
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::SlotFilter::result_type LoKi::Candidates::SlotFilter::
operator()( LoKi::Candidates::SlotFilter::argument a ) const
{
    LoKi::Candidates::SlotFilter::result_type result;
    result.reserve( a.size() );
    std::copy_if( a.begin(), a.end(), std::back_inserter( result ), m_cut );
    return result;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::SlotFilter::fillStream( std::ostream& s ) const
{
    return s << "TC_FILTER(" << m_cut << ")";
}
// ============================================================================

// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap
( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun, 
  const int slot,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun, slot, bad )
{}
// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap
( const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun, 
  const int slot,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun, slot, bad )
{}
// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap
( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function& fun, 
  const int slot,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun, slot, bad )
{}
// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun,
  const int slot, 
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun, slot, bad )
{}
// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap
( const LoKi::BasicFunctors<const Hlt::Stage*>::Function& fun, 
  const int    slot,
  const double bad )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun, slot, bad )
{}
// ============================================================================
//  constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap
( const LoKi::BasicFunctors<const LHCb::Track*>::FunVal& fun, 
  const int    slot ,
  const double bad  )
  : LoKi::AuxFunBase ( std::tie ( fun , slot , bad ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun, slot, bad )
{}
// ============================================================================
// constructor
// ============================================================================
LoKi::Candidates::SlotMap::SlotMap( const LoKi::Candidates::SlotFun& fun )
  : LoKi::AuxFunBase ( std::tie ( fun ) ) 
  , LoKi::BasicFunctors<const Hlt::Candidate*>::Map(), m_fun( fun )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Candidates::SlotMap::~SlotMap() = default;
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Candidates::SlotMap* LoKi::Candidates::SlotMap::clone() const
{
    return new LoKi::Candidates::SlotMap( *this );
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Candidates::SlotMap::result_type LoKi::Candidates::SlotMap::
operator()( LoKi::Candidates::SlotMap::argument a ) const
{
    LoKi::Candidates::SlotMap::result_type result;
    // Loop over candidates
    std::transform( a.begin(), a.end(), std::back_inserter( result ), m_fun );
    //
    return result;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Candidates::SlotMap::fillStream( std::ostream& s ) const
{ return s << "TC_MAP(" << m_fun <<  ")" ; }
// ============================================================================


// ============================================================================
// The END
// ============================================================================
