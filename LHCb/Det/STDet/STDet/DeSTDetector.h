#ifndef _DeSTDetector_H_
#define _DeSTDetector_H_

#include <vector>
#include <string>

#include "DetDesc/DetectorElement.h"

/** @class DeSTDetector DeSTDetector.h "STDet/DeSTDetector.h"
 *
 *  ST detector element class.
 *
 *  @author Yuehong Xie Yuehong.Xie@cern.ch
 */


class STDetectionStation;
class STDetectionLayer;
class ITChannelID;

namespace DeSTDetectorLocation{
  static const std::string& Default = "/dd/Structure/LHCb/IT";
};

static const CLID& CLID_DeSTDetector=9999;

class DeSTDetector : public DetectorElement  {

public:
  // Constructor: 
  DeSTDetector ( const std::string& name = "" ) ;

  // Destructor
  virtual ~DeSTDetector(); 

  // object identification
  const CLID& clID () const ;
  // object identification
  static const CLID& classID () { return CLID_DeSTDetector ; }
  
  // initialization method
  virtual StatusCode initialize();
  
  // return the station from a station ID 
  STDetectionStation* station(const unsigned int stationID) const;
  
  // put a station into station vector
  void setNextStation(STDetectionStation* station);
  
  // get the number of stations
  unsigned int numStations() const;

  // number that are TT
  unsigned int nTT() const;
  
  // return the station from z
  STDetectionStation* stationByZ(const double z) const;
  
  // return the layer from z
  STDetectionLayer* layerByZ(const double z) const;

  // return the layer from layer ID
  STDetectionLayer* layer(const ITChannelID aChannel) const;

  ITChannelID nextChannelRight(const ITChannelID aChannel) const;
  ITChannelID nextChannelLeft(const ITChannelID aChannel) const;

  std::vector<double> zPositions(std::string aType) const; 
  std::vector<double> zPositions(const std::vector<std::string >& typesCont) const;

private:

  unsigned int m_nTT;
  std::vector<STDetectionStation*> m_Stations;

};

inline unsigned int DeSTDetector::numStations() const { 
  return m_Stations.size(); 
}

#endif








