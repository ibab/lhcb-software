// $Id: DeRich1.h,v 1.4 2003-11-21 17:23:24 papanest Exp $
#ifndef RICHDET_DERICH1_H
#define RICHDET_DERICH1_H 1

// local
#include "RichDet/DeRich.h"


// External declarations
extern const CLID& CLID_DERich1;

/** @class DeRich1 DeRich1.h
 *
 * This is the definition of the Rich1 detector class
 *
 * @author Antonis Papanestis
 */

class DeRich1: public DeRich {

public:

  /**
   * Constructor for this class
   */
  DeRich1();

  /**
   * Default destructor
   */
  virtual ~DeRich1();

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


  virtual HepPoint3D nominalCentreOfCurvature(Rich::Side side) const;

  virtual HepNormal3D nominalNormal(Rich::Side side) const;

  virtual HepPlane3D nominalPlane(Rich::Side side) const;

  inline virtual Rich::Side side(HepPoint3D point) const {
    if( point.y() >= 0.0 )
      return Rich::top;
    else
      return Rich::bottom;
  }
  
  
private:

  HepPlane3D m_nominalPlaneTop;
  HepPlane3D m_nominalPlaneBottom;

};
 
#endif    // RICHDET_DERICH1_H
