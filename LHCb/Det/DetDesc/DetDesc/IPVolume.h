// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IPVolume.h,v 1.2 2001-03-15 12:43:38 ibelyaev Exp $
#ifndef  DETDESC_IPVOLUME_H 
#define  DETDESC_IPVOLUME_H 1 

// Include files
#include "GaudiKernel/IInspectable.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/ISolid.h"

#include <iostream>
#include <string> 

class ILVolume;


/// from CLHEP 
class HepPoint3D;
class HepVector3D;
class HepTransform3D;

/** @class IPVolume IPVolume.h DetDesc/IPVolume.h

    Interface to deal with the notion of "positioned Logical Volume" 
    (corresponds to Geant4 notion of "G4PhysicalVolume").
    This is the minimal interface for "physical volume".
    It corresponds to Geant4 "Placement" philosophy.

    @author Vanya Belyaev
*/      

class IPVolume : virtual public IInspectable 
{
 public:
  
  ///  Name of the physical volume
  virtual const std::string&     name       () const = 0; 
  
  /// Name of associated Logical Volume 
  virtual const std::string&     lvolumeName() const = 0;

  /// C++ pointer to Locical Volume 
  virtual       ILVolume*        lvolume    () const = 0; 

  /// Tranformation matrix   
  virtual const HepTransform3D&  matrix     () const = 0;

  /// Inverse transformation matrix 
  virtual       HepTransform3D   matrixInv  () const = 0; 
  
  /// transform point in Mother Reference System  to the Local one 
  virtual       HepPoint3D       toLocal    ( const HepPoint3D& PointInMother ) const = 0;
  
  /// transform point in Local Reference System to the Mother Reference System  
  virtual       HepPoint3D       toMother   ( const HepPoint3D& PointInLocal  ) const = 0;
  
  /// "is inside" method
  virtual       bool             isInside   ( const HepPoint3D& PointInMother ) const = 0;
  
  ///  printout 
  virtual std::ostream& printOut( std::ostream& ) const = 0;

  /// reset to the initial state 
  virtual const IPVolume*        reset      ()    const = 0; 
       
  /** Intersection of the physical volume with with line.
      line is parametrized in the local reference system of the mother 
      logical volume by initial Point and direction Vector.
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
  virtual unsigned int intersectLine( const HepPoint3D        & Point         ,
				                              const HepVector3D       & Vector        , 
				                              ILVolume::Intersections & intersections ,
				                              const double              threshold     ) = 0 ;
  
  /** the same as previos method, but the intersection points ("ticks") are searched 
      in the region  tickMin<= tick <= tickMax
      @param Point initial point at the line
      @param Vector direction vector of the line
      @param intersections output container 
      @param tickMin minimum value of possible Tick
      @param tickMax  maximum value of possible Tick
      @param threshold threshold value 
  */
  virtual unsigned int intersectLine( const HepPoint3D        & Point ,
				                              const HepVector3D       & Vector        ,       
				                              ILVolume::Intersections & intersections ,      
				                              const ISolid::Tick        tickMin       ,
				                              const ISolid::Tick        tickMax       ,
				                              const double              Threshold     ) = 0 ;
  /// virtual destructor
  virtual  ~IPVolume(){};  
};

inline std::ostream& operator<<( std::ostream&  os ,  const IPVolume& pv ) { return pv.printOut( os ) ; };

inline std::ostream& operator<<( std::ostream&  os ,  const IPVolume* ppv )
{  return ( ( 0 == ppv ) ? (os << " IPVolume* points to NULL ") : ( os << *ppv ) ); }

#endif   //   DETDESC_IPVOLUME_H 
