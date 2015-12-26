// Include files
#include <algorithm>

// local
#include "Interpolator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Uniformer
//
// 2008-07-10 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Uniformer::Uniformer(const std::vector<double>& xpoints_in,const std::vector<double>& ypoints_in) {
  //xpoint to interpolate in 2D
  m_xpoints=xpoints_in;

  // if (xpoints_in.size()!=ypoints_in.size()) throw std::runtime_error();

  m_uniformer.reserve(m_xpoints.size()-1);
  for (unsigned i=1;i<m_xpoints.size();++i){
    //build interpolator between each pair of consecutive points
    m_uniformer.emplace_back(m_xpoints[i-1],m_xpoints[i],ypoints_in[i-1],ypoints_in[i]);
  }
}

//find which bin corresponds to val
int Uniformer::findBin(double xval) const
{
  //loop over all bins to look up
  auto i = std::upper_bound(m_xpoints.begin(),m_xpoints.end(),xval)-m_xpoints.begin();
  return i-1;
}

//get bin and calculate result from corresponding interpolator
double Uniformer::getYvalue(double xval) const
{
  auto dind=findBin(xval);
  if (dind>=0 && dind<static_cast<int>(m_xpoints.size())-1) return m_uniformer[dind].value(xval);
  return dind==-1 ? 0. : 1. ;
}
