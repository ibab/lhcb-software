#ifndef      __GAUDI_INTERFACES__I_GIGA_Hits_CNV_SVC_H__
#define      __GAUDI_INTERFACES__I_GIGA_Hits_CNV_SVC_H__ 1 


///
#include "GiGa/IGiGaCnvSvc.h" 

#include "GiGa/IIDIGiGaHitsCnvSvc.h"


/** @class IGiGaHitsCnvSvc     IGiGaHitsCnvSvc.h GiGa/IGiGaHitsCnvSvc.h

    definition of abstract interface to Geant 4 conversion service 
    which is responsible for conversion from Geant4 hits into
    Gaudi Hits 
    
    @author Vanya Belyaev
*/


class IGiGaHitsCnvSvc: virtual public IGiGaCnvSvc
{
  ///
 public:
  ///
  static const InterfaceID& interfaceID() { return IID_IGiGaHitsCnvSvc; }
  ///
  virtual ~IGiGaHitsCnvSvc(){} ; 
  ///
};
///


#endif   //  __GAUDI_INTERFACES__I_GIGA_Hits_CNV_SVC_H__
