#include<math.h>

#include "ITDet/ITWafer.h"
#include "Kernel/ITChannelID.h"

/// Standard constructor. Parameter iFixedEdge determines what edge 
/// is not clipped if the number of strips is not integer
ITWafer::ITWafer(double pitch, int firstStrip, 
		 double ul, double ur, double vd, double vu, double dz): 
  m_Pitch(pitch),
  m_FirstStrip(firstStrip),
  m_UL(ul),
  m_UR(ur),
  m_VU(vu),
  m_VD(vd),
  m_DZ(dz)
{
  m_NumStrips = (int)((m_UR - m_UL)/m_Pitch);

}

/// Get number of strip at the point (u,v)
int ITWafer::stripNum(const double u, const double v) const  
{
  int id = ITChannelID::nullStrip;
  if (u<m_UR && u>m_UL && v<m_VU && v>m_VD) {
    id = m_FirstStrip + (int)floor((u - m_UL)/m_Pitch);
  }
  return id;
}

/// Get number of nearest strip to the point (u,v)
int ITWafer::nearestStripNum(const double u) const
{
  int id =ITChannelID::nullStrip; ;
  if (u<m_UR && u>m_UL) {
    id = m_FirstStrip + (int)floor((u - m_UL)/m_Pitch);
  }
  else if (u>=m_UR) {
    id = m_FirstStrip + m_NumStrips - 1;
  }
  else {
    id = m_FirstStrip;
  }
  return id;
}

/// Check if point (u,v) is inside of this Wafer
bool ITWafer::isInside(const double u, const double v) const
{
 return (u<(m_UR+(0.5*m_Pitch)) && u>(m_UL-(0.5*m_Pitch)) 
         && v<m_VU && v>m_VD); 
}

/// Check if point (u,v) is inside of this Wafer with tolerance
bool ITWafer::isInside(const double u, const double v, double tolerance) const
{
  return (u<(m_UR+tolerance) && u>(m_UL-tolerance) 
         && v<(m_VU+tolerance) && v>(m_VD-tolerance));
}


/// Get u-coordinate of a strip
double ITWafer::U(const int strip) const 
{

  double u = 0.;
  if (isStrip(strip)) {
    u =  m_UL + (double)(strip - m_FirstStrip)*m_Pitch;
  }
 
  return u;
}

std::ostream& ITWafer::printOut( std::ostream& os ) const{

  os << " wafer: " << std::endl;
  os << "      pitch " << m_Pitch << " # strips " << m_NumStrips <<std::endl;
  os << "      uR  " << m_UR << " uL " <<m_UL <<std::endl;
  os << "      vD  " << m_VD << " uL " <<m_VU <<std::endl;

  return os;
}


MsgStream& ITWafer::printOut( MsgStream& os ) const{

  os << " wafer: " << endreq;
  os << "      pitch " << m_Pitch << " # strips " << m_NumStrips <<endreq;
  os << "      uR  " << m_UR << " uL " <<m_UL << endreq;
  os << "      vD  " << m_VD << " uL " <<m_VU << endreq;

  return os;
}










