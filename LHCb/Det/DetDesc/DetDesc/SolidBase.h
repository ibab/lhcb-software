// $Id: SolidBase.h,v 1.18 2008-06-24 13:25:03 jonrob Exp $
#ifndef DETDESC_SOLIDBASE_H 
#define DETDESC_SOLIDBASE_H 1

// DetDesc 
#include "DetDesc/Services.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidException.h"

/// forward declarations 
class IMessageSvc;

/** @class SolidBase SolidBase.h DetDesc/SolidBase.h
 *  
 *  Useful intermediate class for implementation of all solids 
 *
 *  @author Vanya Belyaev
 *  @author Sebastien Ponce
 */

class SolidBase: public virtual ISolid
{
  
public: 
  
  /** - retrieve the name of the concrete Solid object 
   *  - implementation of ISolid abstract interface 
   *  @see ISolid
   *  @return name of the solid object 
   */
  virtual const std::string& name() const { return m_name ; }
  
  /** - reset solid to its inititial state, remove 
   *    all auxillary data fields and pointers. 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @see SolidBase::~SolidBase()
   *  @return  self-reference 
   */
  virtual ISolid* reset ();

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
  virtual unsigned int  intersectionTicks( const Gaudi::XYZPoint  & Point,
                                           const Gaudi::XYZVector & Vector,
                                           ISolid::Ticks     & ticks) const ; 
  virtual unsigned int  intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                           const Gaudi::Polar3DVector & Vector,
                                           ISolid::Ticks     & ticks) const ; 
  virtual unsigned int  intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                           const Gaudi::RhoZPhiVector & Vector,
                                           ISolid::Ticks     & ticks) const ; 
  
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
   *  -# Only intersection ticks within the range 
   *     \a tickMin and \a tickMax are taken into account. 
   *  - implementation of ISolid abstract interface  
   *  @see ISolid 
   *  @param Point initial point for the line
   *  @param Vector vector along the line
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int intersectionTicks( const Gaudi::XYZPoint& Point,
                                          const Gaudi::XYZVector & Vector,
                                          const ISolid::Tick& tickMin,
                                          const ISolid::Tick& tickMax,
                                          ISolid::Ticks& ticks   ) const ; 

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
  
  /** - query the interface 
   *  - implementation of IInterface abstract interface 
   *  @see IInterface 
   *  @param ID unique interafce identifier 
   *  @param ppI placeholder for returned interface
   *  @return status code 
   */
  virtual StatusCode queryInterface
  ( const InterfaceID& ID , void** ppI );
  
  /** - increment the reference count of Interface instance
   *  - implementation of IInterface abstract interface 
   *  @see IInterface 
   *  @return reference count 
   */
  virtual unsigned long addRef  () ;
  
  /** - release interface instance
   *  - implementation of IInterface abstract interface 
   *  @see IInterface 
   *  @return reference count 
   */
  virtual unsigned long release () ;
  
  /** - retrieve the pointer to "the most simplified cover", 
   *    ideally to something like "the bounding box"
   *  - implementation of ISolid interface
   *  @see ISolid 
   *  @see ISolid::cover()
   *  @return pointer to the most simplified cover
   */
  virtual const ISolid* coverTop () const ;
  
  /** - printout to STD/STL stream
   *  - implementation of ISolid interface
   *  @see ISolid 
   *  @param st STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut ( std::ostream& st ) const ;
  
  /** - printout to Gaudi  stream
   *  - implementation of ISolid interface
   *  @see ISolid 
   *  @param st Gaudi stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut ( MsgStream&    st ) const ;
 
public : 
  
  /// accessor to "minimal x" value of the solid   ("bounding box")
  inline double  xMin      () const                { return m_xmin    ; }
  /// accessor to "maximal x" value of the solid   ("bounding box")
  inline double  xMax      () const                { return m_xmax    ; }
  
  /// accessor to "minimal y" value of the solid   ("bounding box")
  inline double  yMin      () const                { return m_ymin    ; }
  /// accessor to "maximal y" value of the solid   ("bounding box")
  inline double  yMax      () const                { return m_ymax    ; }
  
  /// accessor to "minimal z" value of the solid   ("bounding box")
  inline double  zMin      () const                { return m_zmin    ; }
  /// accessor to "maximal z" value of the solid   ("bounding box")
  inline double  zMax      () const                { return m_zmax    ; }
  
  /// accessor to "maximal r" value of the solid   ("bounding sphere")
  inline double  rMax      () const                { return m_rmax    ; }
  
  /// accessor to "maximal rho" value of the solid ("bounding sphere")
  inline double  rhoMax    () const                { return m_rhomax  ; }

protected:
  
  /** Fast check if the point is outside the bounding box of the solid
   *  @param point     point to be checked 
   *  @param tolerance tolerance parameter  
   *  @return true of point is outside the bounding box 
   */
  template <class aPoint>
  inline bool isOutBBox      
  ( const aPoint& point         , 
    const double      tolerance ) const 
  {
    return 
      point.z () < zMin () - tolerance ||
      point.z () > zMax () + tolerance ||
      point.x () < xMin () - tolerance ||
      point.x () > xMax () + tolerance ||
      point.y () < yMin () - tolerance ||
      point.y () > yMax () + tolerance ;
  }
  /** Fast check if the point is outside the bounding box of the solid
   *  @param point     point to be checked 
   *  @return true of point is outside the bounding box 
   */
  template <class aPoint>
  inline bool isOutBBox      
  ( const aPoint& point ) const 
  {
    return 
      point.z () < zMin () ||
      point.z () > zMax () ||
      point.x () < xMin () ||
      point.x () > xMax () ||
      point.y () < yMin () ||
      point.y () > yMax () ;
  }
  
  /** Fast check if the point is outside the bounding sphere of the solid
   *  @param point point to be checked  
   *  @param tolerance tolerance parameter  
   *  @return true of point is outside the bounding sphere 
   */
  template <class aPoint>
  inline bool isOutBSphere   
  ( const aPoint& point         ,
    const double      tolerance ) const 
  {
    const  double rmax = rMax()  + tolerance ;
    return rmax <= 0 || point.mag2() > rmax * rmax ;
  }
  
  /** Fast check if the point is outside the bounding sphere of the solid
   *  @param point point to be checked  
   *  @return true of point is outside the bounding sphere 
   */
  template <class aPoint>
  inline bool isOutBSphere ( const aPoint& point  ) const 
  {
    return  point.mag2 () > rMax () * rMax () ;
  }
  
  /** Fast check if the point is outside the bounding cylinder 
   *  of the solid
   *  @param point point to be checked 
   *  @param tolerance tolerance parameter  
   *  @return true of point is outside the bounding cylinder 
   */
  template <class aPoint>
  inline bool isOutBCylinder 
  ( const aPoint& point         , 
    const double      tolerance ) const 
  {
    const double rhomax = rhoMax() + tolerance ;
    return rhomax <= 0 ? true : 
      point.z     () < zMin   () - tolerance ? true :
      point.z     () > zMax   () + tolerance ? true :
      point.perp2 () > rhomax * rhomax       ? true : false ;
  }
  
  /** Fast check if the point is outside the bounding cylinder 
   *  of the solid
   *  @param point point to be checked 
   *  @return true of point is outside the bounding cylinder 
   */
  template <class aPoint>
  inline bool isOutBCylinder ( const aPoint& point ) const 
  {
    return 
      point.z     () < zMin   () ||
      point.z     () > zMax   () ||
      point.perp2 () > rhoMax () * rhoMax () ;
  }
  
  /** Fast check if the segment of the line between two points 
   *  is outside the bounding box 
   *  @param p1  first  point of the segment 
   *  @param p2  second point of the segment 
   *  @param tolerance  tolerance parameter  
   *  @return true if the whole segment is "outside" of the bounding box 
   */
  template<class aPointA, class aPointB>
  inline bool isOutBBox      
  ( const aPointA& p1            , 
    const aPointB& p2            , 
    const double      tolerance ) const 
  {  
    return 
      ( (p1.z() < zMin()-tolerance && p2.z() < zMin()-tolerance) ||
        (p1.z() > zMax()+tolerance && p2.z() > zMax()+tolerance) ||
        (p1.x() < xMin()-tolerance && p2.x() < xMin()-tolerance) ||
        (p1.x() > xMax()+tolerance && p2.x() > xMax()+tolerance) ||
        (p1.y() < yMin()-tolerance && p2.y() < yMin()-tolerance) ||
        (p1.y() > yMax()+tolerance && p2.y() > yMax()+tolerance) );
  }

  /** Fast check if the segment of the line between two points 
   *  is outside the bounding box 
   *  @param p1  first  point of the segment 
   *  @param p2  second point of the segment 
   *  @return true if the whole segment is "outside" of the bounding box 
   */
  template<class aPointA, class aPointB>
  inline bool isOutBBox      
  ( const aPointA& p1            , 
    const aPointB& p2 ) const 
  { 
    return 
      ( p1.z() < zMin() && p2.z() < zMin() ) ||
      ( p1.z() > zMax() && p2.z() > zMax() ) ||
      ( p1.x() < xMin() && p2.x() < xMin() ) ||
      ( p1.x() > xMax() && p2.x() > xMax() ) ||
      ( p1.y() < yMin() && p2.y() < yMin() ) ||
      ( p1.y() > yMax() && p2.y() > yMax() ) ;
  }

  /** Fast check if the segment of the line between two points 
   *  is outside the bounding box 
   *  @param p     first  point of the segment 
   *  @param v     vector along the line
   *  @param tmin "minimal value of tick"
   *  @param tmax "maximal value of tick"
   *  @param tolerance  tolerance parameter  
   *  @return true if the whole segment is "outside" of the bounding box 
   */
  template <class aPoint, class aVector>
  inline bool isOutBBox      
  ( const aPoint&   p             , 
    const aVector&  v             , 
    const ISolid::Tick& tmin          ,
    const ISolid::Tick& tmax          , 
    const double        tolerance ) const 
  {
    return isOutBBox( p + tmin * v , p + tmax * v , tolerance );
  }
  
  /** Fast check if the segment of the line between two points 
   *  is outside the bounding box 
   *  @param p     first  point of the segment 
   *  @param v     vector along the line
   *  @param tmin "minimal value of tick"
   *  @param tmax "maximal value of tick"
   *  @return true if the whole segment is "outside" of the bounding box 
   */
  template <class aPoint, class aVector>
  inline bool isOutBBox      
  ( const aPoint&   p             , 
    const aVector&  v             , 
    const ISolid::Tick& tmin          ,
    const ISolid::Tick& tmax ) const 
  {
    return isOutBBox( p + tmin * v , p + tmax * v );
  }
  
  /** Fast check if the line cross the bounding sphere  
   *  @param p     first  point on the line  
   *  @param v     vector along the line
   *  @param tolerance  tolerance parameter  
   *  @return true if line do not cross the bounding sphere  
   */
  template <class aPoint, class aVector>
  inline bool crossBSphere      
  ( const aPoint&   p             , 
    const aVector&  v             , 
    const double        tolerance = 0 ) const 
  {
    const double pp   = p.mag2 ()           ;
    const double vv   = v.mag2 ()           ;
    const double pv   = p.Dot  ( v )        ;
    const double rmax = rMax() + tolerance  ;
    const double dd   = rmax * rmax         ;
    return ( rmax > 0                       && 
             !(0 == vv && pp > dd)          &&
             !( (vv*pp) - (pv*pv) > vv*dd ) );
  }
  
  /** Fast check if the line cross the surface of bounding cylinder 
   *  @param p     first  point on the line  
   *  @param v     vector along the line
   *  @param tolerance  tolerance parameter  
   *  @return true if line do not cross the surface of bounding cylinder  
   */
  template <class aPoint, class aVector>
  inline bool crossBCylinder      
  ( const aPoint&   p             , 
    const aVector&  v             , 
    const double        tolerance = 0 ) const 
  {
    const double pp     = p.perp2 ()                  ;
    const double vv     = v.perp2 ()                  ;
    const double pv     = p.Dot ( v ) - p.z() * v.z() ;
    const double rhomax = rhoMax() + tolerance  ;
    const double dd     = rhomax * rhomax     ;
    return ( rhomax > 0                     &&
             !( 0 == vv && pp > dd )        &&
             !( (vv*pp) - (pv*pv) > vv*dd ) );
  }

protected:

  /// set "minimal x" for the solid 
  inline void    setXMin   ( const double value )  { m_xmin   = value ; }
  /// set "maximal x" for the solid 
  inline void    setXMax   ( const double value )  { m_xmax   = value ; }

  /// set "minimal y" for the solid 
  inline void    setYMin   ( const double value )  { m_ymin   = value ; }
  /// set "maximal y" for the solid 
  inline void    setYMax   ( const double value )  { m_ymax   = value ; }

  /// set "minimal z" for the solid 
  inline void    setZMin   ( const double value )  { m_zmin   = value ; }
  /// set "maximal z" for the solid 
  inline void    setZMax   ( const double value )  { m_zmax   = value ; }

  /// set "maximal r" for the solid 
  inline void    setRMax   ( const double value )  { m_rmax   = value ; }
  /// set "maximal r" for the solid 
  inline void    setRhoMax ( const double value )  { m_rhomax = value ; }

  /** check bounding parameters 
   *  @exception SolidException 
   *  for wrong set of bounding parameters
   */
  void checkBP() const ;
  
  /** 
   *  accessor to massage service
   *  @return pointer to message service 
   */
  IMessageSvc* msgSvc() const;
    
  /** check that maxNumberOfTicks is smaller than the maximum capacity of Ticks container
   *  @exception SolidException if maxNumberOfTicks too large
   */
  inline void checkTickContainerCapacity() const {
    if( maxNumberOfTicks() > ISolid::MaxTicks ) {
      std::stringstream msg ;
      msg << "Volume \'" << name() << "\' has too large maxNumberOfTicks: "
	  << maxNumberOfTicks() << ". Please increase max capacity in ISolid::Ticks" ;
      throw SolidException(msg.str()) ;
    }
  }
  
protected:
  
  /** standard constructor 
   *  @param Name name of the solid 
   */
  SolidBase( const std::string& Name="Undefined" );
  
  /// virtual destructor 
  virtual ~SolidBase();

private:
  /// default constructor is disabled 
  //  SolidBase();

  // assignement operator is disabled 
  SolidBase& operator=( const SolidBase& ) ;

template<class aPoint, class aVector>
unsigned int intersectionTicksImpl( const aPoint  & Point,
                                    const aVector & Vector,
                                    const ISolid::Tick& tickMin,
                                    const ISolid::Tick& tickMax,
                                    ISolid::Ticks&  ticks) const;

unsigned int intersectionTicksImpl( ISolid::Ticks& ticks ) const;
  
protected:
  
  std::string     m_name  ; ///< name of the solid
  mutable ISolid* m_cover ; ///< pointer to cover 
  
  double m_xmin   ;
  double m_ymin   ;
  double m_zmin   ;
  
  double m_xmax   ;
  double m_ymax   ;
  double m_zmax   ;
  
  double m_rmax   ;
  double m_rhomax ;
  
  /// reference to services
  DetDesc::Services* m_services;
};

// ===========================================================================
// The End 
// ===========================================================================
#endif //< DETDESC_SOLIDBASE_H
// ===========================================================================
