// $Id: $
// Include files

// local
#include "Event/HltVertexReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexReports
//-----------------------------------------------------------------------------

std::ostream& LHCb::HltVertexReports::fillStream(std::ostream& ss) const
{
  ss << " HltVertexReports : { "  << std::endl;
  for(const auto& i : m_vertexReports ) {
    ss << " selectionName : " << i.first;
    ss << " HltVertexReport : size " << i.second.size()
       << '\n';
    for( const auto& iv : i.second ) {
      ss << " x " << iv->position().x();
      ss << " y " << iv->position().y();
      ss << " z " << iv->position().z();
      ss << " chi2 " << iv->chi2();
      ss << " ndf " << iv->nDoF();
      const Gaudi::SymMatrix3x3 & cov = iv->covMatrix();
      ss << " cov xx " << cov[0][0];
      ss << " yy " << cov[1][1];
      ss << " zz " << cov[2][2];
      ss << " xy " << cov[0][1];
      ss << " xz " << cov[0][2];
      ss << " yz " << cov[1][2];
      ss << '\n';
    }
  }
  ss << " }" << std::endl;
  return ss;

}
