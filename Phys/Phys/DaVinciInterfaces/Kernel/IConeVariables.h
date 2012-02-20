#ifndef KERNEL_ICONEVARIABLES_H 
#define KERNEL_ICONEVARIABLES_H 1

// Include files
#include "Event/Particle.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/**
 *  @class IConeVariables IConeVariables.h Kernel/IConeVariables.h
 *  
 *  Abstract Interface of the cone variables calculation tool
 *  
 *  @author Anton Poluektov a.o.poluektov@warwick.ac.uk
 *  @date   2012-02-18
 */


namespace LHCb { 
  class ConeInfo {
    public: 

      int mult;
      double p;
      double px;
      double py;
      double pz;
      double pt;
  
      double pasy;
      double ptasy;
      double pxasy;
      double pyasy;
      double pzasy;
  
      double deltaEta;
      double deltaPhi;
  };
}

class GAUDI_API IConeVariables : virtual public IAlgTool {

public:

  DeclareInterfaceID( IConeVariables, 1, 0 );

  virtual StatusCode getConeInfo(const LHCb::Particle* top, 
                                 const LHCb::Particle* part, 
                                 LHCb::ConeInfo& info) = 0;

protected:

  virtual ~IConeVariables();

};

#endif // KERNEL_ICONEVARIABLES_H
