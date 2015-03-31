// $Id: $

// local
#include "Event/CaloHypo.h"

//-----------------------------------------------------------------------------

std::ostream& LHCb::CaloHypo::fillStream(std::ostream& s) const
{
  s << "{" << std::endl
    << "hypothesis : " << m_hypothesis << std::endl
    << "lh : " << m_lh << std::endl;
  if ( m_position )
  {
    s << "position : " << *m_position << std::endl;
  }
  else
  {
    s << "No position" << std::endl;
  }
  return s << " }";
}

LHCb::CaloHypo* LHCb::CaloHypo::clone() const
{
  CaloHypo* hypo = new CaloHypo( *this );
  if ( NULL != position() ) { hypo->setPosition( position()->clone() ); }
  return hypo;
}
