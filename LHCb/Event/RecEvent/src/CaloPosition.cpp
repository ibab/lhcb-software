// $Id: $

// local
#include "Event/CaloPosition.h"

//-----------------------------------------------------------------------------

std::ostream& LHCb::CaloPosition::fillStream(std::ostream& s) const
{
  s << "{ " << std::endl
    << "z :	" << (float)m_z << std::endl
    << "parameters : " << m_parameters << std::endl
    << "covariance :" << std::endl << m_covariance << std::endl
    << "center : " << m_center << std::endl
    << "spread :" << std::endl << m_spread << std::endl << " }";
  return s;
}
