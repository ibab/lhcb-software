// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/RichDet/RichDet/DeRichSphMirror.h,v 1.2 2002-10-30 11:36:56 papanest Exp $
#ifndef DERICHSPHMIRROR_H
#define DERICHSPHMIRROR_H 1


// Include files
#include "CLHEP/Geometry/Point3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"


// External declarations
extern const CLID& CLID_DeRichSphMirror;


/** @class DeRichSphMirror DeRichSphMirror.h
 *
 * This is the definition of the Rich Spherical Mirror detector class
 *
 * @author Antonis Papanestis
 */
class DeRichSphMirror: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRichSphMirror();

  /**
   * Default destructor
   */
  virtual ~DeRichSphMirror();

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
   * Retrieves the centre of curvarute of this mirror
   * @return the centre of curvature of this mirror as a HepPoint3D
   */
  inline HepPoint3D centreOfCurvature(){
    return m_centreOfCurvature;
  }
  
  /**
   * Retrieves the centre of this spherical mirror
   * @return the centre of this sperical mirror as a HepPoint3D
   */
  //inline HepPoint3D mirrorCentre(){
  //  return m_mirrorCentre;
  //}
  
  /**
   * Retrieves the number of this spherical mirror
   * @return the number of this spherical mirror (0-55)
   */
  inline int mirrorNumber(){
    return m_mirrorNumber;
  }

  /**
   * Checks if the direction intersects with the mirror
   * @return StatusCode
   */
  StatusCode intersects(const HepPoint3D& globalP, const HepVector3D& globalV);

  /**
   * Checks if the direction intersects with the mirror and returns the
   * intersction point
   * @return StatusCode
   */
  StatusCode intersects(const HepPoint3D& globalP, 
                        const HepVector3D& globalV,
                        HepPoint3D& intersectionPoint);

private:

  const ISolid* m_solid;

  HepPoint3D m_centreOfCurvature;
  HepPoint3D m_mirrorCentre;

  double m_alignmentConstantX;
  double m_alignmentConstantY;

  double m_radius;

  int m_mirrorNumber;

};

#endif    // DERICHSPHMIRROR_H
