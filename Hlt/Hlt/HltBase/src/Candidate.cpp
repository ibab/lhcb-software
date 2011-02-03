// $Id: Candidate.cpp,v 1.3 2010-08-05 10:58:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/INamedInterface.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
#include "Event/HltStage.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
/** @file  Candidate.cpp
 *  The implementation of class Htl::Candidate 
 *  @see Hlt::Candidate 
 */
// ============================================================================
// anonymous namespace to hide local symbols:
namespace 
{ 
  // ===========================================================================
  /** @var CLID_Candidate 
   *  Class ID definition 
   */
  const CLID CLID_Candidate = 7561;
  // ==========================================================================
  /** @var e_EMPTY
   *  the empty string 
   */
  const std::string s_EMPTY = ""  ;
  // ==========================================================================
}
// ============================================================================
// Class ID 
// ============================================================================
const CLID& Hlt::Candidate::classID() { return CLID_Candidate; }
// ============================================================================
// Destructor
// ============================================================================
Hlt::Candidate::~Candidate() {}
// ============================================================================
// remove the stage 
// ============================================================================
void Hlt::Candidate::removeFromStages(const Hlt::Stage* value)
{
  SmartRef<Hlt::Stage> ref(value);
  SmartRefVector<Hlt::Stage>::iterator iter =
    std::remove(m_stages.begin(), m_stages.end(), ref);
  m_stages.erase(iter, m_stages.end());
}
// ===========================================================================
// Update  Reference to the stages
// ===========================================================================
void Hlt::Candidate::setStages( const SmartRefVector<Hlt::Stage>& value ) 
{
  m_stages.clear() ;
  for ( SmartRefVector<Hlt::Stage>::const_iterator istage = value.begin() ;
        value.end() != istage ; ++istage ) { addToStages ( *istage ); }
}
// ===========================================================================
// Add to (pointer) reference to the stages
// ===========================================================================
void Hlt::Candidate::addToStages ( const Hlt::Stage* value ) 
{ 
  if ( 0 != value ) 
  {
    m_stages.push_back ( value ) ;
    m_stages.back()->setOwner ( this ) ;
  }
}
// ===========================================================================
// Last worker (algorithm name)
// ===========================================================================
const Hlt::Candidate::Worker& 
Hlt::Candidate::lastWorker() const
{ return m_workers.empty() ? s_EMPTY : m_workers.back() ; }
// ===========================================================================
// Add worker
// ===========================================================================
void Hlt::Candidate::addToWorkers ( const INamedInterface* worker ) 
{ if ( 0 != worker ) { m_workers.push_back ( worker -> name() ) ; }  }
// ===========================================================================
// Has stage ? 
// ===========================================================================
bool Hlt::Candidate::hasStage ( const Hlt::Stage* stage ) const
{
  return 
    0 != stage && 
    m_stages.end() != std::find ( m_stages.begin () , 
                                  m_stages.end   () , 
                                  SmartRef<Hlt::Stage> ( stage  ) ) ;
}
// ============================================================================
// the output operator 
// ============================================================================
std::ostream& Hlt::Candidate::fillStream(std::ostream& s) const
{
  s << "Hlt::Candidate/" << (const void*)this << " : " ;
  //
  Gaudi::Utils::toStream ( m_stages.begin () , 
                           m_stages.end   () , s , 
                           "\n\tStages: [ " , "] " , " ,\n\t\t  "   ) ;  
  Gaudi::Utils::toStream ( m_workers.begin () , 
                           m_workers.end   () , s , 
                           "\n\tWorkers: [" , "]" , ","    ) ;
  return s << std::endl ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const Hlt::Candidate* c , std::ostream& s ) 
{
  if ( 0 == c ){ return s << "<NULL>" ; }
  return c->fillStream ( s ) ;
}
// ============================================================================
/*  easy accessor to various  underlying objects through bypassin 
 *  of stage-object
 *  @see Hlt::Stage::get_ 
 *  @see Hlt::Candidate::get
 *  @param slot the slot to be used for data extraction 
 *     - 0 corresponds to the current stage , 
 *     - negative value corresponds to initiator stage 
 *     - positive value corresponds to step-back in history
 *  @return the obejct 
 */
// ============================================================================
const ContainedObject*
Hlt::Candidate::get_( const int slot ) const 
{
  const Hlt::Stage* _stage = this->get<Stage>( slot ) ;
  if ( 0 == _stage ) { return 0 ; }                                 // RETURN 
  return _stage->get_ () ;
}  
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
