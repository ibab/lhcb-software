// $Id: SolidTubs.h,v 1.11 2005-12-07 07:33:50 cattanem Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef     DETDESC_SOLIDTUBS_H
#define     DETDESC_SOLIDTUBS_H 1  
// STD and STL includes 
#include <cmath>
#include <iostream>
// LHCbDefinitions includes 
#include "Kernel/Point3DTypes.h" 
#include "Kernel/Vector3DTypes.h"
#include "Kernel/PhysicalConstants.h" 
// DetDesc 
#include "DetDesc/SolidBase.h" 
// forward declarations
template <class TYPE>
class SolidFactory;
class StreamBuffer;
class MsgStream;
//

/** @class SolidTubs SolidTubs.h DetDesc/SolidTubs.h
 *
 *  A simple implementation of TUBS - the tube segment
 *  
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 */

class SolidTubs: public virtual SolidBase
{
  /// frined factory for instantiation 
  friend class SolidFactory<SolidTubs>;
  
public:
  
  /** constructor
   *  @param name           name of tube segment 
   *  @param ZHalfLength    half-length of the tube segment
   *  @param OuterRadius    outer radius of tube segment 
   *  @param InnerRadius    inner radius of tube segment 
   *  @param StartPhiAngle  start phi angle 
   *  @param DeltaPhiAngle  delta phi angle  
   *  @param CoverModel     covering model   
   *  @exception SolidException wrong parameter range 
   */
  SolidTubs( const std::string& name                             ,
             const double       ZHalfLength                      , 
             const double       OuterRadius                      ,
             const double       InnerRadius   =   0.0            , 
             const double       StartPhiAngle =   0.0 * degree   , 
             const double       DeltaPhiAngle = 360.0 * degree   ,
             const int          CoverModel    =   0              );     

  /// destructor 
  virtual ~SolidTubs();

  /** - retrieve the tubs type 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid 
   *  return box type
   */
  inline std::string typeName () const { return "SolidTubs"; };

  /** - check for the given 3D-point. 
   *    Point coordinated are in the local reference 
   *    frame of the solid.   
   *  - implementation of ISolid absstract interface  
   *  @see ISolid 
   *  @param point point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  bool isInside ( const Gaudi::XYZPoint& point ) const; 
  
  /** retrieve the pointer to "the most simplified cover" 
   *    -# for Model = 0 
   *      - the cover for general tube segment is tube 
   *      - the cover for tube  is the cylinder 
   *      - the cover for cylinder is the box 
   *    -#  for Model = 1 
   *      - the cover for general tube segment is teh cylinder segment 
   *      - the cover for cylinder segment is cylinder 
   *      - the cover for cylinder is the box 
   *  implementation  of ISolid abstract interface 
   *  @see ISolid 
   *  @return pointer to the most simplified cover 
   */
  const ISolid* cover () const;
  
  /** - printout to STD/STL stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( std::ostream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os STD/STL stream
   *  @return reference to the stream 
   */
  virtual std::ostream& printOut ( std::ostream& os = std::cout ) const;
  
  /** - printout to Gaudi MsgStream stream    
   *  - implementation  of ISolid abstract interface 
   *  - reimplementation of SolidBase::printOut( MsgStream& )
   *  @see SolidBase 
   *  @see ISolid 
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream 
   */
  virtual MsgStream&    printOut ( MsgStream&    os             ) const;
  
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
  virtual unsigned int 
  intersectionTicks 
  ( const Gaudi::XYZPoint&  Point  ,        
    const Gaudi::XYZVector& Vector ,       
    ISolid::Ticks   &  ticks  ) const ;
  
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
  virtual unsigned int 
  intersectionTicks 
  ( const Gaudi::XYZPoint & Point   ,
    const Gaudi::XYZVector& Vector  ,
    const Tick       & tickMin ,
    const Tick       & tickMax ,
    Ticks            & ticks   ) const ;
    
  /** - serialization for reading
   *  - implementation of ISerialize abstract interface 
   *  - reimplementation of SolidBase::serialize 
   *  @see ISerialize 
   *  @see ISolid  
   *  @see SolidBase   
   *  @param      s               reference to stream buffer
   *  @exception  SolidException  wrong parameters range 
   *  @return reference to stream buffer
   */
  StreamBuffer& serialize( StreamBuffer& s )       ; 
  
  /** - serialization for writing
   *  - implementation of ISerialize abstract interface 
   *  - reimplementation of SolidBase::serialize 
   *  @see ISerialize 
   *  @see ISolid  
   *  @see SolidBase   
   *  @param s reference to stream buffer
   *  @return reference to stream buffer
   */
  StreamBuffer& serialize( StreamBuffer& s ) const ; 

  ///@{ 
  /** accessors to the tube segment parameters */
  inline double innerRadius     () const 
  { return m_tubs_innerRadius   ; };
  inline double outerRadius     () const 
  { return m_tubs_outerRadius   ; };
  inline double zHalfLength     () const 
  { return m_tubs_zHalfLength   ; };
  inline double startPhiAngle   () const 
  { return m_tubs_startPhiAngle ; }; 
  inline double deltaPhiAngle   () const 
  { return m_tubs_deltaPhiAngle ; }; 
  inline double innerDiameter   () const 
  { return m_tubs_innerRadius   * 2 ; };
  inline double outerDiameter   () const 
  { return m_tubs_outerRadius   * 2 ; };
  inline double zLength         () const 
  { return m_tubs_zHalfLength   * 2 ; };
  inline double endPhiAngle     () const 
  { return m_tubs_startPhiAngle + m_tubs_deltaPhiAngle ; }; 
  ///@}
  
protected:
  
  /** check for R 
   *  @param point to be checked 
   *  @return true if point is "inside rho" 
   */
  inline const bool insideRho ( const Gaudi::XYZPoint& point ) const ;
  
  /** check for phi 
   *  @param point to be checked 
   *  @return true if point is "inside phi" 
   */
  inline const bool insidePhi ( const Gaudi::XYZPoint& point ) const ;
  
  
  /// gap in phi ?
  inline const bool noPhiGap() const { return m_noPhiGap ; }
  
protected:
  
  /** constructor 
   *  @param name name of tube segment 
   */
  SolidTubs( const std::string& name = "Anonymous Tubs");

  /** set bounding parameters dor base class SolidBox 
   *  @return status code
   */
  StatusCode setBP();
  
private:
  
  SolidTubs           ( const SolidTubs & );  ///< no copy-constructor 
  SolidTubs& operator=( const SolidTubs & );  ///< no assignment
  

private:

  // parameters of tube segment 
  double                  m_tubs_zHalfLength   ;
  double                  m_tubs_outerRadius   ;
  double                  m_tubs_innerRadius   ;
  double                  m_tubs_startPhiAngle ;
  double                  m_tubs_deltaPhiAngle ;
  // model for constructing the cover solid 
  int                     m_tubs_coverModel    ;  
  //
  bool                    m_noPhiGap           ;
};

  
// ===========================================================================
/** check for R 
 *  @param point to be checked 
 *  @return true if point is "inside rho" 
 */
// ===========================================================================
inline const bool SolidTubs::insideRho ( const Gaudi::XYZPoint& point ) const 
{
  const double rho2 = point.perp2();
  if( rho2 > outerRadius() * outerRadius() ) { return false ; }
  if( 0    < innerRadius() && 
      rho2 < innerRadius() * innerRadius() ) { return false ; }
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
inline const bool SolidTubs::insidePhi ( const Gaudi::XYZPoint& point ) const 
{
  if( noPhiGap()                                    ) { return true ; }
  double phi = point.phi() ;   // [-180,180] 
  if( startPhiAngle ()                   <= phi &&
      startPhiAngle () + deltaPhiAngle() >= phi     ) { return true ; }
  phi += 360 * degree ;
  if( startPhiAngle ()                   <= phi &&
      startPhiAngle () + deltaPhiAngle() >= phi     ) { return true ; }
  // 
  return false ;
};
// ===========================================================================

// ===========================================================================
// The END 
// ===========================================================================
#endif ///<   DETDESC_SOLIDTUBS_H
// ===========================================================================

