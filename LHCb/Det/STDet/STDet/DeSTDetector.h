#ifndef _DeSTDetector_H_
#define _DeSTDetector_H_

#include <string>
#include <vector>

#include "Kernel/STChannelID.h"
#include "DetDesc/DetectorElement.h"

/** @class DeSTDetector DeSTDetector.h "STDet/DeSTDetector.h"
 *
 *  Base class for TT and IT Detector Elements
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

class DeSTStation;
class DeSTSector;
class DeSTLayer;

namespace DeSTDetLocation {


  /// IT location in transient detector store
  static const std::string& TT = "/dd/Structure/LHCb/BeforeMagnetRegion/TT";

  /// TT location in transient detector store
  static const std::string& IT = "/dd/Structure/LHCb/AfterMagnetRegion/T/IT";

  /// "short cut" if you know the type, ie TT or IT
  const std::string& location(const std::string& type);

  /// type if you know the location
  unsigned int detType(const std::string& type);

};

class DeSTDetector : public DetectorElement  {

public:

  /** Constructor */
  DeSTDetector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeSTDetector(); 
 
  /** initialization method 
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** @return number of first station */
  unsigned int firstStation() const; 

  /** @return number of last station */
  unsigned int lastStation() const; 

  /** @return number of stations */
  unsigned int nStation() const; 

  /**  locate the station based on a channel id
  @return  station */
  DeSTStation* findStation(const LHCb::STChannelID aChannel);     

  /** locate station based on a point  
  @return station */
  DeSTStation* findStation(const Gaudi::XYZPoint & point) ;     
    
  /** check contains channel
  *  @param channel
  *  @return bool
  */
  bool contains(const LHCb::STChannelID aChannel) const;

  /** @return detector pitch */
  double pitch() const;
 
  /** vector of stattions
  * @return vector of stations
  */
  const std::vector<DeSTStation*> stations() const;

  /** flat vector of sectors
  * @return vector of sectors
  */
  const std::vector<DeSTSector*> sectors() const;

  /** flat vector of layers
  * @return vector of layers
  */
  const std::vector<DeSTLayer*> layers() const;


  /** 
  *  short cut to pick up the wafer corresponding to x,y,z
  * @param point in global frame  
  * @return sector 
  */
  virtual DeSTSector* findSector(const Gaudi::XYZPoint& aPoint) = 0; 

  /** 
  *  short cut to pick up the wafer corresponding to a channel
  * @param channel 
  * @return sector 
  */
  virtual DeSTSector* findSector(const LHCb::STChannelID aChannel) = 0; 

  /** check channel number is valid */
  bool isValid(const LHCb::STChannelID aChannel);

  /** get the next channel left */
  LHCb::STChannelID nextLeft(const LHCb::STChannelID testChan) const;

  /** get the next channel right */
  LHCb::STChannelID nextRight(const LHCb::STChannelID testChan) const;

  /** get the trajectory 
   @return trajectory
  */
  void trajectory(const LHCb::STChannelID& aChan);

  /** get the number of strips in detector*/
  unsigned int nStrip() const; 

  /** get the number of strips in a sector*/
  unsigned int nStripPerSector() const; 
 
protected:

  /** set the first Station number */
  void setFirstStation(const unsigned int iStation);

  /** set the detector pitch */
  void setPitch(const double pitch);

  /** set the strip number  */
  void setNstrip(const unsigned int nStrip);
 
  std::vector<DeSTStation*> m_stations;

  std::vector<DeSTSector*> m_sectors;

  std::vector<DeSTLayer*> m_layers;

private:

  bool isValidStrip(const unsigned int aStrip) const;

  unsigned int m_firstStation;
  unsigned int m_nStripPerSector;
  double m_pitch;
  
};

inline const std::string& DeSTDetLocation::location(const std::string& type){
  return (type =="TT" ? DeSTDetLocation::TT : DeSTDetLocation::IT);
}

inline unsigned int DeSTDetLocation::detType(const std::string& type){
  return (type =="TT" ? LHCb::STChannelID::typeTT : LHCb::STChannelID::typeIT );
}

#include "STDet/DeSTSector.h"
#include "STDet/DeSTStation.h"
#include "STDet/DeSTLayer.h"

inline unsigned int DeSTDetector::firstStation() const{
  return m_firstStation;
}

inline unsigned int DeSTDetector::lastStation() const{
  return m_firstStation + m_stations.size();
}

inline unsigned int DeSTDetector::nStation() const{
  return m_stations.size();
}

inline bool DeSTDetector::contains(const LHCb::STChannelID aChannel) const{
  return ((aChannel.station() >= firstStation())
	  &&(aChannel.station() < lastStation()));
}

inline double DeSTDetector::pitch() const{
  return m_pitch;
}

inline void DeSTDetector::setPitch(const double pitch) {
  m_pitch = pitch;
}

inline unsigned int DeSTDetector::nStrip() const{
  return m_nStripPerSector*m_sectors.size();
}

inline unsigned int DeSTDetector::nStripPerSector() const{
  return m_nStripPerSector*m_sectors.size();
}

inline void DeSTDetector::setNstrip(const unsigned int nStrip) {
  m_nStripPerSector = nStrip;
}

inline const std::vector<DeSTStation*> DeSTDetector::stations() const{
  return m_stations;
}

inline const std::vector<DeSTSector*> DeSTDetector::sectors() const{
  return m_sectors;
}

inline const std::vector<DeSTLayer*> DeSTDetector::layers() const{
  return m_layers;
}

inline bool DeSTDetector::isValid(const LHCb::STChannelID aChannel) {
  DeSTSector* aSector = findSector(aChannel);
  return (aSector == 0 ? false : aChannel.strip() > 0 && aChannel.strip() <= aSector->nStrip() );   
}

inline bool  DeSTDetector::isValidStrip(const unsigned int iStrip) const{
  return(iStrip < m_nStripPerSector);
} 

inline LHCb::STChannelID DeSTDetector::nextLeft(const LHCb::STChannelID aChannel) const{

  LHCb::STChannelID testChan(aChannel.type(),
                       aChannel.station(),
                       aChannel.layer(), 
                       aChannel.detRegion(),
                       aChannel.sector(), 
                       aChannel.strip() - 1u);

  return (isValidStrip(aChannel) == true ? testChan : LHCb::STChannelID(0u,0u,0u,0u,0u,0u)); 
}

inline LHCb::STChannelID DeSTDetector::nextRight(const LHCb::STChannelID aChannel) const{

  LHCb::STChannelID testChan(aChannel.type(),
                       aChannel.station(),
                       aChannel.layer(), 
                       aChannel.detRegion(),
                       aChannel.sector(), 
                       aChannel.strip() + 1u);

  return (isValidStrip(aChannel) == true ? testChan : LHCb::STChannelID(0u,0u,0u,0u,0u,0u)); 
}

inline void DeSTDetector::trajectory(const LHCb::STChannelID& aChan) {
  DeSTSector* aSector = findSector(aChan);
  if (aSector != 0){
    aSector->trajectory(aChan);
  }
  return;  
}

#endif // _DeSTDetector_H








