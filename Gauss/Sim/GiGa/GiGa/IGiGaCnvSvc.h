#ifndef           __GIGA_CONVERSION_IGIGACNVSVC_H__ 
#define           __GIGA_CONVERSION_IGIGACNVSVC_H__  1 



#include "GaudiKernel/IConversionSvc.h" 

class IGiGaSvc;
class IGiGaSetUpSvc;
class IParticlePropertySvc;


extern const IID& IID_IGiGaCnvSvc; 

class IGiGaCnvSvc: virtual public IConversionSvc
{
  
  ///
 public: 
  ///
  
  virtual IGiGaSvc*              gigaSvc  ()  = 0 ;  
  virtual IGiGaSetUpSvc*         setupSvc ()  = 0 ; 
  virtual IParticlePropertySvc*  ppSvc    ()  = 0 ; 

 
  ///
  virtual ~IGiGaCnvSvc(){}; 
  ///

};


#endif     //     __GIGA_CONVERSION_IGIGACNVSVC_H__ 
