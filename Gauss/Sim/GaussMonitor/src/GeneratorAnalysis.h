// $Id: GeneratorAnalysis.h,v 1.1 2007-03-07 18:51:42 gcorti Exp $
#ifndef GENERATORANALYSIS_H 
#define GENERATORANALYSIS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"

/** @class GeneratorAnalysis GeneratorAnalysis.h 
 *  
 *  Analysis algorithms for the generator sequences with many reference
 *  histograms for generators comparisons
 *  
 *  @author P. Szczypka  
 *  @date   2007-02-16
 */
class GeneratorAnalysis : public GaudiHistoAlg {
public:
  /// Standard constructor
  GeneratorAnalysis( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GeneratorAnalysis( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void bookHistos(const bool neutral); ///< Book histograms
  void normHistos();                   ///< Normalize histograms
  /// Get B Hadron Info from PID
  void bHadronInfo(const LHCb::ParticleID m_mPID,
                   const LHCb::ParticleID m_dPID);
  
private:
  std::string    m_dataPath;            ///< location of input data
  double         m_minEta;              ///< Min pseudo rapidity acceptance
  double         m_maxEta;              ///< Max pseudo rapidity acceptance

  int            m_counter       , m_counterStable;
  int            m_counterCharged, m_counterChInEta;
  int            m_nEvents;
  
  std::string    m_generatorName;

  /// Histograms of generic quantities
  AIDA::IHistogram1D* m_hFNPart;
  AIDA::IHistogram1D* m_hFNStable;
  AIDA::IHistogram1D* m_hFNSCharg;
  AIDA::IHistogram1D* m_hFNSChEta;
  AIDA::IHistogram1D* m_hFProcess;
  AIDA::IHistogram1D* m_hFNPileUp;
  AIDA::IHistogram1D* m_hFPrimX;
  AIDA::IHistogram1D* m_hFPrimY;
  AIDA::IHistogram1D* m_hFPrimZ;
  AIDA::IHistogram1D* m_hFPrimZZ;
  AIDA::IHistogram1D* m_hFPartP;
  AIDA::IHistogram1D* m_hFPartPDG;
  AIDA::IHistogram1D* m_hFProtoP;
  AIDA::IHistogram1D* m_hFProtoPDG;
  AIDA::IHistogram1D* m_hFProtoLTime;
  AIDA::IHistogram1D* m_hFStableEta;
  AIDA::IHistogram1D* m_hFStablePt;

  /// B Hadron Histograms
  AIDA::IHistogram1D* m_bMesonFraction;
  AIDA::IHistogram1D* m_bMesonStates;
  AIDA::IHistogram1D* m_bMesonStatesCode;

  // 'Normal' Histograms
  AIDA::IHistogram1D* m_hNPart;
  AIDA::IHistogram1D* m_hNStable;
  AIDA::IHistogram1D* m_hNSCharg;
  AIDA::IHistogram1D* m_hNSChEta;
  AIDA::IHistogram1D* m_hProcess;
  AIDA::IHistogram1D* m_hNPileUp;
  AIDA::IHistogram1D* m_hPrimX;
  AIDA::IHistogram1D* m_hPrimY;
  AIDA::IHistogram1D* m_hPrimZ;
  AIDA::IHistogram1D* m_hPrimZZ;
  AIDA::IHistogram1D* m_hPartP;
  AIDA::IHistogram1D* m_hPartPDG;
  AIDA::IHistogram1D* m_hProtoP;
  AIDA::IHistogram1D* m_hProtoPDG;
  AIDA::IHistogram1D* m_hProtoLTime;
  AIDA::IHistogram1D* m_hStableEta;
  AIDA::IHistogram1D* m_hStablePt;

  // Histograms for charged stable particles in acceptance
  AIDA::IHistogram1D* m_ptChargedStable;
  AIDA::IHistogram1D* m_pChargedStable;
  AIDA::IHistogram1D* m_ptMaxChargedStable;
  AIDA::IHistogram1D* m_multChargedStable;
  AIDA::IHistogram1D* m_energyChargedStable;
  AIDA::IHistogram1D* m_energyMaxChargedStable;

  //Histograms for all particles
  AIDA::IHistogram1D* m_prapChargedStableAll;
  AIDA::IHistogram1D* m_ptChargedStableAll;
  AIDA::IHistogram1D* m_ptMaxChargedStableAll;
  AIDA::IHistogram1D* m_multChargedStableAll;
  AIDA::IHistogram1D* m_energyChargedStableAll;
  AIDA::IHistogram1D* m_energyMaxChargedStableAll;
  AIDA::IHistogram1D* m_pChargedStableAll;

  // Basic Histograms for all Charged stable particles
  AIDA::IHistogram1D* m_hAllChargedStableMult     ;
  AIDA::IHistogram1D* m_hAllChargedStablePRap     ;
  AIDA::IHistogram1D* m_hAllChargedStableEnergy   ;
  AIDA::IHistogram1D* m_hAllChargedStableEnergyMax;
  AIDA::IHistogram1D* m_hAllChargedStableP        ;
  AIDA::IHistogram1D* m_hAllChargedStablePMax     ;
  AIDA::IHistogram1D* m_hAllChargedStablePt       ;
  AIDA::IHistogram1D* m_hAllChargedStablePtMax    ;
  AIDA::IHistogram1D* m_hAllChargedStablePID      ;
  int m_nAllChargedStable; //< Counter for all particles

  // Histograms for neutral stable particles in acceptance
  AIDA::IHistogram1D* m_hNeutralStableMult     ;
  AIDA::IHistogram1D* m_hNeutralStablePRap     ;
  AIDA::IHistogram1D* m_hNeutralStableEnergy   ;
  AIDA::IHistogram1D* m_hNeutralStableEnergyMax;
  AIDA::IHistogram1D* m_hNeutralStableP        ;
  AIDA::IHistogram1D* m_hNeutralStablePMax     ;
  AIDA::IHistogram1D* m_hNeutralStablePt       ;
  AIDA::IHistogram1D* m_hNeutralStablePtMax    ;
  AIDA::IHistogram1D* m_hNeutralStablePID      ;
  int m_nNeutralStable; //< Counter for neutral stable particles in acceptance
  // Histograms for charged stable particles in acceptance
  AIDA::IHistogram1D* m_hChargedStableMult     ;
  AIDA::IHistogram1D* m_hChargedStablePRap     ;
  AIDA::IHistogram1D* m_hChargedStableEnergy   ;
  AIDA::IHistogram1D* m_hChargedStableEnergyMax;
  AIDA::IHistogram1D* m_hChargedStableP        ;
  AIDA::IHistogram1D* m_hChargedStablePMax     ;
  AIDA::IHistogram1D* m_hChargedStablePt       ;
  AIDA::IHistogram1D* m_hChargedStablePtMax    ;
  AIDA::IHistogram1D* m_hChargedStablePID      ;
  int m_nChargedStable; //< Counter for charged stable particles in acceptance

  // Histograms for all charged stable particles
  AIDA::IHistogram1D* m_hAllParticlesMult     ;
  AIDA::IHistogram1D* m_hTest     ;
  AIDA::IHistogram1D* m_hAllParticlesPRap     ;
  AIDA::IHistogram1D* m_hAllParticlesEnergy   ;
  AIDA::IHistogram1D* m_hAllParticlesP        ;
  AIDA::IHistogram1D* m_hAllParticlesPt       ;
  AIDA::IHistogram1D* m_hAllParticlesPID      ;
  int m_nAllParticles; //< Counter for all charged stable particles

  // Basic histograms for all charged stable particles
  AIDA::IHistogram1D* m_ptfullRangeStable       ;
  AIDA::IHistogram1D* m_pfullRangeStable       ;
  AIDA::IHistogram1D* m_ptMaxfullRangeStable    ;
  AIDA::IHistogram1D* m_multfullRangeStable     ;
  AIDA::IHistogram1D* m_energyfullRangeStable   ;
  AIDA::IHistogram1D* m_energyMaxfullRangeStable;

  // Basic histograms for all particles
  AIDA::IHistogram1D* m_ptfullRangeAll       ;
  AIDA::IHistogram1D* m_pfullRangeAll       ;
  AIDA::IHistogram1D* m_ptMaxfullRangeAll    ;
  AIDA::IHistogram1D* m_multfullRangeAll     ;
  AIDA::IHistogram1D* m_energyfullRangeAll   ;
  AIDA::IHistogram1D* m_energyMaxfullRangeAll;

  // Pointers to histograms
  std::vector<AIDA::IHistogram1D*> m_pHisto;

  int m_nBHistos;
  // Location where to find HepMC event
  std::string m_inputData ;

  bool m_neutralHistos ; //< Flag for producing Neutral Histograms
  enum bHadronType{BdMeson=1,BuMeson,BsMeson,BcMeson,BbMeson,TbMeson,BBaryon};
  enum spinState{S0L0J0,S1L1J0,S1L0J1,S0L1J1,S1L1J1,S1L1J2};

  int m_nS0L0J0, m_nS1L1J0, m_nS1L0J1, m_nS0L1J1, m_nS1L1J1, m_nS1L1J2;
  int m_stateB,m_stateBStar,m_stateBStarStar;
  int m_nMesonType;

  int m_nBdMeson, m_nBuMeson, m_nBsMeson, m_nBcMeson, m_nBbMeson, m_nBBaryon;
  int m_nTbMeson, m_nBParticles;

  int m_nParticles, m_nPartCount;

  double fraction(const int a, const int b){
    return ( double(a) / double(b) );
  };
  

  double err_fraction ( const int a, const int b){
    return sqrt(double(a)*(1.-(double(a)/double(b))))/(double(b));
  };

};
#endif // GENERATORANALYSIS_H
