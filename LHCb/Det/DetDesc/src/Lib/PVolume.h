#ifndef     __DETDESC_VOLUMES_PVOLUME_H__
#define     __DETDESC_VOLUMES_PVOLUME_H__

#include <iostream>
#include <string>



#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"

#include "GaudiKernel/DataObject.h" 

class IDataProviderSvc;
class IMessageSvc;
class MsgStream;

#include "DetDesc/IPVolume.h"
#include "DetDesc/ILVolume.h"

class HepRotation;
class HepTransform3D;
class HepPoint3D; 

class GaudiException;
class PVolumeException;


///
///  class PVolume; a simple implementation of IPVolume interface
///
///  Author: Vanya Belyaev 
///

class PVolume: public IPVolume
{
  //
  friend class LVolume;
  //
 protected:
  /// constructor         ( protected ) 
  PVolume           ( const     std::string&     PhysVol_name                     ,
                      const     std::string&     LogVol_name                      ,
                      const     HepPoint3D&      Position                         , // position in Mother Reference Frame!
                      const     HepRotation&     Rotation       = HepRotation()   ); // rotation with respect to Mother Reference Frame
  ///
  PVolume           ( const     std::string&     PhysVol_name                     ,
                      const     std::string&     LogVol_name                      ,
                      const     HepTransform3D&  Transform                        ); // position in Mother Reference Frame!
  ///
  virtual ~PVolume();
  ///  
public:
  //     the name of the physical volume
  inline const std::string&     name       () const ; 
  // index is to be used for "replica" and "parametrized" implementation
  inline const std::string&     lvolumeName() const ;
  inline       ILVolume*        lvolume    () const ;  
  // index is to be used for "replica" and "parametrized" implementation
  inline const HepTransform3D&  matrix     () const ;
  inline       HepTransform3D   matrixInv  () const ; 
  //
  inline       HepPoint3D       toLocal    ( const HepPoint3D& PointInMother ) const;
  inline       HepPoint3D       toMother   ( const HepPoint3D& PointInLocal  ) const;
  //
  inline       bool             isInside   ( const HepPoint3D& PointInMother ) const;
  /// 
  virtual inline      std::ostream& printOut( std::ostream& ) const;
  virtual inline      MsgStream&    printOut( MsgStream&    ) const;
  /// reset to the initial state
  inline const IPVolume*        reset() const; 
  /// intersection of the physical volume with with line.
  /// line is parametrized in the local reference system of the mother 
  /// logical volume by initial Point and direction Vector.
  /// Method returns the number of intersection points("ticks") and 
  /// the container of pairs - ticks and pointer to the corresponding 
  /// material. The simplification is determined by value of threshold
  /// (in units of radiation length) 
  /// Method throws LVolumeException in the case, then 
  /// solid is not defined or material is not accessible.  
  inline  unsigned int intersectLine( const HepPoint3D        & Point         ,  // initial point at the line 
                                      const HepVector3D        & Vector        ,  // direction vector of the line 
                                      ILVolume::Intersections & intersections ,  // output container 
                                      const double              threshold     ); // threshold value 
  
  ///
  /// the same as previos method, but the intersection points ("ticks") are searched 
  /// in the region  tickMin<= tick <= tickMax 
  inline unsigned int intersectLine( const HepPoint3D        & Point         , // initial point at the line 
                                     const HepVector3D        & Vector        , // direction vectot of the line 
                                     ILVolume::Intersections & intersections , // output container  
                                     const ISolid::Tick        tickMin       , // minimum value of possible Tick
                                     const ISolid::Tick        tickMax       , // maximum value of possible Tick
                                     const double              Threshold     );// threshold value 
  ///
 private:
  // no default constructor 
  PVolume()                           ;
  // no public copy constructor 
  PVolume           ( const PVolume& );
  // no public assignment
  PVolume& operator=( const PVolume& );
  // technical - 
  // find logical volume by name 
  ILVolume*                 findLogical   () const ; 
  // return data service 
  inline IDataProviderSvc*  dataSvc       () const ; 
  // return message service 
  inline IMessageSvc*       msgSvc        () const ;  
  // Assertion 
  inline void Assert( bool               assertion , 
                      const std::string& name      ) const;
  //
  // Assertion 
  inline void Assert( bool                  assertion , 
                      const std::string&    name      , 
                      const GaudiException& Exception ) const;
  ///  from IInspectable interface
  virtual bool acceptInspector( IInspector* pInspector )       ; 
  virtual bool acceptInspector( IInspector* pInspector ) const ; 
  ///
 private:
  // "native" features  - 
  std::string         m_pv_name;     // name of physical volume
  std::string         m_pv_lvname;   // name of corresponding logical volume ( path in transient store
  HepTransform3D      m_pv_matrix;   // transformation matrix from (unknown) MOTHER to LOCAL frame  
  // C++ pointer to the corresponding logical volume  
  mutable ILVolume*   m_pv_lvolume;     
  // "very private" 
  IDataProviderSvc*   m_pv_dataSvc; 
};
///

/// local 
#include "PVolume.icpp" 
///

#endif  //  __DETDESC_VOLUMES_PVOLUME_H__

