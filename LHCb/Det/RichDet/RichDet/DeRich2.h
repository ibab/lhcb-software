// $Id: DeRich2.h,v 1.8 2004-07-01 11:02:51 papanest Exp $
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

  virtual inline const HepPoint3D & nominalCentreOfCurvature(Rich::Side side) const
  {
    return ( Rich::right == side ? m_nominalCentreOfCurvatureRight :
             m_nominalCentreOfCurvature );
  }

  virtual inline const HepNormal3D & nominalNormal(Rich::Side side) const
  {
    return ( Rich::right == side ? m_nominalNormalRight : m_nominalNormal );
  }

  virtual inline const HepPlane3D & nominalPlane(Rich::Side side) const
  {
    return ( Rich::left == side ? m_nominalPlaneLeft : m_nominalPlaneRight );
  }

  inline virtual Rich::Side side( const HepPoint3D & point ) const
  {
    return ( point.x() >= 0.0 ? Rich::left : Rich::right );
  }

private:

  HepPlane3D m_nominalPlaneLeft;
  HepPlane3D m_nominalPlaneRight;

  HepPoint3D  m_nominalCentreOfCurvatureRight;
  HepNormal3D m_nominalNormalRight;

};

#endif    // DERICH2_H
