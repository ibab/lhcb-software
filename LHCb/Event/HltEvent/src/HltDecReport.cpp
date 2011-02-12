// $Id: $
// Include files

// local
#include "Event/HltDecReport.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecReport
//-----------------------------------------------------------------------------

std::ostream& LHCb::HltDecReport::fillStream(std::ostream& s) const
{
  return s << "{ "
           << " decision : " << decision()
           << " errorBits : " << errorBits()
           << " numberOfCandidates : " << numberOfCandidates()
           << " executionStage : " << executionStage()
           << " intDecisionID : " << intDecisionID()
           << " }" ;
}
