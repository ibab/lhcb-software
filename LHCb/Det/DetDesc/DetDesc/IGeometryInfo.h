
// ===========================================================================
#ifndef  DETDESC_IGEOMETRYINFO_H
#define  DETDESC_IGEOMETRYINFO_H 1

/** GaudiKernel includes */
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"

/// DetDesc include 
#include "DetDesc/ILVolume.h"
#include "DetDesc/AlignmentCondition.h"

/** forward declarations */
class StatusCode;


/** the unique interface identifier 
 * ( unique interface identifier , major & minor versions)
 */
static const InterfaceID IID_IGeometryInfo( 155 , 3 , 1 );

/** @interface IGeometryInfo IGeometryInfo.h "DetDesc/IGeometryInfo.h"
 *
 *  An abstract interface to get all geometry information for 
 *  a detector element
 *
 *  @version 3
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Juan Palacios juancho@nikhef.nl
 *  @date xx/xx/xxxx
 */

class IGeometryInfo : virtual public IInterface
{
public:

  /// type of vector of daughter elements
  typedef std::vector<IGeometryInfo*>  IGIChildrens;
  /// Iterator over Daughters
  typedef IGeometryInfo::IGIChildrens::iterator IGIChildrenIterator;
  /// Const Iterator over daughters
  typedef IGeometryInfo::IGIChildrens::const_iterator IGIChildrenConstIterator;

  /** retrieve the uniqie interface identifier 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IGeometryInfo; }
  
  virtual StatusCode cache()=0;
  
  virtual StatusCode updateChildren()=0;

  /** Is the GeometryInfo object associated with Logical Volume?
   *  This is to distinguish the Ghost case from the others.
   *
   *   A Detector Element ( or any arbitrary "Geometry Object")
   *  can be associated with Logical Volume in a Geometry Tree
   *  almost all behaviour of all other functions depends on
   *  the fact of this association.
   *
   *  There can be 3 types of such "association":
   *  -# "Regular case":
   *    - An Logical Volume embedded into the overall
   *      geometry tree is associated with this element.
   *  -# "Orphan":
   *    - A standalone Logical Volume ( which is not
   *      embedded into overall geometry tree ) is
   *      associated with this "geometry object".
   *      An interesting case: a "top" volume acts like "orphan" :-))
   *  -# "Ghost":
   *      No any Logical Volume is associated  with
   *      this "geometry element".
   *      Can be useful for naming, grouping  and
   *      classification and navigation.
   *
   */
  virtual bool hasLVolume () const = 0;

  /** Has the GeometryInfo object support in Geometry tree?
   *  distinguish "regular" from others
   */
  virtual bool hasSupport () const = 0;

  /// Access to this IGeometryInfo condition

  virtual const AlignmentCondition* alignmentCondition() const = 0;

  /** @defgroup PureGeometricalInfo IGeometryInfo pure geometrical information
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @{
   */

  /**  Full transformation matrix from Global Reference System
   *   to the local reference system of this Geometry Info object,
   *   including misalignments.
   *  - for "ghosts","orphans" and top-level elements
   *    it is just an Identity transformation
   *  @see toGlobalMatrix()
   *  @return the full 3D transformation from the global reference system
   */
  virtual const Gaudi::Transform3D&  toLocalMatrix() const = 0;

  /**  Ideal transformation matrix from Global Reference System
   *   to the local reference system of this Geometry Info object,
   *   excluding misalignments.
   *   @see toGlobalMatrixNominal()
   *   @return the 3D transformation from the "Global" reference system 
   *           in the nominal frame. 
   */
  virtual const Gaudi::Transform3D&  toLocalMatrixNominal() const = 0;  

  /**  Ideal transformation matrix from local Reference System
   *   to the global reference system of this Geometry Info object,
   *   excluding misalignments.
   *   @see toLocalMatrixNominal()
   *   @return the 3D transformation from "Local" system in the 
   *           nominal frame. 
   */
  virtual const Gaudi::Transform3D&  toGlobalMatrixNominal() const = 0;  

  /**  Ideal transformation matrix from parent Reference System
   *   to the local reference system of this Geometry Info object,
   *   excluding misalignment.
   *   @see ownToGlobalMatrixNominal()
   *   @return the transformation from parent's reference system 
   *           in the nominal frame. 
   */
  virtual const Gaudi::Transform3D&  ownToLocalMatrixNominal() const = 0;  

  /**
   * Transformation that takes this IGeometryInfoPlus from an off-nominal
   * position within a its parent to the nominal one. It can be thought of as
   * the inverse of the "delta" matrix.
   * @return the 3D transformation that returns an off-nominal position
   *         to the nominal position in the frame of the parent
   */
  virtual const Gaudi::Transform3D&  ownToNominalMatrix() const = 0;

  /**
   * Transformation that takes this IGeometryInfoPlus from a nominal
   * position within a its parent to the off-nominal one. It can be thought of as
   * the "delta" matrix.
   * @return the 3D transformation that returns a nominal position
   *         to the off-nominal position in the frame of the parent
   */
  virtual Gaudi::Transform3D  ownToOffNominalMatrix() const = 0;

  /**
   * Transformation from this reference frame to the 
   * reference frame of the parent.
   * Includes local misalignment.
   * @return the 3D transrmation taking this frame to the frame of
   *         the parent.
   */
  virtual Gaudi::Transform3D  ownMatrix() const = 0;

  /**
   * Upate the nominal-> off-nominal 3D transformation of this IGeometryInfo.
   */
  virtual StatusCode  ownToOffNominalMatrix(const Gaudi::Transform3D&) = 0;

  /**
   * Update the transformation parametrs describing the
   * nominal->off-nominal 3D transformation of this GeometryInfo
   */
  virtual StatusCode ownToOffNominalParams(const std::vector<double>& trans,
                                           const std::vector<double>& rot,
                                           const std::vector<double>& pivot =
                                           std::vector<double>(3)) =0;

  /**  
   *   transformation matrix from Local Reference System
   *   to the Global Reference System of this IGeometryInfo.
   *   @see toLocalMatrix()
   *  - for "ghosts","orphans" and top-level elements
   *    it is just an Identity transformation
   *   @return the 3D transformation from the local to the
   *           global reference system
   */
  virtual const Gaudi::Transform3D&  toGlobalMatrix() const = 0;

  /** 
   *  Perform transformation of point from the Global Reference System
   *  to Local Reference System of Geometry Info object
   *  @see toLocalMatrix()
   *  @see toGlobalMatrix()
   *  @see toGlobal( const Gaudi::XYZPoint&)
   *  @param  globalPoint point in Global Reference System
   *  @return point in Local reference system
   */
  virtual Gaudi::XYZPoint toLocal ( const Gaudi::XYZPoint& globalPoint ) const = 0;

  /** perform transformation of point from the LocalReference System
   *  to Global  Reference System
   *  @see toLocalMatrix()
   *  @see toGlobalMatrix()
   *  @see toLocal( const Gaudi::XYZPoint&)
   *  @param  localPoint point in Local Reference System
   *  @return point in Global reference system
   */
  virtual Gaudi::XYZPoint toGlobal ( const Gaudi::XYZPoint& localPoint  ) const = 0;

  /** 
   *  Perform transformation a vector from the Global Reference System
   *  to Local Reference System of Geometry Info object
   *  @see toLocalMatrix()
   *  @see toGlobalMatrix()
   *  @see toGlobal( const Gaudi::XYZVector&)
   *  @param  globalDirection vector in Global Reference System
   *  @return vector in Local reference system direction
   */
  virtual Gaudi::XYZVector toLocal ( const Gaudi::XYZVector& globalDirection ) const = 0;

  /** 
   *  Perform transformation of a vector from the Local Reference System
   *  to the Global Reference System
   *  @see toLocalMatrix()
   *  @see toGlobalMatrix()
   *  @see toLocal( const Gaudi::XYZVector&)
   *  @param  localDirection vector in Local Reference System
   *  @return vector in Global reference system direction
   */
  virtual Gaudi::XYZVector toGlobal ( const Gaudi::XYZVector& localDirection  ) const = 0;

  /** Check for given 3D-point - "Is inside the volume?"
   *  -  For regular case the defnition "is inside" is trivial
   *  -  For "orphan" case the definition is less trivial but also
   *     can be defined in a proper way, e.g. by considering the
   *     point as  "local point". Within this approach the previous
   *     case is just the combination of transformation into local
   *     frame and then  *is inside* method for pseudo-orphan.
   *  -  For "ghosts" only one formal definition can be done -
   *     it is just the logical  "OR" for all registered
   *     daughter Geometry Elements. Obviously it triggers the
   *     recursive calls. What to do if the ghost element has no
   *     any descendants? The best way is just to return "false" -
   *     "something" can not be *inside* of "nothing".  :-))
   *  @see toLocal( const Gaudi::XYZPoint& )
   *  @see toGlobal( const Gaudi::XYZPoint& )
   *  @see toLocalMatrix()
   *  @see toGlobalMatrix()
   *  @param globalPoint point in Global Referency System
   *  @return true if point "is inside" of the Geometry Info element
   */
  virtual bool isInside( const Gaudi::XYZPoint& globalPoint ) const = 0;
  /** @} */ // end of group PureGeometricalInfo

  /** @defgroup BelongsTo  IGeometryInfo "Find daughter by point" family
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @{
   */

  /** To which daughter the given global point belongs to? @n
   *  "Find daughter by point" family @n
   *  If we are clever enough to define *isInside(...)* method
   *  in a proper way (for regular case, orphans and ghosts) we
   *  can easily find the daughter detector element what contains
   *  the given point.  @n
   *  But here we have several choices, which triggers several methods:
   *  -# look throught the first-level dauhters only
   *  -# look recursively through all levels
   *  -# look recursively through certain amount of levels  only.
   *    - The dephth is to be controlled.
   *
   * @see isInside(const Gaudi::XYZPoint&)
   * @see belongsTo( const Gaudi::XYZPoint& )
   * @param globalPoint the point in global Reference System
   * @return the name of daughter to which point belongs to.
   */
  virtual std::string belongsToPath
  ( const Gaudi::XYZPoint& globalPoint ) = 0;
  
  /** To which daughter the given global point belongs to? @n
   *  "Find daughter by point" family @n
   *  If we are clever enough to define *isInside(...)* method
   *  in a proper way (for regular case, orphans and ghosts) we
   *  can easily find the daughter detector element what contains
   *  the given point.  @n
   *  But here we have several choices, which triggers several methods:
   *  -# look throught the first-level dauhters only
   *  -# look recursively through all levels
   *  -# look recursively through certain amount of levels  only.
   *    - The dephth is to be controlled.
   *
   * @see isInside(const Gaudi::XYZPoint&)
   * @see belongsToPath( const Gaudi::XYZPoint& )
   * @param globalPoint the point in global Reference System
   * @return pointer to daughter geometry info element
   */
  virtual IGeometryInfo* belongsTo 
  ( const Gaudi::XYZPoint& globalPoint ) = 0;
  
  /** To which daughter the given global point belongs to? @n
   *  "Find daughter by point" family @n
   *  If we are clever enough to define *isInside(...)* method
   *  in a proper way (for regular case, orphans and ghosts) we
   *  can easily find the daughter detector element what contains
   *  the given point.  @n
   *  But here we have several choices, which triggers several methods:
   *  -# look throught the first-level dauhters only
   *  -# look recursively through all levels
   *  -# look recursively through certain amount of levels  only.
   *    - The dephth is to be controlled.
   *
   * @see isInside(const Gaudi::XYZPoint&)
   * @see belongsTo( const Gaudi::XYZPoint& )
   * @see belongsToPath( const Gaudi::XYZPoint& )
   * @see belongsToPath( const Gaudi::XYZPoint& , const int )
   *
   * - if level = 0 - no search, return the name of current level
   * - if level < 0 - perform search up to the most deepest level
   * - if level > 0 - perform search up to not more then "level" levels;
   *
   * @param globalPoint the point in global Reference System
   * @param level       depth
   * @return the name of daughter to which point belongs to.
   */
  virtual std::string belongsToPath
  ( const Gaudi::XYZPoint& globalPoint , const int         level ) = 0;
  
  /** To which daughter the given global point belongs to? @n
   *  "Find daughter by point" family @n
   *  If we are clever enough to define *isInside(...)* method
   *  in a proper way (for regular case, orphans and ghosts) we
   *  can easily find the daughter detector element what contains
   *  the given point.  @n
   *  But here we have several choices, which triggers several methods:
   *  -# look throught the first-level dauhters only
   *  -# look recursively through all levels
   *  -# look recursively through certain amount of levels  only.
   *    - The dephth is to be controlled.
   * @see isInside(const Gaudi::XYZPoint&)
   * @see belongsTo( const Gaudi::XYZPoint& )
   * @see belongsToPath( const Gaudi::XYZPoint& )
   * @see belongsToPath( const Gaudi::XYZPoint& , const int )
   *
   * /li if level = 0 - no search, return the name of current level
   * /li if level < 0 - perform search up to the most deepest level
   * /li if level > 0 - perform search up to not more then "level" levels;
   *
   * @param globalPoint the point in global Reference System
   * @param level       depth
   * @return pointer to daughter geometry info element
   */
  virtual IGeometryInfo* belongsTo 
  ( const Gaudi::XYZPoint& globalPoint , const int         level ) = 0;
  /** @} */ // end of group BelongsTo

  /** @defgroup FullGeometryInfo IGeometryInfo Full geometry information
   *
   *  Sometimes is is nesessary to know the exact full geometry location
   *  of a Point in the most detailed way then DetectorElement tree allows.
   *  I see only one way to do it - just to go into the real geometry tree.
   *  This operation can be time consuming and therefore it is worth to
   *  return an additinal information with it
   *  The depth can be controlled explicitly via "level" parameter.
   *
   *  @see isInside( const Gaudi::XYZPoint* )
   *  @see belongsTo( const Gaudi::XYZPoint&                 )
   *  @see belongsTo( const Gaudi::XYZPoint&     , const int )
   *  @see belongsToPath( const Gaudi::XYZPoint&             )
   *  @see belongsToPath( const Gaudi::XYZPoint& , const int )
   *
   *  @{
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   */

  /** find full geometry information for given point
   * @param point input 3D point
   * @param level number of levels to nagigate down the hierarchy
   * @param start is the location (or path) of "nearest regular
   *        DetectorElement", which simultaneously contains the
   *        Point and has the connection with Actual Geometry Tree.
   * @param volumePath retuned information
   */
  virtual StatusCode fullGeoInfoForPoint
  ( const Gaudi::XYZPoint&        point       ,
    const int                level       ,
    IGeometryInfo*&          start       ,
    ILVolume::PVolumePath&   volumePath  ) = 0;
  
  /** find full geometry information for given point
   * @param point input 3D point
   * @param level number of levels to nagigate down the hierarchy
   * @param start is the location (or path) of "nearest regular
   *        DetectorElement", which simultaneously contains the
   *        Point and has the connection with Actual Geometry Tree.
   * @param replicaPath retuned information
   */
  virtual StatusCode fullGeoInfoForPoint
  ( const Gaudi::XYZPoint&        point       ,
    const int                level       ,
    IGeometryInfo*&          start       ,
    ILVolume::ReplicaPath&   replicaPath ) = 0;
  
  /** find full geometry information for given point
   * @param point input 3D point
   * @param level number of levels to nagigate down the hierarchy
   * @param start is the location (or path) of "nearest regular
   *        DetectorElement", which simultaneously contains the
   *        Point and has the connection with Actual Geometry Tree.
   * @param volumePath retuned information
   */
  virtual StatusCode fullGeoInfoForPoint
  ( const Gaudi::XYZPoint&        point       ,
    const int                level       ,
    std::string&             start       ,
    ILVolume::PVolumePath&   volumePath  ) = 0;
  
  /** find full geometry information for given point
   * @param point input 3D point
   * @param level number of levels to nagigate down the hierarchy
   * @param start is the location (or path) of "nearest regular
   *        DetectorElement", which simultaneously contains the
   *        Point and has the connection with Actual Geometry Tree.
   * @param replicaPath retuned information
   */
  virtual StatusCode fullGeoInfoForPoint
  ( const Gaudi::XYZPoint&        point       ,
    const int                level       ,
    std::string&             start       ,
    ILVolume::ReplicaPath&   replicaPath ) = 0;
  /** @} */ // end of group FullGeometryInfo

  /** How to answer the question -
   *  What is the distance in radiation lengths between 2 points? @n
   *  Probably it is very frequent question during patters
   *  recognition and track fitting, e.g. any kinds of Kalman
   *  Filtering need the estimation of multiple scatetring.  @n
   *  Obviously it is not an easy task. Especially if one needs
   *  "the exact" answer. "The exact" answer can be obtained only
   *  from actual geometry tree/ but it is possible to implement
   *  an rough estimation within  IGeometryInfo tree. @n
   *  If one asks for the calculations of quantities like
   *  "distanceInRadiationLengthUnits(Point1,Point2)"
   *  it is almost obvious (at least for me) that the
   *  algorithm is the following:
   *  -# localize both points in the same low level
   *     parent volume. It means that we should be
   *     able to find the "closest common ancestry", which is
   *     not "ghost" - "orphan" seems to me also acceptable,
   *     but it is to be checked
   *  -# from this step one should be able to go to
   *     tree of real geometry (if one need *the exact* answer)
   *     Otherwise. the detector can have a simplified "mean"
   *     density of scatterers. Sometimes it is enough for rought
   *     estimatioins
   */

  /** the name of associated Logical Volume
   *  @return the name of associated Logical Volume
   */
  virtual const std::string& lvolumeName() const = 0 ;

  /** associated Logical Volume
   *  @return the pointer to associated Logical Volume
   */
  virtual const ILVolume* lvolume () const = 0 ;
  
  /**  the information about the support
   *  @param start  "start" geometry info
   *  @param replicaPath replica path
   *  @return status code
   */
  virtual StatusCode location
  ( IGeometryInfo*&        start ,
    ILVolume::ReplicaPath& replicaPath ) const = 0;
  
  /**  the information about the support
   *  @param start  "start" geometry info
   *  @param replicaPath replica path
   *  @return status code
   */
  virtual StatusCode location
  ( std::string&           start ,
    ILVolume::ReplicaPath& replicaPath ) const = 0;
  
  /** @defgroup LogVol IGeometryInfo get Logical Volume by its full address
   *  "start" point in DetectorDescription Tree  and the replica path in the
   *  Actual Geometry Tree, starting with the logical volume
   *  associated with "starting point"
   * @{
   */

  /** the name of Logical Volume, addressed by  start and Replica Path
   *  @param start start
   *  @param replicaPath replicaPath
   *  @return the name of Logical Volume
   */
  virtual std::string lvolumePath
  ( const std::string&           start       ,
    const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /** the Logical Volume, addressed by  start and Replica Path
   *  @param start start
   *  @param replicaPath replicaPath
   *  @return pointer to Logical Volume
   */
  virtual const ILVolume* lvolume 
  ( const std::string&           start       ,
    const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /** the name of Logical Volume, addressed by  start and Replica Path
   *  @param start start
   *  @param replicaPath replicaPath
   *  @return the name of Logical Volume
   */
  virtual std::string lvolumePath
  ( IGeometryInfo*               start       ,
    const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /** the Logical Volume, addressed by  start and Replica Path
   *  @param start start
   *  @param replicaPath replicaPath
   *  @return pointer to Logical Volume
   */
  virtual const ILVolume* lvolume 
  ( IGeometryInfo*               start       ,
    const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /// retrive reference to replica path (mistrerious "rpath" or "npath")
  virtual const ILVolume::ReplicaPath& supportPath() const = 0;

  /** @} */ // end of group LogVol
  
  /** @defgroup Navigation IGeometryInfo navigation functions
   * (according to feedback from users to release v3)
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   * @{
   */

  /** pointer to the parent IGeometryInfo
   *  @return pointer to the parent IGeometryInfo
   */
  virtual IGeometryInfo* parentIGeometryInfo() = 0 ;

  /** pointer to the parent IGeometryInfo (const version)
   *  @return pointer to the parent IGeometryInfo
   */
  virtual const IGeometryInfo* parentIGeometryInfo() const = 0 ;

  //  virtual IGeometryInfo* supportIGeometryInfo() = 0 ;

  virtual IGeometryInfo* supportIGeometryInfo() const = 0 ;

  /** (reference to) container of children IGeometryInfo
   *  return  reference to container of children IGeometryInfo
   */
  virtual IGeometryInfo::IGIChildrens& childIGeometryInfos() = 0;

  /** (reference to) container of children IGeometryInfo (const version)
   *  return  reference to container of children IGeometryInfo
   */
  virtual const IGeometryInfo::IGIChildrens& childIGeometryInfos() const = 0;

  /** "begin" iterator for manipulation of daughter elements
   *  return "begin" iterator
   */
  virtual IGeometryInfo::IGIChildrens::iterator        childBegin()       = 0 ;

  /** "begin" iterator for manipulation of daughter elements (const version)
   *  return "begin" iterator
   */
  virtual IGeometryInfo::IGIChildrens::const_iterator  childBegin() const = 0 ;

  /** "end" iterator for manipulation of daughter elements
   *  return "end" iterator
   */
  virtual IGeometryInfo::IGIChildrens::iterator        childEnd  ()       = 0 ;

  /** "end" iterator for manipulation of daughter elements (const version)
   *  return "end" iterator
   */
  virtual IGeometryInfo::IGIChildrens::const_iterator  childEnd  () const = 0 ;
  /** @} */ // end of group Navigation

  /** @defgroup IGeometryInfoPrintOut IGeometryInfo printout methods
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @{
   */
  
  /** printOut to STD/STL stream
   *  @param os reference to STD/STL stream
   *  @return reference to STD/STL stream
   */
  virtual std::ostream& printOut ( std::ostream& os = std::cout ) const = 0;
  
  /** printOut to Message stream
   *  return reference to Message stream
   */
  virtual MsgStream&    printOut ( MsgStream&                   ) const = 0;
  /** @} */ // end of group IGeometryInfoPrintOut
  
  /** reset to the initial state, clear cache
   *  @return self-reference
   */
  virtual IGeometryInfo* reset () = 0;
  
  virtual   ~IGeometryInfo() ;  ///< The virtual destructor
  
};

// ============================================================================
/** @defgroup IGeometryInfoOperators IGeometryInfo inline operators
 * 
 * @{
 */
// ============================================================================

// ============================================================================
/** output operator to STD/STL stream
 *  @param os reference to STD/STL ouput stream
 *  @param gi reference to IGeometryInfo object
 *  @return reference to STD/STL ouput stream
 */
// ============================================================================
inline std::ostream& operator<<( std::ostream&        os ,
                                 const IGeometryInfo& gi )
{ return gi.printOut( os ); }

// ============================================================================
/** output operator to STD/STL stream
 *  @param os reference to STD/STL ouput stream
 *  @param gi pointer to IGeometryInfo object
 *  @return reference to STD/STL ouput stream
 */
// ============================================================================
inline std::ostream& operator<<( std::ostream& os        ,
                                 const IGeometryInfo* gi )
{ return ( 0 == gi ? (os<<"IGeometryInfo* points to NULL") : os<<*gi ); }

// ============================================================================
/** output operator to Message stream
 *  @param os reference to Message ouput stream
 *  @param gi reference to IGeometryInfo object
 *  @return reference to Message ouput stream
 */
// ============================================================================
inline MsgStream&    operator<<( MsgStream&           os ,
                                 const IGeometryInfo& gi )
{ return gi.printOut(os); }

// ============================================================================
/** output operator to Message stream
 *  @param os reference to Message ouput stream
 *  @param gi pointer to IGeometryInfo object
 *  @return reference to Message ouput stream
 */
// ============================================================================
inline MsgStream&    operator<<( MsgStream&    os        ,
                                 const IGeometryInfo* gi )
{ return ( 0 == gi ? (os<<"IGeometryInfo* points to NULL") : os<<*gi ); }

// ============================================================================
/** @} */ // end of group IGeometryInfoOperators
// ============================================================================


// ============================================================================
/** @defgroup IGeometryInfoPredficates IGeometryInfo useful predicates
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @{ 
 */
// ============================================================================

/**  @class IGeometryInfo_isInside IGeometryInfo.h
 *    useful predicate(functor) to be used in conjunction
 *    with STL algorithms
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
class IGeometryInfo_isInside
{
public:
  /** explicit constructor
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @param Point point in the Global Referency System 
   */ 
  explicit IGeometryInfo_isInside( const Gaudi::XYZPoint& Point )
    : m_point( Point ) {};
  
  /** check for 3D-point
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @see IGeometryInfo::isInside( const Gaudi::XYZPoint&)
   *  @param gi poiner to IGeometryInfo object
   *  @return true if the point is inside the Geoemtry Info element
   */
  inline bool operator () ( const IGeometryInfo* gi )  const
  { return gi && gi->isInside( m_point ); };
  
private:
  
  Gaudi::XYZPoint  m_point; ///< point in Global Reference System 
  
};
// ============================================================================
/** @} */ // end of group IGeometryInfoPredicates 
// ============================================================================

/// ===========================================================================
#endif  ///< DETDESC__IGEOMETRYINFO_H
/// ===========================================================================


