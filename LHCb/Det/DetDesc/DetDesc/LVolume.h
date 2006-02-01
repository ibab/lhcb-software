// $Id: LVolume.h,v 1.23 2006-02-01 19:39:09 marcocle Exp $ 
#ifndef     DETDESC_VOLUMES_LVOLUME_H
#define     DETDESC_VOLUMES_LVOLUME_H

/// DetDesc  includes 
#include "DetDesc/LogVolBase.h"
#include "DetDesc/CLIDLVolume.h"

/// forward declarations 
template <class TYPE> 
class DataObjectFactory;

/** @class LVolume LVolume.h DetDesc/LVolume.h
 *
 *  A simple implementation of ILVolume interface
 * 
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class LVolume: public LogVolBase
{
  /// friend factory for instantiation 
  friend class DataObjectFactory<LVolume>;

public:
    
  /** constructor, pointer to ISolid* must be valid!, 
   *  overvise constructor throws LogVolumeException!  
   *  @exception LVolumeException for wrong parameters set
   *  @param name         name of logical volume 
   *  @param Solid        pointer to ISolid object 
   *  @param material     name of the material 
   *  @param sensitivity  name of sensitive detector object (for simulation)
   *  @param magnetic     name of magnetic field object (for simulation)
   */
  LVolume( const std::string& name             , 
           ISolid*            Solid            ,
           const std::string& material         ,
           const std::string& sensitivity = "" ,
           const std::string& magnetic    = "" );

  /// destructor 
  virtual ~LVolume();
  
public:
  
  /** object/class  identification (virtual)
   *  @return uniqie class identifier 
   */
  virtual const CLID& clID    () const ;
  
  /** object/class  identification (static)
   *  @return uniqie class identifier 
   */
  static  const CLID& classID ()       ;
  
  /** is this volume "Assembly" of other volumes?
   *  @see ILVolume 
   *  notion of Assembly Volume is imported from Geant4.
   *  "Assembly Volume" has no associated material and shape, 
   *  thus material name shoudl me empty, pointer to solid
   *  and pointer to material should be both nulls
   *  @return true if volume is Assembly
   */ 
  inline virtual bool            
  isAssembly () const { return false   ; } 
  
  /** the solid, associated with the Logical Volume  
   *  @see ILVolume 
   *  @return the solid, associated with the Logical Volume  
   */
  inline virtual const ISolid*   
  solid      () const { return m_solid ; }
  
  /** the material, associated with the Logical Volume  
   *  For Assembly Volumes material pointes to NULL!
   *  @see ILVolume 
   *  @exception LVolumeException no material
   *  @return the material, associated with the Logical Volume  
   */
  inline virtual const Material* 
  material   () const 
  { return 0 != m_material ? m_material : findMaterial() ; };
  
  /** the material(by name), associated with the Logical Volume  
   *  @see ILVolume 
   *  @return the material(by name), associated with the Logical Volume  
   */
  inline virtual const std::string&  
  materialName () const { return m_materialName; }
  
  /** check for the given 3D-point. Point coordinates are in the 
   *  local reference frame of the logical volume 
   *  @see ILVolume 
   *  @param LocalPoint point (in local reference system of the solid)
   *  @return true if the point is inside the solid
   */
  inline virtual bool isInside 
  ( const Gaudi::XYZPoint& LocalPoint ) const
  { return m_solid->isInside( LocalPoint ) ; };
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @see ILVolume 
   *  @param  LocalPoint   point in local reference system of logical volume 
   *  @param  Level depth  level
   *  @param  pVolumePath  vector of physical volumes
   *  @return status code 
   */
  virtual StatusCode belongsTo
  ( const Gaudi::XYZPoint&        LocalPoint  ,
    const int                Level       , 
    ILVolume::PVolumePath&   pVolumePath ) const ;
  
  /** calculate the daughter path containing the Point in Local frame , 
   *  can be VERY slow for complex geometry, 
   *  therefore use the appropriate Level for usage 
   *  @see ILVolume 
   *  @param  LocalPoint  point in local reference system of logical volume 
   *  @param  Level       depth level
   *  @param  replicaPath replica path 
   *  @return status code 
   */
  virtual StatusCode belongsTo
  ( const Gaudi::XYZPoint&        LocalPoint  ,
    const int                Level       , 
    ILVolume::ReplicaPath&   replicaPath ) const  ;
  
  /** intersection of the logical volume with with the line \n
   *  The line is parametrized in the local reference system 
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
   *  @see ILVolume 
   *  @exception LogVolumeException solid or/and matherial problems 
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
    const double              threshold     ) const ;
  
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
   *  @see ILVolume 
   *  @exception LogVolumeException solid or/and matherial problems 
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  virtual unsigned int intersectLine
  ( const Gaudi::XYZPoint         & Point         , 
    const Gaudi::XYZVector        & Vector        , 
    ILVolume::Intersections  & intersections , 
    const ISolid::Tick         tickMin       , 
    const ISolid::Tick         tickMax       , 
    const double               Threshold     ) const ;
  
  /** printout to STD/STL stream
   *  @see ILVolume 
   *  @param os STD/STL stream
   *  @return reference to the stream
   */
  virtual std::ostream& printOut 
  ( std::ostream & os = std::cout ) const ;
  
  /** printout to Gaudi MsgStream stream
   *  @see ILVolume 
   *  @param os Gaudi MsgStream  stream
   *  @return reference to the stream
   */
  virtual MsgStream&    printOut
  ( MsgStream    & os             ) const;

  /** reset to initial state, 
   *  clear chaches, etc...
   *  @see ILVolume 
   *  @return self reference
   */
  inline  ILVolume* reset () 
  {
    /// reset the solid 
    if( 0 != m_solid ) { m_solid->reset() ; }
    /// reset the material 
    m_material = 0 ;
    /// reset the base 
    return LogVolBase::reset() ;
  };
  
protected:
  
  /** default constructor
   *  @param Name name of logical volume 
   */
  LVolume( const std::string& Name = "");       
  
private: 
  
  /// copy constructor is private! 
  LVolume           ( const LVolume& );
  /// assignment operator is private! 
  LVolume& operator=( const LVolume& ); 
  
private:
  
  /** find C++ pointer to material by it's name 
   *  @exception LogVolumeException if the material is not found 
   *  @return the C++ pointer to material 
   */
  const Material* findMaterial() const ;
  
  /** Auxillary method  to calculate own intersections
   *  @exception LogVolumeException for wrong parameters or geometry error
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container
   *  @param tickMin minimum value of possible Tick
   *  @param tickMax maximum value of possible Tick
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  unsigned int intersectBody
  ( const Gaudi::XYZPoint        & Point         , 
    const Gaudi::XYZVector       & Vector        , 
    ILVolume::Intersections & intersections , 
    ISolid::Tick            & tickMin       , 
    ISolid::Tick            & tickMax       , 
    const double              Threshold     ) const ;  
  
  /** Auxillary method  to calculate own intersections
   *  @exception LVolumeException wrong parameters or geometry error
   *  @param Point initial point at the line
   *  @param Vector direction vector of the line
   *  @param intersections output container
   *  @param Threshold threshold value 
   *  @return number of intersections  
   */
  unsigned int intersectBody
  ( const Gaudi::XYZPoint&        Point         , 
    const Gaudi::XYZVector&       Vector        , 
    ILVolume::Intersections& intersections , 
    const double             Threshold     ) const ;
  
private:
  
  /// solid 
  ISolid*                   m_solid        ; 
  /// material 
  std::string               m_materialName ; 
  mutable const Material*   m_material     ; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif  ///< DETDESC_LVOLUME_H
// ============================================================================








