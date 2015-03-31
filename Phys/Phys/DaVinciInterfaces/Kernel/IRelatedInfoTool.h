#ifndef KERNEL_IRELATEDINFOTOOL_H 
#define KERNEL_IRELATEDINFOTOOL_H 1

// Include files
#include "Event/Particle.h"
#include "Event/RelatedInfoMap.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/**
 *  @class IRelatedInfoTool IRelatedInfoTool.h Kernel/IRelatedInfoTool.h
 *  
 *  Abstract Interface for the tool calculating extra particle info 
 *  to be stored into DST via a Particle to VectorMap relation
 *  
 *  @author Anton Poluektov a.o.poluektov@warwick.ac.uk
 *  @date   2012-02-18
 */

class GAUDI_API IRelatedInfoTool : virtual public IAlgTool 
{

public:

  DeclareInterfaceID( IRelatedInfoTool, 1, 0 );

  /// Method to calculate extra information 
  virtual StatusCode calculateRelatedInfo(const LHCb::Particle* top, 
                                          const LHCb::Particle* part) = 0;

  /// Get the value of calculated parameters
  virtual LHCb::RelatedInfoMap* getInfo(void) = 0;

};

#endif // KERNEL_IRELATEDINFOTOOL_H
