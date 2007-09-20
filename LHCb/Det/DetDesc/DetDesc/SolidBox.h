// $Id: SolidBox.h,v 1.18 2007-09-20 15:17:05 wouter Exp $
// ===========================================================================
#ifndef     DETDESC_SOLIDBOX_H
#define     DETDESC_SOLIDBOX_H
/// STD and STL 
#include <cmath> 
#include <iostream> 
/// Geometry Definitions 
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
/// DetDesc 
#include "DetDesc/SolidBase.h" 
/// forward declarations
template <class TYPE>
class SolidFactory;          ///< DetDesc 

/** @class SolidBox SolidBox.h "DetDesc/SolidBox.h"
 *
 *  A simple implementation of BOX 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxx
 */

class SolidBox: public virtual SolidBase
{
  /// friend factory 
  friend class SolidFactory<SolidBox>;

public:
  
  /** standard(public)  constructor 
   *  @param      Name        name of this box  
   *  @param      xHalf half-size of the box in x-direction
   *  @param      yHalf half-size of the box in y-direction
   *  @param      zHalf half-size of the box in z-direction
   *  @exception  SolidException wrong parameters range 
   */
  SolidBox( const std::string& Name  ,
            const double       xHalf , 
            const double       yHalf , 
            const double       zHalf); 
  
  /** destructor  
   */
  virtual ~SolidBox();  
  
  /** - retrieve the box type 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  return box type
   */
  inline       std::string  typeName ()  const { return "SolidBox" ; };
  
  /** - check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual bool isInside  ( const Gaudi::XYZPoint& point ) const;
  virtual bool isInside ( const Gaudi::Polar3DPoint& point ) const ;
  virtual bool isInside ( const Gaudi::RhoZPhiPoint& point ) const ;
  /** - retrieve the pointer to "simplified" solid - "cover"
   *  - implementation of ISolid abstract interface 
   *  The Box is the most simple shape
   *  @see ISolid 
   *  @return pointer to "simplified" solid - "cover"
   */
  virtual const ISolid* cover () const ;
  
  /** - retrieve the pointer to "the most simplified cover" 
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::coverTop() 
   *  The Box is the most simple shape
   *  @see ISolid 
   *  @see SolidBase 
   *  @return pointer to the most simplified cover 
   */
  virtual const ISolid* coverTop () const ;
  
  /** - printout to STD/STL stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( std::ostream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os STD/STL stream
   *  @return reference to the stream 
   */
  virtual std::ostream& printOut ( std::ostream& os ) const;
  
  /** - printout to Gaudi MsgStream stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( MsgStream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream 
   */
  virtual MsgStream&    printOut ( MsgStream&    os ) const;
  
  /** - calculate the intersection points("ticks") of the solid objects 
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
  virtual unsigned int intersectionTicks( const Gaudi::XYZPoint&  Point  ,
                                          const Gaudi::XYZVector& Vector ,
                                          ISolid::Ticks   &  ticks  ) const ; 
  virtual unsigned int intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                          const Gaudi::Polar3DVector & Vector,
                                          ISolid::Ticks     & ticks) const ; 
  virtual unsigned int intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                          const Gaudi::RhoZPhiVector & Vector,
                                          ISolid::Ticks     & ticks) const ;
  /** calculate the intersection points("ticks") of the solid objects 
   *  with given line. 
   *  - Line is parametrized with parameter \a t : 
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  - \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  Only intersection ticks within the range 
   *   \a tickMin and \a tickMax are taken into account.
   *  @see ISolid::intersectionTicks()
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int intersectionTicks( const Gaudi::XYZPoint & Point,
                                          const Gaudi::XYZVector& Vector,
                                          const ISolid::Tick&     tickMin,
                                          const ISolid::Tick&     tickMax ,
                                          ISolid::Ticks&  ticks   ) const ;

  virtual unsigned int intersectionTicks( const Gaudi::Polar3DPoint& Point,
                                          const Gaudi::Polar3DVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks& ticks   ) const ; 

  virtual unsigned int intersectionTicks( const Gaudi::RhoZPhiPoint& Point,
                                          const Gaudi::RhoZPhiVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks& ticks   ) const ; 


  /**  return the full x-size of the box 
   *  @return the full x-size of the box 
   */
  inline         double  xsize      ()  const 
  { return m_box_xHalfLength*2 ; };
  
  /**  return the full y-size of the box 
   *  @return the full y-size of the box 
   */
  inline         double  ysize      ()  const 
  { return m_box_yHalfLength*2 ; };
  
  /**  return the full z-size of the box 
   *  @return the full z-size of the box 
   */
  inline         double  zsize      ()  const 
  { return m_box_zHalfLength*2 ; };
  
  /**  return the half x-size of the box 
   *  @return the half x-size of the box 
   */
  inline         double  xHalfLength()  const 
  { return m_box_xHalfLength   ; };
  
  /**  return the half y-size of the box 
   *  @return the half y-size of the box 
   */
  inline         double  yHalfLength()  const 
  { return m_box_yHalfLength   ; };
  
  /**  return the half z-size of the box 
   *  @return the half z-size of the box 
   */
  inline         double  zHalfLength()  const 
  { return m_box_zHalfLength   ; };


  /** Calculate the maximum number of ticks that a straight line could
      make with this solid
  *  @return maximum number of ticks
  */
  Ticks::size_type maxNumberOfTicks() const { return 2 ; }

protected:
  
  /** default (protected) constructor 
   */
  SolidBox(); 
  
  /** set parameters for bounding solids (box, sphere and cylinder)
   */
  void setBP() ;
  
private:
  
  SolidBox           ( const SolidBox & );  ///< no copy-constructor 
  SolidBox& operator=( const SolidBox & );  ///< no assignment 

  /**
   * implementation of isInside
   * @param  point reference to any kind of point with x(), y(), z()
   * @return bool
   */
  template <class aPoint>
  bool isInsideImpl(const aPoint& point) const;
  template<class aPoint, class aVector>

  unsigned int intersectionTicksImpl( const aPoint  & Point,
                                      const aVector & Vector,
                                      const ISolid::Tick& tickMin,
                                      const ISolid::Tick& tickMax,
                                      ISolid::Ticks&  ticks) const;

  template<class aPoint, class aVector>
  unsigned int intersectionTicksImpl( const aPoint  & Point,
                                      const aVector & Vector,
                                      ISolid::Ticks& ticks ) const;
  
private:
  
  double               m_box_xHalfLength ;   ///< x/2 
  double               m_box_yHalfLength ;   ///< y/2 
  double               m_box_zHalfLength ;   ///< z/2

};


/// ===========================================================================
#endif ///<  DETDESC_SOLIDBOX_H
/// ===========================================================================
