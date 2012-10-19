#ifndef MCOTTIMEMONITOR_H 
#define MCOTTIMEMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "AIDA/IHistogram1D.h"

/** @class MCOTTimeMonitor MCOTTimeMonitor.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2009-04-22
 */

class MCOTTimeMonitor : public GaudiHistoAlg {

public: 
  /// Standard constructor
  MCOTTimeMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCOTTimeMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  void scale( const HistoID& histoID, const double& weight ) const;
  
private:

  unsigned m_nEvents; ///< Number of events

};
#endif // MCOTTIMEMONITOR_H

void MCOTTimeMonitor::scale( const HistoID& histoID, const double& weight ) const {
  AIDA::IHistogram1D* h =  histo1D( histoID );
  h->scale( weight );
}

