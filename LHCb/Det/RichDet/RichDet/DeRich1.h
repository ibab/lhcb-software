// $Id: DeRich1.h,v 1.7 2004-07-01 11:02:51 papanest Exp $
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


class DeRich1 : public DeRich {

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

  virtual inline const HepPoint3D & nominalCentreOfCurvature(Rich::Side side) const
  {
    return ( Rich::bottom == side ? m_nominalCentreOfCurvatureBottom :
             m_nominalCentreOfCurvature );
  }

  virtual inline const HepNormal3D & nominalNormal(Rich::Side side) const
  {
    return ( Rich::bottom == side ? m_nominalNormalBottom : m_nominalNormal );
  }

  virtual inline const HepPlane3D & nominalPlane(Rich::Side side) const
  {
    return ( Rich::top == side ? m_nominalPlaneTop : m_nominalPlaneBottom );
  }

  inline virtual Rich::Side side( const HepPoint3D & point) const 
  {
    return ( point.y() >= 0.0 ? Rich::top : Rich::bottom );
  }
  
private:

  HepPlane3D m_nominalPlaneTop;
  HepPlane3D m_nominalPlaneBottom;

  HepPoint3D  m_nominalCentreOfCurvatureBottom;
  HepNormal3D m_nominalNormalBottom;

};

#endif    // RICHDET_DERICH1_H
