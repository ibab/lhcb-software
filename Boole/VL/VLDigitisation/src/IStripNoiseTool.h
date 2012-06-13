#ifndef ISTRIP_NOISE_TOOL_H
#define ISTRIP_NOISE_TOOL_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class IStripNoiseTool IStripNoiseTool.h
 *  Interface class for estimating noise in silicon strips
 *
 */

// Declaration of the interface id 
static const InterfaceID IID_IStripNoiseTool("IStripNoiseTool", 1, 0);

class IStripNoiseTool : virtual public IAlgTool {

public:
  /// Static access to interface id
  static const InterfaceID& interfaceID() {return IID_IStripNoiseTool;}

  virtual double noise(const unsigned int sensor, const unsigned int strip) = 0;
  virtual double averageNoise(const unsigned int sensor) = 0;

};
#endif 
