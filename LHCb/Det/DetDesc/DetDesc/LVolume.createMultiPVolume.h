#ifndef     __DETDESC_VOLUMES_LVOLUME_CREATEMULTIPVOLUME_H__
#define     __DETDESC_VOLUMES_LVOLUME_CREATEMULTIPVOLUME_H__ 1 

#include <strstream> 


///
/// implementation of LVolume::createMultiPVolume methods  
///


IPVolume* LVolume::createMultiPVolume( const std::string&   PVname_base     , 
                                       const std::string&   LVnameForPV     , 
				       const unsigned long  nStep           , 
				       const HepPoint3D&    initialPosition ,
				       const HepRotation&   initialRotation ,     
				       const HepVector3D&   stepTranslation ,
                                       const HepRotation&   stepRotation    )
{
  ///
  ///
  ///

  IPVolume* pv = 0 ; 
  HepPoint3D  position( initialPosition ) ; 
  HepRotation rotation( initialRotation ) ; 

  ///
  ///
  ///
  
  for( long step = 0 ; step < nStep ; ++step ) 
    {
      ///
      ///
      const int buffer_size = 256;  
      char buffer[ buffer_size ] = { 0 , 0 }; 
      std::ostrstream ost(buffer, buffer_size); 
      ost << PVname_base << "_[" << step << "]"; 
      ///
      const unsigned int len = strlen(ost.str()); 
      char *resstr = new char[len+1]; 
      strncpy(resstr,ost.str(),len);
      resstr[len] = 0; 
      ///
      ///
      std::string pvname( resstr ); 
      delete [] resstr;
      ///
      ///
      ///
      pv = createPVolume( pvname , LVnameForPV , position , rotation ); 
      ///
      position += stepTranslation ; 
      rotation *= stepRotation    ;  
    }

  return pv;

};

///
///
///


#endif  //  __DETDESC_VOLUMES_LVOLUME_CREATEMULTIPVOLUME_H__
