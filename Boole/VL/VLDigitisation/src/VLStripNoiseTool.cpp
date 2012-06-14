// Gaudi
#include "GaudiKernel/ToolFactory.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "VLStripNoiseTool.h"

/** @file VLStripNoiseTool.cpp
 *
 *  Implementation of class : VLStripNoiseTool
 *
 */

DECLARE_TOOL_FACTORY(VLStripNoiseTool)

//=============================================================================
/// Constructor
//=============================================================================
VLStripNoiseTool::VLStripNoiseTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) : 
    GaudiTool(type, name, parent),
    m_det(0) {

  declareInterface<IStripNoiseTool>(this);

  /// Strip capacitance per unit length (in pF / cm)
  declareProperty("StripCapacitance", m_capacitancePerStripLength = 2.);
  /// Constant noise term (in electrons)
  declareProperty("NoiseBase", m_noiseConstant = 500.);
  /// Capacitance dependent noise term (in electrons / pF)
  declareProperty("NoiseSlope", m_noiseSlope = 50.);

}

//=============================================================================
/// Initialization 
//=============================================================================
StatusCode VLStripNoiseTool::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Estimate noise in channel 
//=============================================================================
double VLStripNoiseTool::noise(const unsigned int sensor, 
                               const unsigned int strip) {

  const DeVLSensor* sens = m_det->sensor(sensor);
  if (!sens) return 0.;
  const double length = sens->stripLength(strip) / Gaudi::Units::cm;
  const double capacitance = length * m_capacitancePerStripLength;
  // TODO: add noise due to routing line
  return m_noiseConstant + m_noiseSlope * capacitance; 

}

//=============================================================================
/// Estimate average strip noise of a sensor 
//=============================================================================
double VLStripNoiseTool::averageNoise(const unsigned int sensor) {

  const DeVLSensor* sens = m_det->sensor(sensor);
  const unsigned int nStrips = sens->numberOfStrips();
  if (nStrips <= 0) return 0.;
  double sum = 0.;
  for (unsigned int strip = nStrips; strip--;) {
    sum += noise(sensor, strip);
  }
  sum /= nStrips;
  return sum;

}

