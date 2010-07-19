// $Id: Candidate.cpp,v 1.1 2010-07-19 16:24:08 ibelyaev Exp $
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
  static const CLID CLID_Candidate = 7561;
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
