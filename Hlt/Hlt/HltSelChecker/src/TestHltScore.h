// $Id: TestHltScore.h,v 1.3 2005-12-09 07:00:20 pkoppenb Exp $
#ifndef TESTHLTSCORE_H 
#define TESTHLTSCORE_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

/** @class TestHltScore TestHltScore.h
 *  
 *  Reads and tests HltScore class
 *  Pusblished histograms and counters to Gaucho
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-05-10
 *  @author Jose Helder Lopez
 *  @date   2005-06-08
 */
class HltAlgorithmScore ;
class HltScore ;
class IAlgorithm2ID ;

class TestHltScore : public GaudiHistoAlg {
public: 
  /// Standard constructor
  TestHltScore( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestHltScore( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  StatusCode printAlgorithm(const HltAlgorithmScore* );
  const HltScore* getScore(void);
  StatusCode printScore(const HltScore*);
  StatusCode fillStats(const HltScore*); 
  StatusCode printStatistics();
  StatusCode fillHistos(const HltScore* score);

private:
  IAlgorithm2ID* m_algorithm2ID ; ///< AlgorithmID

  // statistics
  int m_selEvents ; ///< number of selected events
  int m_rejEvents ; ///< number of rejected events
  int m_notEvents ; ///< number of not processed events
  int m_selPV ; ///< number of PVs in selected events
  int m_rejPV ; ///< number of PVs in rejected events
  int m_selTracks ; ///< number of tracks in selected events
  int m_rejTracks ; ///< number of tracks in rejected events
  int m_selPhotons ; ///< number of photons in selected events
  int m_rejPhotons ; ///< number of photons in rejected events
  int m_selPi0s ; ///< number of Pi0s in selected events
  int m_rejPi0s ; ///< number of Pi0s in rejected events
  int m_selB    ; ///< number of B in selected events
  int m_selDstar ; ///< number of D* in rejected events

  // Histograms
  IHistogramSvc* m_histosvc;
  IHistogram1D* m_hSelPV;  ///< histo of number of PVs in selected events
  IHistogram1D* m_hRejPV;  ///< histo of number of PVs in rejected events
  IHistogram1D* m_hSelTracks;  ///< histo of number of tracks in selected events
  IHistogram1D* m_hRejTracks;  ///< histo of number of tracks in rejected events
  IHistogram1D* m_hSelPhotons;  ///< histo of number of photons in selected events
  IHistogram1D* m_hRejPhotons;  ///< histo of number of photons in rejected events
  IHistogram1D* m_hSelPi0s;  ///< histo of number of Pi0s in selected events
  IHistogram1D* m_hRejPi0s;  ///< histo of number of Pi0s in rejected events
  IHistogram1D* m_hSelB;  ///< histo of number of B in selected events
  IHistogram1D* m_hSelDstar;  ///< histo of number of D* in selected events
  
  IHistogram1D* m_hExcSel; ///< Exclusive selections: One bin for each selection
  IHistogram1D* m_hMassBd; ///< Bd mass
  IHistogram1D* m_hMassDstar; ///< Dstar mass
  IHistogram1D* m_hMassDimu; ///< Dimu mass
  std::vector<IHistogram1D*> m_hMassCand ; ///< Candidate masses
  
  int m_dstarSel ; ///< ID of D* selection
  int m_psiSel ; ///< ID of Psi selection
  std::string m_dstarSelName ;///< Name of D* selection
  std::string m_psiSelName ;///< Name of Psi selection
   
  // Status of the algorithm, for online monitoring
  std::string m_status;

};
#endif // TESTHLTSCORE_H
