// $Id: DeRich.h,v 1.1 2003-11-21 17:23:24 papanest Exp $
#ifndef DERICH_H
#define DERICH_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

// RichKernel
#include "RichKernel/RichSide.h"

/** @class DeRich DeRich.h
 *
 * This is the definition of the Rich detector class
 *
 * @author Antonis Papanestis
 */
class DeRich: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRich()
  {}

  /**
   * Default destructor
   */
  virtual ~DeRich() 
  {}
  

  /**
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize() = 0;


  virtual HepPoint3D nominalCentreOfCurvature(Rich::Side side) const = 0;

  virtual HepNormal3D nominalNormal(Rich::Side side) const = 0;

  virtual HepPlane3D nominalPlane(Rich::Side side) const = 0;

  virtual Rich::Side side(HepPoint3D point) const = 0;

  inline virtual double sphMirrorRadius() const {
    return m_sphMirrorRadius;
  }
  
  inline virtual double sphMirrorMaxX() const {
    return m_sphMirrorMaxX;
  }
  
  inline virtual double sphMirrorMaxY() const {
    return m_sphMirrorMaxY;
  }
  
protected:
  
  double m_sphMirrorRadius;
  double m_sphMirrorMaxX;
  double m_sphMirrorMaxY;  

  HepPoint3D  m_nominalCentreOfCurvature;
  HepNormal3D m_nominalNormal;

};

#endif    // DERICH_H
