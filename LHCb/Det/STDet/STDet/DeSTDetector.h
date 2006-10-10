// $Id: DeSTDetector.h,v 1.18 2006-10-10 14:21:58 mneedham Exp $
#ifndef _DeSTDetector_H_
#define _DeSTDetector_H_

#include <string>
#include <vector>

#include "Kernel/STChannelID.h"
#include "Kernel/LHCbID.h"

#include "DetDesc/DetectorElement.h"

#include "GaudiKernel/VectorMap.h"

/** @class DeSTDetector DeSTDetector.h "STDet/DeSTDetector.h"
 *
 *  Base class for TT and IT Detector Elements
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

class DeSTStation;
class DeSTSector;
class DeSTLayer;

namespace LHCb{
  class Trajectory;
}

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

  typedef std::vector<DeSTSector*> Sectors;
  typedef std::vector<DeSTLayer*> Layers;
  typedef std::vector<DeSTStation*> Stations;

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

  /** Implementation of sensitive volume identifier for a given point in the 
      global reference frame. This is the sensor number defined in the xml.
  */
  const int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

  /**  locate the station based on a channel id
  @return  station */
  DeSTStation* findStation(const LHCb::STChannelID aChannel);     

  /** locate station based on a point  
  @return station */
  DeSTStation* findStation(const Gaudi::XYZPoint & point) ;     

  /**  locate the layer based on a channel id
  @return  layer */
  DeSTLayer* findLayer(const LHCb::STChannelID aChannel);     

  /** locate layer based on a point 
   *return layer */
  DeSTLayer* findLayer(const Gaudi::XYZPoint& point);
    
  /** check contains channel
  *  @param channel
  *  @return bool
  */
  bool contains(const LHCb::STChannelID aChannel) const;

  /** check channel number is valid */
  bool isValid(const LHCb::STChannelID aChannel);

  /** vector of stattions
  * @return vector of stations
  */
  const Stations& stations() const;

  /** flat vector of sectors
  * @return vector of sectors
  */
  const Sectors& sectors() const;

  /** flat vector of layers
  * @return vector of layers
  */
  const Layers& layers() const;

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
  DeSTSector* findSector(const LHCb::STChannelID aChannel); 

  /** get the next channel left */
  LHCb::STChannelID nextLeft(const LHCb::STChannelID testChan);

  /** get the next channel right */
  LHCb::STChannelID nextRight(const LHCb::STChannelID testChan);

  /** get the trajectory 
   @return trajectory
  */
  std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, const double offset);

  /** get the trajectory representing the first strip
   @return trajectory
  */
  std::auto_ptr<LHCb::Trajectory> trajectoryFirstStrip(const LHCb::LHCbID& id);

  /** get the trajectory representing the last strip
   @return trajectory
  */
  std::auto_ptr<LHCb::Trajectory> trajectoryLastStrip(const LHCb::LHCbID& id);

  /** get the number of strips in detector*/
  unsigned int nStrip() const; 

protected:

  /** set the first Station number */
  void setFirstStation(const unsigned int iStation);

  /** set the strip number  */
  void setNstrip(const unsigned int nStrip);
 
  Stations m_stations;

  Sectors m_sectors;

  Layers m_layers;

  typedef GaudiUtils::VectorMap<unsigned int,DeSTSector*> SectorMap;
  SectorMap m_sMap;

private:


  unsigned int m_firstStation;
  unsigned int m_nStrip;
  
};

inline const std::string& DeSTDetLocation::location(const std::string& type){
  return (type =="TT" ? DeSTDetLocation::TT : DeSTDetLocation::IT);
}

inline unsigned int DeSTDetLocation::detType(const std::string& type){
  return (type =="TT" ? LHCb::STChannelID::typeTT : LHCb::STChannelID::typeIT);
}

#include "STDet/DeSTSector.h"
#include "STDet/DeSTStation.h"
#include "STDet/DeSTLayer.h"

inline unsigned int DeSTDetector::firstStation() const{
  return m_firstStation;
}

inline unsigned int DeSTDetector::lastStation() const{
  return m_firstStation + m_stations.size()-1u;
}

inline unsigned int DeSTDetector::nStation() const{
  return m_stations.size();
}

inline bool DeSTDetector::contains(const LHCb::STChannelID aChannel) const{
  return ((aChannel.station() >= firstStation())
	  &&(aChannel.station() < lastStation()));
}


inline unsigned int DeSTDetector::nStrip() const{
  return m_nStrip;
}

inline void DeSTDetector::setNstrip(const unsigned int nStrip) {
  m_nStrip = nStrip;
}

inline const DeSTDetector::Stations& DeSTDetector::stations() const{
  return m_stations;
}

inline const DeSTDetector::Sectors&  DeSTDetector::sectors() const{
  return m_sectors;
}

inline const DeSTDetector::Layers& DeSTDetector::layers() const{
  return m_layers;
}

inline LHCb::STChannelID DeSTDetector::nextLeft(const LHCb::STChannelID 
                                                aChannel) {
  DeSTSector* aSector = findSector(aChannel);
  return (0 != aSector ? aSector->nextLeft(aChannel) : 
          LHCb::STChannelID(0u,0u,0u,0u,0u,0u) ); 
}

inline LHCb::STChannelID DeSTDetector::nextRight(const LHCb::STChannelID
                                                 aChannel) {
  DeSTSector* aSector = findSector(aChannel);
  return (0 != aSector ? aSector->nextRight(aChannel) : 
          LHCb::STChannelID(0u,0u,0u,0u,0u,0u) ); 
}

inline bool DeSTDetector::isValid(const LHCb::STChannelID aChannel){
  /*
  DeSTDetector::Sectors::iterator iter = m_sectors.begin();
  while((iter != m_sectors.end())&&((*iter)->contains(aChannel) == false)){
    ++iter;
  } // iter
  return (iter != m_sectors.end() ? (*iter)->isStrip(aChannel.strip()) :false);
  */
  DeSTSector* aSector = findSector(aChannel);
  return (aSector != 0 ? aSector->isStrip(aChannel.strip()) : false); 
}

inline DeSTSector* DeSTDetector::findSector(const LHCb::STChannelID aChannel){
 SectorMap::iterator iter = m_sMap.find(aChannel.uniqueSector());
 return (iter != m_sMap.end() ? iter->second : 0);
}

#endif // _DeSTDetector_H








