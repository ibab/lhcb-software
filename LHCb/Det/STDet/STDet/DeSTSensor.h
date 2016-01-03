#ifndef _DeSTSensor_H_
#define _DeSTSensor_H_

#include <string>
#include <memory>

#include "Kernel/STChannelID.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "STDet/DeSTBaseElement.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"

/** @class DeSTSensor DeSTSensor.h "STDet/DeSTSensor.h"
 *
 *  Class representing a ST Sensor
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
*/


class DeSTSector;

namespace LHCb{
  class Trajectory;
}

class DeSTSensor : public DeSTBaseElement  {

public:

  /** Constructor */
  DeSTSensor ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeSTSensor();

  /** initialization method
   * @return StatusCode OK or not
   */
  virtual StatusCode initialize();

  /** check whether contains
  *  @param  aChannel channel
  *  @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const;

  /** sector identfier
   * @return id
   */
  unsigned int id() const;

  /** set sector id */
  void setID(const unsigned int id);

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
   * @param  u local u
   * @return bool strip
   **/
  unsigned int localUToStrip(double u) const;

  /** convert strip to local U
   * @param strip
   * @param offset
   * @return local u
   */
  double localU(unsigned int strip, double offset = 0.) const;

  /** trajectory
   * @return trajectory for the fit
   */
  std::unique_ptr<LHCb::Trajectory> trajectory(unsigned int strip,
                                               double offset) const;


  /** plane corresponding to the sector
   * @return the plane
   */
  Gaudi::Plane3D plane() const;

  /** plane corresponding to the module entrance
   * @return the plane
   */
  Gaudi::Plane3D entryPlane() const;

  /** plane corresponding to the module exit
   * @return the plane
   */
  Gaudi::Plane3D exitPlane() const;

  /** localInActive
   * @param  point point in local frame
   * @param  tol   tolerance
   * @return in active region
   */
  bool localInActive(const Gaudi::XYZPoint& point,
                     Gaudi::XYZPoint tol = Gaudi::XYZPoint(0.,0.,0.)) const;

  /** globalInActive
   * @param  point point in global frame
   * @param  tol   tolerance
   * @return bool in active region
   */
  bool globalInActive(const Gaudi::XYZPoint& point,
                      Gaudi::XYZPoint tol = Gaudi::XYZPoint(0.,0.,0.)) const;

  /** localInBondGap
   * @param  v     coordinate in local frame
   * @param  tol   tolerance
   * @return bool if in bond gap
   */
  bool localInBondGap(const double v,
                      const double tol) const;

  /** globalInActive
   * @param  point point in global frame
   * @param  tol   tolerance
   * @return bool in bondgap
   */
  bool globalInBondGap(const Gaudi::XYZPoint& point,
                       double tol = 0) const;

  /** thickness
   * @return double thickness
   */
  double thickness() const;

  /** active height
  * @return double activeHeight
  **/
  double activeHeight() const;

  /** active height
  * @return double activeWidth
  **/
  double activeWidth() const;

  /**  sensor capacitance **/
  double capacitance() const;

  /** x sense of local frame relative to global */
  bool xInverted() const;

  /** y sense of local frame relative to global */
  bool yInverted() const;

  /** print to stream */
  std::ostream& printOut( std::ostream& os ) const;

  /** print to msgstream */
  MsgStream& printOut( MsgStream& os) const;

protected:

  void cacheParentProperties(const DeSTSector* parent);
  StatusCode initGeometryInfo();

private:

  void clear();
  void determineSense();
  StatusCode cacheInfo();
  StatusCode registerConditionsCallbacks();

  Gaudi::Plane3D m_plane;
  Gaudi::Plane3D m_entryPlane;
  Gaudi::Plane3D m_exitPlane;

  Gaudi::XYZVector m_direction;
  std::pair<double,double> m_range;

  unsigned int m_firstStrip;
  unsigned int m_id;
  double m_pitch;
  unsigned int m_nStrip;
  std::string m_versionString;

  double m_thickness;
  double m_uMinLocal;
  double m_uMaxLocal;
  double m_vMinLocal;
  double m_vMaxLocal;

  LHCb::Trajectory* m_midTraj;

  bool m_xInverted;
  bool m_yInverted;

  double m_deadWidth;
  double m_stripLength;

};

inline bool DeSTSensor::contains(const LHCb::STChannelID aChannel) const{
  return (aChannel.uniqueSector() == elementID().uniqueSector());
}

inline unsigned int DeSTSensor::id() const{
  return m_id;
}

inline void DeSTSensor::setID(unsigned int id) {
  m_id = id;
}

inline double DeSTSensor::pitch() const{
  return m_pitch;
}

inline bool DeSTSensor::isStrip(const unsigned int strip) const {
  return (strip >= m_firstStrip && strip < m_firstStrip + m_nStrip);
}

inline unsigned int DeSTSensor::nStrip() const{
  return m_nStrip;
}


inline double DeSTSensor::thickness() const{
  return m_thickness;
}

inline double DeSTSensor::activeWidth() const{
  return fabs(m_uMinLocal - m_uMaxLocal);
}

inline double DeSTSensor::activeHeight() const{
  return fabs(m_vMinLocal - m_vMaxLocal);
}

inline double DeSTSensor::capacitance() const {
  // by chance the CMS and LHCb sensors have same capacitance
  static const double rho = 1.4 * Gaudi::Units::picofarad/Gaudi::Units::cm;
  return rho * activeHeight();
}

inline Gaudi::Plane3D DeSTSensor::plane() const {
  return m_plane;
}

inline Gaudi::Plane3D DeSTSensor::entryPlane() const {
  return m_entryPlane;
}

inline Gaudi::Plane3D DeSTSensor::exitPlane() const {
  return m_exitPlane;
}

inline bool DeSTSensor::globalInActive( const Gaudi::XYZPoint& gpoint,
                                 Gaudi::XYZPoint tol) const
{
  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInActive(lPoint,tol);
}

inline bool DeSTSensor::globalInBondGap(const Gaudi::XYZPoint& gpoint,
                                        double tol) const
{
  Gaudi::XYZPoint lPoint = toLocal(gpoint);
  return localInBondGap(lPoint.Y(),tol);
}

inline bool DeSTSensor::localInBondGap( const double v, double tol) const{

  return ((v+tol > m_uMinLocal) && (v-tol < m_uMaxLocal) );

}

inline bool DeSTSensor::xInverted() const {
  return  m_xInverted;
}

inline bool DeSTSensor::yInverted() const {
  return  m_yInverted;
}

/** ouput operator for class DeSTSensor
 *  @see DeSTSensor
 *  @see MsgStream
 *  @param os      reference to STL output stream
 *  @param aSensor reference to DeSTSensor object
 */
inline std::ostream& operator<<( std::ostream& os , const DeSTSensor* aSensor )
{ return aSensor->printOut( os ); }


/** ouput operator for class DeSTSensor
 *  @see DeSTSensor
 *  @see MsgStream
 *  @param os      reference to MsgStream output stream
 *  @param aSensor reference to DeSTSensor object
 */
inline MsgStream& operator<<( MsgStream& os , const DeSTSensor* aSensor )
{ return aSensor->printOut( os ); }

#endif // _DeSTSector_H










