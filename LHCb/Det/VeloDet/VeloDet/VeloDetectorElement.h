#ifndef VELODETECTORELEMENT_H
#define VELODETECTORELEMENT_H 1

// Include files
#include "GaudiKernel/Kernel.h"
#include "DetDesc/DetectorElement.h"
#include "GaudiKernel/IGeometryInfo.h"

// CLHEP files
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "VeloSubSector.h"

// External declarations
static const CLID CLID_VeloDetectorElement = 4021;

/**
 * Velo detector element.
 * This class specialises the DetectorElement class to contain
 * information which may be common to \f$R\f$ and \f$\phi\f$
 * silicon detectors.
 */

class VeloDetectorElement : public DetectorElement {
  
public:
  
  /// Detector types
  enum Type {R, Phi};

  /// Detector orientation
  enum Orientation {Upstream, Downstream,Unknown};

  /// Constructors
  VeloDetectorElement() : 
    m_orientation(Unknown) {

  }
  VeloDetectorElement(Type type) :
    m_type(type),
    m_orientation(Unknown) {

  }

  /// Destructor
  virtual ~VeloDetectorElement() {

  }

  /// Return detector type (\f$R\f$ or \f$\phi\f$)
  virtual Type type() const {
    return m_type;
  }
  /// Set detector type
  virtual void setType(Type type) {
    m_type = type;
  }

  /// Query detector type: is it an \f$R\f$ detector?
  virtual bool isR() const {
    return m_type==R;
  }

  /// Query detector type: is it a \f$\phi\f$ detector?
  virtual bool isPhi() const {
    return m_type==Phi;
  }

  /// Return detector orientation (upstream or downstream)
  virtual Orientation orientation() const {
    if (m_orientation == Unknown) {
      HepRotation rot = this->geometry()->matrixInv().getRotation();
      Hep3Vector axis;
      HepDouble angle;
      rot.getAngleAxis(angle, axis);
      if (axis.theta()<halfpi) {
	m_orientation = Upstream;
      } else {
	m_orientation = Downstream;
      }
    }
    return m_orientation;
  }
  /// Set detector orientation
  virtual void setOrientation(Orientation orientation) {
    m_orientation = orientation;
  }

  /// Return reference to vector of sub-sector definitions
  std::vector<VeloSubSector>& subSector() { 
    return m_subSector;
  }
  /// Clear vector of sub-sector definitions
  void clearSubSector() {
    m_subSector.clear();
  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloDetectorElement::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloDetectorElement; 
  }

private:

  /// Detector type (\f$R\f$ or \f$\phi\f$)
  Type                       m_type;
  /// Detector orientation (upstream or downstream)
  mutable Orientation        m_orientation;
  /// Definition of sub-sectors on the silicon wafer
  std::vector<VeloSubSector> m_subSector;

};

#endif // VELODETECTORELEMENT_H









