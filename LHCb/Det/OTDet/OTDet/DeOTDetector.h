// $Id: DeOTDetector.h,v 1.3 2002-05-29 12:15:36 cattanem Exp $
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

static const CLID& CLID_DeOTDetector = 8999;

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

  unsigned int numStation()  { return m_numStation; }

  std::vector<OTLayer*> allLayers() const { return m_layers; }

private:

  /// data members
  unsigned int m_numStation;
  unsigned int m_FirstOTStation;
  std::vector<OTLayer*> m_layers;
};

#endif  // OTDET_DEOTDETECTOR_H
