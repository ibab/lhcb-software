// $Id: HltOffMonitorExample.h,v 1.2 2007-02-05 09:09:08 hernando Exp $
#ifndef HLTOFFMONITOREXAMPLE_H 
#define HLTOFFMONITOREXAMPLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class HltOffMonitorExample HltOffMonitorExample.h
 *  
 *  functionality:
 *       example algorithm of how to deal with the HltSummary
 *       get the HltSelectionSummary info
 *       get the tracks and vertices stores into the HltSelectionSummary
 *       compare tracks saved in summary with offline tracks
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
