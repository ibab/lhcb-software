// $Id: SolidBase.h,v 1.5 2002-05-13 11:35:19 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/05/11 18:25:46  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// ===========================================================================
#ifndef DETDESC_SOLIDBASE_H 
#define DETDESC_SOLIDBASE_H 1
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
// DetDesc 
#include "DetDesc/ISolid.h"

/** @class SolidBase SolidBase.h DetDesc/SolidBase.h
 *  
 *  Useful intermediate class for implementation of all solids 
 *
 *  @author Vanya Belyaev
 *  @date   02/08/2001
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
   *  @param tickMin minimum value of Tick 
   *  @param tickMax maximu value of Tick 
   *  @param ticks output container of "Ticks"
   *  @return the number of intersection points
   */
  virtual unsigned int 
  intersectionTicks 
  ( const HepPoint3D  & Point   ,         
    const HepVector3D & Vector  ,         
    ISolid::Ticks     & ticks   ) const ; 
  
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
  virtual unsigned int 
  intersectionTicks 
  ( const HepPoint3D  & Point   ,         
    const HepVector3D & Vector  ,         
    const ISolid::Tick& tickMin ,         
    const ISolid::Tick& tickMax ,         
    ISolid::Ticks     & ticks   ) const ; 
  
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
 
  /** - serialization for reading
   *  - implementation of ISerialize abstract interface 
   *  @see ISerialize 
   *  @see ISolid  
   *  @param sb reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& sb )       ;
  
  /** - serialization for writing
   *  - implementation of ISerialize abstract interface 
   *  @see ISerialize 
   *  @see ISolid  
   *  @param sb reference to stream buffer
   *  @return reference to stream buffer
   */
  virtual StreamBuffer& serialize( StreamBuffer& sb ) const ;
  
  /** - accept the inspector 
   *  - implementation of IInspectable abstract interface   
   *  @see IInspectable
   *  @see IInspector 
   *  @see ISolid
   *  @param pI pointer to Inspector 
   *  @return FALSE if teh object doesn't accept the Inspector.
   */
  virtual bool acceptInspector( IInspector* pI )       ; 
  
  /** - accept the inspector (const version)
   *  - implementation of IInspectable abstract interface   
   *  @see IInspectable
   *  @see IInspector 
   *  @see ISolid
   *  @param pI pointer to Inspector 
   *  @return FALSE if teh object doesn't accept the Inspector.
   */
  virtual bool acceptInspector( IInspector* pI ) const ;

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
  inline bool isOutBBox      
  ( const HepPoint3D& point         , 
    const double      tolerance = 0 ) const 
  {
    return 
      0 == tolerance ?      
      ( point.z () < zMin ()             ? true :
        point.z () > zMax ()             ? true :
        point.x () < xMin ()             ? true :
        point.x () > xMax ()             ? true :
        point.y () < yMin ()             ? true :
        point.y () > yMax ()             ? true : false ) :
      ( point.z () < zMin () - tolerance ? true :
        point.z () > zMax () + tolerance ? true :
        point.x () < xMin () - tolerance ? true :
        point.x () > xMax () + tolerance ? true :
        point.y () < yMin () - tolerance ? true :
        point.y () > yMax () + tolerance ? true : false ) ;
  };
  
  /** Fast check if the point is outside the bounding sphere of the solid
   *  @param point point to be checked  
   *  @param tolerance tolerance parameter  
   *  @return true of point is outside the bounding sphere 
   */
  inline bool isOutBSphere   
  ( const HepPoint3D& point         ,
    const double      tolerance = 0 ) const 
  {
    if( 0 == tolerance ) 
      { return  point.mag2 () > rMax () * rMax () ? true : false ; }
    const  double rmax = rMax()  + tolerance ;
    return rmax <= 0 ? true : point.mag2() > rmax * rmax ;
  };
  
  /** Fast check if the point is outside the bounding cylinder 
   *  of the solid
   *  @param point point to be checked 
   *  @param tolerance tolerance parameter  
   *  @return true of point is outside the bounding cylinder 
   */
  inline bool isOutBCylinder 
  ( const HepPoint3D& point         , 
    const double      tolerance = 0 ) const 
  {
    if( 0 != tolerance ) 
      {
        const double rhomax = rhoMax() + tolerance ;
        return rhomax <= 0 ? true : 
          point.z     () < zMin   () - tolerance ? true :
          point.z     () > zMax   () + tolerance ? true :
          point.perp2 () > rhomax * rhomax       ? true : false ;
      };
    return 
      point.z     () < zMin   ()             ? true :
      point.z     () > zMax   ()             ? true :
      point.perp2 () > rhoMax () * rhoMax () ? true : false ;
  };
  
  /** Fast check if the segment of the line between two points 
   *  is outside the bounding box 
   *  @param p1  first  point of the segment 
   *  @param p2  second point of the segment 
   *  @param tolerance  tolerance parameter  
   *  @return true if the whole segment is "outside" of the bounding box 
   */
  inline bool isOutBBox      
  ( const HepPoint3D& p1            , 
    const HepPoint3D& p2            , 
    const double      tolerance = 0 ) const 
  { 
    if( 0 == tolerance ) 
      {
        if( p1.z() < zMin() && p2.z() < zMin() ) { return true ; }
        if( p1.z() > zMax() && p2.z() > zMax() ) { return true ; }
        if( p1.x() < xMin() && p2.x() < xMin() ) { return true ; }
        if( p1.x() > xMax() && p2.x() > xMax() ) { return true ; }
        if( p1.y() < yMin() && p2.y() < yMin() ) { return true ; }
        if( p1.y() > yMax() && p2.y() > yMax() ) { return true ; }
      }
    else
      {
        const double zmin =             zMin() - tolerance ;
        if( p1.z() < zmin   && p2.z() < zmin   ) { return true ; }
        const double zmax =             zMax() + tolerance ;
        if( p1.z() > zmax   && p2.z() > zmax   ) { return true ; }
        const double xmin =             xMin() - tolerance ;
        if( p1.x() < xmin   && p2.x() < xmin   ) { return true ; }
        const double xmax =             xMax() + tolerance ;
        if( p1.x() > xmax   && p2.x() > xmax   ) { return true ; }
        const double ymin =             yMin() - tolerance ;
        if( p1.y() < ymin   && p2.y() < ymin   ) { return true ; }
        const double ymax =             yMax() + tolerance ;
        if( p1.y() > ymax   && p2.y() > ymax   ) { return true ; }
      }    
    //
    return  false ;
  };

  /** Fast check if the segment of the line between two points 
   *  is outside the bounding box 
   *  @param p     first  point of the segment 
   *  @param v     vector along the line
   *  @param tmin "minimal value of tick"
   *  @param tmax "maximal value of tick"
   *  @param tolerance  tolerance parameter  
   *  @return true if the whole segment is "outside" of the bounding box 
   */
  inline bool isOutBBox      
  ( const HepPoint3D&   p             , 
    const HepVector3D&  v             , 
    const ISolid::Tick& tmin          ,
    const ISolid::Tick& tmax          , 
    const double        tolerance = 0 ) const 
  {
    return isOutBBox( p + tmin * v , p + tmax * v , tolerance );
  };  
  
  /** Fast check if the line cross the bounding sphere  
   *  @param p     first  point on the line  
   *  @param v     vector along the line
   *  @param tolerance  tolerance parameter  
   *  @return true if line do not cross the bounding sphere  
   */
  inline bool crossBSphere      
  ( const HepPoint3D&   p             , 
    const HepVector3D&  v             , 
    const double        tolerance = 0 ) const 
  {
    const double pp   = p.mag2 ()           ;
    const double vv   = v.mag2 ()           ;
    const double pv   = p.dot  ( v )        ;
    const double rmax = rMax() + tolerance  ;
    if( rmax <= 0              ) { return false ; }
    const double dd   = rmax * rmax         ;
    if( 0 == vv && pp > dd     ) { return false ; }
    if( pp - pv * pv / vv > dd ) { return false ; }
    //
    return true ;
  };
  
  /** Fast check if the line cross the surface of bounding cylinder 
   *  @param p     first  point on the line  
   *  @param v     vector along the line
   *  @param tolerance  tolerance parameter  
   *  @return true if line do not cross the surface of bounding cylinder  
   */
  inline bool crossBCylinder      
  ( const HepPoint3D&   p             , 
    const HepVector3D&  v             , 
    const double        tolerance = 0 ) const 
  {
    const double pp     = p.perp2 ()                  ;
    const double vv     = v.perp2 ()                  ;
    const double pv     = p.dot ( v ) - p.z() * v.z() ;
    const double rhomax = rhoMax() + tolerance  ;
    if( rhomax <= 0            ) { return false ; }
    const double dd     = rhomax * rhomax     ;
    if( 0 == vv && pp > dd     ) { return false ; }
    if( pp - pv * pv / vv > dd ) { return false ; }
    //
    return true ;
  };

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
   *  @return status code
   */
  StatusCode checkBP() const ;
  
    
protected:
  
  /** standard constructor 
   *  @param Name name of the solid 
   */
  SolidBase( const std::string& Name = "Anonymous Solid" );
  
  /// virtual destructor 
  virtual ~SolidBase();
  
protected:
  
  std::string     m_name  ; ///< name of the solid
  unsigned long   m_count ; ///< reference counter 
  mutable ISolid* m_cover ; ///< pointer to cover 
  
  double m_xmin   ;
  double m_ymin   ;
  double m_zmin   ;
  
  double m_xmax   ;
  double m_ymax   ;
  double m_zmax   ;
  
  double m_rmax   ;
  double m_rhomax ;
  
};

// ===========================================================================
// The End 
// ===========================================================================
#endif //< DETDESC_SOLIDBASE_H
// ===========================================================================
