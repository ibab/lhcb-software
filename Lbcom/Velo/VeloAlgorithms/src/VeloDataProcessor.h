// $Id: VeloDataProcessor.h,v 1.3 2003-04-14 07:34:26 cattanem Exp $
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
 * Fill VeloFullDigit, based on MCVeloFE
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
  std::string m_inputContainer;       ///< Name of input container
  std::string m_outputContainer;      ///< Name of output container
  
  double noiseSigma();
  double m_noiseConstant;

};



#endif // VELODATAPROC_H



























