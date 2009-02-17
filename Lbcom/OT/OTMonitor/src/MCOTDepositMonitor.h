// $Id: MCOTDepositMonitor.h,v 1.9 2009-02-17 09:04:12 janos Exp $
#ifndef OTMONITOR_MCOTDEPOSITMONITOR_H
#define OTMONITOR_MCOTDEPOSITMONITOR_H 1

/// STL
#include <map>

/// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class MCOTDepositMonitor MCOTDepositMonitor.h
 *
 *  Monitoring algorithm for the MCOTDeposits of the Outer Tracker.
 *
 *  @author M.Needham
 *  @date   21/4/2001
 *  @author adapted by J.Nardulli
 *  @date   20/07/2004
 */

namespace AIDA 
{
  class IHistogram1D;
  class IHistogram2D;
}

namespace LHCb
{
  class MCOTDeposit;
}

class DeOTDetector;

class MCOTDepositMonitor : public GaudiHistoAlg {

public:
 
  /// constructor
  MCOTDepositMonitor( const std::string& name, 
		      ISvcLocator *svcloc );

  /// destructor
  virtual ~MCOTDepositMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  /// book the histograms
  void initHistograms();

  unsigned depSpectrum( unsigned station )    { return 100u*station + 1; };
  unsigned noiseSpectrum( unsigned station )  { return 100u*station + 2; };
  unsigned xTalkSpectrum( unsigned station )  { return 100u*station + 3; };
  unsigned dPulseSpectrum( unsigned station ) { return 100u*station + 4; };
    
  /// fill the histograms
  void fillHistograms( const LHCb::MCOTDeposit* aDeposit );

private:
  DeOTDetector*                             m_det           ;
  /// drift time histo per station
  std::map< unsigned, AIDA::IHistogram1D* > m_spectrumHistos;
  /// y vs x of hits per station 
  std::map< unsigned, AIDA::IHistogram2D* > m_hitDistHistos ;
  
};

#endif // OTMONITOR_MCOTDEPOSITMONITOR_H
