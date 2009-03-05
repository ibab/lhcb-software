// $Id: RawBankReadoutStatusMonitor.h,v 1.3 2009-03-05 15:36:36 odescham Exp $
#ifndef COMPONENT_RAWBANKREADOUTSTATUSMONITOR_H 
#define COMPONENT_RAWBANKREADOUTSTATUSMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
// root
#include <TH1D.h>
#include <TH2D.h>
#include <TAxis.h>

/** @class RawBankReadoutStatusMonitor RawBankReadoutStatusMonitor.h component/RawBankReadoutStatusMonitor.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-01-23
 */
class RawBankReadoutStatusMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  RawBankReadoutStatusMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankReadoutStatusMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  void setLabels1D( AIDA::IHistogram1D* histo1D );
  void setLabels2D( AIDA::IHistogram2D* histo2D );
  std::vector<std::string> m_bankTypes;
  int m_degree;
  int m_nSources;
  std::string m_lab;
  std::vector<std::string> m_labels;
  bool m_first;
};
#endif // COMPONENT_RAWBANKREADOUTSTATUSMONITOR_H
