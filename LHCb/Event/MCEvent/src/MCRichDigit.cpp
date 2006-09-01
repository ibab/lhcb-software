
//-----------------------------------------------------------------------------
/** @file MCRichDigit.cpp
 *
 *  Implementation file for class : MCRichDigit
 *
 *  CVS Log :-
 *  $Id: MCRichDigit.cpp,v 1.1 2006-09-01 10:32:38 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    01/09/2006
 */
//-----------------------------------------------------------------------------

#include "Event/MCRichDigit.h"

std::ostream& LHCb::MCRichDigit::fillStream(std::ostream& s) const
{
  s << "{ " << "hits :";
  for ( LHCb::MCRichDigitHit::Vector::const_iterator iH = m_hits.begin();
        iH != m_hits.end(); ++iH )
  {
    s << " [ " << (*iH).history() << " " << *(*iH).mcRichHit() << " ]";
  }
  s << std::endl;
  s << "history : " << m_history << std::endl << " }";
  return s;
}
