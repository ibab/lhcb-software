// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/ILVolume.h,v 1.1 2001-03-13 21:57:30 mato Exp $
#ifndef  DETDESC_ILVOLUME_H
#define  DETDESC_ILVOLUME_H

// Include files
#include "DetDesc/ISolid.h"
#include "GaudiKernel/IInspectable.h"

#include <iostream> 
#include <string> 
#include <vector> 
#include <deque> 


// Forward declarations
class StatusCode;
class HepPoint3D;
class HepVector3D;
class MsgStream; 

class Material; 
class IPVolume;
class ISolid;


/** @class ILVolume ILVolume.h DetDesc/ILVolume.h

    An abstract interface to named node in the geometry tree.

    @author Vanya Belyaev
*/

class ILVolume : virtual public IInspectable 
{
  ///
 public:
  /// general typedefs 
  typedef  std::vector<IPVolume*>                        PVolumes;
  typedef  PVolumes::size_type                           ReplicaType;
  typedef  std::deque<ReplicaType>                       ReplicaPath;
  typedef  std::deque<IPVolume*>                         PVolumePath;
  /// typedefs, specific for calculation of distance in the units of radiation length
  typedef std::pair<ISolid::Tick,ISolid::Tick>           Interval;  
  typedef std::pair<Interval,const Material*>            Intersection;
  typedef std::vector<Intersection>                      Intersections; 
  ///
 public:
  /// return the name(identification)  of Logical Volume  
  virtual const   std::string&                       name        () const = 0;
  /// return the solid, associated with the Logical Volume  
  virtual const   ISolid*                            solid       () const = 0;
  /// return the material(by name) , associated with the Logical Volume  
  virtual const   std::string&                       materialName() const = 0;
  /// return the material, associated with the Logical Volume  
  virtual const   Material*                          material    () const = 0; 
  /// return number of Physical(positioned) Volumes inside gived Logical Volume
  virtual         ILVolume::ReplicaType              noPVolumes  () const = 0; 
  /// get daughter (Physical Volume) by index
  virtual         IPVolume*                          operator[]( const ILVolume::ReplicaType& index ) const = 0;
  /// get daughter (Physical Volume) by it's name
  virtual         IPVolume*                          operator[]( const std::string&           name  ) const = 0;
  /// get daughter (Physical Volume) by index 
  virtual         IPVolume*                          pvolume   ( const ILVolume::ReplicaType& index ) const = 0;
  /// get daughter (Physical Volume) by name 
  virtual         IPVolume*                          pvolume   ( const std::string&           name  ) const = 0;
  /// begin iterator  for manipulation with daughters
  virtual         ILVolume::PVolumes::iterator       pvBegin     ()        = 0 ; 
  /// begin iterator for manipulation with daughters (const version) 
  virtual         ILVolume::PVolumes::const_iterator pvBegin     () const  = 0 ; 
  /// end iterator for manipulation with daughters  
  virtual         ILVolume::PVolumes::iterator       pvEnd       ()        = 0 ; 
  /// end iterator for manipulation with daughters (const version)  
  virtual         ILVolume::PVolumes::const_iterator pvEnd       () const  = 0 ; 
  /// traverse the sequence of paths  (transform the sequence of replicas to sequence of  physical volumes 
  virtual StatusCode traverse ( ILVolume::ReplicaPath::const_iterator pathBegin,
				ILVolume::ReplicaPath::const_iterator pathEnd  ,
				ILVolume::PVolumePath&                volumePath ) = 0;
  /// traverse the sequence of paths  (transform the sequence of replicas to sequence of  physical volumes 
  virtual StatusCode traverse ( const ILVolume::ReplicaPath&  replicaPath,
				ILVolume::PVolumePath&        volumePath ) = 0;  
  /// is the given point  inside this Logical Volume? (delegation to Solid)
  virtual bool       isInside ( const HepPoint3D& LocalPoint ) const = 0; 
  /** calculate the daughter path containing the Point in Local frame , 
      can be VERY slow for complex geometry, therefore use the appropriate Level for usage 
  */
  virtual StatusCode belongsTo( const HepPoint3D&        localPoint ,
				const int                level      , 
				ILVolume::PVolumePath&   volumePath ) = 0;
  /** calculate the daughter path containing the Point in Local frame , 
      can be VERY slow for complex geometry , therefore use the appropriate Level for usage 
  */
  virtual StatusCode belongsTo( const HepPoint3D&        localPoint ,
				const int                level      , 
				ILVolume::ReplicaPath&   replicaPath ) = 0;        
  /** intersection of the logical volume with with line.
      line is parametrized in the local reference system of the logical volume 
      by initial Point and direction Vector.
      Method returns the number of intersection points("ticks") and 
      the container of pairs - ticks and pointer to the corresponding 
      material. The simplification is determined by value of threshold
      (in units of radiation length) 
      Method throws LVolumeException in the case, then 
      solid is not defined or material is not accessible.

      @param Point initial point at the line
      @param Vector direction vector of the line
      @param intersections output container 
      @param threshold threshold value 
  */
  virtual unsigned int intersectLine( const HepPoint3D  & Point,  
				      const HepVector3D       & Vector        ,  
				      ILVolume::Intersections & intersections ,
				      const double              threshold     ) = 0 ;  
  /** the same as previous method, but the intersection points ("ticks") are searched 
      in the region  tickMin<= tick <= tickMax

      @param Point initial point at the line
      @param Vector direction vector of the line
      @param intersections output container
      @param tickMin minimum value of possible Tick
      @param tickMax maximum value of possible Tick
      @param threshold threshold value 

  */
  virtual unsigned int intersectLine( const HepPoint3D  & Point ,
				      const HepVector3D       & Vector        ,
				      ILVolume::Intersections & intersections , 
				      const ISolid::Tick        tickMin       ,
				      const ISolid::Tick        tickMax       , 
				      const double              Threshold     ) = 0 ; 
  /// name of sensitive "detector" - needed for simulation 
  virtual const std::string& sdName   ()                const = 0 ; 
  ///  magnetic properties  (if needed) 
  virtual const std::string& mfName   ()                const = 0 ; 
  /// destructor 
  virtual ~ILVolume(){};
  /// for overloading of std::ostream& << 
  virtual std::ostream&      printOut( std::ostream & ) const = 0;
  /// reset to the initial state 
  virtual const ILVolume* reset ()                      const = 0; 
  ///  
};
///
inline std::ostream& operator<<( std::ostream& os , const ILVolume& lv ) { return lv.printOut( os ); };
///
inline std::ostream& operator<<( std::ostream& os , const ILVolume*  plv )
{ return ( ( 0 == plv ) ? ( os << " ILVolume* points to NULL ") : ( os << *plv) ) ; };
///

#endif  //   DETDESC_ILVOLUME_H
