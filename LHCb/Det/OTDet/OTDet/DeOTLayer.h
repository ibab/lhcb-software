// $Id: DeOTLayer.h,v 1.4 2004-06-24 09:48:34 jnardull Exp $
#ifndef OTDET_DEOTLAYER_H
#define OTDET_DEOTLAYER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// Kernel
#include "Kernel/OTChannelID.h"

// OTDet
#include "OTDet/DeOTQuarter.h"

/** @class DeOTLayer DeOTLayer.h "OTDet/DeOTLayer.h"
 *
 *  This is the detector element class for an Outer Tracker Layer.
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

  /// return the quarter for a given quarterID
  DeOTQuarter* quarter(unsigned int quarterID) const;

  /// return the quarter for a given 3D point (depreciated !)
  DeOTQuarter* quarter(const HepPoint3D& point) const;

  /// return the module for a given 3D point
  DeOTModule* module(const HepPoint3D& point) const;

  /// check if point is inside volume
  bool isInside(const HepPoint3D& point) const;

  /// get the vector of all OT quarter
  std::vector<DeOTQuarter*>& quarters() { return m_quarters; }

  /// get the vector of all OT quarters
  const std::vector<DeOTQuarter*>& quarters() const { return m_quarters; }

private:

  unsigned int m_layerID;              ///< layer ID number
  double m_stereoAngle;                ///< layer stereo angle 
  std::vector<DeOTQuarter*> m_quarters;///< vector of quarters

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline bool DeOTLayer::isInside(const HepPoint3D& point) const
{
  return this->geometry()->isInside(point);
}

#endif  // OTDET_DEOTLAYER_H
