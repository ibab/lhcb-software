// $Id: DeOTDetector.h,v 1.16 2004-12-10 08:11:37 jnardull Exp $
#ifndef OTDET_DEOTDETECTOR_H
#define OTDET_DEOTDETECTOR_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// OTDet
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

// Kernel
#include "Kernel/OTChannelID.h"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

/** @class DeOTDetector DeOTDetector.h "OTDet/DeOTDetector.h"
 *
 *  This is the detector element class for the Outer Tracker. It 
 *  is able to get the geometry from the XML. Many geometry parameters are
 *  stored inside it grand-children, which are derived from DetectorElement
 *  as well: DeOTModule. DeOTDetector is used by the OT digitization and 
 *  the track reconstruction.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   26-05-2002
 */

namespace DeOTDetectorLocation{
  static const std::string& Default = "/dd/Structure/LHCb/OT";
};

static const CLID& CLID_DeOTDetector = 8101;

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

  /// get the transformation matrix - Angles related to beam tilt angle
  StatusCode getAngles() const;

  /// Find the channels and the distances from the MCHits
  StatusCode calculateHits(const HepPoint3D& entryPoint, 
                           const HepPoint3D& exitPoint,
                           std::vector<OTChannelID>& channels,
                           std::vector<double>& driftDistances );

  /// return the station for a given stationID
  DeOTStation* station(unsigned int stationID) const;
  
  /// return the layer for a given channelID
  DeOTLayer* layer(OTChannelID aChannel) const;

  /// return the quarter for a given channelID
  DeOTQuarter* quarter(OTChannelID aChannel) const;

  /// return the module for a given channel ID
  DeOTModule* module(OTChannelID aChannel) const;

  /// return the module for a given point
  DeOTModule* module(const HepPoint3D& point) const;

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

  /// get the maximum # channels in one module
  unsigned int nMaxChanInModule() { return m_nMaxChanInModule; }

  /// get the vector of all OT modules
  std::vector<DeOTModule*>& modules() { return m_modules; }

  /// get the vector of all OT modules
  const std::vector<DeOTModule*>& modules() const { return m_modules; }

  /// get the total number of readout channels in the OT
  unsigned int nChannels()  { return m_nChannels; }

private:

  double m_resolution;            ///< straw resolution
  double m_resolutionCor;         ///< magn. field correction on resolution
  double m_propagationDelay;      ///< speed of propagation along wire
  double m_maxDriftTime;          ///< maximum drift time
  double m_maxDriftTimeCor;       ///< magn. correction on maximum drift time
  double m_cellRadius;            ///< cell radius

  unsigned int m_numStations;     ///< number of stations
  unsigned int m_firstOTStation;  ///< first OT station
  std::vector<DeOTStation*> m_stations;///< vector of stations
  std::vector<DeOTLayer*> m_layers;    ///< vector of layers
  std::vector<DeOTQuarter*> m_quarters;///< vector of layers
  std::vector<DeOTModule*> m_modules; ///< vector of modules containing geometry
  unsigned int m_nChannels;       ///< total number of channels in OT
  unsigned int m_nMaxChanInModule;///< the maximum # channels in 1 module
  
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

inline double DeOTDetector::resolution(const double by) const
{  
  // Calculate resolution
  // The form is p1+p2*B^2 (empirical fit to the testbeam results)
  // p1, p2 vary for different gas mixes
  // The parameterization is only valid for Bx<1.4T (or there abouts)

  return ( m_resolution - (m_resolutionCor * by * by) );
}

inline double DeOTDetector::maxDriftTimeFunc(const double by) const
{
  // Calculate max drift as function of B 
  // The form is p1+p2*B^2 (empirical fit to the testbeam results)
  // p1, p2 vary for different gas mixes
  // The parameterization is only valid for Bx<1.4T (or there abouts)

  return (m_maxDriftTime + (m_maxDriftTimeCor * by * by) );
}


#endif  // OTDET_DEOTDETECTOR_H
