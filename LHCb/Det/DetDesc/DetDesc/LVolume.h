/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.13  2001/08/09 16:47:57  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
#ifndef     DETDESC_VOLUMES_LVOLUME_H
#define     DETDESC_VOLUMES_LVOLUME_H
///@{
/** STD and STL includes */
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm> 
///@} 
///@{ 
/** GaudiKernel includes */  
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/SmartRefVector.h" 
///@} 
///@{
/** DetDesc  includes */
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h" 
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume_predicates.h" 
#include "DetDesc/LVolumeException.h" 
#include "DetDesc/CLIDLVolume.h"
///@} 
///@{ 
/** forward declarations */
template <class TYPE> 
class DataObjectFactory;
class HepPoint3D; 
class HepVector3D; 
class HepRotation;
class HepTransform3D;
class ISvcLocator;
class IDataProviderSvc;
class GaudiException; 
class Surface;
class ITime;
///@} 

/** @class LVolume LVolume.h DetDesc/LVolume.h
 *
 *  A simple implementation of ILVolume interface 
 *
 *  @author  Vanya Belyaev
 */


class LVolume: public DataObject ,
               public ILVolume   , 
               public IValidity
{
  /// friend factory for instantiation 
  friend class DataObjectFactory<LVolume>;

public:
  
  /** @defgroup Constructors 
   *  public constructors/destructors  for class LVolume 
   *  @{ 
   */
  
  /** constructor, pointer to ISolid* must be valid!, 
   *  overvise constructor throws LVolumeException!  
   *  @exception LVolumeException wrong paramaters value
   *  @param name name of logical volume 
   *  @param Solid pointer to ISolid object 
   *  @param material name of the material 
   *  @param validSince  begin of validity range 
   *  @param validTill  end of validity range 
   *  @param sensitivity  name of sensitive detector object (for simulation)
   *  @param magnetic  nam eof magnetic field object (for simulation)
   */
  LVolume( const std::string& name             , 
           ISolid*            Solid            ,
           const std::string& material         ,
           const ITime&       validSince       , 
           const ITime&       validTill        , 
           const std::string& sensitivity = "" ,
           const std::string& magnetic    = "" );
  
  /** constructor, pointer to ISolid* must be valid!, 
   *  overvise constructor throws LVolumeException!  
   *  @exception LVolumeException wrong paramaters value
   *  @param name name of logical volume 
   *  @param Solid pointer to ISolid object 
   *  @param material name of the material 
   *  @param sensitivity  name of sensitive detector object (for simulation)
   *  @param magnetic  nam eof magnetic field object (for simulation)
   */
  LVolume( const std::string& name             , 
           ISolid*            Solid            ,
           const std::string& material         ,
           const std::string& sensitivity = "" ,
           const std::string& magnetic    = "" );

  /// destructor 
  virtual ~LVolume();
  /** @} end of group Constructors */ 
  
public:
  
  
  /** @defgroup DataObject 
   *  implementation of DataObject methods 
   *  @{ 
   */ 
  virtual const CLID& clID   () const ;
  static  const CLID& classID()       ;
  /** @} end of group Data Object */
  
  /** @defgroup ILVolume 
   *  Implementation of ILVolume interface 
   * @{
   */
  
  /**  retrieve  the name(identification)  of Logical Volume  
   *  @return    the name(identification)  of Logical Volume  
   */
  virtual const std::string&  name () const 
  { return DataObject::fullpath();}

  /**  return the solid, associated with the Logical Volume  
   *  @return the solid, associated with the Logical Volume  
   */
  virtual const ISolid*       solid        () const 
  { return m_lv_solid ; }
  
  /**  return the material, associated with the Logical Volume  
   *  @exception LVolumeException no material
   *  @return the material, associated with the Logical Volume  
   */
  virtual const Material*     material     () const 
  { 
    return m_lv_material = 
      0 != m_lv_material ? m_lv_material : findMaterial() ;
  };
  
  /**  return the material(by name), associated with the Logical Volume  
   *  @return the material(by name), associated with the Logical Volume  
   */
  virtual const std::string&  materialName () const 
  { return m_lv_materialName; }
  
  /**  return vector of physical volumes 
   *  @return vector of physical volumes 
   */
  virtual       PVolumes& pvolumes ()       
  { return m_lv_pvolumes ; } 

  /**  return vector of physical volumes (const version)
   *  @return vector of physical volumes 
   */
  virtual const PVolumes& pvolumes () const 
  { return m_lv_pvolumes ; }
  
  /**  return number of Physical(positioned) Volumes 
   *  @return number of Physical(positioned) Volumes
   */
  virtual ILVolume::ReplicaType noPVolumes () const
  { return m_lv_pvolumes.size() ; }
  
  /** get daughter (Physical Volume) by index
   *  @param  index    physical volume index 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual IPVolume* operator[]( const ILVolume::ReplicaType& index ) const
  {
    return m_lv_pvolumes.size() > index ? 
      *(m_lv_pvolumes.begin()+index) : 0 ;
  };
  
  /** get daughter (Physical Volume) by name 
   *  @param  name    physical volume name 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual IPVolume* operator[]( const std::string&           name  ) const
  { 
    ILVolume::PVolumes::const_iterator pvi = 
      std::find_if( m_lv_pvolumes.begin  () , 
                    m_lv_pvolumes.end    () , 
                    IPVolume_byName( name ) ) ;
    return m_lv_pvolumes.end() != pvi ? *pvi : 0 ;
  };

  /** get daughter (Physical Volume) by index
   *  @param  index    physical volume index 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual IPVolume* pvolume   ( const ILVolume::ReplicaType& index ) const
  {
    return m_lv_pvolumes.size() > index ? 
      *(m_lv_pvolumes.begin()+index) : 0 ;
  };
  
  /** get daughter (Physical Volume) by name 
   *  @param  name    physical volume name 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual IPVolume* pvolume   ( const std::string&           name  ) const  
  { 
    ILVolume::PVolumes::const_iterator pvi = 
      std::find_if( m_lv_pvolumes.begin  () , 
                    m_lv_pvolumes.end    () , 
                    IPVolume_byName( name ) ) ;
    return m_lv_pvolumes.end() != pvi ? *pvi : 0 ;
  };
  
  /**  retrieve begin iterator  for manipulation with daughters
   *   @return begin iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::iterator       pvBegin     ()
  { return m_lv_pvolumes.begin () ;}
  
  
  /**  retrieve begin iterator  for manipulation with daughters
   *   (const version)
   *   @return begin iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::const_iterator pvBegin     () const
  { return m_lv_pvolumes.begin () ;}
  
  /**  retrieve end iterator  for manipulation with daughters
   *   @return end iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::iterator       pvEnd       ()
  { return m_lv_pvolumes.end ()  ;}
  
  /**  retrieve end iterator  for manipulation with daughters
   *   (const version)
   *   @return end iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::const_iterator pvEnd       () const 
  { return m_lv_pvolumes.end () ;}
  
  /** traverse the sequence of paths  \n 
   *  transform the sequence of replicas to sequence of  physical volumes 
   *  @param replicaPath replica-Path 
   *  @param volumePath  vector of physical volumes 
   *  @return status code 
   */
  virtual StatusCode 
  traverse ( ILVolume::ReplicaPath::const_iterator pathBegin,
             ILVolume::ReplicaPath::const_iterator pathEnd  ,
             ILVolume::PVolumePath&                pVolumePath );
  
  /** traverse the sequence of paths  \n 
   *  transform the sequence of replicas to sequence of  physical volumes 
   *  @param replicaPath replica-Path 
   *  @param volumePath  vector of physical volumes 
   *  @return status code 
   */
  virtual StatusCode 
  traverse ( const ILVolume::ReplicaPath&  path,
             ILVolume::PVolumePath&        pVolumePath )
  { return traverse( path.begin() , path.end() , pVolumePath ); }
  
  /** check for the given 3D-point. 
   *  Point coordinated are in the local reference 
   *  frame of the solid.   
   *  @param LocalPoint point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  virtual bool isInside ( const HepPoint3D& LocalPoint ) const
  { return m_lv_solid->isInside( LocalPoint ); }
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @param  localPoint point in local reference system of logical volume 
   *  @param  level depth level
   *  @param  volumePath  vector of physical volumes
   *  @return status code 
   */
  virtual StatusCode
  belongsTo( const HepPoint3D&        LocalPoint ,
             const int                Level      , 
             ILVolume::PVolumePath&   pVolumePath );
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @param  localPoint point in local reference system of logical volume 
   *  @param  level depth level
   *  @param  volumePath  vector of physical volumes
   *  @return status code 
   */
  virtual StatusCode 
  belongsTo( const HepPoint3D&        LocalPoint ,
             const int                Level      , 
             ILVolume::ReplicaPath&   replicaPath );
  
  /** intersection of the logical volume with with the line \n
   *  Theine is parametrized in the local reference system 
   *  of the logical volume by initial Point and direction Vector \n 
   *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
   * 
   *   Method returns the number of intersection points("ticks") and 
   *   the container of pairs - ticks and pointer to the corresponding 
   *   material. The simplification is determined by value of threshold
   *   (in units of radiation length) 
   *   Method throws LVolumeException in the case, then 
   *   solid is not defined or material is not accessible.
   *
   *  @exception LVolumeException solid or/and matherial problems 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container 
   *  @param threshold threshold value 
   *  @return number of intersections  
   */
  virtual unsigned int 
  intersectLine( const HepPoint3D        & Point         , 
                 const HepVector3D       & Vector        , 
                 ILVolume::Intersections & intersections , 
                 const double              threshold     );
  
  /** intersection of the logical volume with with the line \n
   *  Theine is parametrized in the local reference system 
   *  of the logical volume by initial Point and direction Vector \n 
   *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
   * 
   *   Method returns the number of intersection points("ticks") and 
   *   the container of pairs - ticks and pointer to the corresponding 
   *   material. The simplification is determined by value of threshold
   *   (in units of radiation length) 
   *   Method throws LVolumeException in the case, then 
   *   solid is not defined or material is not accessible.
   *
   *  @exception LVolumeException solid or/and matherial problems 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param threshold threshold value 
   *  @return number of intersections  
   */
  virtual unsigned int 
  intersectLine( const HepPoint3D         & Point        , 
                 const HepVector3D        & Vector       , 
                 ILVolume::Intersections & intersections , 
                 const ISolid::Tick        tickMin       , 
                 const ISolid::Tick        tickMax       , 
                 const double              Threshold     );
  
  
  /**         name of sensitive "detector" - needed for simulation 
   *  @return name of sensitive "detector"
   */
  virtual const std::string& sdName () const 
  { return m_lv_sdName; } ;
  
  /** magnetic properties  (if needed for simulation)  
   *  @return name of magnetic field  object
   */
  virtual const std::string& mfName () const 
  { return m_lv_mfName; } ;
  
  /** accessors to surfaces 
   *  @return vector of surfaces 
   */  
  virtual       Surfaces& surfaces()       
  { return m_lv_surfaces ; }
  
  /** accessors to surfaces  (const version) 
   *  @return vector of surfaces 
   */  
  virtual const Surfaces& surfaces() const  
  { return m_lv_surfaces ; }

  /** printout to STD/STL stream
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut( std::ostream & os = std::cout) const ;
  
  /** printout to Gaudi MsgStream stream
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut( MsgStream    & os ) const;

  /** reset to initila state, 
   *  clear chaches, etc...
   *  @return self reference
   */
  virtual ILVolume* reset () 
  {
    /// reset solid 
    if( 0 != m_lv_solid ) { m_lv_solid->reset() ; }
    /// reset material 
    m_lv_material = 0 ;
    /// reset all physical volumes 
    std::for_each( m_lv_pvolumes.begin ()         , 
                   m_lv_pvolumes.end   ()         ,
                   std::mem_fun(&IPVolume::reset) ) ;
    /// return self-reference
    return this;
  };
  /**@} end of group ILVolume */ 
  
  /** @defgroup Serialize 
   *  serialize methods from DataObject and ISerialize inetrface
   *  @see DataObject
   *  @see ILVolume 
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


  /** @defgroup IValidity
   *  Implementation of IValidity interface
   *  @see IValidity 
   *  @{ 
   */
  virtual bool         isValid          ()                ;   
  virtual bool         isValid          ( const ITime& )  ;     
  virtual const ITime& validSince       ()                ;    
  virtual const ITime& validTill        ()                ;   
  virtual void         setValidity      ( const ITime& , 
                                          const ITime& )  ;  
  virtual void         setValiditySince ( const ITime& )  ;  
  virtual void         setValidityTill  ( const ITime& )  ;   
  virtual StatusCode   updateValidity   ()                ; 
  /** @} end of group IValidity */  
  
  /** @defgroup IInspectable  
   * implementation of IInspectable interface 
   * @see IInspectable 
   * @see ILVolume 
   * @{ 
   */
  virtual bool acceptInspector( IInspector* pInspector )       ;
  virtual bool acceptInspector( IInspector* pInspector ) const ;
  /** @} end of group IInspectable */
  
  /** @defgroup IInterface 
   *  implementations of vurtual functions from class IInterface 
   *  @see IInterface
   *  @see ILVolume 
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

  /** @defgroup Create 
   *  create daughter physical volume 
   *  @{ 
   */
  IPVolume* createPVolume( const std::string&    PVname                    , 
                           const std::string&    LVnameForPV               ,
                           const HepPoint3D&     position = HepPoint3D  () ,  
                           const HepRotation&    rotation = HepRotation () ); 
  IPVolume* createPVolume( const std::string&    PVname                    , 
                           const std::string&    LVnameForPV               ,
                           const HepTransform3D& Transform                 );
  /** @} end of group Create */
  
  /** @defgroup Multy 
   * create a parametric group of physical volumes 
   * @{
   */  

  /** create one-parametric group of physical volumes 
   *  @param PVname_base base name for the group 
   *  @param LVnameForPV name of logical lvolume 
   *  @param nStep number of copies
   *  @param initialPosition positionof the first clone  
   *  @param initialRotation rotation of the first clone
   *  @param stepTranslation relative position of next clone 
   *  @param stepRotation relative rotation of next clone 
   *  @return pointer to physical volume 
   */
  IPVolume* createMultiPVolume( const std::string&   PVname_base     , 
                                const std::string&   LVnameForPV     , 
                                const unsigned long  nStep           , 
                                const HepPoint3D&    initialPosition ,
                                const HepRotation&   initialRotation ,     
                                const HepVector3D&   stepTranslation ,
                                const HepRotation&   stepRotation    );
  
  /** create two-parametric group of physical volumes 
   *  @param PVname_base base name for the group 
   *  @param LVnameForPV name of logical lvolume 
   *  @param initialPosition positionof the first clone  
   *  @param initialRotation rotation of the first clone
   *  @param nStep1 number of copies for first parameter 
   *  @param stepTranslation1 relative position of next clone 
   *  @param stepRotation1 relative rotation of next clone 
   *  @param nStep2 number of copies for first parameter 
   *  @param stepTranslation2 relative position of next clone 
   *  @param stepRotation2 relative rotation of next clone 
   *  @return pointer to physical volume 
   */ 
  IPVolume* createMultiPVolume( const std::string&   PVname_base      , 
                                const std::string&   LVnameForPV      , 
                                const HepPoint3D&    initialPosition  ,
                                const HepRotation&   initialRotation  ,     
                                const unsigned long  nStep1           , 
                                const HepVector3D&   stepTranslation1 ,
                                const HepRotation&   stepRotation1    ,
                                const unsigned long  nStep2           , 
                                const HepVector3D&   stepTranslation2 ,
                                const HepRotation&   stepRotation2    );

  /** create three-parametric group of physical volumes 
   *  @param PVname_base base name for the group 
   *  @param LVnameForPV name of logical lvolume 
   *  @param initialPosition positionof the first clone  
   *  @param initialRotation rotation of the first clone
   *  @param nStep1 number of copies for first parameter 
   *  @param stepTranslation1 relative position of next clone 
   *  @param stepRotation1 relative rotation of next clone 
   *  @param nStep2 number of copies for first parameter 
   *  @param stepTranslation2 relative position of next clone 
   *  @param stepRotation2 relative rotation of next clone 
   *  @param nStep3 number of copies for first parameter 
   *  @param stepTranslation3 relative position of next clone 
   *  @param stepRotation3 relative rotation of next clone 
   *  @return pointer to physical volume 
   */ 
  IPVolume* createMultiPVolume( const std::string&   PVname_base      , 
                                const std::string&   LVnameForPV      , 
                                const HepPoint3D&    initialPosition  ,
                                const HepRotation&   initialRotation  ,     
                                const unsigned long  nStep1           , 
                                const HepVector3D&   stepTranslation1 ,
                                const HepRotation&   stepRotation1    ,
                                const unsigned long  nStep2           , 
                                const HepVector3D&   stepTranslation2 ,
                                const HepRotation&   stepRotation2    ,
                                const unsigned long  nStep3           , 
                                const HepVector3D&   stepTranslation3 ,
                                const HepRotation&   stepRotation3    );
  /** @} end of group Multy */
  
  /** set solid for logical volume
   *  @param solid pointer to ISolid object
   *  @return pointer to ISolid object 
   */
  const ISolid* setSolid( ISolid* solid );
  
protected:
  
  /** constructor
   *  @param Name name of logical volume 
   */
  LVolume( const std::string& Name = "");       

  /** create an empty PVolume 
   *  return reference to new PVolume 
   */
  IPVolume* createPVolume();
  
private: 
  
  /// copy constructor is private! 
  LVolume           ( const LVolume& );
  /// assignment operator is private! 
  LVolume& operator=( const LVolume& ); 
  
private:
  
  /// find C++ pointer to material by it's name 
  Material* findMaterial() const ;
  
  /** @defgroup Assertions 
   *  inline  assertion methods
   *  @{ 
   */
  /** Assertion
   *  @param assertion condition 
   *  @param name      exception message
   *  @param sc        status code 
   */
  inline void 
  Assert( bool               assertion                       , 
          const std::string& name                            ,
          const StatusCode&  sc        = StatusCode::FAILURE ) const
  { 
    if( !assertion ) { throw LVolumeException( name, this , sc ); }
  };
  
  /** Assertion
   *  @param assertion condition 
   *  @param name      exception message
   *  @param Exception previous exception 
   *  @param sc        status code 
   */
  inline void 
  Assert( bool                  assertion , 
          const std::string&    name      ,
          const GaudiException& Exception , 
          const StatusCode&     sc        = StatusCode::FAILURE ) const  
  { 
    if( !assertion ) 
      { throw LVolumeException( name, Exception , this , sc ); }
  };
  /** @} end of group Assertions */ 

  /// Auxillary method  to calculate intersections with daughter volumes  
  unsigned int  
  intersectDaughters( const HepPoint3D&        Point              , 
                      const HepVector3D&       Vector             , 
                      ILVolume::Intersections& childIntersections , 
                      const ISolid::Tick       tickMin            , 
                      const ISolid::Tick       tickMax            , 
                      const double             Threshold          );  
  /// Auxillary method  to calculate intersections with daughter volumes  
  unsigned int  
  intersectDaughters( const HepPoint3D&        Point              , 
                      const HepVector3D&       Vector             , 
                      ILVolume::Intersections& childIntersections , 
                      const double             Threshold          );  
  
protected:
  
  /** static accessor to 
   *  data service used for retriving of the material 
   *  @return pointer to data service 
   */
  static IDataProviderSvc* dataSvc()  ;
  
private:
  
  /// solid 
  ISolid*            m_lv_solid        ; 
  // physical volumes 
  PVolumes           m_lv_pvolumes     ; 
  /// surfaces 
  Surfaces           m_lv_surfaces     ; 
  ///@{ 
  /** material */
  std::string        m_lv_materialName ; 
  mutable  Material* m_lv_material     ; 
  ///@}
  /// name of sensitive detector object 
  std::string        m_lv_sdName       ;
  /// name of magnetic field source 
  std::string        m_lv_mfName       ;
  
  ///@{ 
  /**  IValidity */ 
  ITime* m_lv_validSince   ;
  ITime* m_lv_validTill    ; 
  ///@}

  /// reference counter 
  static unsigned long  s_count ;
};

/// ===========================================================================
#endif  ///< DETDESC_LVOLUME_H
/// ===========================================================================








