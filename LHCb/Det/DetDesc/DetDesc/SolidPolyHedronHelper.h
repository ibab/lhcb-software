// $Id: SolidPolyHedronHelper.h,v 1.6 2005-01-25 14:09:19 cattanem Exp $ 
// ===========================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef     DETDESC_SOLIDPOLYHEDRONHELPER_H
#define     DETDESC_SOLIDPOLYHEDRONHELPER_H 1
/// STD & STL 
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Plane3D.h"
/// DetDesc 
#include "DetDesc/SolidBase.h"
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidTicks.h"


/** @class SolidPolyHedronHelper SolidPolyHedronHelper.h 
 *
 *  Small helper class to simplify the construction 
 *                        of concrete "polihedron-like" classes
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    18/01/2001
 */

class SolidPolyHedronHelper: virtual public SolidBase
{
  ///
public:
  
  /// useful type for vector of planes 
  typedef std::vector<HepPlane3D>   PLANES   ;
  typedef std::vector<HepPoint3D>   VERTICES ;
  
public: 
  
  /** - Check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual bool isInside ( const HepPoint3D& point ) const ;
  
  /** - Calculate the intersection points("ticks") of the solid objects 
   *    with given line. 
   *  -# Line is parametrized with parameter \a t :
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  -# \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int  
  intersectionTicks 
  ( const HepPoint3D & Point  ,        
    const HepVector3D& Vector ,        
    ISolid::Ticks    & ticks  ) const ;
  
protected:
  
  /** standard constructor
   *  @param Name name of the solid object
   */
  SolidPolyHedronHelper ( const std::string& Name = "Anonymous PoliHedron");
  
  /// destructor 
  virtual ~SolidPolyHedronHelper ();
  
  /**  add a face/plane given with 3 points
   *  @see addFace( const HepPoint3D& , const HepPoint3D& ,
   *                const HepPoint3D& , const HepPoint3D& )
   *  @param Point1  the first  3D-point of the plane
   *  @param Point2  the second 3D-point of the plane 
   *  @param Point3  the third  3D-point of the plane 
   *  @return "false" if 3 points belongs to one line 
   */
  bool addFace
  ( const HepPoint3D& Point1 , 
    const HepPoint3D& Point2 , 
    const HepPoint3D& Point3 );
  
  /**  add a face/plane given with 4 points
   *  @see addFace( const HepPoint3D& , const HepPoint3D& ,
   *                const HepPoint3D& )
   *  @param Point1  the first  3D-point of the plane
   *  @param Point2  the second 3D-point of the plane 
   *  @param Point3  the third  3D-point of the plane 
   *  @param Point4  the fourth 3D-point of the plane 
   *  @exception SolidException  if 4 pointd do not define the place
   *  @return "false" if 3 points belongs to one line 
   */
  bool addFace
  ( const HepPoint3D& Point1 , 
    const HepPoint3D& Point2 , 
    const HepPoint3D& Point3 , 
    const HepPoint3D& Point4 );  
  
  /**  return vector of faces/planes 
   *  @return vector of faces/planes 
   */
  inline const  PLANES&   planes  () const { return m_ph_planes   ; } 
  
  /** define "inside" method for the plane.
   *  Assume that normal direction is EXTERNAL!!!
   *  @return "true" if point is "inside" 
   */
  inline bool inside 
  (  const HepPoint3D& Point , 
     const HepPlane3D& Plane ) const 
  { return 0 >= Plane.distance( Point ) ; };

  /** set bounding parameters 
   *  @return status code 
   */
  StatusCode setBP();
  
protected: 
  
  /// vector of faces/planes 
  PLANES   m_ph_planes   ;
  VERTICES m_ph_vertices ;
  
};

// ===========================================================================
// The END 
// ===========================================================================
#endif  ///<  DETDESC_SOLIDPOLYHEDRONHELPER_H 
/// ==========================================================================
