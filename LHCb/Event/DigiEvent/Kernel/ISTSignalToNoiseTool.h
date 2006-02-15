// $I $
#ifndef _ISTSIGNALTONOISETOOL_H
#define _ISTSIGNALTONOISETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"

namespace LHCb{
  class STCluster;
  class STChannelID;
};

class DeSTSector;

/** @class ISTSignalToNoiseTool ISTSignalToNoiseTool.h Kernel/ISTSignalToNoiseTool.h
 *
 *  Interface for cluster charge properties class
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISTSignalToNoiseTool("ISTSignalToNoiseTool", 0 , 0); 

class ISTSignalToNoiseTool : virtual public IAlgTool {

public: 

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTSignalToNoiseTool; }

  // cluster S/N    
  virtual double signalToNoise(const LHCb::STCluster* aCluster) = 0;
  virtual double signalToNoise(const SmartRefVector<LHCb::STDigit>& digitCont) = 0;


  // digit S/N
  virtual double signalToNoise(const LHCb::STDigit* aDigit) = 0;

  virtual double noiseInADC(const LHCb::STChannelID& aChan) = 0;
  virtual double noiseInElectrons(const LHCb::STChannelID& aChan) = 0;
  virtual double noiseInADC(const DeSTSector* aSector) = 0;
  virtual double noiseInElectrons(const DeSTSector* aSector) = 0;


  virtual double convertToADC(const double aCharge) = 0;

};

#endif // _ISTSIGNALTONOISETOOL_H



