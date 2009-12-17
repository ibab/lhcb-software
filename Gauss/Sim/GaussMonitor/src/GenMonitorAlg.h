// $Id: GenMonitorAlg.h,v 1.7 2009-12-17 19:29:19 gcorti Exp $
#ifndef GAUSSMONITOR_GENMONITORALG_H 
#define GAUSSMONITOR_GENMONITORALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

/** @class GenMonitorAlg GenMonitorAlg.h Algorithms/GenMonitorAlg.h
 *  
 *  Monitoring algorithms for the generator sequences
 *  
 *  @author Patrick Robbe (modified G.Corti)
 *  @date   2005-04-11
 */
class GenMonitorAlg : public GaudiHistoAlg {
public:
  /// Standard constructor
  GenMonitorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenMonitorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void bookHistos();                  ///< Book histograms
  
private:
  std::string    m_dataPath;            ///< location of input data
  double         m_minEta;              ///< Min pseudo rapidity acceptance
  double         m_maxEta;              ///< Max pseudo rapidity acceptance

  int            m_counter       , m_counterstable;
  int            m_counterCharged, m_counterChInEta;
  int            m_nEvents;
  
  std::string    m_generatorName;

  AIDA::IHistogram1D* m_hNPart;
  AIDA::IHistogram1D* m_hNStable;
  AIDA::IHistogram1D* m_hNSCharg;
  AIDA::IHistogram1D* m_hNSChEta;
  AIDA::IHistogram1D* m_hProcess;
  AIDA::IHistogram1D* m_hNPileUp;
  AIDA::IHistogram1D* m_hPrimX;
  AIDA::IHistogram1D* m_hPrimY;
  AIDA::IHistogram1D* m_hPrimZ;
  AIDA::IHistogram1D* m_hPrimZV;
  AIDA::IHistogram1D* m_hPrimZE;
  AIDA::IHistogram1D* m_hPrimT;
  AIDA::IHistogram1D* m_hPartP;
  AIDA::IHistogram1D* m_hPartPDG;
  AIDA::IHistogram1D* m_hProtoP;
  AIDA::IHistogram1D* m_hProtoPDG;
  AIDA::IHistogram1D* m_hProtoLTime;
  AIDA::IHistogram1D* m_hStableEta;
  AIDA::IHistogram1D* m_hStablePt;
  AIDA::IHistogram2D* m_hPrimXvsZ;
  AIDA::IHistogram2D* m_hPrimYvsZ;

};
#endif // GAUSSMONITOR_GENMONITORALG_H
