// $Id: DeOTStation.h,v 1.1 2003-06-11 11:49:36 cattanem Exp $
#ifndef OTDET_DEOTSTATION_H
#define OTDET_DEOTSTATION_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// Kernel
#include "Kernel/OTChannelID.h"

// OTDet
#include "OTDet/DeOTLayer.h"

/** @class DeOTStation DeOTStation.h "OTDet/DeOTStation.h"
 *
 *  This is the detector element class for a Outer Tracker Station.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

static const CLID& CLID_DeOTStation = 8102;

class DeOTStation : public DetectorElement {

public:
  
  /// Constructor
  DeOTStation ( const std::string& name    = "" ) ;
  
  /// Destructor
  ~DeOTStation () ;
  
  /// object identification
  const CLID& clID () const ;
  /// object identification
  static const CLID& classID () { return CLID_DeOTStation ; }
  
  /// initialization method 
  virtual StatusCode initialize();

  /// get stationID
  unsigned int stationID() const { return m_stationID; };
  
  /// return the layer for a given layerID
  DeOTLayer* layer(unsigned int layerID) const;

  /// return the layer for a given 3D point
  DeOTLayer* layer(const HepPoint3D& point) const;

  /// check if point is inside volume
  bool isInside(const HepPoint3D& point) const;

  /// get the vector of all OT layers
  std::vector<DeOTLayer*>& layers() { return m_layers; }

  /// get the vector of all OT modules
  const std::vector<DeOTLayer*>& layers() const { return m_layers; }

private:

  unsigned int m_stationID;         ///< station ID number
  double m_z;                       ///< z of the station
  double m_zSize;                   ///< size in z of the station
  std::vector<DeOTLayer*> m_layers; ///< vector of layers

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline bool DeOTStation::isInside(const HepPoint3D& point) const
{
  return fabs(point.z()-m_z) < 0.5 * m_zSize;
}

#endif  // OTDET_DEOTSTATION_H
