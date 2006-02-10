// $Id: VeloDataProcessor.h,v 1.6 2006-02-10 14:02:49 cattanem Exp $
#ifndef VELODATAPROC_H 
#define VELODATAPROC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local

/** @class VeloDataProcessor VeloDataProcessor.h
 *
 * Emulate Data Processor Board 
 *
 *  @author Chris Parkes, update Tomasz Szumlak
 *  @date   14/01/02
 */

class VeloDataProcessor : public GaudiAlgorithm {
public:
  /// Standard constructor
  VeloDataProcessor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDataProcessor( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  /// perform digitisation, i.e. convert electrons to ADC counts
  virtual float digitise(float electrons);
  // data members
  std::string m_inputContainer;      ///< Name of input container
  std::string m_outputVeloDigit; ///< Name of output container for digi
  double noiseSigma();
  double m_noiseConstant;
  double m_noiseCapacitance;
  double m_averageStripCapacitance;
  double m_ADCFullScale;
  double m_electronsFullScale;

};
#endif // VELODATAPROC_H
