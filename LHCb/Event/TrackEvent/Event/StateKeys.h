#ifndef StateKeys_H
#define StateKeys_H 1

//! @author J.A. Hernando, E. Rodrigues
//! @date 11/05/2005
  

namespace StateKeys 
{
  
  enum Location {LocationUnknown,
                 ClosestToBeam,
                 FirstMeasurement,
                 EndVelo,
                 AtTT,
                 AtT,
                 BegRich1,
                 EndRich1,
                 BegRich2,
                 EndRich2,
                 Calo,
                 Muon
  };

};
#endif
