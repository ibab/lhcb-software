#ifndef      __GAUDI_INTERFACES__I_GIGA_KINE_CNV_SVC_H__
#define      __GAUDI_INTERFACES__I_GIGA_KINE_CNV_SVC_H__ 1 


///
#include "GiGa/IGiGaCnvSvc.h" 

///
class StatusCode; 
class G4PrimaryVertex; 

#include "GiGa/IIDIGiGaKineCnvSvc.h"


/** @class IGiGaKineCnvSvc     IGiGaKineCnvSvc.h GiGa/IGiGaKineCnvSvc.h

    definition of abstract interface to Geant 4 conversion service 
    which is responsible for conversion from primary event kinematics from 
    Gaudi representation to GEANT4 primary event structures 
    
    @author Vanya Belyaev
*/


class IGiGaKineCnvSvc: virtual public IGiGaCnvSvc
{

 public:
  ///
  static const InterfaceID& interfaceID() { return IID_IGiGaKineCnvSvc; }
  ///
  virtual ~IGiGaKineCnvSvc(){} ; 
  ///
};


///
///
/// 

#endif   //  __GAUDI_INTERFACES__I_GIGA_KINE_CNV_SVC_H__
