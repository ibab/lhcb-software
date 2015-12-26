// $Id: Interpolator.cpp,v 1.1 2009-07-01 18:27:11 polye Exp $
// Include files


// local
#include "Interpolator.h"
#include <iostream>

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
  //y points to interpolate in 2D
  m_ypoints=ypoints_in;

  //number of points
  m_npoints=m_xpoints.size();
  m_nbins=m_npoints-1;

  for (int i=1;i<m_npoints;i++){
    //build interpolator between each pair of consecutive points
    m_uniformer.emplace_back(m_xpoints[i-1],m_xpoints[i],m_ypoints[i-1],m_ypoints[i]);
  }
  //std::cout <<"Uniformer correctly created"<<std::endl;
}

//find which bin corresponds to val
int Uniformer::findBin(const double& xval)
{

  int out_bin=-5;
  //loop over all bins to look up
  for (int i=1;i<m_npoints;i++){
    if (m_xpoints[i-1]<=xval && m_xpoints[i]>xval) out_bin=i-1;
  }
  //if last bin
  if (out_bin==-5)
  {
    if (xval>=m_xpoints[m_npoints-1]) out_bin= m_nbins;
    else out_bin=-1;
  }
  //std::cout<< " bin in uniformer is "<<out_bin<<std::endl;
  return out_bin;

}

//get bin and calculate result from corresponding interpolator
double Uniformer::getYvalue(const double& xval)
{
  int dind=findBin(xval);
  if (dind>=0 && dind<m_nbins) return m_uniformer[dind].value(xval);
  if (dind==-1) return 0.;
  return 1.;
}
