// $Id: ILVolume.h,v 1.11 2005-12-07 13:19:07 cattanem Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef  DETDESC_ILVOLUME_H
#define  DETDESC_ILVOLUME_H 1 
/// STD & STL includes  
#include <iostream> 
#include <string> 
#include <vector> 
/// GaudiKernel includes 
#include "GaudiKernel/MsgStream.h"
/// DetDesc includes
#include "DetDesc/ISolid.h"
/// CLHEP includes 
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

/// Forward declarations 
class StatusCode;
class Material; 
class IPVolume;
class ISolid;
template <class TYPE>
class SmartRefVector;
class Surface;

/// Declaration of the interface ID ( interface id, major & minor versions)
static const InterfaceID IID_ILVolume( 153 , 3 , 1 );

/** @interface ILVolume ILVolume.h "DetDesc/ILVolume.h"
 *
 *  An abstract interface to named node in the geometry tree.
 *
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xxxx 
 */

class ILVolume : virtual public IInterface
{
  ///
public:
  
  /**  general typedefs  */ 
  typedef  std::vector<IPVolume*>                        PVolumes;
  typedef  PVolumes::size_type                           ReplicaType;
  typedef  std::vector<ReplicaType>                      ReplicaPath;
  typedef  std::vector<const IPVolume*>                  PVolumePath;

  /** typedefs, specific for calculation of distance 
   *  in the units of radiation length
   */
  typedef std::pair<ISolid::Tick,ISolid::Tick>           Interval;  
  typedef std::pair<Interval,const Material*>            Intersection;
  typedef std::vector<Intersection>                      Intersections;
  
  /// typedef for surfaces 
  typedef SmartRefVector<Surface>                         Surfaces;
  
public:
  
  /** retrieve the unique interface identifier 
   *  @see Interface::interfaceID()
   *  @return uniqie interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_ILVolume; }
  
public: 
  
  /** retrieve  the name(identification)  of Logical Volume  
   *  @return    the name(identification)  of Logical Volume  
   */
  virtual const   std::string&  name          () const = 0;
  
  /** is this volume "Assembly" of other volumes?
   *  The notion of "Assembly Volume" is imported from Geant4.
   *  "Assembly Volume" has no associated material and shape, 
   *  thus material name shoudl me empty, pointer to solid
   *  and pointer to material should be both nulls
   *  @return true if volume is Assembly
   */ 
  virtual bool                  isAssembly    () const = 0 ;
  
  /**  return the solid, associated with the Logical Volume  
   *  @return the solid, associated with the Logical Volume  
   */
  virtual const ISolid*         solid         () const = 0;
  
  /**  return the material(by name), associated with the Logical Volume  
   *  @return the material(by name), associated with the Logical Volume  
   */
  virtual const std::string&    materialName  () const = 0;
  
  /**  return the material, associated with the Logical Volume
   *   for Assembly volumes material name is useless 
   *  @return the material, associated with the Logical Volume  
   */
  virtual const Material*       material      () const = 0; 
  
  /**  return vector of physical volumes 
   *   for Assembly volumes material is useless 
   *  @return vector of physical volumes 
   */
  virtual       PVolumes&       pvolumes      ()       = 0 ;
  
  /**  return vector of physical volumes (const version)
   *  @return vector of physical volumes 
   */
  virtual const PVolumes&       pvolumes      () const = 0;
  
  /**  return number of Physical Volumes inside gived Logical Volume
   *  @return number of Physical Volumes inside gived Logical Volume
   */
  virtual ILVolume::ReplicaType noPVolumes    () const = 0;
  
  /** get daughter (Physical Volume) by index
   *  @param  index    physical volume index 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual const IPVolume* operator[]
  ( const ILVolume::ReplicaType& index ) const = 0;
  
  /** get daughter (Physical Volume) by name 
   *  @param  name    physical volume name 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual const IPVolume* operator[]
  ( const std::string&           name  ) const = 0;
  
  /** get daughter (Physical Volume) by index
   *  @param  index    physical volume index 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual const IPVolume* pvolume   
  ( const ILVolume::ReplicaType& index ) const = 0;
  
  /** get daughter (Physical Volume) by name 
   *  @param  name    physical volume name 
   *  @return pointer to daughter (Physical Volume) 
   */
  virtual const IPVolume* pvolume   
  ( const std::string&           name  ) const = 0;
  
  /**  retrieve begin iterator  for manipulation with daughters
   *   @return begin iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::iterator       pvBegin     ()        = 0 ; 
  
  /**  retrieve begin iterator  for manipulation with daughters
   *   (const version)
   *   @return begin iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::const_iterator pvBegin     () const  = 0 ; 

  /**  retrieve end iterator  for manipulation with daughters
   *   @return end iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::iterator       pvEnd       ()        = 0 ; 
  
  /**  retrieve end iterator  for manipulation with daughters
   *   (const version)
   *   @return end iterator  for manipulation with daughters
   */
  virtual ILVolume::PVolumes::const_iterator pvEnd       () const  = 0 ; 

  /** traverse the sequence of paths  \n 
   *  transform the sequence of replicas to sequence of  physical volumes 
   *  @param pathBegin  begin iterator for replica-Path 
   *  @param pathEnd    end iterator for replica-Path 
   *  @param volumePath  vector of physical volumes 
   *  @return status code 
   */
  virtual StatusCode traverse 
  ( ILVolume::ReplicaPath::const_iterator pathBegin,
    ILVolume::ReplicaPath::const_iterator pathEnd  ,
    ILVolume::PVolumePath&                volumePath ) const = 0;
  
  /** traverse the sequence of paths  \n 
   *  transform the sequence of replicas to sequence of  physical volumes 
   *  @param replicaPath replica-Path 
   *  @param volumePath  vector of physical volumes 
   *  @return status code 
   */
  virtual StatusCode traverse 
  ( const ILVolume::ReplicaPath&  replicaPath,
    ILVolume::PVolumePath&        volumePath ) const = 0; 
  
  /** check for the given 3D-point. 
   *
   *  Point coordinated are in the local reference 
   *  frame of the logical volume.
   *
   *  For Assembly Volumes "inside" means 
   *  inside of at least one daughter volume 
   * 
   *  @param LocalPoint point (in local reference system of the volume)
   *  @return true if the point is inside the logical volume 
   */
  virtual bool isInside ( const Gaudi::XYZPoint& LocalPoint ) const = 0; 
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @param  localPoint point in local reference system of logical volume 
   *  @param  level depth level
   *  @param  volumePath  vector of physical volumes
   *  @return status code 
   */
  virtual StatusCode 
belongsTo
  ( const Gaudi::XYZPoint&        localPoint ,
    const int                level      , 
    ILVolume::PVolumePath&   volumePath ) const = 0;
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @param  localPoint point in local reference system of logical volume 
   *  @param  level depth level
   *  @param  replicaPath replica path 
   *  @return status code 
   */
  virtual StatusCode belongsTo
  ( const Gaudi::XYZPoint&        localPoint ,
    const int                level      , 
    ILVolume::ReplicaPath&   replicaPath ) const = 0;      
  
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
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint        & Point         ,  
    const Gaudi::XYZVector       & Vector        ,  
    ILVolume::Intersections & intersections ,
    const double              threshold     ) const = 0 ; 
  
  /** intersection of the logical volume with with the line \n
   *  Theine is parametrized in the local reference system 
   *  of the logical volume by initial Point and direction Vector \n 
   *  @f$ \vec{x}(t) = \vec{p} + t\times \vec{v} @f$  \n 
   * 
   *  Method returns the number of intersection points("ticks") and 
   *  the container of pairs - ticks and pointer to the corresponding 
   *  material. The simplification is determined by value of threshold
   *  (in units of radiation length) 
   *  Method throws LVolumeException in the case, then 
   *  solid is not defined or material is not accessible.
   *
   *  @exception LVolumeException solid or/and matherial problems 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint        & Point         ,
    const Gaudi::XYZVector       & Vector        ,
    ILVolume::Intersections & intersections , 
    const ISolid::Tick        tickMin       ,
    const ISolid::Tick        tickMax       , 
    const double              Threshold     ) const = 0 ;
  
  /** name of sensitive "detector" - needed for simulation 
   *  @return name of sensitive "detector"
   */
  virtual const std::string& sdName   ()                const = 0 ; 
  
  /** magnetic properties  (if needed for simulation)  
   *  @return name of magnetic field  object
   */
  virtual const std::string& mfName   ()                const = 0 ;
  
  /** accessors to attached surfaces 
   *  @return vector of surfaces 
   */  
  virtual       Surfaces& surfaces()        = 0 ; 
  
  /** accessors to attached surfaces  (const version) 
   *  @return vector of surfaces 
   */  
  virtual const Surfaces& surfaces() const  = 0 ;
  
  /** printout to STD/STL stream
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut( std::ostream & os = std::cout) const = 0;
  
  /** printout to Gaudi MsgStream stream
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut( MsgStream    & os ) const = 0;
  
  /** reset to the initial state 
   *  @return self-reference 
   */
  virtual ILVolume*  reset() = 0; 
  
protected: 

  /** (virtual) destructor
   */ 
  virtual ~ILVolume() ;
};

// ============================================================================
/** output operator to STD/STL stream
 *  @param  os      reference to output   stream
 *  @param  lv      reference to ILVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline std::ostream& operator<<( std::ostream& os , const ILVolume& lv ) 
{ return lv.printOut( os ); };

// ============================================================================
/** output operator to STD/STL stream
 *  @param  os      reference to output   stream
 *  @param  lv      pointer   to ILVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline std::ostream& operator<<( std::ostream& os , const ILVolume*  lv )
{ return ((0==lv)?( os<<" ILVolume* points to NULL "):(os<<(*lv)));};

// ============================================================================
/** output operator to Gaudi MsgStream stream
 *  @param  os      reference to output   stream
 *  @param  lv      reference to ILVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline MsgStream& operator<<( MsgStream& os , const ILVolume& lv ) 
{ return lv.printOut( os ); };

// ============================================================================
/** output operator to Gaudi MsgStream stream
 *  @param  os      reference to output   stream
 *  @param  lv      pointer   to ILVolume object
 *  @return reference to the stream
 */
// ============================================================================
inline MsgStream& operator<<( MsgStream& os , const ILVolume*  lv )
{ return ((0==lv)?( os<<" ILVolume* points to NULL "):(os<<(*lv)));};

// ============================================================================
// The End 
// ============================================================================
#endif  ///< DETDESC_ILVOLUME_H
// ============================================================================
