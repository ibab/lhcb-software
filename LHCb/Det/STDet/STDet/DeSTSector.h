#ifndef _DeSTSector_H_
#define _DeSTSector_H_

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "Kernel/STChannelID.h"
#include "Kernel/LHCbConstants.h"
#include "STDet/DeSTBaseElement.h"
#include "STDet/DeSTSensor.h"

#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"



/** @class DeSTSector DeSTSector.h "STDet/DeSTSector.h"
 *
 *  Class representing a TT Half module (7 sensors ganged together)
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
 \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
*/


namespace LHCb{
  class Trajectory;
  class PiecewiseTrajectory;
}

class DeSTSensor;


class DeSTSector : public DeSTBaseElement  {

public:

  /** status enum
  * <b> For details on definitions see:</b>
  * \li <a href="http://ckm.physik.unizh.ch/software/det/deadStrips.php"><b>documentation</b></a><p>
  */
  enum Status{
    OK = 0,
    Open = 1,
    Short = 2,
    Pinhole = 3,
    ReadoutProblems = 4,
    NotBonded = 5,
    LowGain = 6,
    Noisy = 7,
    OtherFault = 9,
    Dead = 10,
    UnknownStatus = 100
  };

  typedef std::vector<DeSTSensor*> Sensors;
  typedef LHCb::PiecewiseTrajectory STTraj;

  /** Constructor */
  DeSTSector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeSTSector();

  /** initialization method
   * @return StatusCode OK or not
   */
  virtual StatusCode initialize();

  /** get the sector name as expected in the Condition DB
   * @return std::string "sector name"
   */
  virtual std::string conditionsPathName() const = 0;

  /** get the noise of the corresponding strip
   * @param aChannel channel
   * @return double noise of the strip
   */
  double noise(const LHCb::STChannelID& aChannel) const;

  /** get the average noise in the sector
   * @return double average noise
   */
  double sectorNoise() const;

  /** get the average noise of a beetle
   * @param beetle beetle number (1-4)
   * @return double average noise
   */
  double beetleNoise(const unsigned int& beetle) const;

  /** get the average noise of a beetle port
   * @param beetle beetle number (1-4)
   * @param port beetle port number (1-3)
   * @return double average noise
   */
  double portNoise(const unsigned int& beetle, const unsigned int& port) const;

  /** set the Noise of the corresponding strip
   * @param strip strip number
   * @param value Noise value
   */
  void setNoise(const unsigned int& strip, const double& value);

  /** set the Noise of the corresponding channel
   * @param chan channel
   * @param value Noise value
   */
  void setNoise(const LHCb::STChannelID &chan, const double& value);

  /** set the Noise vector
   * @param values Noise vector
   */
  void setNoise(const std::vector<double>& values);

  /** get the Noise of the corresponding strip
   * @param aChannel channel
   * @return double noise of the strip
   */
  double rawNoise(const LHCb::STChannelID& aChannel) const;

  /** get the average raw noise in the sector
   * @return double average noise
   */
  double rawSectorNoise() const;

  /** get the average raw noise of a beetle
   * @param beetle beetle number (1-4)
   * @return double average noise
   */
  double rawBeetleNoise(const unsigned int& beetle) const;

  /** get the average raw noise of a beetle port
   * @param beetle beetle number (1-4)
   * @param port beetle port number (1-3)
   * @return double average noise
   */
  double rawPortNoise(const unsigned int& beetle, const unsigned int& port) const;

  /** get the common mode noise of the corresponding strip
   * @param aChannel channel
   * @return double noise of the strip
   */
  double cmNoise(const LHCb::STChannelID& aChannel) const;

  /** get the average common noise in the sector
   * @return double average noise
   */
  double cmSectorNoise() const;

  /** get the average common mode noise of a beetle
   * @param beetle beetle number (1-4)
   * @return double average noise
   */
  double cmBeetleNoise(const unsigned int& beetle) const;

  /** get the average common mode noise of a beetle port
   * @param beetle beetle number (1-4)
   * @param port beetle port number (1-3)
   * @return double average noise
   */
  double cmPortNoise(const unsigned int& beetle, const unsigned int& port) const;

  /** set the cmNoise of the corresponding strip
   * @param strip strip number
   * @param value cmNoise value
   */
  void setCMNoise(const unsigned int& strip, const double& value);

  /** set the cmNoise of the corresponding channel
   * @param chan channel
   * @param value cmNoise value
   */
  void setCMNoise(const LHCb::STChannelID &chan, const double& value);

  /** set the cmNoise vector
   * @param values cmNoise vector
   */
  void setCMNoise(const std::vector<double>& values);

  /** set the ACD count from the electron number vector
   * @param values
   */
  void setADCConversion(const std::vector<double>& values);

  /** get the ADC count from the electron number
   * @param e electron number
   * @param aChannel channel
   * @return ADC count
   */
  double toADC(const double& e, const LHCb::STChannelID& aChannel) const;

  /** get the ADC count from the electron number
   * @param e electron number
   * @param aStrip strip number
   * @return ADC count
   */
  double toADC(const double& e, const unsigned int& aStrip) const;

  /** get the electron number from the ADC count
   * @param val ADV count
   * @param aChannel channel
   * @return electron number
   */
  double toElectron(const double& val,
                    const LHCb::STChannelID& aChannel) const;

  /** get the electron number from the ADC count
   * @param val ADV count
   * @param aStrip strip number
   * @return electron number
   */
  double toElectron(const double& val, const unsigned int& aStrip) const;

  /** sector identfier
   * @return id
   */
  unsigned int id() const;

  /** set sector id */
  void setID(const unsigned int id);

  /** check whether contains
   *  @param  aChannel channel
   *  @return bool
   */
  virtual bool contains(const LHCb::STChannelID aChannel) const;

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


  /** trajectory
   * @return trajectory for the fit
   */
  std::unique_ptr<LHCb::Trajectory> trajectory(const LHCb::STChannelID& aChan,
                                             const double offset) const;

  /** trajectory
   * @return trajectory of the first strip
   */
  std::unique_ptr<LHCb::Trajectory> trajectoryFirstStrip() const;

  /** trajectory
   * @return trajectory of the last strip
   */
  std::unique_ptr<LHCb::Trajectory> trajectoryLastStrip() const;

  /** Trajectory parameterized along y-axis */
  void trajectory(unsigned int strip, double offset, double& dxdy, double& dzdy,
                  double& xAtYEq0, double& zAtYEq0, double& ybegin, double& yend) const ;

  /**
   * @return total capacitance
   * ie sensors, cable + pitch adaptor
   */
  double capacitance() const;

  /**
   * @return sensor Capacitance
   */
  double sensorCapacitance() const;

  /** strip length
   * @return strip length
   */
  double stripLength() const;

  /** thickness
   * @return double thickness
   */
  double thickness() const;

  /** get the next channel left
  * @return next chan left
  */
  LHCb::STChannelID nextLeft(const LHCb::STChannelID testChan) const;

  /** get the next channel right
  * @return next chan left
  */
  LHCb::STChannelID nextRight(const LHCb::STChannelID testChan) const;

  /// Workaround to prevent hidden base class function
  inline const std::type_info& type(const std::string &name) const
                      {return ParamValidDataObject::type(name);}
  /**
   * @return std::string type
   */
  std::string type() const;

  /** @return double stereo angle */
  double angle() const;

  /** @return double sin of stereo angle */
  double sinAngle() const;

  /** @return cosine of stereo angle */
  double cosAngle() const;

  /** @return check if is a stereo ladder */
  bool isStereo() const;

  /** beetle corresponding to channel  1-3 (IT) 1-4 (TT)*/
  unsigned int beetle(const LHCb::STChannelID& chan) const;


  /** beetle corresponding to channel  1-3 (IT) 1-4 (TT)*/
  unsigned int beetle(const unsigned int strip) const;

  /** n beetle
   * @return double nBeetles
  */
  unsigned int nBeetle() const;

  /** measured efficiency
   * @ return double measured Eff
   */
  double measEff() const;

  /** set measured Eff of sector  */
  void setMeasEff(const double measEff) ;

  /** Status of sector
   @return Status of readout sector
  */
  Status sectorStatus() const;

  /** Status of the Beetle corresponding to strip */
  Status beetleStatus(const LHCb::STChannelID& chan) const;

  /** Status of the Beetle with given id  1-3 (IT), 1-4 (TT) */
  Status beetleStatus(const unsigned int id) const;

  /** vector of beetle status */
  std::vector<DeSTSector::Status> beetleStatus() const;

  /** Status of channel */
  Status stripStatus(const LHCb::STChannelID& chan) const;

  /** get vector of strip status for all strips in sector */
  std::vector<Status> stripStatus() const;

  /** set the sector status */
  void setSectorStatus(const Status& newStatus);

  /** set vector of beetleStatus
  * @param unsigned int beetle [numbering from 1]
  * @param Status newStatus
  **/
  void setBeetleStatus(const unsigned int beetle, const Status& newStatus);

  /** set vector of beetleStatus
  * @param LHCb::STChannelID chan id of beetle
  * @param Status newStatus
  **/
  void setBeetleStatus(const LHCb::STChannelID& chan, const Status& newStatus);

  /** set vector of beetleStatus
  * @param unsigned int strip [numbering from 1]
  * @param Status newStatus
  **/
  void setStripStatus(const unsigned int strip, const Status& newStatus);

  /** set vector of beetleStatus
  * @param LHCb::STChannelID chan id of strip
  * @param Status newStatus
  **/
  void setStripStatus(const LHCb::STChannelID& chan, const Status& newStatus);

  /** short cut for strip status ok
  * @return isOKStrip
  */
  bool isOKStrip(const LHCb::STChannelID& chan) const;

  /** strip to channel
  * @param strip
  * @return corresponding channel */
  LHCb::STChannelID stripToChan(const unsigned int strip) const;

  /** production ID
  *  @return unsigned int prodID
  */
  virtual unsigned int prodID() const = 0;

  /** version */
  std::string versionString() const;

  /** dead width */
  double deadWidth() const;

  /** print to stream */
  std::ostream& printOut( std::ostream& os ) const;

  /** print to msgstream */
  MsgStream& printOut( MsgStream& os) const;

  /** flat vector of sensors
  * @return vector of sensors
  */
  const Sensors& sensors() const;

  /** locate sensor based on a point
  * @return module */
  DeSTSensor* findSensor(const Gaudi::XYZPoint& point) const;


  /** find the middle sensor. rounding down if odd **/
  DeSTSensor* middleSensor() const;

  /** check if inside the active area
  * @param  point point in global frame
  * @param  tol   tolerance
  * @return bool isInside
  **/
  bool globalInActive(const Gaudi::XYZPoint& point,
                      Gaudi::XYZPoint tol = Gaudi::XYZPoint(0.,0.,0.)) const;

  /** globalInActive
   * @param  point point in global frame
   * @param  tol   tolerance
   * @return bool in bondgap
   */
  bool globalInBondGap(const Gaudi::XYZPoint& point,
                       double tol = 0) const;

  /**
  * Nickname for the sensor
  **/
  const std::string& nickname() const ;

  /**
  * fraction active channels
  * @return bool fraction active
  */
  double fractionActive() const;

  /** direct access to the status condition, for experts only */
  const Condition* statusCondition() const;

  /** direct access to the noise condition, for experts only */
  const Condition* noiseCondition() const;

  /** x sense of local frame relative to global */
  bool xInverted() const;

  /** y sense of local frame relative to global */
  bool yInverted() const;

protected:

  StatusCode registerConditionsCallbacks();
  StatusCode cacheInfo();

  Sensors m_sensors;
  double m_thickness;
  std::string m_nickname;

private:

  typedef std::map<unsigned int,Status> StatusMap;

  //  void clear();

  StatusCode updateStatusCondition();
  StatusCode updateNoiseCondition();
  void toEnumMap(const std::map<int,int>& input, DeSTSector::StatusMap& output);
  std::unique_ptr<LHCb::Trajectory> createTraj(const unsigned int strip, const double offset ) const;
  void setStatusCondition( const std::string& type, const unsigned int entry,
                           const DeSTSector::Status& newStatus );


  unsigned int m_firstStrip;
  unsigned int m_firstBeetle;
  unsigned int m_id;
  double m_pitch;
  unsigned int m_nStrip;
  double m_capacitance;
  double m_stripLength;
  // std::pair<double, double> m_range;

  double m_deadWidth;
  std::string m_type;

  double m_dxdy ;
  double m_dzdy ;
  double m_dy ;
  Gaudi::XYZVector m_dp0di ;
  Gaudi::XYZPoint  m_p0 ;
  double m_angle ;
  double m_cosAngle ;
  double m_sinAngle ;
  mutable double m_measEff;

  // status info
  Status m_status;
  mutable StatusMap m_beetleStatus;
  mutable StatusMap m_stripStatus;
  std::string m_statusString;
  std::string m_versionString;


  // Noise info
  std::string m_noiseString;
  std::vector< double > m_noiseValues;
  std::vector< double > m_electronsPerADC;
  std::vector< double > m_cmModeValues;


 protected:

  bool m_isStereo;

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

inline std::string DeSTSector::versionString() const{
  return m_versionString;
}

inline double DeSTSector::deadWidth() const{
  return m_deadWidth;
}

inline bool DeSTSector::isStrip(const unsigned int strip) const {
  return (strip >= m_firstStrip && strip < m_firstStrip + m_nStrip);
}

inline double DeSTSector::capacitance() const{
  return m_capacitance;
}

inline double DeSTSector::sensorCapacitance() const{
  const Sensors& theSensors = sensors();
  return theSensors.size() * theSensors.front()->capacitance(); ;
}

inline double DeSTSector::stripLength() const {
  return m_stripLength;
}

inline double DeSTSector::thickness() const{
  return m_thickness;
}

inline std::string DeSTSector::type() const {
  return m_type;
}

inline double DeSTSector::angle() const {
  return m_angle;
}

inline double DeSTSector::sinAngle() const {
  return m_sinAngle;
}

inline double DeSTSector::cosAngle() const {
  return m_cosAngle;
}

inline bool DeSTSector::isStereo() const{
  return m_isStereo;
}

inline void DeSTSector::trajectory(unsigned int strip,
                                   double offset,
                                   double& dxdy, double& dzdy,
                                   double& xAtYEq0, double& zAtYEq0,
                                   double& ybegin, double& yend) const
{
  const double numstrips = offset + strip - m_firstStrip ;
  dxdy    = m_dxdy ;
  dzdy    = m_dzdy ;
  xAtYEq0 = m_p0.x() + numstrips * m_dp0di.x() ;
  zAtYEq0 = m_p0.z() + numstrips * m_dp0di.z() ;
  ybegin  = m_p0.y() + numstrips * m_dp0di.y() ;
  yend    = ybegin + m_dy ;
}

inline unsigned int DeSTSector::beetle(const LHCb::STChannelID& chan) const{
  return beetle(chan.strip());
}


inline unsigned int DeSTSector::beetle(const unsigned int strip) const{
  return ((strip-1u)/LHCbConstants::nStripsInBeetle) + 1u;
}

inline unsigned int DeSTSector::nBeetle() const{
  return nStrip()/LHCbConstants::nStripsInBeetle;
}

inline void DeSTSector::setNoise(const LHCb::STChannelID &chan, const double& value)
{
  // just delegate
  setNoise(chan.strip(),value);
}

inline void DeSTSector::setCMNoise(const LHCb::STChannelID &chan, const double& value)
{
  // just delegate
  setCMNoise(chan.strip(),value);
}

inline DeSTSector::Status DeSTSector::sectorStatus() const{
  return m_status;
}

inline DeSTSector::Status DeSTSector::beetleStatus(const LHCb::STChannelID& chan) const{
  return beetleStatus(beetle(chan));
}

inline void DeSTSector::setBeetleStatus(const LHCb::STChannelID& chan,
                                        const DeSTSector::Status& newStatus){
  // just delegate
  setBeetleStatus(beetle(chan), newStatus);
}

inline void DeSTSector::setStripStatus(const LHCb::STChannelID& chan,
                                      const DeSTSector::Status& newStatus){
  // just delegate
  setStripStatus(chan.strip(), newStatus);
}

inline DeSTSector::Status DeSTSector::beetleStatus(const unsigned int id) const{
  DeSTSector::Status theStatus = sectorStatus();
  if (theStatus == DeSTSector::OK){
    DeSTSector::StatusMap::const_iterator iter = m_beetleStatus.find(id);
    if (iter != m_beetleStatus.end()) theStatus = iter->second;
  }
  return theStatus;
}

inline DeSTSector::Status DeSTSector::stripStatus(const LHCb::STChannelID& chan) const{
  DeSTSector::Status theStatus = beetleStatus(chan);
  if (theStatus == DeSTSector::OK){
    DeSTSector::StatusMap::const_iterator iter = m_stripStatus.find(chan.strip());
    if (iter != m_stripStatus.end()) theStatus = iter->second;
  }
  return theStatus;
}


inline bool DeSTSector::isOKStrip(const LHCb::STChannelID& chan) const{
  return (stripStatus(chan) == DeSTSector::OK ? true : false);
}

inline std::vector<DeSTSector::Status> DeSTSector::beetleStatus() const{
  std::vector<Status> vec; vec.resize(nBeetle());
  for (unsigned int iBeetle = m_firstBeetle; iBeetle <= nBeetle(); ++iBeetle){
    if (sectorStatus() != DeSTSector::OK){
      vec[iBeetle - 1] = sectorStatus();
      continue;
    }
    DeSTSector::StatusMap::const_iterator iter = m_beetleStatus.find(iBeetle);
    if (iter != m_beetleStatus.end()){
      vec[iBeetle - 1] = iter->second;
    }
    else{
      vec[iBeetle - 1] = DeSTSector::OK;
    }
  } // nStrip
  return vec;
}

inline std::vector<DeSTSector::Status> DeSTSector::stripStatus() const{
  std::vector<Status> vec; vec.resize(nStrip());
  for (unsigned int iStrip = m_firstStrip; iStrip <= nStrip(); ++iStrip){
    if (sectorStatus() != DeSTSector::OK){
      vec[iStrip - 1] = sectorStatus();
      continue;
    }
    LHCb::STChannelID chan = stripToChan(iStrip);
    if (beetleStatus(chan) != DeSTSector::OK){
      vec[iStrip - 1] = beetleStatus(chan);
      continue;
    }
    DeSTSector::StatusMap::const_iterator iter = m_stripStatus.find(iStrip);
    if (iter != m_stripStatus.end()){
      vec[iStrip - 1] = iter->second;
    }
    else{
      vec[iStrip - 1] = DeSTSector::OK;
    }
  } // nStrip
  return vec;
}

inline LHCb::STChannelID DeSTSector::stripToChan(const unsigned int strip) const{
  return isStrip(strip) ? LHCb::STChannelID(elementID().type(),elementID().station(),
                                      elementID().layer(),elementID().detRegion(),
                                      elementID().sector(), strip)
                        : LHCb::STChannelID(0);
}

inline const DeSTSector::Sensors& DeSTSector::sensors() const{
  return m_sensors;
}

inline DeSTSensor* DeSTSector::middleSensor() const {
  return m_sensors[m_sensors.size()/2u];
}

inline bool DeSTSector::xInverted() const {
  return middleSensor()->xInverted();
}

inline bool DeSTSector::yInverted() const {
  return middleSensor()->yInverted();
}

/** ouput operator for class DeSTSector
 *  @see DeSTSector
 *  @see MsgStream
 *  @param os      reference to STL output stream
 *  @param aSector reference to DeSTSector object
 */
inline std::ostream& operator<<( std::ostream& os , const DeSTSector* aSector )
{ return aSector->printOut( os ); }


/** ouput operator for class DeSTSector
 *  @see DeSTSector
 *  @see MsgStream
 *  @param os      reference to MsgStream output stream
 *  @param aSector reference to DeSTSector object
 */
inline MsgStream& operator<<( MsgStream& os , const DeSTSector* aSector )
{ return aSector->printOut( os ); }

inline const std::string& DeSTSector::nickname() const{
  return m_nickname;
}

inline const Condition* DeSTSector::statusCondition() const{
  return condition(m_statusString);
}

inline const Condition* DeSTSector::noiseCondition() const{
  return condition(m_noiseString);
}

inline double DeSTSector::measEff() const {
  return m_measEff;
}
#endif // _DeSTSector_H
