// $Id: DeOTDetector.h,v 1.6 2002-06-10 15:58:11 cattanem Exp $
#ifndef OTDET_DEOTDETECTOR_H
#define OTDET_DEOTDETECTOR_H 1

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "OTDet/OTLayer.h"
#include "Kernel/OTChannelID.h"


/** @class DeOTDetector DeOTDetector.h "OTDet/DeOTDetector.h" 
 *
 *  OT detector element class.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 */

static const CLID& CLID_DeOTDetector = 10100;

class DeOTDetector : public DetectorElement {

public:
  
  /// Constructor
  DeOTDetector ( const std::string& name    = "" ) ;
  
  /// Destructor
  ~DeOTDetector () ;
  
  /// object identification
  const CLID& clID () const ;
  /// object identification
  static const CLID& classID () { return CLID_DeOTDetector ; }
  
  /// initialization method 
  virtual StatusCode initialize(); 

  /// return the closest layer given a z position
  OTLayer* layer(double z) const;

  /// return the layer given a channel ID
  OTLayer* layer(OTChannelID aChannel) const;

  /// return the distance along the wire given a channel and position x,y
  double distanceAlongWire(OTChannelID channelID,
                           double xHit,
                           double yHit) const;

  /// return the channel right from a given channel
  OTChannelID nextChannelRight(OTChannelID aChannel) const;

  /// return the channel left from a given channel
  OTChannelID nextChannelLeft(OTChannelID aChannel) const;

  unsigned int numStations()  { return m_numStations; }

  unsigned int firstOTStation()  { return m_firstOTStation; }

  std::vector<OTLayer*> allLayers() const { return m_layers; }

private:

  /// data members
  unsigned int m_numStations;
  unsigned int m_firstOTStation;
  std::vector<OTLayer*> m_layers;
};

#endif  // OTDET_DEOTDETECTOR_H
