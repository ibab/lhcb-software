// $Id: $
#ifndef MUHISTOGRAMS_H 
#define MUHISTOGRAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "LoKi/HLT.h"
#include "LoKi/HLTTypes.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"
#include "HltMonitorBase.h"
/** @class MuMonitor MuMonitor.h
 *  
 *  Fill histogram of mu per bunch crossing on NoBias events
 *
 *  Some code from the HLT_PASS_RE in C++ thread in lhcb-hlt2-development
 *
 *  @author Patrick Koppenburg
 *  @date   2011-04-01
 */
class MuMonitor : public HltMonitorBase {
public: 
  /// Standard constructor
  MuMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  void handler_1 ( Property& p );
  std::string m_pattern ; ///< HLT filter expression
  std::string m_location; ///< Location of HltDecReports
  LoKi::Types::HLT_Cut m_filter ; ///< Loki filter
  unsigned int m_nBX  ;
  AIDA::IProfile1D* m_hMuVelo ;
  AIDA::IHistogram1D* m_hVelo ;
  AIDA::IHistogram1D* m_bBx ;

};
#endif // MUHISTOGRAMS_H
