// $Id: DeOTLayer.h,v 1.2 2003-06-16 08:52:07 cattanem Exp $
#ifndef OTDET_DEOTLAYER_H
#define OTDET_DEOTLAYER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// Kernel
#include "Kernel/OTChannelID.h"

// OTDet
#include "OTDet/DeOTModule.h"

/** @class DeOTLayer DeOTLayer.h "OTDet/DeOTLayer.h"
 *
 *  This is the detector element class for a Outer Tracker Layer.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

static const CLID& CLID_DeOTLayer = 8103;

class DeOTLayer : public DetectorElement {

public:
  
  /// Constructor
  DeOTLayer ( const std::string& name    = "" ) ;
  
  /// Destructor
  ~DeOTLayer () ;
  
  /// object identification
  const CLID& clID () const ;
  /// object identification
  static const CLID& classID () { return CLID_DeOTLayer ; }
  
  /// initialization method 
  virtual StatusCode initialize();

  /// get layerID
  unsigned int layerID() const { return m_layerID; };

  /// get the stereo angle for this layer
  double stereoAngle() const { return m_stereoAngle; };

  /// return the module for a given moduleID
  DeOTModule* module(unsigned int moduleID) const;

  /// return the module for a given 3D point
  DeOTModule* module(const HepPoint3D& point) const;

  /// check if point is inside volume
  bool isInside(const HepPoint3D& point) const;

  /// get the vector of all OT modules
  std::vector<DeOTModule*>& modules() { return m_modules; }

  /// get the vector of all OT modules
  const std::vector<DeOTModule*>& modules() const { return m_modules; }

private:

  unsigned int m_layerID;             ///< layer ID number
  double m_stereoAngle;               ///< layer stereo angle 
  double m_z;                         ///< z of the layer
  double m_zSize;                     ///< size in z of the layer
  std::vector<DeOTModule*> m_modules; ///< vector of modules

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline bool DeOTLayer::isInside(const HepPoint3D& point) const
{
  return fabs(point.z()-m_z) < 0.5 * m_zSize;
}

#endif  // OTDET_DEOTLAYER_H
