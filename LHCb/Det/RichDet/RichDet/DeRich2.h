// $Id: DeRich2.h,v 1.5 2003-11-21 22:46:10 jonesc Exp $
#ifndef DERICH2_H
#define DERICH2_H 1

// local
#include "RichDet/DeRich.h"

// External declarations
extern const CLID& CLID_DERich2;


/** @class DeRich2 DeRich2.h
 *
 * This is the definition of the Rich2 detector class
 *
 * @author Antonis Papanestis
 */
class DeRich2: public DeRich {

public:
  /**
   * Constructor for this class
   */
  DeRich2();

  /**
   * Default destructor
   */
  virtual ~DeRich2();

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

  inline virtual Rich::Side side(HepPoint3D point) const
  {
    if( point.x() >= 0.0 )
      return Rich::left;
    else
      return Rich::right;
  }


private:

  HepPlane3D m_nominalPlaneLeft;
  HepPlane3D m_nominalPlaneRight;

};

#endif    // DERICH2_H
