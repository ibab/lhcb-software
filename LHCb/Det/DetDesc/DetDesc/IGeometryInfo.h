// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IGeometryInfo.h,v 1.1 2001-03-13 21:57:29 mato Exp $
#ifndef  DETDESC_IGEOMETRYINFO_H
#define  DETDESC_IGEOMETRYINFO_H

// Include Files
#include "GaudiKernel/IInspectable.h"
#include "DetDesc/ILVolume.h" 

#include <iostream>
#include <string> 
#include <functional>

class HepPoint3D;
class HepTransform3D;
class StatusCode;
class ILVolume;


/** @class IGeometryInfo IGeometryInfo.h DetDesc/IGeometryInfo.h

    Interface to get (almost) all geometry information
     
*/ 

class IGeometryInfo : virtual public IInspectable 
{

public:
  typedef  std::vector<IGeometryInfo*>  IGIChildrens;   

  /** Is the GeometyInfo object associated with Logical Volume? 
      This is to distinguish the Ghost case from the others.
      
      A Detector Element ( or any arbitrary "Geometry Object")
      can be associated with Logical Volume in a Geometry Tree 
      almost all behaviour of all other functions depends on 
      the fact of this association. 
    
      There can be 3 types of such "association":
  
      1)  "Regular case" 
          An Logical Volume embedded into the overall 
          geometry tree is associated with this element.
  
      2)  "Orphan": 
          A standalone Logical Volume ( which is not 
          embedded into overall geometry tree ) is 
          associated with this "geometry object". 
          An interesting case: a "top" volume acts like "orphan" :-))
  
      3)  "Ghost": 
          No any Logical Volume is associated  with 
          this "geometry element". 
          Can be useful for naming, grouping  and 
          classification and navigation.           

  */
  virtual       bool             hasLVolume () const = 0;   
  
  /** Has the GeometryInfo object support in Geometry tree?
     distinguish "regular" from others
  */
  virtual       bool             hasSupport () const = 0;   

  /** 
    (A) Pure Geometrical Info  *****************************************************
        
       (what geomerical info can be associated with  
        "virtual" or "fictive" Geometry Elements?) 
       
    (I) 1) return the transformation matrix  from "Global" system 
  
           suggestion: for "ghosts","orphans" and top-mother 
                      it is just an Identity transformation
  */

  /// transformation matrix from the Global Reference system to the Local one 
  virtual const HepTransform3D&  matrix   () const = 0; 
  
  /// transformation matrix from the Local Reference system to the Global one 
  virtual const HepTransform3D&  matrixInv() const = 0; 

  /// perform transformation of point from the Global Reference System to Local Reference system
  virtual       HepPoint3D       toLocal  ( const HepPoint3D& globalPoint ) const = 0;
  
  /// perform transformation of point from the Local Reference System to Global Reference system
  virtual       HepPoint3D       toGlobal ( const HepPoint3D& localPoint  ) const = 0;
    
  /** Is inside the volume? 
        
   (II) 1) for regular case the defnition  *is inside* is trivial 
        2) for "orphan" case the definition is less trivial but also 
           can be defined in a proper way, e.g. by considering the 
           point as  "local point". Within this approach the previos
           case is just the combination of transformation into local
           frame and then  *is inside* method for pseudo-orphan.  
        3) for "ghosts" only one formal definition can be done - 
           it is just the logical  "OR" for all registered 
           daughter Geometry Elements. Obviously it triggers the 
           recursive calls. What to do if the ghost element has no 
           any descendants? The best way is just to return "false" - 
           "something" can not be *inside* of "nothing".  :-)) 
     
        NB! obviously one has a choice in the implementation between    
            findObject and retrieveObject. 
  
  */
  virtual       bool          isInside( const HepPoint3D& GlobalPoint ) const = 0; 
  
  /**  To which daughter the given global point belongs to?
       Returns the name of daughter to which (global) point belongs 
       to ( std::string("") in the case of error ) 
  
       *find daughter by point* family 
       If we are clever enough to define *isInside(...)* method 
       in a proper way (for regular case, orphans and ghosts) we 
       can easily find the daughter detector element what contains
       the given point. 
       But here we have several choices, which are triggred 
       several methods:
        1) look throught the dauhters only 
        2) look recursively through all levels 
        3) look recursively through certain amount of levels  only 
           (case intermediate between A.II.1 and A.II.2).
       The dephth can be controlled either explicitly  
  
       Therefore one can consider 3 possibilities/methods  
       and there exists 2(3?) ways of returning the results - 
       as the path(local or from top?) (const std::string&) 
       and as the C++ pointer to IGeometryInfo interface 
  */ 
  virtual       std::string       belongsToPath( const HepPoint3D& globalPoint ) = 0;   
  
  /// C++ pointer to daughter to which the given (global) point belongs to 
  virtual       IGeometryInfo*    belongsTo    ( const HepPoint3D& globalPoint ) = 0;

  /** name of daughter (on more deeper level) to what the given point belongs to
      if level = 0 - no search, return the name of current level 
      if level < 0 - perform search up to the most deepest level 
      if level > 0 - perform search up to not more then "level" levels;
  */
  virtual       std::string       belongsToPath( const HepPoint3D& globalPoint , 
						 const int         level ) = 0;   

  /** C++ pointer to daughter (on more deeper level) to what the given point belongs to
      if level = 0 - no search, return the pointer to the current Geometry Info object  
      if level < 0 - perform search up to the most deepest level 
      if level > 0 - perform search up to not more then "level" levels;
  */
  virtual       IGeometryInfo*    belongsTo    ( const HepPoint3D& globalPoint , 
						 const int         level ) = 0;
  
  /** Sometimes is is nesessary to know the exact full geometry location
      of a Point in a more detailed way then DetectorElement tree allows.
      I see only one way to do it - just to go into the real geometry tree.
      This operation can be time consuming and therefore it is worth to 
      return an additinal information with it 
      The depth can be controlled either explicitly via "level" parameter
    
      @param point input 3D point
      @param level number of levels to nagigate down the hierarchy
      @param start is the location (or path) of "nearest regular DetectorElement" 
                   which simultaneously contains the Point and has the connection 
                   with Actual Geometry Tree.
      @param volumePath retuned information
  */
  virtual StatusCode fullGeoInfoForPoint( const HepPoint3D&        point       , 
					  const int                level       , 
					  IGeometryInfo*&          start       , 
					  ILVolume::PVolumePath&   volumePath  ) = 0;

  virtual StatusCode fullGeoInfoForPoint( const HepPoint3D&        point       , 
					  const int                level       , 
					  IGeometryInfo*&          start       , 
					  ILVolume::ReplicaPath&   replicaPath ) = 0;

  virtual StatusCode fullGeoInfoForPoint( const HepPoint3D&        point       , 
					  const int                level       , 
					  std::string&             start       , 
					  ILVolume::PVolumePath&   volumePath  ) = 0;

  virtual StatusCode fullGeoInfoForPoint( const HepPoint3D&        point       , 
					  const int                level       , 
					  std::string&             start       , 
					  ILVolume::ReplicaPath&   replicaPath ) = 0;

  //
  //  How to answer the question - 
  //  What is the distance in radiation lengths between 2 points? 
  //  
  //    probably it is very frequent question during patters 
  //    recognition and track fitting, e.g. any kinds of Kalman 
  //    Filtering need the estimation of multiple scatetring.
  //    
  //    Obviously it is not an easy task. Especially if one needs
  //    *the exact* answer. *The exact* answer can be obtained only 
  //    from actual geometry tree/ but it is possible to implement 
  //    an rough estimation within  IGeometryInfo tree. 
  //
  //    If one asks for the calculations of quantities like 
  //    *distanceInRadiationLengthUnits(Point1,Point2)*
  //    it is almost obvious (at least for me) that the 
  //    algorithm is the following:
  //     1) to localize both points in the same low level 
  //    parent volume. It means that we should be 
  //    able to find the "closest common ancestry", which is 
  //    not "ghost" - "orphan" seems to me also acceptable, 
  //    but it is to be checked
  // 
  //     2) from this step one should be able to go to 
  //    tree of real geometry (if one need *the exact* answer) 
  //    Otherwise. the detector can have a simplified "mean"
  //    density of scatterers. Sometimes it is enough for rought 
  //    estimatioins  

  //  virtual       std::string      commonAncestryPath( IGeometryInfo* ) = 0; 
  //  virtual       IGeometryInfo*   commonAncestry    ( IGeometryInfo* ) = 0; 

  //  virtual       std::string      commonAncestryPath( const std::string& ) = 0; 
  //  virtual       IGeometryInfo*   commonAncestry    ( const std::string& ) = 0; 
  

  /** return the name of associated Logical Volume
      for "ghosts" the empty string and zero should be returned
  */
  virtual       const std::string&    lvolumeName() const = 0 ; 

  /// return the C++ pointer to assocoated Logical Volume
  virtual       ILVolume*             lvolume    () const = 0 ; 
  
  /// return the information about the support 
  virtual       StatusCode  location( IGeometryInfo*&        start , 
				      ILVolume::ReplicaPath& replicaPath ) const = 0;

  /// return the information about the support 
  virtual       StatusCode  location( std::string&           start , 
				      ILVolume::ReplicaPath& replicaPath ) const = 0;
  //
  // get the location of Logical Volume  ("shortcut")
  //
  // virtual       StatusCode  shortcut( IGeometryInfo*&        Start , 
  //	        			      ILVolume::REPLICAPATH& ReplicaPath ) = 0;
  // virtual       StatusCode  shortcut( std::string&           Start , 
  //		   		      ILVolume::REPLICAPATH& ReplicaPath ) = 0;
  //
  // here Start - the address (or path) of "regular DetectorElement"	    
  // from which one should start the traversing of Actual Geometry Tree 
  // to find the logical volume corresponding to  *this* DetectorElement.   
  // both arguments - are output arguments. 
  // 

  //
  // get Logical Volume by its full address - "start" point in 
  // DetectorDescription Tree  and the replica path in the Actual Geometry Tree, 
  // starting with the logical volume associated with "starting point"    
  //

  /// return the name of Logical Volume, addressed by  start and Replica Path
  virtual       std::string   lvolumePath( const std::string&           start       , 
					   const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /// return the Logical Volume, addressed by  start and Replica Path
  virtual       ILVolume*     lvolume    ( const std::string&           start       , 
					   const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /// return the name of Logical Volume, addressed by  start and Replica Path
  virtual       std::string   lvolumePath( IGeometryInfo*               start       , 
					   const ILVolume::ReplicaPath& replicaPath ) = 0;

  /// return the Logical Volume, addressed by  start and Replica Path  
  virtual       ILVolume*     lvolume    ( IGeometryInfo*               start       , 
					   const ILVolume::ReplicaPath& replicaPath ) = 0;
  
  /// (overloaded) printOut 
  virtual       std::ostream& printOut   ( std::ostream& ) const = 0;
  
  /// reset to the initial state 
  virtual const IGeometryInfo* reset     (               ) const = 0; 
  
  /// destructor 
  virtual   ~IGeometryInfo(){};
  
  //
  // functions for simplifications of navigation functions 
  // (according to feedback from users to release v3)
  //
  
  /// pointer to the parent IGeometryInfo
  virtual       IGeometryInfo*               parentIGeometryInfo()       = 0 ;

  /// pointer to the parent IGeometryInfo (const version)
  virtual const IGeometryInfo*               parentIGeometryInfo() const = 0 ;
  
  /// (reference to) container of children IGeometryInfo 
  virtual       IGeometryInfo::IGIChildrens& childIGeometryInfos()       = 0; 

  /// (reference to) container of children IGeometryInfo 
  virtual const IGeometryInfo::IGIChildrens& childIGeometryInfos() const = 0; 

  //
  // iterators for manipulation of daughter elements 
  //

  /// "begin" iterator for manipulation of daughter elements
  virtual IGeometryInfo::IGIChildrens::iterator        childBegin()       = 0 ; 

  /// "begin" iterator for manipulation of daughter elements (const version)
  virtual IGeometryInfo::IGIChildrens::const_iterator  childBegin() const = 0 ; 

  /// "end"   iterator for manipulation of daughter elements
  virtual IGeometryInfo::IGIChildrens::iterator        childEnd  ()       = 0 ; 

  /// "end"   iterator for manipulation of daughter elements (const version)
  virtual IGeometryInfo::IGIChildrens::const_iterator  childEnd  () const = 0 ; 

  /// serialization for reading 
  virtual StreamBuffer& serialize( StreamBuffer& )       = 0 ; 

  /// serialization for writing 
  virtual StreamBuffer& serialize( StreamBuffer& ) const = 0 ; 
   
};


inline std::ostream& operator<<( std::ostream& os , const IGeometryInfo& gi ) { return gi.printOut(os); };

inline std::ostream& operator<<( std::ostream& os , const IGeometryInfo* gi ) 
{ return  ( (0 == gi) ?  (os << "IGeometryInfo* points to NULL") : gi->printOut(os) ); };

inline StreamBuffer& operator<<( StreamBuffer& sb , const IGeometryInfo& gi ) { return gi.serialize(sb);};
inline StreamBuffer& operator>>( StreamBuffer& sb ,       IGeometryInfo& gi ) { return gi.serialize(sb);};
										  
/**  @class IGeomteryInfo_isInside
     useful predicate(functor) to be used in conjunction with STL algorithms.
*/                                     
class IGeometryInfo_isInside : std::unary_function<const IGeometryInfo*,bool>
{
  
 public:
  
  explicit IGeometryInfo_isInside( const HepPoint3D& Point )
    : point(&Point){};
  
  /// the only one essential method  
  inline bool operator () ( const IGeometryInfo* gi )  const 
    { return ( ( 0 == gi || 0 == point ) ? false : gi->isInside(*point) ); };
  
 private:
  
  const HepPoint3D* point;
  
};

#endif  //   DETDESC__IGEOMETRYINFO_H
