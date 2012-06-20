#ifndef KERNEL_IEXTRAINFOTOOL_H 
#define KERNEL_IEXTRAINFOTOOL_H 1

// Include files
#include "Event/Particle.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/**
 *  @class IExtraInfoTool IExtraInfoTool.h Kernel/IExtraInfoTool.h
 *  
 *  Abstract Interface for the tool calculating extra particle info 
 *  to be stored into DST of produce tuple
 *  
 *  @author Anton Poluektov a.o.poluektov@warwick.ac.uk
 *  @date   2012-02-18
 */

class GAUDI_API IExtraInfoTool : virtual public IAlgTool {

public:

  DeclareInterfaceID( IExtraInfoTool, 1, 0 );

  /// Method to calculate extra information 
  virtual StatusCode calculateExtraInfo(const LHCb::Particle* top, 
                                        const LHCb::Particle* part) = 0;

  /// Get the first ID of the extra info structure calculated by the tool                                        
  virtual int getFirstIndex(void) = 0;
  
  /// Get the number of IDs of the extra info structure
  virtual int getNumberOfParameters(void) = 0;
  
  /// Get the value of calculated parameter and its name (e.g. for the tuple tool) for the given ID
  virtual int getInfo(int index, double& value, std::string& name) = 0;

protected:

  virtual ~IExtraInfoTool();

};

#endif // KERNEL_IEXTRAINFOTOOL_H
