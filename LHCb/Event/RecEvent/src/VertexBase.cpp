// $Id: $

#include "Event/VertexBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexBase
//
// 2012-04-26 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::VertexBase::fillStream(std::ostream& s) const
{
  s << "{ "
    << "position  : " << m_position << std::endl
    << "covMatrix : " << std::endl << m_covMatrix << std::endl
    << "chi2      : " << m_chi2 << std::endl
    << "nDoF      : " << m_nDoF << std::endl
    << "extraInfo : [";
  for ( ExtraInfo::const_iterator i = extraInfo().begin();
        i != extraInfo().end(); ++i )
  {
    s << " " << i->first << "=" << i->second;
  }
  return s << " ] }";
}

LHCb::VertexBase* LHCb::VertexBase::clone() const 
{
  return new VertexBase(*this);
}

bool LHCb::VertexBase::isPrimary() const 
{
  return false;
}
