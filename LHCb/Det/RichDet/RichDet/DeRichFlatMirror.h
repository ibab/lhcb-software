// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/RichDet/RichDet/DeRichFlatMirror.h,v 1.1 2002-07-16 16:02:35 papanest Exp $

#ifndef DERICHFLATMIRROR_H
#define DERICHFLATMIRROR_H 1


// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Geometry/Plane3D.h"

#include "DetDesc/DetectorElement.h"


// External declarations
extern const CLID& CLID_DeRichFlatMirror;


/** @class DeRichFlatMirror DeRichFlatMirror.h
 *
 * This is the definition of the Rich Flat Mirror detector class
 *
 * @author Antonis Papanestis
 */
class DeRichFlatMirror: public DetectorElement {

public:
  /**
   * Default constructor for this class
   */
  DeRichFlatMirror();

  /**
   * Default destructor
   */
  virtual ~DeRichFlatMirror();

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

  /**
   * Retrieves the normal vector of this mirror
   * @return the normal vector of this mirror as a HepVector3D
   */
  inline HepNormal3D normalVector(){
    return m_normalVector;
  }
  
  /**
   * Retrieves the centre of this flat mirror
   * @return the centre of this flat mirror as a HepPoint3D
   */
  inline HepPoint3D mirrorCentre(){
    return m_mirrorCentre;
  }
  
  /**
   * Retrieves the plane of this flat mirror
   * @return the plane of this flat mirror as a HepPlane3D
   */
  inline HepPlane3D mirrorPlane(){
    return m_mirrorPlane;
  }
  
  /**
   * Retrieves the number of this flat mirror
   * @return the number of this flat mirror (0-39)
   */
  inline int mirrorNumber(){
    return m_mirrorNumber;
  }
  

private:

  HepVector3D m_normalVector;
  HepPoint3D  m_mirrorCentre;
  HepPlane3D  m_mirrorPlane;

  double m_alignmentConstantX;
  double m_alignmentConstantY;

  int m_mirrorNumber;
  
};

#endif    // DERICHFLATMIRROR_H
