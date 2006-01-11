// $Id: DeOTQuarter.h,v 1.3 2006-01-11 09:29:15 janos Exp $
#ifndef OTDET_DEOTQUARTER_H
#define OTDET_DEOTQUARTER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// Kernel
#include "Kernel/OTChannelID.h"

// OTDet
#include "OTDet/DeOTModule.h"

/** @class DeOTQuarter DeOTQuarter.h "OTDet/DeOTQuarter.h"
 *
 *  This is the detector element class for a Outer Tracker Quarter.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

static const CLID& CLID_DeOTQuarter = 8104;

class DeOTQuarter : public DetectorElement {

public:
  
  /// Constructor
  DeOTQuarter ( const std::string& name    = "" ) ;
  
  /// Destructor
  ~DeOTQuarter () ;
  
  /// object identification
  const CLID& clID () const ;
  /// object identification
  static const CLID& classID () { return CLID_DeOTQuarter ; }
  
  /// initialization method 
  virtual StatusCode initialize();

  /// get quarterID
  unsigned int quarterID() const { return m_quarterID; };

  /// get the stereo angle for this layer
  double stereoAngle() const { return m_stereoAngle; };

  /// return the module for a given moduleID
  DeOTModule* module(unsigned int moduleID) const;

  /// return the module for a given 3D point
  DeOTModule* module(const Gaudi::XYZPoint& point) const;

  /// check if point is inside volume
  bool isInside(const Gaudi::XYZPoint& point) const;

  /// get the vector of all OT modules
  std::vector<DeOTModule*>& modules() { return m_modules; }

  /// get the vector of all OT modules
  const std::vector<DeOTModule*>& modules() const { return m_modules; }

private:

  unsigned int m_quarterID;           ///< quarter ID number
  double m_stereoAngle;               ///< layer stereo angle 
  double m_xMin;                      ///< Minimum x of the cover
  double m_yMin;                      ///< Minimum y of the cover
  double m_zMin;                      ///< Minimum z of the cover
  double m_xMax;                      ///< Maximum x of the cover
  double m_yMax;                      ///< Maximum y of the cover
  double m_zMax;                      ///< Maximum z of the cover
  std::vector<DeOTModule*> m_modules; ///< vector of modules

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline bool DeOTQuarter::isInside(const Gaudi::XYZPoint& point) const
{
  // only check x and y (z should have been checked before in layer)
  if ( point.x() > m_xMin && point.x() < m_xMax &&
       point.y() > m_yMin && point.y() < m_yMax && 
       point.z() > m_zMin && point.z() < m_zMax) return true;
  return false;
}

#endif  // OTDET_DEOTQUARTER_H
