#ifndef _DeITDetector_H_
#define _DeITDetector_H_

#include <vector>
#include <string>

#include "DetDesc/DetectorElement.h"

/** @class DeITDetector DeITDetector.h "ITDet/DeITDetector.h"
 *
 *  IT detector element class.
 *
 *  @author Yuehong Xie Yuehong.Xie@cern.ch
 */


class ITDetectionStation;
class ITDetectionLayer;
class ITChannelID;

namespace DeITDetectorLocation{
  static const std::string& Default = "/dd/Structure/LHCb/IT";
};

static const CLID& CLID_DeITDetector=9999;

class DeITDetector : public DetectorElement  {

public:
  // Constructor: 
  DeITDetector ( const std::string& name = "" ) ;

  // Destructor
  virtual ~DeITDetector(); 

  // object identification
  const CLID& clID () const ;
  // object identification
  static const CLID& classID () { return CLID_DeITDetector ; }
  
  // initialization method
  virtual StatusCode initialize();
  
  // return the station from a station ID 
  ITDetectionStation* station(const unsigned int stationID) const;
  
  // put a station into station vector
  void setNextStation(ITDetectionStation* station);
  
  // get the number of stations
  unsigned int numStations() const;
  
  // return the station from z
  ITDetectionStation* stationByZ(const double z) const;
  
  // return the layer from z
  ITDetectionLayer* layerByZ(const double z) const;

  // return the layer from layer ID
  ITDetectionLayer* layer(const ITChannelID aChannel) const;

  ITChannelID nextChannelRight(const ITChannelID aChannel) const;
  ITChannelID nextChannelLeft(const ITChannelID aChannel) const;
  
private:

  std::vector<ITDetectionStation*> m_Stations;

};

inline unsigned int DeITDetector::numStations() const { 
  return m_Stations.size(); 
}

#endif








