// $Id: IPVolume.h,v 1.12 2007-01-17 12:10:11 cattanem Exp $ 
// ===========================================================================
#ifndef  DETDESC_IPVOLUME_H 
#define  DETDESC_IPVOLUME_H 1 
/// STD & STL includes 
#include <iostream>
#include <string> 
/// GaudiKernel includes 
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"
/// DetDesc includes
#include "DetDesc/ILVolume.h"
#include "DetDesc/ISolid.h"

/** Declaration of the interface ID 
 * ( unique interface identifier , major & minor versions)
 */
static const InterfaceID IID_IPVolume( 154 , 5 , 0 );

/** @interface IPVolume IPVolume.h "DetDesc/IPVolume.h"
 *
 *  Interface to deal with the notion of "positioned Logical Volume" 
 *  (corresponds to Geant4 notion of "G4PhysicalVolume").
 *  This is the minimal interface for "physical volume".
 *  It corresponds to Geant4 "Placement" philosophy.
 *
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxxx 
 */      

class IPVolume : virtual public IInterface
{
public:
  
  /** retrieve the unique interface identifier 
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IPVolume; }     
  
public:
  
  /** retrieve name of the physical volume
   *  (unique within mother logical volume)
   *  @return name of physical volume 
   */ 
  virtual const std::string&     name       () const = 0; 
  
  /**  retrieve the name of associated Logical Volume 
   *  @return name of associated Logical Volume 
   */
  virtual const std::string&     lvolumeName() const = 0;
  
  /**  retrieve  the C++ pointer to Logical Volume 
   *  @return pointer to Logical Volume 
   */
  virtual const ILVolume*        lvolume    () const = 0; 
  
  /** get the tranformation matrix   
   *  @return reference to transformation matrix 
   */ 
  virtual const Gaudi::Transform3D&  matrix     () const = 0;
  
  /** get the inverse transformation matrix
   *  @return reference to inverse transformationmatrix 
   */
  virtual const Gaudi::Transform3D&  matrixInv  () const = 0; 
  
  /** transform point from  Mother Reference System  to the Local one
   *  @param PointInMother point in Mother Reference System 
   *  @return point in local reference system 
   */ 
  virtual Gaudi::XYZPoint toLocal 
  ( const Gaudi::XYZPoint& PointInMother  ) const = 0;
  
  /** transform point in Local Reference System to the Mother Reference System
   *  @param PointInLocal point in Local Referency System
   *  @return point in mother reference system 
   */
  virtual Gaudi::XYZPoint toMother 
  ( const Gaudi::XYZPoint& PointInLocal   ) const = 0;
  
  /** check for 3D-point
   *  @param PointInMother pointin Mother Referency System 
   *  @return true if point is inside physical volume 
   */
  virtual bool isInside   
  ( const Gaudi::XYZPoint& PointInMother  ) const = 0;
  
  /** Intersection of the physical volume with with line.
   *  The line is parametrized in the local reference system of the mother
   *  logical volume ("Mother Reference System")  
   *  with initial Point and direction Vector: 
   *   - @f$ \vec{x}(t) = \vec{p} + t \times \vec{v} @f$ @n 
   *  
   * Method returns the number of intersection points("ticks") and 
   * the container of pairs - ticks and pointer to the corresponding 
   * material. @n 
   * The simplification is determined by value of threshold
   * (in units of radiation length) 
   *  
   *  @see ILVolume
   *  @see ISolid 
   *
   *  @exception PVolumeException wrong environment 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container 
   *  @param threshold threshold value 
   */
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint        & Point         ,
    const Gaudi::XYZVector       & Vector        , 
    ILVolume::Intersections & intersections ,
    const double              threshold     ) const = 0 ;
  
  /** Intersection of the physical volume with with line.
   *  The line is parametrized in the local reference system of the mother
   *  logical volume ("Mother Reference System")  
   *  with initial Point and direction Vector: 
   *   - @f$ \vec{x}(t) = \vec{p} + t \times \vec{v} @f$ @n 
   *  
   * Method returns the number of intersection points("ticks") and 
   * the container of pairs - ticks and pointer to the corresponding 
   * material. @n 
   * The simplification is determined by value of threshold
   * (in units of radiation length) 
   *  
   *  @see ILVolume
   *  @see ISolid 
   *
   *  @exception PVolumeException wrong environment 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container 
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param Threshold threshold value 
   */
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint        & Point         ,
    const Gaudi::XYZVector       & Vector        ,       
    ILVolume::Intersections & intersections ,      
    const ISolid::Tick        tickMin       ,
    const ISolid::Tick        tickMax       ,
    const double              Threshold     ) const = 0 ;
  
  /** printout to STD/STL stream
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut
  ( std::ostream & os = std::cout ) const = 0;
  
  /** printout to Gaudi MsgStream stream
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut
  ( MsgStream    & os             ) const = 0;
  
  /** reset to the initial state 
   *  @return self-reference
   */
  virtual IPVolume* reset () = 0 ;  
  
  /** Copy number
   *  for "Regular" case it is an ordering number 
   *  of physical volume withoin logical volume, 
   *  but it can be redefined for certain purposes, e.g. for Rich HPDs
   *  @return copy number 
   */
  //  virtual size_t    copy  () const = 0 ;
  
  /** apply the  misalignemnt to the transformation matrix 
   *  @param ma misalignment matrix (assumed to be small!!!)
   *  @return the resulting transformation matrix
   */
  virtual const Gaudi::Transform3D& 
  applyMisAlignment ( const Gaudi::Transform3D& ma ) = 0 ;
  
  /** reset the  misalignemnt 
   *  @return the "nominal" transformation matrix
   */
  virtual const Gaudi::Transform3D& 
  resetMisAlignment (                          ) = 0 ;
  
  /// virtual destructor
  virtual  ~IPVolume();  
  
};

// ============================================================================
/** output operator to STD/STL stream
 *  @param  os      reference to output   stream
 *  @param  pv      reference to IPVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline std::ostream& operator<<
( std::ostream& os , const IPVolume& pv ) 
{ return pv.printOut( os ); };

// ============================================================================
/** output operator to STD/STL stream
 *  @param  os      reference to output   stream
 *  @param  pv      pointer   to IPVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline std::ostream& operator<<
( std::ostream& os , const IPVolume*  pv )
{ return ((0==pv)?( os<<" IPVolume* points to NULL "):(os<<(*pv)));};

// ============================================================================
/** output operator to Gaudi MsgStream stream
 *  @param  os      reference to output   stream
 *  @param  pv      reference to IPVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline MsgStream& operator<<
( MsgStream& os , const IPVolume& pv ) 
{ return pv.printOut( os ); };

// ============================================================================
/** output operator to Gaudi MsgStream stream
 *  @param  os      reference to output   stream
 *  @param  pv      pointer   to IPVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline MsgStream& operator<<
( MsgStream& os , const IPVolume*  pv )
{ return ((0==pv)?( os<<" IPVolume* points to NULL "):(os<<(*pv)));};

// ============================================================================
// The End 
// ============================================================================
#endif   ///<   DETDESC_IPVOLUME_H 
// ============================================================================
