#ifndef      __GAUDI_INTERFACES__I_GIGA_TRAJ_CNV_SVC_H__
#define      __GAUDI_INTERFACES__I_GIGA_TRAJ_CNV_SVC_H__ 1 


///
#include "GiGa/IGiGaCnvSvc.h" 
///

#include "GiGa/IIDIGiGaTrajCnvSvc.h"


/** @class IGiGaTrajCnvSvc     IGiGaTrajCnvSvc.h GiGa/IGiGaTrajCnvSvc.h

    definition of abstract interface to Geant 4 conversion service 
    which is responsible for conversion from Geant4 trajectories
    to Gaudi Event Model 
    
    @author Vanya Belyaev
*/


class IGiGaTrajCnvSvc: virtual public IGiGaCnvSvc
{
  ///
 public:
  ///
  static const InterfaceID& interfaceID() { return IID_IGiGaTrajCnvSvc; }
  ///
  virtual ~IGiGaTrajCnvSvc(){} ; 
  ///
};


///
///
/// 

#endif   //  __GAUDI_INTERFACES__I_GIGA_TRAJ_CNV_SVC_H__
