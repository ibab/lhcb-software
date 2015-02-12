#ifndef IMCFTATTENUATIONTOOL_H 
#define IMCFTATTENUATIONTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMCFTAttenuationTool ( "IMCFTAttenuationTool", 1, 0 );

/** @class IMCFTAttenuationTool IMCFTAttenuationTool.h
 *  
 *  Interface for the tool that calculates the light attenuation for the MCFTDepositCreator
 *
 *  @author Michel De Cian
 *  @date   2015-01-15
 */

namespace LHCb{
  class MCHit;
}




class IMCFTAttenuationTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCFTAttenuationTool; }

  virtual void attenuation(const LHCb::MCHit* ftHit, double& att, double& attRef) = 0;
  virtual StatusCode initializeTool() = 0;
  
protected:

private:

};
#endif // IMCFTATTENUATIONTOOL_H
