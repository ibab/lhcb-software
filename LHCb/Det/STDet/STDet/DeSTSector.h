#ifndef _DeSTSector_H_
#define _DeSTSector_H_

#include <string>
#include <vector>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTBaseElement.h"

/** @class DeSTSector DeSTSector.h "STDet/DeSTSector.h"
 *
 *  Class representing a TT Half module (7 sensors ganged together)
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */


namespace LHCb{
  class Trajectory;
}


class DeSTSector : public DeSTBaseElement  {

public:

  /** Constructor */
  DeSTSector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeSTSector(); 
 
  /** initialization method 
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** sector identfier 
  * @return id 
  */
  unsigned int id() const;

  /** set sector id */
  void setID(const unsigned int id);

  /** check whether contains
  *  @param channel
  *  @return bool
  */
  bool contains(const LHCb::STChannelID aChannel) const;

  /** detector pitch
  * @return pitch
  */
  double pitch() const;

  /** number of strips
  * @return number of strips
  */
  unsigned int nStrip() const;

  /** 
  * check if valid strip number
  * 
  */
  bool isStrip(const unsigned int strip) const;

  /** convert a local u to a strip 
  * @param local u
  * @return strip 
  **/ 
  unsigned int localUToStrip(const double u) const;

  /** convert strip to local U
  * @param strip
  * @return local u
  */
  double localU(const unsigned int strip) const;

  /** trajectory 
  * @return trajectory for the fit 
  */
  LHCb::Trajectory* trajectory(const LHCb::STChannelID& aChan, 
                               const double offset) const;

  /** 
  * @param point in local frame
  * @param tolerance
  * @return bool in active region 
  */
  bool localInActive(const Gaudi::XYZPoint& point, 
                     Gaudi::XYZPoint tol = Gaudi::XYZPoint(0.,0.,0.)) const;
   
  /** 
  * @param v in local frame
  * @param tolerance
  * @return bool in a bond gap - ie dead region
  */
  bool localInBondGap( const double v ,double tol = 0) const;
  
  /** 
  * @param u in local frame
  * @param v in local frame
  * @param uTol
  * @param vTol
  * @return bool in active region of box 
  */
  bool localInBox(const double u, const double v, 
		  double uTol = 0, double vTol = 0) const;


  /** 
  * @param point in global frame
  * @param tolerance
  * @return bool in active region 
  */
  bool globalInActive(const Gaudi::XYZPoint& point, 
                     Gaudi::XYZPoint tol = Gaudi::XYZPoint(0.,0.,0.)) const;
   
  /** 
  * @param point in global frame
  * @param tolerance
  * @return bool in a bond gap - ie dead region
  */
  bool globalInBondGap(const Gaudi::XYZPoint& gpoint ,double tol = 0) const;
  
  /** 
  * @param point in global frame
  * @param uTol
  * @param vTol
  * @return bool in active region of box 
  */
  bool globalInBox(const Gaudi::XYZPoint& gpoint, 
	 	   Gaudi::XYZPoint tol = Gaudi::XYZPoint(0.,0.,0.)) const;

  /** 
   * @return capacitance
  */ 
  double capacitance() const;

  /** strip length 
  * @return strip length
  */
  double stripLength() const;

  /** get the next channel left */
  LHCb::STChannelID nextLeft(const LHCb::STChannelID testChan) const;

  /** get the next channel right */
  LHCb::STChannelID nextRight(const LHCb::STChannelID testChan) const;

  /**
  * @return type
  */
  std::string type() const;

  /** print to stream */
  std::ostream& printOut( std::ostream& os ) const;
     
  /** print to msgstream */
  MsgStream& printOut( MsgStream& os) const;

private:

  void clear();
  void DeSTSector::cacheTrajectory();
  
  unsigned int m_firstStrip;
  unsigned int m_id;
  double m_pitch;
  unsigned int m_nStrip;
  double m_capacitance;
  double m_stripLength;

  double m_uMinLocal;
  double m_uMaxLocal;
  double m_vMinLocal; 
  double m_vMaxLocal;

  LHCb::Trajectory* m_lowerTraj;
  LHCb::Trajectory* m_upperTraj;

  double m_deadWidth;
  std::vector<double> m_deadRegions;
  std::string m_type;

};

inline unsigned int DeSTSector::id() const{
  return m_id;
}

inline void DeSTSector::setID(unsigned int id) {
  m_id = id;
}

inline bool DeSTSector::contains(const LHCb::STChannelID aChannel) const{
  return (aChannel.uniqueSector() == elementID().uniqueSector());
}

inline double DeSTSector::pitch() const{
  return m_pitch;
}

inline unsigned int DeSTSector::nStrip() const{
  return m_nStrip;
}

inline bool DeSTSector::isStrip(const unsigned int strip) const {
  return (strip >= m_firstStrip && strip < m_firstStrip + m_nStrip);
}

inline double DeSTSector::localU(const unsigned int strip) const{
  // strip to local U is this needed ?
  return (isStrip(strip) ? m_uMinLocal + (double)(strip - m_firstStrip)*m_pitch  : 0  );
}

inline double DeSTSector::capacitance() const{
  return m_capacitance;
}

inline double DeSTSector::stripLength() const {
  return m_stripLength;
}

inline std::string DeSTSector::type() const {
  return m_type;
}

/** ouput operator for class DeSTSector
 *  @see DeSTSector
 *  @see MsgStream
 *  @param os reference to STL output stream
 *  @param de reference to DeSTSector object
 */
inline std::ostream& operator<<( std::ostream& os , const DeSTSector* aSector )
{ return aSector->printOut( os ); }


/** ouput operator for class DeSTSector
 *  @see DeSTSector
 *  @see MsgStream
 *  @param os reference to MsgStream output stream
 *  @param de reference to DeSTSector object
 */
inline MsgStream& operator<<( MsgStream& os , const DeSTSector* aSector )
{ return aSector->printOut( os ); }


#endif // _DeSTSector_H










