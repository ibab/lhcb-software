// $Id: LogVolBase.h,v 1.16 2007-01-17 12:10:13 cattanem Exp $ 
#ifndef     DETDESC_LOGVOLBASE_H
#define     DETDESC_LOGVOLBASE_H
/// STD and STL includes
#include <functional>
#include <algorithm> 
/// GaudiKernel includes 
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/Transform3DTypes.h"
/// DetDesc  includes
#include "DetDesc/ISolid.h" 
#include "DetDesc/Services.h" 
#include "DetDesc/IPVolume.h"
#include "DetDesc/IPVolume_predicates.h" 
#include "DetDesc/LogVolumeException.h"
#include "DetDesc/Surface.h"
#include "DetDesc/ValidDataObject.h"

/// forward declarations 
class IDataProviderSvc;
class IMessageSvc;

/** @class LogVolBase LogVolBase.h DetDesc/LogVolBase.h 
 *
 *  The common base class for ILVolume implementations:
 *    - LVolume 
 *    - LAssembly 
 * 
 *  @author  Vanya Belyaev Ivan.Belyaev 
 *  @author  Sebastien Ponce
 *  @author  Marco Clemencic
 */

class LogVolBase: 
  public virtual ILVolume   , 
  public         ValidDataObject 
{
  
protected:
  
  /** constructor
   *  @exception LVolumeException wrong paramaters value
   *  @param name name of logical volume 
   *  @param sensitivity  name of sensitive detector object (for simulation)
   *  @param magnetic  nam eof magnetic field object (for simulation)
   */
  LogVolBase( const std::string& name        = "" , 
              const std::string& sensitivity = "" ,
              const std::string& magnetic    = "" );
  
  /// destructor 
  virtual ~LogVolBase();
  
public:
  
  /** retrieve  the name(identification)  of Logical Volume  
   *  @see ILVolume 
   *  @return    the name(identification)  of Logical Volume  
   */
  inline virtual const std::string&  name () const 
  {
    static std::string s_empty = "";
    IRegistry* pReg = registry();
    return (0!=pReg) ? pReg->identifier() : s_empty;;
  }
  
  /** vector of physical volumes 
   *  @see ILVolume 
   *  @return vector of physical volumes 
   */
  inline virtual        PVolumes& pvolumes ()      { return m_pvolumes ; } 
  
  /** vector of physical volumes (const version)
   *  @see ILVolume 
   *  @return vector of physical volumes 
   */
  inline virtual const PVolumes& pvolumes () const { return m_pvolumes ; }
  
  /** number of Physical(positioned) Volumes 
   *  @see ILVolume 
   *  @return number of Physical(positioned) Volumes
   */
  inline virtual ILVolume::ReplicaType noPVolumes () const
  { return m_pvolumes.size() ; }
  
  /** daughter (Physical Volume) by index
   *  @see ILVolume 
   *  @param  index    physical volume index 
   *  @return pointer to daughter (Physical Volume) 
   */
  inline virtual const IPVolume* operator[]
  ( const ILVolume::ReplicaType& index ) const
  {
    return m_pvolumes.size() > index ? 
      *(m_pvolumes.begin()+index) : 0 ;
  };
  
  /** daughter (Physical Volume) by name 
   *  @see ILVolume 
   *  @param  name    physical volume name 
   *  @return pointer to daughter (Physical Volume) 
   */
  inline virtual const IPVolume* operator[]
  ( const std::string&           name  ) const
  { 
    ILVolume::PVolumes::const_iterator pvi = 
      std::find_if( m_pvolumes.begin  () , 
                    m_pvolumes.end    () , 
                    IPVolume_byName( name ) ) ;
    return m_pvolumes.end() != pvi ? *pvi : 0 ;
  };
  
  /** get daughter (Physical Volume) by index
   *  @param  index    physical volume index 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual const IPVolume* pvolume   
  ( const ILVolume::ReplicaType& index ) const
  {
    return m_pvolumes.size() > index ? 
      *(m_pvolumes.begin()+index) : 0 ;
  };
  
  /** get daughter (Physical Volume) by name 
   *  @param  name    physical volume name 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual const IPVolume* pvolume   
  ( const std::string&           name  ) const 
  { 
    ILVolume::PVolumes::const_iterator pvi = 
      std::find_if( m_pvolumes.begin  () , 
                    m_pvolumes.end    () , 
                    IPVolume_byName( name ) ) ;
    return m_pvolumes.end() != pvi ? *pvi : 0 ;
  };
  
  /** begin iterator  for manipulation with daughters
   *  @see ILVolume 
   *  @return begin iterator  for manipulation with daughters
   */
  inline virtual ILVolume::PVolumes::iterator       pvBegin     ()
  { return m_pvolumes.begin () ;}
  
  /** begin iterator  for manipulation with daughters (const version)
   *  @see ILVolume 
   *  @return begin iterator  for manipulation with daughters
   */
  inline virtual ILVolume::PVolumes::const_iterator pvBegin     () const
  { return m_pvolumes.begin () ;}
  
  /** retrieve end iterator for manipulation with daughters
   *  @see ILVolume 
   *  @return end iterator  for manipulation with daughters
   */
  inline virtual ILVolume::PVolumes::iterator       pvEnd       ()
  { return m_pvolumes.end ()  ;}
  
  /** retrieve end iterator for manipulation with daughters (const version)
   *  @see ILVolume 
   *  @return end iterator  for manipulation with daughters
   */
  inline virtual ILVolume::PVolumes::const_iterator pvEnd       () const 
  { return m_pvolumes.end () ;}
  
  /** traverse the sequence of paths  \n 
   *  transform the sequence of replicas to sequence of  physical volumes 
   *  @see ILVolume 
   *  @param pathBegin    first replica-Path 
   *  @param pathEnd      last  replica-Path 
   *  @param pVolumePath  vector of physical volumes 
   *  @return status code 
   */
  virtual StatusCode traverse 
  ( ILVolume::ReplicaPath::const_iterator pathBegin,
    ILVolume::ReplicaPath::const_iterator pathEnd  ,
    ILVolume::PVolumePath&                pVolumePath ) const ;
  
  /** traverse the sequence of paths  \n 
   *  transform the sequence of replicas to sequence of  physical volumes 
   *  @see ILVolume 
   *  @param path         replica-Path 
   *  @param pVolumePath  vector of physical volumes 
   *  @return status code 
   */
  inline virtual StatusCode traverse 
  ( const ILVolume::ReplicaPath&  path,
    ILVolume::PVolumePath&        pVolumePath ) const
  { return traverse( path.begin() , path.end() , pVolumePath ); }
  
  /** name of sensitive "detector" - needed for simulation 
   *  @see ILVolume 
   *  @return name of sensitive "detector"
   */
  inline virtual const std::string& sdName () const { return m_sdName; } ;
  
  /** magnetic properties  (if needed for simulation)  
   *  @see ILVolume 
   *  @return name of magnetic field  object
   */
  inline virtual const std::string& mfName () const { return m_mfName; } ;
  
  /** accessors to surfaces 
   *  @see ILVolume 
   *  @return vector of surfaces 
   */  
  inline virtual        Surfaces& surfaces()       { return m_surfaces ; }
  
  /** accessors to surfaces  (const version) 
   *  @see ILVolume 
   *  @return vector of surfaces 
   */  
  inline virtual  const Surfaces& surfaces() const { return m_surfaces ; }
  
  /** printout to STD/STL stream
   *  @see ILVolume 
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut( std::ostream & os = std::cout ) const ;
  
  /** printout to Gaudi MsgStream stream
   *  @see ILVolume 
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut( MsgStream    & os             ) const;
  
  /** reset to initial state, clear chaches, etc...
   *  @see ILVolume 
   *  @return self reference
   */
  inline virtual ILVolume* reset () 
  {
    /// reset all physical volumes 
    std::for_each( m_pvolumes.begin ()         , 
                   m_pvolumes.end   ()         ,
                   std::mem_fun(&IPVolume::reset) ) ;
    /// return self-reference
    return this;
  };
  
  /** query the interface
   *  @see IInterface 
   *  @param ID unique interface identifier 
   *  @param ppI placeholder for returned interface
   *  @return status code 
   */
  virtual StatusCode 
  queryInterface( const InterfaceID& ID , void** ppI ) ;

  /** add the reference
   *  @see IInterface 
   *  @return reference counter 
   */
  virtual unsigned long addRef  ();

  /** release the interface 
   *  @see IInterface 
   *  @return reference counter 
   */
  virtual unsigned long release ();

  /** create daughter physical volume 
   *  @param PVname name of daughter volume 
   *  @param LVnameForPV name of logical volume for the physical volume 
   *  @param position position of logical volume 
   *  @param rotation rotation of logical volume 
   *  @return pointer to created physical volume  
   */
  IPVolume* createPVolume
  ( const std::string&    PVname                    , 
    const std::string&    LVnameForPV               ,
    const Gaudi::XYZPoint&     position = Gaudi::XYZPoint  () ,  
    const Gaudi::Rotation3D&    rotation = Gaudi::Rotation3D () ); 
  
  /** create daughter physical volume 
   *  @param PVname      name of daughter volume 
   *  @param LVnameForPV name of logical volume for the physical volume 
   *  @param Transform   tranformation
   *  @return pointer to created physical volume  
   */
  IPVolume* createPVolume
  ( const std::string&    PVname                    , 
    const std::string&    LVnameForPV               ,
    const Gaudi::Transform3D& Transform                 );
  
protected: 
  
  /** create EMPTY daughter physical volume 
   *  @return pointer to created physical volume  
   */
  IPVolume* createPVolume();
  
  /** Assertion
   *  @param assertion condition 
   *  @param name      exception message
   *  @param sc        status code 
   */
  inline void Assert
  ( bool               assertion                       , 
    const std::string& name                            ,
    const StatusCode&  sc        = StatusCode::FAILURE ) const
  { if( !assertion ) { throw LogVolumeException( name, this , sc ); } };
  
  /** Assertion
   *  @param assertion condition 
   *  @param name      exception message
   *  @param Exception previous exception 
   *  @param sc        status code 
   */
  inline void Assert
  ( bool                  assertion , 
    const std::string&    name      ,
    const GaudiException& Exception , 
    const StatusCode&     sc        = StatusCode::FAILURE ) const  
  { 
    if( !assertion ) 
      { throw LogVolumeException( name, Exception , this , sc ); }
  };
  
  /** Auxillary method  to calculate intersections with daughters
   *  @exception LVolumeException wrong parameters or geometry error
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param childIntersections output container
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  unsigned int intersectDaughters
  ( const Gaudi::XYZPoint&        Point              , 
    const Gaudi::XYZVector&       Vector             , 
    ILVolume::Intersections& childIntersections , 
    const ISolid::Tick     & tickMin            , 
    const ISolid::Tick     & tickMax            , 
    const double             Threshold          ) const ;  
  
  /** Auxillary method  to calculate intersection with daughters
   *  @exception LVolumeException wrong parameters or geometry error
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param childIntersections output container
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  unsigned int  intersectDaughters
  ( const Gaudi::XYZPoint&        Point              , 
    const Gaudi::XYZVector&       Vector             , 
    ILVolume::Intersections& childIntersections , 
    const double             Threshold          ) const ;
  
  /** check for the given 3D-point inside daughter volume
   *  Point coordinates are in the local reference 
   *  frame of the solid.   
   *  @param LocalPoint point (in local reference system of the solid)
   *  @return true if the point is inside the daughter volume
   */
  inline bool isInsideDaughter
  ( const Gaudi::XYZPoint& LocalPoint ) const
  { 
    return  
      m_pvolumes.end() != insideDaughter( LocalPoint );
  };
  
  /** check for the given 3D-point inside daughter volume
   *  Point coordinates are in the local reference 
   *  frame of the solid.   
   *  @param LocalPoint point (in local reference system of the solid)
   *  @return iterator to the daughter volume 
   */
  inline ILVolume::PVolumes::const_iterator insideDaughter
  ( const Gaudi::XYZPoint& LocalPoint ) const
  { 
    return  
      std::find_if( m_pvolumes.begin () , 
                    m_pvolumes.end   () , 
                    IPVolume_isInside( LocalPoint ) ) ;
  };

protected: 
  
  // deduce the copy number of physical volume from its name 
  //  size_t   copyNumber ( const std::string& pvName ) const ;
  //  bool     copyExist  ( const size_t       copy   ) const ;

protected:
  
  /** 
   *  accessor to data service used for retriving of the material 
   *  @return pointer to data service 
   */
  IDataProviderSvc* dataSvc() const;

  /** 
   *  accessor to massage service
   *  @return pointer to message service 
   */
  IMessageSvc* msgSvc() const;

private: 
  
  /// copy constructor is private! 
  LogVolBase            ( const LogVolBase& lvb );
  /// assignment operator is private! 
  LogVolBase& operator= ( const LogVolBase& lvb );
  
private:
  
  /// list of daughter physical volumes 
  PVolumes              m_pvolumes      ; 
  /// list attached surfaces 
  Surfaces              m_surfaces      ; 
  /// name of sensitive detector object 
  std::string           m_sdName        ;
  /// name of magnetic field source 
  std::string           m_mfName        ;
  /// static  volume counter 
  static  unsigned long s_volumeCounter ;
  /// reference to services
  DetDesc::Services* m_services;
};

// ============================================================================
// The End 
// ============================================================================
#endif  ///< DETDESC_LVOLUME_H
// ============================================================================
