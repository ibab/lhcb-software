#ifndef           __GIGA_CONVERSION_IGIGACNVSVC_H__ 
#define           __GIGA_CONVERSION_IGIGACNVSVC_H__  1 



#include "GaudiKernel/IConversionSvc.h" 

class IGiGaSvc;
class IGiGaSetUpSvc;
class IParticlePropertySvc;

#include "GiGa/IIDIGiGaCnvSvc.h"

/** @class IGiGaCnvSvc     IGiGaCnvSvc.h GiGa/IGiGaCnvSvc.h

    definition of abstract interface to Geant 4 conversion service 
    which is responsible for conversion from primary event kinematics from 
    Gaudi representation to GEANT4 primary event structures 
    
    @author Vanya Belyaev
*/


class IGiGaCnvSvc: virtual public IConversionSvc
{
  
  ///
 public: 
  ///  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaCnvSvc; }
  ///
  virtual IGiGaSvc*              gigaSvc  ()  = 0 ;  
  ///
  virtual IGiGaSetUpSvc*         setupSvc ()  = 0 ; 
  ///
  virtual IParticlePropertySvc*  ppSvc    ()  = 0 ;  
  ///

  ///
  virtual ~IGiGaCnvSvc(){}; 
  ///

};


#endif     //     __GIGA_CONVERSION_IGIGACNVSVC_H__ 
