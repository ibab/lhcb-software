/// ===========================================================================
/// CVS tag  $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/08/09 16:48:01  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
#ifndef    DETDESC_PVOLUME_H
#define    DETDESC_PVOLUME_H 1 
///@{
/** STD & STL */
#include <iostream>
#include <string>
///@}
///@{
/** CLHEP includes */
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
///@}
///@{
/**  GaudiKernel includes */  
#include "GaudiKernel/DataObject.h" 
///@}
///@{
/** DetDesc includes */
#include "DetDesc/IPVolume.h"
#include "DetDesc/ILVolume.h"
///@}
///@{
/** forward declarations */
class IDataProviderSvc;
class IMessageSvc;
class MsgStream;
class GaudiException;
class PVolumeException;
///@}


/** @class PVolume PVolume.h 
 *  
 *  simple implementation of IPVolume interface
 *  @see IPVolume 
 * 
 *  @author: Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxx
 */ 

class PVolume: public IPVolume
{
  /// friend class 
  friend class LVolume;

protected:

  /** constructor    
   *  @param PhysVol_name name of phys volume 
   *  @param LogVol_name name of log volume 
   *  @param Position position of physical volume inside mother volume 
   *  @param Rotation  rotation of physical volume with respect to mother 
   */
  PVolume ( const std::string& PhysVol_name                  ,
            const std::string& LogVol_name                   ,
            const HepPoint3D&  Position     = HepPoint3D  () ,
            const HepRotation& Rotation     = HepRotation () );

  /** constructor    
   *  @param PhysVol_name name of phys volume 
   *  @param LogVol_name name of log volume 
   *  @param Transform transformation matrix 
   */
  PVolume ( const std::string&     PhysVol_name ,
            const std::string&     LogVol_name  ,
            const HepTransform3D&  Transform    ); 
  /// destructor 
  virtual ~PVolume();

public:


  /** retrieve name of the physical volume
   *  (unique within mother logical volume)
   *  @return name of physical volume 
   */ 
  virtual const std::string& name () const 
  { return m_pv_name ; };
  
  /**  retrieve the name of associated Logical Volume 
   *  @return name of associated Logical Volume 
   */
  virtual const std::string& lvolumeName() const 
  { return m_pv_lvname ; };

  /**  retrieve  the C++ pointer to Logical Volume 
   *  @return pointer to Logical Volume 
   */
  virtual ILVolume* lvolume () const
  { return 
      0 != m_pv_lvolume  ? m_pv_lvolume : m_pv_lvolume = findLogical() ; 
  };

  /** get the tranformation matrix   
   *  @return reference to transformation matrix 
   */ 
  virtual const HepTransform3D&  matrix () const
  { return m_pv_matrix ; }

  /** get the inverse transformation matrix
   *  @return reference to inverse transformationmatrix 
   */
  virtual const HepTransform3D&  matrixInv  () const 
  {
    if( 0 == m_pv_imatrix ) { m_pv_imatrix = findMatrix() ; }
    return *m_pv_imatrix ;
  };
  
  /** transform point from  Mother Reference System  to the Local one
   *  @param PointInMother point in Mother Reference System 
   *  @return point in local reference system 
   */ 
  virtual HepPoint3D toLocal ( const HepPoint3D& PointInMother ) const 
  { return m_pv_matrix * PointInMother ; }
  
  /** transform point in Local Reference System to the Mother Reference System
   *  @param PointInLocal point in Local Referency System
   *  @return point in mother reference system 
   */
  virtual HepPoint3D toMother ( const HepPoint3D& PointInLocal  ) const 
  {
    if( 0 == m_pv_imatrix ) { m_pv_imatrix = findMatrix() ; }  
    return (*m_pv_imatrix) * PointInLocal ;
  };
  
  /** check for 3D-point
   *  @param PointInMother pointin Mother Referency System 
   *  @return true if point is inside physical volume 
   */
  virtual bool isInside   ( const HepPoint3D& PointInMother ) const 
  {
    if( 0 == m_pv_lvolume ) { m_pv_lvolume = findLogical() ; }
    return m_pv_lvolume->isInside( toLocal( PointInMother ) ) ;
  };
  
  /** printout to STD/STL stream 
   *  @param os reference to STD/STL stream 
   *  @return reference to STD/STL stream 
   */
  virtual std::ostream& printOut( std::ostream& os = std::cout ) const;
  
  /** printout to Gaudi stream 
   *  @param os reference to Gaudi stream 
   *  @return reference to Gaudi stream 
   */
  virtual MsgStream&    printOut( MsgStream&    os             ) const;
  
  /** reset to the initial state 
   *  @return self-reference
   */
  inline IPVolume* reset () 
  {
    if( 0 != m_pv_lvolume ) { m_pv_lvolume->reset() ; }
    m_pv_lvolume = 0 ;
    if( 0 != m_pv_imatrix ) { delete m_pv_imatrix ; m_pv_imatrix = 0 ; }
    return this;
  };
  
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
  virtual unsigned int 
  intersectLine( const HepPoint3D        & Point         ,
                 const HepVector3D       & Vector        , 
                 ILVolume::Intersections & intersections ,
                 const double              threshold     )
  {
    return lvolume()->intersectLine( matrix() * Point  , 
                                     matrix() * Vector , 
                                     intersections     , 
                                     threshold         ); 
  };
  
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
  virtual unsigned int 
  intersectLine( const HepPoint3D        & Point ,
                 const HepVector3D       & Vector        ,       
                 ILVolume::Intersections & intersections ,      
                 const ISolid::Tick        tickMin       ,
                 const ISolid::Tick        tickMax       ,
                 const double              threshold     ) 
  { 
    return lvolume()->intersectLine( matrix() * Point    , 
                                     matrix() * Vector   , 
                                     intersections       , 
                                     tickMin             , 
                                     tickMax             ,
                                     threshold           );
  };
  
  /** @defgroup IInterface 
   *  implementations of vurtual functions from class IInterface 
   *  @see IInterface
   *  @see IPVolume 
   *  @{ 
   */
  /** query the interface
   *  @param ID unique interface identifier 
   *  @param ppI placeholder for returned interface
   *  @return status code 
   */
  virtual StatusCode 
  queryInterface( const InterfaceID& ID , void** ppI ) ;
  /** add the reference
   *  @return reference counter 
   */
  virtual unsigned long addRef  ();
  
  /** release the interface 
   *  @return reference counter 
   */
  virtual unsigned long release ();  
  /** @} end of group IInterface */
  
  ///  from IInspectable interface
  virtual bool acceptInspector( IInspector* pInspector )       ; 
  virtual bool acceptInspector( IInspector* pInspector ) const ; 
  ///

  /** @defgroup Serialize 
   *  method from ISerialize interface 
   *  @see IPVolume 
   *  @see ISerialize
   *  @{ 
   */

  /** serialization for reading 
   *  - implementation of DataObject method
   *  - implementation of ISerialize interface
   *  @see DataObject
   *  @see ILVolume 
   *  @see ISerialize 
   *  @param s reference to stream buffer 
   *  @return reference to stream buffer 
   */ 
  virtual StreamBuffer& serialize(StreamBuffer& s );

  /** serialization for writing 
   *  - implementation of DataObject method
   *  - implementation of ISerialize interface
   *  @see DataObject
   *  @see ILVolume 
   *  @see ISerialize 
   *  @param s reference to stream buffer 
   *  @return reference to stream buffer 
   */ 
  virtual StreamBuffer& serialize(StreamBuffer& s )  const;
  /** @} end of group Serialize */ 


protected:
  
  /** find logical volume by name 
   *  @return pointet to logical volume 
   */
  ILVolume*       findLogical () const ; 
  
  /** inverse the matrix
   *  @return pointer to inverse matrix 
   */
  HepTransform3D* findMatrix  () const ;
  
  /** @defgroup Assertions 
   *  assertion methods for class PVolume 
   *  @{ 
   */ 
  /** Assertion 
   *  @exception PVolumeException for wrong condition 
   *  @param assertion condition 
   *  @param name      exception message
   */
  void Assert( bool               assertion , 
               const std::string& name      ) const;
  
  
  /** Assertion 
   *  @exception PVolumeException for wrong condition 
   *  @param assertion condition 
   *  @param name       exception message
   *  @param Exception  previous exception 
   */  
  void Assert( bool                  assertion , 
               const std::string&    name      ,
               const GaudiException& Exception ) const;
  ///@} end of group Assertions 
  
protected:
  
  /** the static accessor to the data service
   *  @return pointer to data service 
   */
  static IDataProviderSvc* dataSvc();
  
private:
  
  /// no default constructor 
  PVolume()                           ;
  /// no public copy constructor 
  PVolume           ( const PVolume& );
  /// no public assignment
  PVolume& operator=( const PVolume& );
  
private:
  
  /// name of physical volume 
  std::string             m_pv_name    ;
  /// name of logical volume 
  std::string             m_pv_lvname  ;
  /// transformation matrix 
  HepTransform3D          m_pv_matrix  ;
  /// pointer to inverse transformation matrix 
  mutable HepTransform3D* m_pv_imatrix ;
  /// pointer to logical volume 
  mutable ILVolume*       m_pv_lvolume ;
  /// reference/object counter 
  static unsigned long    s_count      ;
};

/// ===========================================================================
#endif  ///< DETDESC_PVOLUME_H
/// ===========================================================================

