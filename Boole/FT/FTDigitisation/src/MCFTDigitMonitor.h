#ifndef MCFTDIGITMONITOR_H 
#define MCFTDIGITMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class MCFTDigitMonitor MCFTDigitMonitor.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2012-07-05
 */
class MCFTDigitMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  MCFTDigitMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDigitMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  unsigned int QuarterModule(unsigned int module);  

protected:

private:
  std::string m_digitLocation; ///< FT energy deposit Location
};
#endif // MCFTDIGITMONITOR_H
