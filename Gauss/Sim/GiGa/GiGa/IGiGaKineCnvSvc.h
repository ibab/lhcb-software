#ifndef      __GAUDI_INTERFACES__I_GIGA_KINE_CNV_SVC_H__
#define      __GAUDI_INTERFACES__I_GIGA_KINE_CNV_SVC_H__


///
///
///

#include "GaudiKernel/IConversionSvc.h" 

///
///
///
class StatusCode; 
class G4PrimaryVertex; 


///
///
///
///   IGiGaKineCnvSvc: definition of abstract interface for conversion service 
///                    which is responsible for conversion from primary event kinematics from 
///                    Gaudi representation to GEANT4 primary event structures 
///
///   Author Vanya Belyaev 
///   Date   7 Aug 2000
///
///
  

class IGiGaKineCnvSvc: virtual public IConversionSvc
{

 public:

  ///
  virtual ~IGiGaKineCnvSvc(){} ; 
  ///
};


///
///
/// 

#endif   //  __GAUDI_INTERFACES__I_GIGA_KINE_CNV_SVC_H__
