#ifndef      __DETDESC_VOLUMES_LVOLUME_CREATEPVOLUME_H__
#define      __DETDESC_VOLUMES_LVOLUME_CREATEPVOLUME_H__ 1



///
///  inmplementation of LVolume::createPVolume  methods 
///
/// Author :: Vanya Belyaev 
///

///
/// specific for this implementation 
///  creation of Physical Volume 

IPVolume* LVolume::createPVolume( const std::string&    PVname         , 
				  const std::string&    LVnameForPV    )
{
  HepPoint3D  Position;
  HepRotation Rotation;
  return createPVolume( PVname , LVnameForPV , Position , Rotation );
};

///
/// creation of Physical Volume 
///

IPVolume* LVolume::createPVolume( const std::string&    PVname         , 
				  const std::string&    LVnameForPV    ,
				  const HepPoint3D&     Position       )
{
  HepRotation Rotation;
  return createPVolume( PVname , LVnameForPV , Position , Rotation );
};


///
/// creation of Physical Volume 
///

IPVolume* LVolume::createPVolume( const std::string&    PVname         , 
				  const std::string&    LVnameForPV    ,
				  const HepPoint3D&     Position       ,
                                  const HepRotation&    Rotation       ) 
{
  //
  PVolume* pv = 0; 
  try{ pv = new  PVolume( PVname , LVnameForPV , Position , Rotation , dataSvc() , msgSvc() ); }
  catch( const GaudiException& Exception ) 
    { Assert( false , "LVolume::createPVolume() (3) , exception caught! " , Exception ); } 
  catch(...)                        
    { Assert( false , "LVolume::createPVolume() (3) , unknown exception caught! "     ); } 
  ///
  Assert( 0 != pv , "LVolume::createPVolume, could not create volume "+PVname+"(lv="+LVnameForPV+")" );
  /// 
  m_lv_pvolumes.push_back( pv ); 
  ///  
  return pv; 
};




#endif  //   __DETDESC_VOLUMES_LVOLUME_CREATEPVOLUME_H__
