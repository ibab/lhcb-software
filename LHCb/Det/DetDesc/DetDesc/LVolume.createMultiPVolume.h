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
  
  for( unsigned long step = 0 ; step < nStep ; ++step ) 
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
/// two loops 
///


IPVolume* LVolume::createMultiPVolume( const std::string&   PVname_base      , 
                                       const std::string&   LVnameForPV      , 
				       const HepPoint3D&    initialPosition  ,
				       const HepRotation&   initialRotation  ,     
				       const unsigned long  nStep1           , 
				       const HepVector3D&   stepTranslation1 ,
                                       const HepRotation&   stepRotation1    ,
				       const unsigned long  nStep2           , 
				       const HepVector3D&   stepTranslation2 ,
                                       const HepRotation&   stepRotation2    )
{
  ///
  ///
  ///

  IPVolume* pv = 0 ; 
  HepPoint3D  position1( initialPosition ) ; 
  HepRotation rotation1( initialRotation ) ; 

  ///
  ///
  ///
  for( unsigned long step1 = 0 ; step1 < nStep1 ; ++step1 )
    {
      ///
      HepPoint3D  position2( position1 ) ; 
      HepRotation rotation2( rotation1 ) ; 
      ///
      for( unsigned long step2 = 0 ; step2 < nStep2 ; ++step2 ) 
	{
	  ///
	  ///
	  const int buffer_size = 256;  
	  char buffer[ buffer_size ] = { 0 , 0 }; 
	  std::ostrstream ost(buffer, buffer_size); 
	  ost << PVname_base << "_[" << step1 << "][" << step2 << "]"; 
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
	  pv = createPVolume( pvname , LVnameForPV , position2 , rotation2 ); 
	  ///
	  position2 += stepTranslation2 ; 
	  rotation2 *= stepRotation2    ;  
	  ///
	}
      ///
      position1 += stepTranslation1 ; 
      rotation1 *= stepRotation1    ;  
      ///
    }

  return pv;

};


///
/// three loops 
///


IPVolume* LVolume::createMultiPVolume( const std::string&   PVname_base      , 
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
                                       const HepRotation&   stepRotation3    )
{
  ///
  ///
  ///

  IPVolume* pv = 0 ; 
  HepPoint3D  position1( initialPosition ) ; 
  HepRotation rotation1( initialRotation ) ; 

  ///
  ///
  ///
  for( unsigned long step1 = 0 ; step1 < nStep1 ; ++step1 )
    {
      ///
      HepPoint3D  position2( position1 ) ; 
      HepRotation rotation2( rotation1 ) ; 
      ///
      for( unsigned long step2 = 0 ; step2 < nStep2 ; ++step2 )
	{
	  /// 
	  HepPoint3D  position3( position2 ) ; 
	  HepRotation rotation3( rotation2 ) ; 
	  ///
	  for( unsigned long step3 = 0 ; step3 < nStep3 ; ++step3 ) 
	    {
	      ///
	      ///
	      const int buffer_size = 256;  
	      char buffer[ buffer_size ] = { 0 , 0 }; 
	      std::ostrstream ost(buffer, buffer_size); 
	      ost << PVname_base << "_[" << step1 << "][" << step2 << "][" << step3 << "]"; 
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
	      pv = createPVolume( pvname , LVnameForPV , position3 , rotation3 ); 
	      ///
	      position3 += stepTranslation3 ; 
	      rotation3 *= stepRotation3    ;  
	      ///
	    }
	  ///	  
	  position2 += stepTranslation2 ; 
	  rotation2 *= stepRotation2    ;  
	  ///
	}
      ///
      position1 += stepTranslation1 ; 
      rotation1 *= stepRotation1    ;  
      ///
    }
  
  return pv;

};

///
///
///


#endif  //  __DETDESC_VOLUMES_LVOLUME_CREATEMULTIPVOLUME_H__












