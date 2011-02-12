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
  for(Container::const_iterator i=m_vertexReports.begin();
      i!=m_vertexReports.end(); ++i){
    ss << " selectionName : " << i->first;
    ss << " HltVertexReport : size " << i->second.size()
       << std::endl;
    for( HltVertexReport::const_iterator iv=i->second.begin();
         iv!=i->second.end();++iv ){
      const VertexBase & v = **iv;
      ss << " x " << v.position().x();
      ss << " y " << v.position().y();
      ss << " z " << v.position().z();
      ss << " chi2 " << v.chi2();
      ss << " ndf " << v.nDoF();
      const Gaudi::SymMatrix3x3 & cov = v.covMatrix();
      ss << " cov xx " << cov[0][0];
      ss << " yy " << cov[1][1];
      ss << " zz " << cov[2][2];
      ss << " xy " << cov[0][1];
      ss << " xz " << cov[0][2];
      ss << " yz " << cov[1][2];
      ss << std::endl;
    }
  }
  ss << " }" << std::endl;
  return ss;

}
