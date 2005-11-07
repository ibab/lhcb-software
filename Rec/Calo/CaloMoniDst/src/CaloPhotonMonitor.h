#ifndef CALOPHOTONMONITOR_H
#define CALOPHOTONMONITOR_H 1
// Include files
// from STL
#include <string>
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
// from Calo
#include "CaloKernel/CaloAlgorithm.h"
// Event
#include "Event/Track.h"
#include "Event/MCParticle.h"
// Associator
#include "Relations/IAssociatorWeighted.h"

// Monitoring Algorithm
#include "CaloMoniAlg.h"

// forward declarations
class ICaloHypoLikelyhood;
class DeCalorimeter;
class IDataProviderSvc;
class IParticlePropertySvc ;
class IIncidentSvc    ;
class IHistogram1D    ;
class IHistogram2D    ;


/** @class CaloPhotonMonitor CaloPhotonMonitor.h
 *
 *  Photon Selection Monitoring
 *  (LHCb 2004-03)
 *
 *  @author Frédéric Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */

class CaloPhotonMonitor : public CaloMoniAlg
{
  // friend factory for instantiation
  friend class AlgFactory<CaloPhotonMonitor>;
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
  CaloPhotonMonitor( const std::string& name ,
                          ISvcLocator*       pSvc );

  // destructor
  virtual ~CaloPhotonMonitor() ;

  std::vector<IHistogram1D*> defHisto1d(const unsigned int,
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
  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detSpd;
  DeCalorimeter *m_detPrs;

  std::string    m_nameOfECAL;
  std::string    m_nameOfSPD ;
  std::string    m_nameOfPRS ;

  double         m_zEcal     ;
  double         m_zPrs      ;
  double         m_zSpd      ;
  double         m_shiftSpd  ;

  double         m_zConv     ;

  // IP

  HepPoint3D     m_vertex;

  // Services
  IIncidentSvc*                    m_incSvc    ;
  IDataProviderSvc*                m_evtSvc    ;
  IHistogramSvc*                   m_histoSvc  ;

  // Associator
  typedef IAssociatorWeighted<CaloCluster,Track,float> IMatch ;
  typedef IMatch::DirectType                                   Table  ;
  typedef Table::Range                                         Range  ;

  std::string                      m_matchType         ;
  std::string                      m_matchName         ;
  IMatch*                          m_match             ;

  typedef IAssociatorWeighted<CaloCluster,MCParticle,float> Asct;
  std::string m_associatorType;
  std::string m_associatorName;
  Asct*       m_associator;

  typedef const Asct::DirectType    GammaMCTable;
  typedef const GammaMCTable::Range GammaMCRange;

  // Tools
  std::string    m_hypotoolType;
  std::string    m_hypotoolName;
  ICaloHypoLikelihood* m_hypotool;

  // Particle Properties
  IParticlePropertySvc*            m_ppSvc                         ;
  std::string                      m_gammaName                     ;
  ParticleID                       m_gammaID                       ;
  std::string                      m_pi0Name                       ;
  ParticleID                       m_pi0ID                         ;

  // track Extrapolation
  bool m_extrapolation;

  // histogramming related variables
  bool                m_pdf;
  std::vector<double> m_ebin;
  std::vector<double> m_prsbin;
  std::vector<double> m_chi2bin;
  std::vector<double> m_shapebin;

  // geometrical Acceptance
  double m_zAcc;
  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;

  // Signal/background definitions
  double m_etmin;
  double m_dr;
  double m_dz;
  double m_de;
  double m_mergedDist;

  // histograms

  IHistogram1D*   m_lhSig   ;
  IHistogram1D*   m_lhSigSpd;
  IHistogram1D*   m_lhBkg   ;
  IHistogram1D*   m_lhBkgSpd;

  int m_nbinlh;
  IHistogram2D*   m_effpur;
  IHistogram2D*   m_effpur_spd;
  IHistogram2D*   m_effpur_nospd;

  int m_nbinpt;
  double m_ptmin;
  double m_ptmax;
  IHistogram1D*   m_efficiency;
  IHistogram1D*   m_purity;
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

  std::vector<IHistogram1D*>    m_signalEPrs     ;
  std::vector<IHistogram1D*>    m_backgrEPrs     ;
  std::vector<IHistogram1D*>    m_signalChi2     ;
  std::vector<IHistogram1D*>    m_backgrChi2     ;
  std::vector<IHistogram1D*>    m_signalShape     ;
  std::vector<IHistogram1D*>    m_backgrShape     ;

  std::vector<IHistogram1D*>    m_signalEPrsSpd     ;
  std::vector<IHistogram1D*>    m_backgrEPrsSpd     ;
  std::vector<IHistogram1D*>    m_signalChi2Spd     ;
  std::vector<IHistogram1D*>    m_backgrChi2Spd     ;
  std::vector<IHistogram1D*>    m_signalShapeSpd     ;
  std::vector<IHistogram1D*>    m_backgrShapeSpd     ;
};

// ============================================================================
// The END
// ============================================================================
#endif // CALOPHOTONMONITOR_H
// ============================================================================
