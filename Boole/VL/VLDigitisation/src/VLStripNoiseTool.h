#ifndef VL_STRIP_NOISE_TOOL_H
#define VL_STRIP_NOISE_TOOL 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "IStripNoiseTool.h"

/** @class VLStripNoiseTool VLStripNoiseTool.h
 *
 *  Tool to estimate noise (in electrons) in a silicon strip
 *
 */

class DeVL;

class VLStripNoiseTool : public GaudiTool, 
                               virtual public IStripNoiseTool {
  
public: 
  /// Constructor
  VLStripNoiseTool(const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);
  /// Destructor
  virtual ~VLStripNoiseTool() {}

  /// Initialization
  virtual StatusCode initialize();

  virtual double noise(const unsigned int sensor, const unsigned int strip);
  virtual double averageNoise(const unsigned int sensor);
  
protected:

  /// Strip capacitance per unit length
  double m_capacitancePerStripLength;

  /// Constant noise term (in electrons)
  double m_noiseConstant;
  /// Capacitance dependent noise term (in electrons / pF)
  double m_noiseSlope;

  /// Pointer to detector element
  DeVL* m_det;

};
#endif 
