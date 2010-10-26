// $Id: Candidate.cpp,v 1.3 2010-08-05 10:58:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
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
  s << "('Candidate',[ ";
  std::string delim = "";
  for (StagesConstIterator i = stages().begin(); i != stages().end(); ++i) {
    s << delim << "'" << **i << "'";
    delim = ",";
  }
  s << "],[";
  delim = "";
  for (WorkersConstIterator i = workers().begin(); i != workers().end(); ++i) {
    s << delim << "'" << *i << "'";
    delim = ", ";
  }
  s << "])";
  return s;
}

// ============================================================================
// The END 
// ============================================================================
