// $Id: SolidPolyHedronHelper.h,v 1.15 2007-09-20 15:17:05 wouter Exp $ 
// ===========================================================================
#ifndef     DETDESC_SOLIDPOLYHEDRONHELPER_H
#define     DETDESC_SOLIDPOLYHEDRONHELPER_H 1
/// STD & STL 
#include <vector>
/// Geometry definitions 
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
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
  typedef std::vector<Gaudi::Plane3D>   PLANES   ;
  typedef std::vector<Gaudi::XYZPoint>   VERTICES ;
  
public: 
  
  /** - Check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual bool isInside ( const Gaudi::XYZPoint& point ) const ;
  virtual bool isInside ( const Gaudi::Polar3DPoint   & point ) const;
  virtual bool isInside ( const Gaudi::RhoZPhiPoint   & point ) const; 
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

  virtual unsigned int intersectionTicks( const Gaudi::XYZPoint & Point,
                                          const Gaudi::XYZVector& Vector,
                                          ISolid::Ticks& ticks  ) const ;

  virtual unsigned int intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                          const Gaudi::Polar3DVector & Vector,
                                          ISolid::Ticks     & ticks) const ; 

  virtual unsigned int intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                          const Gaudi::RhoZPhiVector & Vector,
                                          ISolid::Ticks     & ticks) const ;
  /** Calculate the maximum number of ticks that a straight line could
      make with this solid
  *  @return maximum number of ticks
  */
  Ticks::size_type maxNumberOfTicks() const ;

protected:
  
  /** standard constructor
   *  @param Name name of the solid object
   */
  SolidPolyHedronHelper ( const std::string& Name = "Anonymous PoliHedron");
  
  /// destructor 
  virtual ~SolidPolyHedronHelper ();
  
  /**  add a face/plane given with 3 points
   *  @see addFace( const Gaudi::XYZPoint& , const Gaudi::XYZPoint& ,
   *                const Gaudi::XYZPoint& , const Gaudi::XYZPoint& )
   *  @param Point1  the first  3D-point of the plane
   *  @param Point2  the second 3D-point of the plane 
   *  @param Point3  the third  3D-point of the plane 
   *  @return "false" if 3 points belongs to one line 
   */
  bool addFace
  ( const Gaudi::XYZPoint& Point1 , 
    const Gaudi::XYZPoint& Point2 , 
    const Gaudi::XYZPoint& Point3 );
  
  /**  add a face/plane given with 4 points
   *  @see addFace( const Gaudi::XYZPoint& , const Gaudi::XYZPoint& ,
   *                const Gaudi::XYZPoint& )
   *  @param Point1  the first  3D-point of the plane
   *  @param Point2  the second 3D-point of the plane 
   *  @param Point3  the third  3D-point of the plane 
   *  @param Point4  the fourth 3D-point of the plane 
   *  @exception SolidException  if 4 pointd do not define the place
   *  @return "false" if 3 points belongs to one line 
   */
  bool addFace
  ( const Gaudi::XYZPoint& Point1 , 
    const Gaudi::XYZPoint& Point2 , 
    const Gaudi::XYZPoint& Point3 , 
    const Gaudi::XYZPoint& Point4 );  
  
  /**  return vector of faces/planes 
   *  @return vector of faces/planes 
   */
  inline const  PLANES&   planes  () const { return m_ph_planes   ; } 
  
  /** define "inside" method for the plane.
   *  Assume that normal direction is EXTERNAL!!!
   *  @return "true" if point is "inside" 
   */
  template<class aPoint>
  inline bool inside 
  (  const aPoint& Point , 
     const Gaudi::Plane3D& Plane ) const 
  { return 0 >= Plane.Distance( Point ) ; };

  /** set bounding parameters 
   */
  void setBP();

private:
  /**
   * implementation of isInside
   * @param  point reference to any kind of point with x(), y(), z()
   * @return bool
   */
  template<class aPoint>
  bool isInsideImpl(const aPoint& point) const;

  template<class aPoint, class aVector>
  unsigned int intersectionTicksImpl( const aPoint  & Point,
                                      const aVector & Vector,
                                      ISolid::Ticks& ticks ) const;
  
protected: 
  
  /// vector of faces/planes 
  PLANES   m_ph_planes   ;
  VERTICES m_ph_vertices ;
  
};

// ===========================================================================
// The END 
// ===========================================================================
#endif  ///<  DETDESC_SOLIDPOLYHEDRONHELPER_H 
// ===========================================================================
