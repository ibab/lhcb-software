// $Id: DeRich.h,v 1.4 2004-07-01 11:02:51 papanest Exp $
#ifndef DERICH_H
#define DERICH_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

// LHCbKernel
#include "Kernel/RichSide.h"

#include "RichDet/RichMirrorSegPosition.h"

/** @class DeRich DeRich.h
 *
 * This is the definition of the Rich detector class
 *
 * @author Antonis Papanestis
 */

// Namespace for location in TDS
namespace DeRichLocation {
  static const std::string& Rich1 = "/dd/Structure/LHCb/Rich1";
  static const std::string& Rich2 = "/dd/Structure/LHCb/Rich2";
}

class DeRich: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeRich();

  /**
   * Default destructor
   */
  virtual ~DeRich();

  /**
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize();

  virtual const HepPoint3D & nominalCentreOfCurvature(Rich::Side side) const = 0;

  virtual const HepNormal3D & nominalNormal(Rich::Side side) const = 0;

  virtual const HepPlane3D & nominalPlane(Rich::Side side) const = 0;

  virtual Rich::Side side( const HepPoint3D & point ) const = 0;

  inline virtual double sphMirrorRadius() const {
    return m_sphMirrorRadius;
  }

  /**
   * Method to find the row/column of a spherical mirror segment.  It can be used to 
   * test if the mirror segment is at the edge or not
   * @return row/column
   */
  virtual RichMirrorSegPosition sphMirrorSegPos( int mirrorNumber );

  /**
   * Method to find the row/column of a flat mirror segment.  It can be used to 
   * test if the mirror segment is at the edge or not
   * @return row/column
   */
  virtual RichMirrorSegPosition flatMirrorSegPos( int mirrorNumber );

  /**
   * Method to test if the user parameter vector is in the xml description
   * @return true/false
   */
  bool hasParamVector( std::string vectorName );
  
  /**
   * Method to test if the user parameter is in the xml description
   * @return true/false
   */
  bool hasParam( std::string paramName );
  
protected:  

  double m_sphMirrorRadius; ///< The nominal radius of the spherical mirror

  HepPoint3D  m_nominalCentreOfCurvature; ///< of the spherical mirror

  HepNormal3D m_nominalNormal; ///< The nominal normal vector of the flat mirror

  typedef std::vector<std::string> strings;

  strings m_vectorNames;  ///< the names of the user parameter vectors
  strings m_paramNames;   ///< the names of the user parameters
  
  int m_sphMirrorSegRows;  ///< number of spherical mirror rows
  int m_sphMirrorSegCols;  ///< number of spherical mirror columns
  int m_flatMirrorSegRows; ///< number of flat mirror rows
  int m_flatMirrorSegCols; ///< number of flat mirror columns
  
  bool m_positionInfo;     ///< flag to test if the xml supports mirror position info

};

#endif    // DERICH_H
