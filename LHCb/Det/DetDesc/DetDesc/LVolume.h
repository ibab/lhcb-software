#ifndef     __DETDESC_VOLUMES_LVOLUME_H_
#define     __DETDESC_VOLUMES_LVOLUME_H_


#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm> 

template <class TYPE> class DataObjectFactory; 

class HepPoint3D; 
class HepVector3D; 
class HepRotation;
class HepTransform3D;


class IDataProviderSvc;
class GaudiException; 

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/IPVolume.h"
#include "GaudiKernel/ISolid.h" 
#include "GaudiKernel/ITime.h" 
#include "GaudiKernel/MsgStream.h" 

#include "DetDesc/IPVolume_predicates.h" 
#include "DetDesc/CLIDLVolume.h"


///
///  class LVolume: a simple implementition of ILVolume interface 
///
/// Author: Vanya Belyaev 
///


class LVolume: public DataObject ,
               public ILVolume   , 
	       public IValidity
{
  ///
  friend class DataObjectFactory<LVolume>;
  ///
 private:
  ///
  /// default constructor is private!!!!!
  LVolume();       
  ///
 public: 
  
  //
  // constructor, pointer to ISolid* must be valid!, overvise constructor throws LVolumeException!  
  LVolume( const std::string& name            , 
	   ISolid*            Solid           ,
           const std::string& material        ,
           const ITime&       validSince      , 
           const ITime&       validTill       , 
           IDataProviderSvc*  dataService = 0 , 
           IMessageSvc*       messService = 0 );

  // constructor, pointer to ISolid* must be valid!, overvise constructor throws LVolumeException!  
  LVolume( const std::string& name            , 
	   ISolid*            Solid           ,
           const std::string& material        ,
           const std::string& sdname          ,
           const ITime&       validSince      , 
           const ITime&       validTill       , 
           IDataProviderSvc*  dataService = 0 , 
           IMessageSvc*       messService = 0 );
  // constructor, pointer to ISolid* must be valid!, overvise constructor throws LVolumeException!  
  LVolume( const std::string& name            , 
	   ISolid*            Solid           ,
           const std::string& material        ,
           IDataProviderSvc*  dataService = 0 , 
           IMessageSvc*       messService = 0 );
  // constructor, pointer to ISolid* must be valid!, overvise constructor throws LVolumeException!  
  LVolume( const std::string& name            , 
	   ISolid*            Solid           ,
           const std::string& material        ,
           const std::string& sdname          ,
           IDataProviderSvc*  dataService = 0 , 
           IMessageSvc*       messService = 0 );
  
  // destructor 
  virtual ~LVolume();
  
 public:

  ///
  /// from DataObject base class
  ///
  inline const CLID& clID   () const { return classID(); } 
  ///
  static const CLID& classID()       { return CLID_LVolume; };
  ///
  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s );
  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s )  const;
  ///

  ///
  /// functions from ILVolume - see ILVolume.h for description
  ///

  /// return the name(full path) for this LVolume
  inline const   std::string&  name        () const { return DataObject::fullpath() ; } 

  /// return the C++ pointer to solid 
  inline const   ISolid*       solid       () const;

  /// return the material (C++ pointer)
  inline  const  Material*     material    ()      ;

  /// return the material (name)
  inline  const  std::string&  materialName() const;

  /// get number of daughter volumes 
  inline         ILVolume::ReplicaType   noPVolumes() const; 
  
  /// get daughter physical volume by index 
  inline         IPVolume*               operator[]( const ILVolume::ReplicaType& index ) const;
  
  /// get daughter physical volume by name  
  inline         IPVolume*               operator[]( const std::string&           name  ) const;

  /// get daughter (Physical Volume) by index 
  inline         IPVolume*               pvolume   ( const ILVolume::ReplicaType& index ) const;
  
  /// get daughter (Physical Volume) by name 
  inline         IPVolume*               pvolume   ( const std::string&           name  ) const;
  
  /// iterators for manipulation with daughters  
  
  /// begin iterator 
  inline ILVolume::PVolumes::iterator       pvBegin     (); 

  /// begin iterator (const version)
  inline ILVolume::PVolumes::const_iterator pvBegin     () const; 

  /// end iterator 
  inline ILVolume::PVolumes::iterator       pvEnd       (); 

  /// end iterator   (const version)
  inline ILVolume::PVolumes::const_iterator pvEnd       () const; 
  
  /// traverse the sequence of paths  (transform the sequence of replicas to sequence of  physical volumes
  inline StatusCode traverse ( ILVolume::ReplicaPath::const_iterator pathBegin,
			       ILVolume::ReplicaPath::const_iterator pathEnd  ,
			       ILVolume::PVolumePath&                pVolumePath );

  /// traverse the sequence of paths  (transform the sequence of replicas to sequence of  physical volumes
  inline StatusCode traverse ( const ILVolume::ReplicaPath&  path,
			       ILVolume::PVolumePath&        pVolumePath );
  
  /// is this point inside?
  inline bool       isInside ( const HepPoint3D& LocalPoint ) const; 
  
  /// return the PVolumePath to the local point at the givel Level 
  inline StatusCode belongsTo( const HepPoint3D&        LocalPoint ,
			       const int                Level      , 
			       ILVolume::PVolumePath&   pVolumePath );
  
  /// return the ReplicaPath to the local point at the givel Level 
  inline StatusCode belongsTo( const HepPoint3D&        LocalPoint ,
			       const int                Level      , 
			       ILVolume::ReplicaPath&   replicaPath );        
  
  /// for overloading of std::ostream& << 
  inline std::ostream& printOut( std::ostream& ) const;
  
  /// for overloading of MsgStream& << 
  MsgStream&    printOut( MsgStream&    ) const;

  /// reset to the initial state
  inline const ILVolume* reset () const; 

  /// return the name of sensitive "detector" (needed for simulation)
  inline const std::string& sdName () const { return m_lv_sdName; } ;  
  ///
  /// intersection of the logical volume with with line.
  /// line is parametrized in the local reference system of the logical volume 
  /// by initial Point and direction Vector.
  /// Method returns the number of intersection points("ticks") and 
  /// the container of pairs - ticks and pointer to the corresponding 
  /// material. The simplification is determined by value of threshold
  /// (in units of radiation length) 
  /// Method throws LVolumeException in the case, then 
  /// solid is not defined or material is not accessible.  
  unsigned int intersectLine( const HepPoint3D        & Point         ,  // initial point at the line 
			      const HepVector3D       & Vector        ,  // direction vector of the line 
			      ILVolume::Intersections & intersections ,  // output container 
			      const double              threshold     ); // threshold value 
  ///
  /// the same as previous method, but the intersection points ("ticks") are searched 
  /// in the region  tickMin<= tick <= tickMax 
  unsigned int intersectLine( const HepPoint3D         & Point        , // initial point at the line 
			      const HepVector3D        & Vector       , // direction vectot of the line 
			      ILVolume::Intersections & intersections , // output container  
			      const ISolid::Tick        tickMin       , // minimum value of possible Tick
			      const ISolid::Tick        tickMax       , // maximum value of possible Tick
			      const double              Threshold     );// threshold value 
  
  //
  //   
  // from IValidity interface
  //   
  inline       bool    isValid          ()                ;   
  inline       bool    isValid          ( const ITime& )  ;   
  
  const        ITime&  validSince       ()                ;    
  const        ITime&  validTill        ()                ;   
  
  void                 setValidity      ( const ITime& , 
					  const ITime& )  ;  
  void                 setValiditySince ( const ITime& )  ;  
  void                 setValidityTill  ( const ITime& )  ;   
  
  StatusCode           updateValidity   ()                ;   // not yet
  
  ///
  ///  from IInspectable interface 
  ///
  virtual bool acceptInspector( IInspector* pInspector )       ; 
  ///
  virtual bool acceptInspector( IInspector* pInspector ) const ; 

  ///
  /// specific for this implementation 
  ///
    
  IPVolume* createPVolume( const std::string&    PVname         , 
			   const std::string&    LVnameForPV    );
  IPVolume* createPVolume( const std::string&    PVname         , 
			   const std::string&    LVnameForPV    ,
                           const HepPoint3D&     position       );     /// position of PVolume inside LVolume
  IPVolume* createPVolume( const std::string&    PVname         , 
			   const std::string&    LVnameForPV    ,
                           const HepPoint3D&     position       ,      /// position of PVolume inside LVolume
                           const HepRotation&    rotation       );     /// rotation to be applied 
  IPVolume* createPVolume( const std::string&    PVname         , 
			   const std::string&    LVnameForPV    ,
                           const HepTransform3D& Transform      );
  ///
  /// create group of physical volumes  
  /// 

  /// one loop 
  IPVolume* createMultiPVolume( const std::string&   PVname_base     , 
				const std::string&   LVnameForPV     , 
				const unsigned long  nStep           , 
				const HepPoint3D&    initialPosition ,
				const HepRotation&   initialRotation ,     
				const HepVector3D&   stepTranslation ,
				const HepRotation&   stepRotation    );
  /// two loops 
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
  /// three loops 
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

  ///
 private: 
  
  
  // copy constructor is private! 
  LVolume           ( const LVolume& );
  
  // assignment operator is private! 
  LVolume& operator=( const LVolume& ); 
  
 private:
  
  //
  // technicalities:
  
  // find C++ pointer to material by it's name 
  Material*                          findMaterial() ;
  
  //
  // exceptions:
  class LVolumeException;                                                                  
  
  // assertion
  inline void Assert( bool               assertion                       , 
		      const std::string& name                            ,
                      const StatusCode&  sc        = StatusCode::FAILURE ) const;  
  // assertion
  inline void Assert( bool                  assertion , 
		      const std::string&    name      ,
                      const GaudiException& Exception , 
                      const StatusCode&     sc        = StatusCode::FAILURE ) const;  
  
  // data service used for retriving of the material 
  IDataProviderSvc*            dataSvc       ();
  
  // message service used for printing in the exceptions 
  inline IMessageSvc*          msgSvc        ()  const;

  ///
  /// Auxillary method  to calculate intersections with daughter volumes  
  ///
  unsigned int  intersectDaughters( const HepPoint3D&        Point              , 
				    const HepVector3D&       Vector             , 
				    ILVolume::Intersections& childIntersections , 
				    const ISolid::Tick       tickMin            , 
				    const ISolid::Tick       tickMax            , 
				    const double             Threshold          );  

  ///
  /// Auxillary method  to calculate intersections with daughter volumes  
  ///
  unsigned int  intersectDaughters( const HepPoint3D&        Point              , 
				    const HepVector3D&       Vector             , 
				    ILVolume::Intersections& childIntersections , 
				    const double             Threshold          );  
  
  
 private:
  //
  ISolid*                      m_lv_solid        ;  //    pointer to solid
  std::string                  m_lv_materialName ;  //    name of material 
  ILVolume::PVolumes           m_lv_pvolumes     ;  //    container of daughter's physical volumes 
  //
  mutable  Material*           m_lv_material     ;  //    pointer to material 
  //
  std::string                  m_lv_sdName       ;  //     name of sensitive "detector" for simulation   
  // IValidity:
  ITime*                       m_lv_validSince   ;  //     
  ITime*                       m_lv_validTill    ;  // 
  
  // "very private" 
  IDataProviderSvc*            m_lv_dataSvc      ;  //     data service used for manipulations 
  IMessageSvc*                 m_lv_messSvc      ;  //     message service used for printing 
  //

};

///
///
///

inline MsgStream& operator<<( MsgStream& os , const LVolume&  lv ) { return lv.printOut( os ); };
inline MsgStream& operator<<( MsgStream& os , const LVolume* plv ) { return ( ( 0 == plv ) ? ( os << "LVolume* points to NULL") : ( os << *plv) ) ; };

///
///
///

#include "DetDesc/LVolume.LVolumeException.h" 
#include "DetDesc/LVolume.icpp" 
///
///
///

  
#endif  //  __DETDESC_VOLUMES_LVOLUME_H_



