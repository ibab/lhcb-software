// $Id: DeOTDetector.h,v 1.8 2002-08-07 15:38:25 jvantilb Exp $
#ifndef OTDET_DEOTDETECTOR_H
#define OTDET_DEOTDETECTOR_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// OTDet
#include "OTDet/OTLayer.h"

// Kernel
#include "Kernel/OTChannelID.h"


/** @class DeOTDetector DeOTDetector.h "OTDet/DeOTDetector.h"
 *
 *  This is the detector element class for the Outer Tracker. It 
 *  is able to get the geometry from the XML. The geometry parameters are
 *  stored inside OTLayers. This detector element is used by the OT 
 *  digitization and the track reconstruction.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   26-05-2002
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
  OTLayer* layer(const double z) const;

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

  /// get the number of tracker stations
  unsigned int numStations()  { return m_numStations; }

  /// get the first station with OT technology
  unsigned int firstOTStation()  { return m_firstOTStation; }

  /// get the vector of all OT layers
  std::vector<OTLayer*> allLayers() const { return m_layers; }

private:

  unsigned int m_numStations;     ///< number of stations
  unsigned int m_firstOTStation;  ///< first OT station
  std::vector<OTLayer*> m_layers; ///< vector of layers containing geometry
};

#endif  // OTDET_DEOTDETECTOR_H
