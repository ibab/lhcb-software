// $Id: VeloDataProcessor.h,v 1.4 2005-10-24 15:48:43 mtobin Exp $
#ifndef VELODATAPROC_H 
#define VELODATAPROC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// local

/** @class VeloDataProcessor VeloDataProcessor.h
 *
 * Fill VeloFullFPGADigit, based on MCVeloFE
 * Emulate Data Processor Board 
 * Currently just a fast test version.
 *  @author Chris Parkes
 *  @date   14/01/02
 */
class MCVeloFE;

class VeloDataProcessor : public Algorithm {
public:
  /// Standard constructor
  VeloDataProcessor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDataProcessor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /// perform digitisation, i.e. convert electrons to ADC counts
  virtual float digitise(float electrons);

  // data members
  std::string m_inputContainer;        ///< Name of input container
  std::string m_outputVeloFPGADigit;        ///< Name of output container for digi
  std::string m_outputVeloDigit; ///< And for history object

  double noiseSigma();
  double m_noiseConstant;

};
#endif // VELODATAPROC_H
