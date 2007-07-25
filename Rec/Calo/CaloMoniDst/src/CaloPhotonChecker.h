#ifndef CALOPHOTONCHECKER_H
#define CALOPHOTONCHECKER_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelationWeighted.h"
//#include "Relations/IAssociatorWeighted.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/MCParticle.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloMoniAlg.h"

// forward declarations
class ICaloHypoLikelyhood;
class DeCalorimeter;
class IIncidentSvc    ;


/** @class CaloPhotonChecker CaloPhotonChecker.h
 *
 *  Photon Selection Monitoring
 *  (LHCb 2004-03)
 *
 *  @author Frédéric Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */

class CaloPhotonChecker : public CaloMoniAlg
{
  // friend factory for instantiation
  friend class AlgFactory<CaloPhotonChecker>;
public:

  /** standard algorithm initialization
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code
   */
  virtual StatusCode initialize();

  /** standard algorithm execution
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code
   */
  virtual StatusCode execute   ();

  /** standard algorithm finalization
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code
   */
  virtual StatusCode finalize  ();

protected:

  /** Standard constructor
   *  @param name algorithm name
   *  @param pSvc service locator
   */
  CaloPhotonChecker( const std::string& name ,
                          ISvcLocator*       pSvc );

  // destructor
  virtual ~CaloPhotonChecker() {}

  std::vector<AIDA::IHistogram1D*> defHisto1d(const unsigned int,
                                      const double, const double,
                                      const unsigned int,
                                      const std::string, const std::string,
                                      const unsigned int);

  inline unsigned int bin(const double value,const std::vector<double> vect) const {
    unsigned int index=0;
    if (value>=vect[ vect.size()-1 ]) {return vect.size();}
    while ((index<vect.size()) && (value>=vect[ index ])) {index++;}
    return index;
  }

private:

  unsigned int m_nEvents;
  unsigned int m_nCandidats;
  unsigned int m_nPhotons;
  unsigned int m_nMCPhotons;
  unsigned int m_nWrongs;

  // Detector Information
  DeCalorimeter* m_ecal;
  DeCalorimeter* m_spd;
  DeCalorimeter* m_prs;
  Gaudi::Plane3D m_ecalPlane;
  Gaudi::Plane3D m_prsPlane;
  Gaudi::Plane3D m_spdPlane;
  double         m_zConv     ;



  std::string m_TrTableName;
  std::string m_MCTableName;

  // Tools
  std::string    m_hypotoolType;
  std::string    m_hypotoolName;
  ICaloHypoLikelihood* m_hypotool;

  // Particle Properties
  IParticlePropertySvc*            m_ppSvc                         ;
  std::string                      m_gammaName                     ;
  LHCb::ParticleID                       m_gammaID                       ;
  std::string                      m_pi0Name                       ;
  LHCb::ParticleID                       m_pi0ID                         ;


  // histogramming related variables
  bool                m_pdf;
  std::vector<double> m_ebin;
  std::vector<double> m_prsbin;
  std::vector<double> m_chi2bin;
  std::vector<double> m_shapebin;


  // Signal/background definitions
  double m_etmin;
  double m_dr;
  double m_dz;
  double m_de;
  double m_mergedDist;

  // histograms

  AIDA::IHistogram1D*   m_lhSig   ;
  AIDA::IHistogram1D*   m_lhSigSpd;
  AIDA::IHistogram1D*   m_lhBkg   ;
  AIDA::IHistogram1D*   m_lhBkgSpd;

  int m_nbinlh;
  AIDA::IHistogram2D*   m_effpur;
  AIDA::IHistogram2D*   m_effpur_spd;
  AIDA::IHistogram2D*   m_effpur_nospd;

  int m_nbinpt;
  double m_ptmin;
  double m_ptmax;
  AIDA::IHistogram1D*   m_efficiency;
  AIDA::IHistogram1D*   m_purity;
  double m_lhcut;

  std::vector<double> m_mc_g;
  std::vector<double> m_rec_bkg;
  std::vector<double> m_rec_sig;

  double              m_lh_mcg;
  std::vector<double> m_lh_recsig;
  std::vector<double> m_lh_recbkg;
  double              m_lh_mcg_conv;
  std::vector<double> m_lh_recsig_conv;
  std::vector<double> m_lh_recsig_spd;
  std::vector<double> m_lh_recbkg_spd;
  double              m_lh_mcg_noconv;
  std::vector<double> m_lh_recsig_noconv;
  std::vector<double> m_lh_recsig_nospd;
  std::vector<double> m_lh_recbkg_nospd;

  std::vector<AIDA::IHistogram1D*>    m_signalEPrs     ;
  std::vector<AIDA::IHistogram1D*>    m_backgrEPrs     ;
  std::vector<AIDA::IHistogram1D*>    m_signalChi2     ;
  std::vector<AIDA::IHistogram1D*>    m_backgrChi2     ;
  std::vector<AIDA::IHistogram1D*>    m_signalShape     ;
  std::vector<AIDA::IHistogram1D*>    m_backgrShape     ;

  std::vector<AIDA::IHistogram1D*>    m_signalEPrsSpd     ;
  std::vector<AIDA::IHistogram1D*>    m_backgrEPrsSpd     ;
  std::vector<AIDA::IHistogram1D*>    m_signalChi2Spd     ;
  std::vector<AIDA::IHistogram1D*>    m_backgrChi2Spd     ;
  std::vector<AIDA::IHistogram1D*>    m_signalShapeSpd     ;
  std::vector<AIDA::IHistogram1D*>    m_backgrShapeSpd     ;
};

#endif // CALOPHOTONCHECKER_H
