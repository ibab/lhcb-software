// $Id: RayIntersector.cpp,v 1.1 2005-10-28 16:02:30 hernando Exp $
// Include files

// local
#include "RayIntersector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RayIntersector
//
// 2005-10-27 : Jose A. Hernando
//-----------------------------------------------------------------------------

const EVector& RayIntersector::distance(const EVector& x1, 
                                        const EVector& v1) 
{
  m_x1 = x1;
  m_v1 = v1;
  
  m_v2 = m_w.cross(m_v1);
  
  m_x   = m_x1-m_x2;
  
  m_v10 = m_x.dot(m_v1);
  m_v20 = m_x.dot(m_v2);
  m_v11 = m_v1.dot(m_v1);
  m_v22 = m_v2.dot(m_v2);
  m_v12 = m_v1.dot(m_v2);
  
  m_D  = -m_v11*m_v22+m_v12*m_v12;
  double N1 =  m_v10*m_v22-m_v20*m_v12;
  double N2 = -m_v20*m_v11+m_v10*m_v12;
  
  m_phi1 = N1/m_D;
  m_phi2 = N2/m_D;

  m_distance = m_phi2*m_v2;

  return m_distance;
}
 

double RayIntersector::ddistance(const EVector& dx, const EVector& dv1, 
                                 const EVector& dv2)
{
  double n1 = -m_v11*(dx.dot(m_v2)+m_x.dot(dv2))-m_v20*2.*dv1.dot(m_v1);
  double n2 =  m_v12*(dx.dot(m_v1)+m_x.dot(dv1)) 
    + m_v10*(dv1.dot(m_v2)+m_v1.dot(dv2));
  
  double d1 = -2*dv1.dot(m_v1)*m_v22-2*m_v11*dv2.dot(m_v2);
  double d2 =  2*m_v12*(m_v1.dot(dv2)+m_v2.dot(dv1));
  double dd =  ((n1+n2)-m_phi2*(d1+d2))/m_D;

  double dtot = dd*sqrt(m_v22)+m_phi2*m_v2.dot(dv2)/sqrt(m_v22);

  return dtot;
}


double RayIntersector::ddistance(const EVector& dx1, const EVector& dv1,
                                 const EVector& dx2, const EVector& dw) 
{
  EVector dx  = dx1-dx2;
  EVector dv2 = dw.cross(m_v1)+m_w.cross(dv1);
  return ddistance(dx,dv1,dv2);
}





