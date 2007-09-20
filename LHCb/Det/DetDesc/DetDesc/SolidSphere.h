// $Id: SolidSphere.h,v 1.17 2007-09-20 15:17:05 wouter Exp $ 
// ===========================================================================
#ifndef     DETDESC_SOLIDSPHERE_H
#define     DETDESC_SOLIDSPHERE_H 1 
// DetDesc 
#include "DetDesc/SolidBase.h" 
// Units
#include "GaudiKernel/SystemOfUnits.h" 

template <class TYPE>
class SolidFactory;


/** @class SolidSphere SolidSphere.h "DetDesc/SolidSphere.h"
 *
 *  A simple implementation of SPHERE
 *  
 *  @author Vanya Belyaev 
 */

class SolidSphere: public virtual SolidBase
{
  /// friend factory fo instantiation
  friend class SolidFactory<SolidSphere>;
  
public:
  
  /** constructor 
   *  @param name             name of sphere segment 
   *  @param OuterRadius      outer radius of sphere segement        
   *  @param InsideRadius     inner  radius of sphere segement
   *  @param StartPhiAngle    start phi angle 
   *  @param DeltaPhiAngle    delta phi angle 
   *  @param StartThetaAngle  start theta angle 
   *  @param DeltaThetaAngle  delta theta angle 
   *  @param CoverModel       covering model 
   */
  SolidSphere( const std::string& name                             ,
               const double       OuterRadius                      ,
               const double       InsideRadius    =   0.0          , 
               const double       StartPhiAngle   =   0.0 * Gaudi::Units::degree,
               const double       DeltaPhiAngle   = 360.0 * Gaudi::Units::degree,
               const double       StartThetaAngle =   0.0 * Gaudi::Units::degree,
               const double       DeltaThetaAngle = 180.0 * Gaudi::Units::degree,
               const int          CoverModel      =   0            );

  // destructor 
  virtual ~SolidSphere();
  
  /** - retrieve the specific type of the solid
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @return specific type of the solid
   */
  inline std::string typeName () const { return "SolidSphere"; };
  
  /** - check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  bool isInside ( const Gaudi::XYZPoint& point ) const;
  bool isInside ( const Gaudi::Polar3DPoint& point ) const ;
  bool isInside ( const Gaudi::RhoZPhiPoint& point ) const ;

  /** -# retrieve the pointer to "simplified" solid - "cover"
   *  -# implementation of ISolid abstract interface  
   *    The simplification scheme: 
   *  - for Model == 0 
   *     -# The cover for the general sphere segment is the clove 
   *         (remove gap in theta) 
   *     -# The cover for the clove is the sphere 
   *         (remove gap in phi) 
   *     -# The cover for the sphere is the ball 
   *         (set the inner radius to zero) 
   *     -# The cover for the ball is the box 
   *  - alternative model 
   *     -# The cover for general sphere segment is 
   *          the segment with inner radius equal to zero 
   *     -# The cover for the general sphere segment with 
   *        inner radius equal to zero is the 
   *        the sphere segemnt with no phi gap 
   *     -# The cover for the general sphere segment with 
   *         inner radius equal to zero and nophi gap 
   *         is the ball 
   *     -# the cover for the ball is the box   
   *  @see ISolid 
   *  @see SolidSphere::m_sphere_coverModel  
   *  @return pointer to "simplified" solid - "cover"
   */
  const ISolid* cover () const;
  
  /** - printout to STD/STL stream
   *  - implementation of ISolid abstract interface  
   *  - reimplementation of SolidBase::printOut( std::ostream& ) 
   *  @see ISolid 
   *  @see SolidBase 
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut ( std::ostream& os = std::cout ) const;  
  
  /** - printout to Gaudi  stream
   *  - implementation of ISolid abstract interface  
   *  - reimplementation of SolidBase::printOut( MsgStream& ) 
   *  @see ISolid 
   *  @see SolidBase 
   *  @param os Gaudi stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut ( MsgStream& os ) const;  
  
  /** -# calculate the intersection points("ticks") of the solid objects 
   *    with given line. 
   *  - Line is parametrized with parameter \a t :
   *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
   *      - \f$ \vec{p} \f$ is a point on the line 
   *      - \f$ \vec{v} \f$ is a vector along the line  
   *  - \a tick is just a value of parameter \a t, at which the
   *    intersection of the solid and the line occurs
   *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector  
   *    (\f$\vec{v}\f$) are defined in local reference system 
   *   of the solid 
   *  -# implementation of ISolid abstract interface  
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
  
  /**  return the inner radius of sphere segment
   *  @return the inner radius of sphere segment 
   */
  inline       double               insideRadius   () const 
  { return m_sphere_insideR  ; };
  
  /**  return the squared inner radius of sphere segment  
   *  @return the squared inner radius of sphere segment 
   */
  inline       double               insideR2   () const 
  { return m_sphere_insideR2 ; }; 
  
  /**  return the outer radius of sphere segment 
   *  @return the outer radius of sphere segment 
   */ 
  inline       double               outerRadius  () const 
  { return m_sphere_outerR  ; };
  
  /**  return the squared outer radius of sphere segment  
   *  @return the squared outer radius of sphere segment 
   */
  inline       double               outerR2       () const 
  { return m_sphere_outerR2 ; }; 
  
  /**  return the start of phi angle of sphere segment 
   *  @return the start of phi angle of sphere segment 
   */
  inline       double               startPhiAngle  () const 
  { return m_sphere_startPhiAngle  ; }; 
  
  /**  return the delta of phi angle of sphere segment 
   *  @return the delta of phi angle of sphere segment 
   */
  inline       double               deltaPhiAngle  () const 
  { return m_sphere_deltaPhiAngle  ; }; 
  
  /**  return the start of theta angle of sphere segment 
   *  @return the start of theta angle of sphere segment
   */ 
  inline       double               startThetaAngle() const 
  { return m_sphere_startThetaAngle; };
 
  /**  return the delta of theta angle of sphere segment 
   *  @return the delta of theta angle of sphere segment 
   */
  inline       double               deltaThetaAngle() const
  { return m_sphere_deltaThetaAngle; }; 
  
  /**  return the inner diameter of sphere segment 
   *  @return the inner diameter of sphere segment 
   */
  inline       double  insideDiameter () const 
  { return insideRadius()    * 2 ; }; 

  /**  return the outer diameter of sphere segment 
   *  @return the outer diameter of sphere segment 
   */
  inline       double  outerDiameter  () const 
  { return outerRadius()     * 2 ; };
 
  /**  return the end of phi angle of sphere segment 
   *  @return the end of phi angle of sphere segment 
   */
  inline       double  endPhiAngle    () const 
  { return m_sphere_startPhiAngle   + m_sphere_deltaPhiAngle   ; }; 
  
  /**  return the end of theta angle of sphere segment 
   *  @return the end of theta angle of sphere segment 
   */
  inline       double  endThetaAngle  () const 
  { return m_sphere_startThetaAngle + m_sphere_deltaThetaAngle ; };
  
  /** Calculate the maximum number of ticks that a straight line could
      make with this solid
  *  @return maximum number of ticks
  */
  Ticks::size_type maxNumberOfTicks() const { return 4 ; }

protected:
  
  /// gap in phi   ?
  const bool noPhiGap   () const { return m_noPhiGap   ; }
  /// gap in theta ?
  const bool noThetaGap () const { return m_noThetaGap ; }
  
  /** check for R 
   *  @param point to be checked 
   *  @return true if point is "inside rho" 
   */
  template<class aPoint>
  inline const bool insideR     ( const aPoint& point ) const ;
  
  /** check for phi 
   *  @param point to be checked 
   *  @return true if point is "inside phi" 
   */
  template<class aPoint>
  inline const bool insidePhi   ( const aPoint& point ) const ;
  
  /** check for theta
   *  @param point to be checked 
   *  @return true if point is "inside theta" 
   */
  template<class aPoint>
  inline const bool insideTheta ( const aPoint& point ) const ;
  
protected:

  /** protected constructor 
   *  @param name name of the sphere segment \
   */
  SolidSphere( const std::string& name = "Anonymous Sphere" );

  /** set bounding parameters 
   */
  void setBP();
  
private:
  //
  SolidSphere           ( const SolidSphere& );  // no copy-constructor 
  SolidSphere& operator=( const SolidSphere& );  // no assignment 

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
                                      ISolid::Ticks& ticks ) const;

  //
private:

  /// sphere segment parameters 
  double                m_sphere_outerR2         ;  
  double                m_sphere_insideR2        ; 
  double                m_sphere_startPhiAngle   ;
  double                m_sphere_deltaPhiAngle   ; 
  double                m_sphere_startThetaAngle ; 
  double                m_sphere_deltaThetaAngle ;
  /// cover model 
  int                   m_sphere_coverModel      ;
  //
  double                m_sphere_outerR          ;  
  double                m_sphere_insideR         ; 
  // 
  bool                  m_noPhiGap               ;
  bool                  m_noThetaGap             ;
  //
};
// ===========================================================================


// ===========================================================================
/** check for R 
 *  @param point to be checked 
 *  @return true if point is "inside rho" 
 */
// ===========================================================================
template <class aPoint>
inline const bool SolidSphere::insideR( const aPoint& point ) const 
{
  const double r2 = point.mag2();
  if(  r2 >  outerR2 () ) { return false ; }
  if(  r2 < insideR2 () ) { return false ; }
  // inside! 
  return true ;
};
// ===========================================================================

// ===========================================================================
/** check for phi 
 *  @param point to be checked 
 *  @return true if point is "inside phi" 
 */
// ===========================================================================
template <class aPoint>
inline const bool SolidSphere::insidePhi( const aPoint& point ) const 
{
  if( noPhiGap()                                    ) { return true ; }
  double phi = point.phi() ;   // [-180,180] 
  if( startPhiAngle ()                   <= phi &&
      startPhiAngle () + deltaPhiAngle() >= phi     ) { return true ; }
  phi += 360 * Gaudi::Units::degree ;
  if( startPhiAngle ()                   <= phi &&
      startPhiAngle () + deltaPhiAngle() >= phi     ) { return true ; }
  // 
  return false ;
};
// ===========================================================================

// ===========================================================================
/** check for theta
 *  @param point to be checked 
 *  @return true if point is "inside theta" 
 */
// ===========================================================================
template <class aPoint>
inline const bool SolidSphere::insideTheta( const aPoint& point ) const 
{
  if( noThetaGap()                                        ) { return true ; }
  const double theta = point.theta() ;  
  if( startThetaAngle ()                     <= theta &&
      startThetaAngle () + deltaThetaAngle() >= theta     ) { return true ; }
  // 
  return false ;
};
// ===========================================================================

// ===========================================================================
// The END 
// ===========================================================================
#endif ///<  DETDESC_SOLIDSPHERE_H
// ===========================================================================
