// $Id: DeOTDetector.h,v 1.10 2002-11-11 11:40:39 jvantilb Exp $
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

  /// get the straw resolution
  double resolution() const {return m_resolution;}
  
  /// get the resolution with magn. field correction
  double resolution(const double by) const;

  /// returns the propagation delay (ns/mm)
  double propagationDelay() const {return m_propagationDelay;} ;

  /// Calculate the propagation delay along the wire
  double propagationTime(const OTChannelID aChannel, 
                         const double x, const double y ) const;

  /// Calculate max drift as function of By
  double maxDriftTimeFunc(const double by) const;
  
  /// r-t relation without correction for the magnetic field
  double driftTime(const double driftDist) const;
  
  /// r-t relation with correction for the magnetic field
  double driftTime(const double driftDist, const double by) const;

  /// returns the drift delay without magnetic field correction (ns/mm)
  double driftDelay() const {return m_maxDriftTime/m_cellRadius;}

  /// inverse r-t relation without correction for the magnetic field
  double driftDistance( const double driftTime) const;

  /// inverse r-t relation with correction for the magnetic field
  double driftDistance( const double driftTime, const double by ) const;

  /// get the number of tracker stations
  unsigned int numStations()  { return m_numStations; }

  /// get the first station with OT technology
  unsigned int firstOTStation()  { return m_firstOTStation; }

  /// get the vector of all OT layers
  std::vector<OTLayer*> allLayers() const { return m_layers; }

  /// get the first of all channelIDs
  OTChannelID beginChannel() const;

  /// return the 'end' channel (= 0)
  OTChannelID endChannel() const;

  /// get the next channelID.
  OTChannelID nextChannel(OTChannelID aChannel) const;

private:

  double m_resolution;            ///< straw resolution
  double m_resolutionCor;         ///< magn. field correction on resolution
  double m_propagationDelay;      ///< speed of propagation along wire
  double m_maxDriftTime;          ///< maximum drift time
  double m_maxDriftTimeCor;       ///< magn. correction on maximum drift time
  double m_cellRadius;            ///< cell size

  unsigned int m_numStations;     ///< number of stations
  unsigned int m_firstOTStation;  ///< first OT station
  std::vector<OTLayer*> m_layers; ///< vector of layers containing geometry
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline double DeOTDetector::driftTime(const double driftDist) const
{
  // r-t relation without correction for the magnetic field
  // convert r to t - hack as drift dist can > rCell
  if ( fabs(driftDist) < m_cellRadius ) {
    return ( fabs(driftDist) / m_cellRadius) * m_maxDriftTime;
  } else {
    return m_maxDriftTime;
  }
}

inline double DeOTDetector::driftDistance( const double driftTime) const
{
  // inverse r-t relation without correction for the magnetic field
  return driftTime * m_cellRadius / m_maxDriftTime;
}

inline OTChannelID DeOTDetector::beginChannel() const
{
  return OTChannelID(m_firstOTStation,1,1,1);
}

inline OTChannelID DeOTDetector::endChannel() const
{
  return OTChannelID(0,0,0,0);
}

#endif  // OTDET_DEOTDETECTOR_H
