// $Id: GeometryInfo.h,v 1.10 2003-06-16 13:42:36 sponce Exp $ 
#ifndef     DETDESC_GEOMETRYINFO_H
#define     DETDESC_GEOMETRYINFO_H 1 
/** STD and STL includes */ 
#include <iostream>
#include <string>
#include <functional>
#include <algorithm> 
/** CLHEP includes */ 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
/* GaudiKernel includes */
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
/** DetDesc includes */ 
#include "DetDesc/Services.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/IGeometryInfo.h" 
#include "DetDesc/IDetectorElement.h" 

/** forward declarations */
class GaudiException   ;
class IDataProviderSvc ;

/** @class GeometryInfo GeometryInfo.h DetDesc/GeometryInfo.h
 *
 *  The most trivial implementation of IGeometryInfo abstract  interface 
 *  
 *  @attention almost all methods of the class 
 *         are inline and non-virtual! Do not try to 
 *         inherit from this class withour seriouse modifications! 
 *
 *  @author Vanya Belyaev 
 *  @author Sebastien Ponce
 */

class GeometryInfo:   public IGeometryInfo
{
  ///  friend class 
  friend class DetectorElement;

public:
  
  /// type of vector of children's names 
  typedef  std::vector<std::string>     ChildName;
  
  /** @defgroup GeometryInfoConstructors
   *  constructors and destructor for classGeometry Info @n
   *  all of them are protected!
   * @{ 
   */ 
  
  /** constructor, creates "ghost"
   *  @param de pointer to detector element 
   */
  GeometryInfo( IDetectorElement* de ); 
  
  /** constructor, creates "orphan"    
   *  @param de     pointer to detector element 
   *  @param LogVol name of logical volume 
   */
  GeometryInfo( IDetectorElement*   de     ,
                const std::string&  LogVol ); 
  
  /** constructor, create regular element 
   *  @param de     pointer to detector element 
   *  @param LogVol name of logical volume
   *  @param Support name of support element 
   *  @param ReplicaPath replica path/address 
   */
  GeometryInfo( IDetectorElement*            de          ,
                const std::string&           LogVol      , 
                const std::string&           Support     ,
                const ILVolume::ReplicaPath& ReplicaPath );
  
  /** constructor, create regular element 
   *  @param de              pointer to detector element 
   *  @param LogVol          name of logical volume
   *  @param Support         name of support element 
   *  @param ReplicaNamePath replica path/address 
   */
  GeometryInfo( IDetectorElement*  de              ,
                const std::string& LogVol          , 
                const std::string& Support         ,
                const std::string& ReplicaNamePath );
  /// destructor 
  virtual ~GeometryInfo();
  ///@} end of group GeometryInfoConstructors
  
private:

  /// copy constructor is private! 
  GeometryInfo           ( const GeometryInfo& );
  /// assignment is private! 
  GeometryInfo& operator=( const GeometryInfo& );
  
public:
  
  /** @defgroup GeometryInfoIGeometryInfo
   *  implementation of methods from IGeometryInfo interface
   *  @see IGeometryInfo 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @{ 
   */
  /// Is this "geometry object" assosiated with Logical Volume?
  inline bool hasLVolume   () const ;   //     
                        
  /// Is this "geometry object" supported?
  inline bool hasSupport   () const ;   //                             
  /** transformation matrix from global reference 
   *  system to the local one 
   */
  inline const HepTransform3D&  matrix       () const ; 
  /** transformation matrix from local  reference 
   *  system to the global one 
   */
  inline const HepTransform3D&  matrixInv    () const ; 
  /** tranform the point from the global reference systemn 
   * to the local  reference system
   */  
  inline HepPoint3D toLocal  
  ( const HepPoint3D& globalPoint ) const ;       
  
  /** tranform the point from the local  reference system 
   *  to the global reference system
   */  
  inline HepPoint3D toGlobal 
  ( const HepPoint3D& localPoint  ) const ;       

  /** is the given point in the global reference system 
   * inside this detector element?
   */
  inline bool isInside     
  ( const HepPoint3D& globalPoint ) const ;      

  /**  return the name of the daughter element to which 
   *  the given point belongs to 
   */
  inline std::string belongsToPath
  ( const HepPoint3D& globalPoint );      
  
  /** return the C++ pointer to the daughter element to 
   *  which the given point belongs to
   */    
  inline IGeometryInfo* belongsTo 
  ( const HepPoint3D& globalPoint );    
  
  /** return the name of the daughter element to which 
   *  the given point belongs to (taking into account the level)
   */
  inline std::string belongsToPath
  ( const HepPoint3D& globalPoint ,       
    const int         level       );       

  /** return the C++ pointer to the daughter element to 
   *  which the given point belongs to  (taking into account the level)   
   */
  inline IGeometryInfo* belongsTo 
  ( const HepPoint3D& globalPoint ,        
    const int         level       );      

  /** return the full geometry info for a given point in the 
   * global reference system 
   */
  inline StatusCode fullGeoInfoForPoint
  ( const HepPoint3D&        point      , 
    const int                level      , 
    IGeometryInfo*&          start      , 
    ILVolume::PVolumePath&   volumePath );       

  /**  return the full geometry info for a given point in the
   *   global reference system 
   */
  inline StatusCode fullGeoInfoForPoint
  ( const HepPoint3D&        point      , 
    const int                level      , 
    IGeometryInfo*&          start      , 
    ILVolume::ReplicaPath&   volumePath );      

  /** return the full geometry info for a given point in the 
   *  global reference system 
   */
  inline StatusCode fullGeoInfoForPoint
  ( const HepPoint3D&        point      , 
    const int                level      , 
    std::string&             start      , 
    ILVolume::PVolumePath&   volumePath );       

  /** return the full geometry info for a given point in the
   *  global reference system 
   */
  inline StatusCode fullGeoInfoForPoint
  ( const HepPoint3D&        point      , 
    const int                level      , 
    std::string&             start      , 
    ILVolume::ReplicaPath&   volumePath );     

  /// return the name of the assosiated logical volume 
  inline const std::string&      lvolumeName() const ;

  /// return the C++ pointer4 to the assosiated logical volume 
  inline const ILVolume*         lvolume    () const ;
  
  /// return the location (address) of this geometry object  
  inline StatusCode location   
  ( IGeometryInfo*&              start , 
    ILVolume::ReplicaPath&       replicaPath ) const ;   

  /// return the location (address) of this geometry object  
  inline StatusCode location   
  ( std::string&                 start , 
    ILVolume::ReplicaPath&       replicaPath ) const ;  
 
  /// return the name of the logical volume with a given address 
  inline std::string lvolumePath
  ( const std::string&           start       , 
    const ILVolume::ReplicaPath& replicaPath );   

  /// return the C++ pointer to  the logical volume with a given address 
  inline const ILVolume* lvolume    
  ( const std::string&           start       , 
    const ILVolume::ReplicaPath& replicaPath );    
  
  /// return the name of the logical volume with a given address 
  inline std::string lvolumePath
  ( IGeometryInfo*               start       , 
    const ILVolume::ReplicaPath& replicaPath );   

  /// return the C++ pointer to  the logical volume with a given address 
  inline const ILVolume* lvolume 
  ( IGeometryInfo*               start       , 
    const ILVolume::ReplicaPath& replicaPath );

  /// pointer to the parent IGeometryInfo 
  inline IGeometryInfo* parentIGeometryInfo()       ;

  /// pointer to the parent IGeometryInfo (const version)
  inline const IGeometryInfo* parentIGeometryInfo() const ;

  /// (reference to) container of children IGeometryInfo 
  inline IGeometryInfo::IGIChildrens& childIGeometryInfos()       ;

  /// (reference to) container of children IGeometryInfo 
  inline const IGeometryInfo::IGIChildrens& childIGeometryInfos() const ; 

  /// iterators for manipulation of daughter elements 
  /// begin iterator
  inline IGeometryInfo::IGIChildrens::iterator        childBegin()       ; 

  /// begin iterator (const version)
  inline IGeometryInfo::IGIChildrens::const_iterator  childBegin() const ; 

  /// end  iterator 
  inline IGeometryInfo::IGIChildrens::iterator        childEnd  ()       ; 

  /// end  iterator (const version)
  inline IGeometryInfo::IGIChildrens::const_iterator  childEnd  () const ; 

  /// IInspectable interface
  bool acceptInspector( IInspector* )       ; 
  bool acceptInspector( IInspector* ) const ; 
  /// serialization  for reading 
  StreamBuffer& serialize( StreamBuffer& )       ; 
  /// serialization  for writing 
  StreamBuffer& serialize( StreamBuffer& ) const ;
  /// overloades printout to  std::ostream 
  std::ostream& printOut   ( std::ostream& ) const;
  /// overloaded printout to the MsgStream
  MsgStream&    printOut   ( MsgStream&    ) const;
  /// reset to the initial state 
  IGeometryInfo*  reset();
  ///@}
  /** query the interface
   *  @param ID interface identifier
   *  @param ppI placeholder for returned interface
   *  @return status code
   */ 
  StatusCode queryInterface( const InterfaceID& ID , void** ppI ) ;
  /// add reference
  unsigned long addRef();
  /// release 
  unsigned long release();
  
 private:
  ///
  inline bool isInsideDaughter 
  ( const HepPoint3D& globalPoint ) const ;   

  inline bool childLoaded      () const { return m_gi_childLoaded ; }     

  inline StatusCode        reset            ( const int Level );         

  IGeometryInfo* geoByName ( const std::string& name ) const ;    

  ILVolume*         findLogical      () const ;          

  HepTransform3D*   calculateMatrix  () const ;       

  /// could throw GeometryInfoException
  StatusCode loadChildren     () const ; 

  /// retrive the replica path (mistrerious "rpath")
  inline        ILVolume::ReplicaPath& supportPath     () const ; 

  /// Assertion 
  inline void Assert 
  ( bool assertion          , 
    const std::string& name ) const; 
  
  /// Assertion 
  inline void Assert 
  ( bool assertion           , 
    const std::string& name  , 
    const GaudiException& ge ) const;
 
  inline IDetectorElement* detElem          () const 
  { return m_gi_iDetectorElement ; }
  
private:
  
  IDataProviderSvc* dataSvc () const;
  
private:
  
  /// flag for logical volume association 
  bool                                 m_gi_has_logical     ;    
  /** name of logical volume ( full path (address) 
   *  in the Transient Store )  
   */
  std::string                          m_gi_lvolumeName     ;  
  /// C++ pointer to the logical volume (loaded on demand only!)
  mutable ILVolume*                    m_gi_lvolume; 
  /** transformation from the  global reference system 
   *  to the local  reference system
   */
  mutable HepTransform3D*              m_gi_matrix          ;
  /** transformation FROM local reference system  to the global 
   *  reference system
   */
  mutable HepTransform3D*              m_gi_matrixInv       ; 
  /// flag for support association 
  bool                                 m_gi_has_support     ; 
  /**  name of DetectorElement (full path(address) in the 
   *   Transient Store) , which supports the addres)
   */
  std::string                          m_gi_supportName     ;   
  /// C++ pointer to element, which supports the address (loaded on demand) 
  mutable IGeometryInfo*               m_gi_support         ;  
  /// the address itself (numeric replic apath) 
  mutable ILVolume::ReplicaPath        m_gi_supportPath     ;   
  /// another form of address  (name replica path)
  std::string                          m_gi_supportNamePath ;   
  //
  ///  The corresponding IDtectorElement object 
  mutable IDetectorElement*            m_gi_iDetectorElement ;

  /// flag for  parent object 
  mutable bool                         m_gi_parentLoaded   ;
  /// C++ pointer to parent object (resolved on demand only) 
  mutable IGeometryInfo*               m_gi_parent         ; 
  /// flag for children objects 
  mutable bool                         m_gi_childLoaded    ;
  /**  container of pointers to children objects 
   *  (resolved on demand only)
   */
  mutable IGeometryInfo::IGIChildrens  m_gi_childrens      ;
  /** container of names of children objects 
   * (resolved on demand only)
   */
  mutable GeometryInfo::ChildName      m_gi_childrensNames ;
  
  /// object/reference counter  
  static unsigned long m_count;
  
  /// reference to services
  DetDesc::Services* m_services;
  
};
///
#include "GeometryInfo.icpp" 
///
#endif   // __DETDESC_GEOMETRYINFO_GEOMETRYINFO_H__




