// $Id: DeRich2.h,v 1.3 2003-04-01 13:01:49 jonrob Exp $
#ifndef DERICH2_H
#define DERICH2_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

// RichKernel
#include "RichKernel/RichSide.h"

// External declarations
extern const CLID& CLID_DERich2;


/** @class DeRich2 DeRich2.h
 *
 * This is the definition of the Rich2 detector class
 *
 * @author Antonis Papanestis
 */
class DeRich2: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRich2();

  /**
   * Default destructor
   */
  ~DeRich2();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize();


  HepPoint3D nominalCentreOfCurvature(Rich::Side side) const;

  HepNormal3D nominalNormal(Rich::Side side) const;

  HepPlane3D nominalPlane(Rich::Side side) const;

  inline double sphMirrorRadius() const {
    return m_sphMirrorRadius;
  }
  
  inline double sphMirrorMaxX() const {
    return m_sphMirrorMaxX;
  }
  
  inline double sphMirrorMaxY() const {
    return m_sphMirrorMaxY;
  }
  
private:

  ///
  double m_sphMirrorRadius;
  double m_sphMirrorMaxX;
  double m_sphMirrorMaxY;  

  HepPoint3D  m_nominalCentreOfCurvature;
  HepNormal3D m_nominalNormal;
  HepPlane3D m_nominalPlaneLeft;
  HepPlane3D m_nominalPlaneRight;

};

#endif    // DERICH2_H
