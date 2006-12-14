// $Id: HltOffMonitorExample.h,v 1.1 2006-12-14 11:24:17 hernando Exp $
#ifndef HLTOFFMONITOREXAMPLE_H 
#define HLTOFFMONITOREXAMPLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class HltOffMonitorExample HltOffMonitorExample.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-12-13
 */
class HltOffMonitorExample : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltOffMonitorExample( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltOffMonitorExample( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_summaryName;
  std::string m_selectionName;
  int m_idSelection;

private:

};
#endif // HLTHADOFFMONITOREXAMPLE_H
