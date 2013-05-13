#ifndef CALOPHOTONIDALG_H 
#define CALOPHOTONIDALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from LHCb
#include "CaloInterfaces/ICaloHypoEstimator.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloUtils/ClusterFunctors.h"
#include "boost/static_assert.hpp"
#include "DetDesc/Condition.h"
#include "Relations/Relation1D.h"
#include "Relations/IRelationWeighted.h"
#include "CaloUtils/Calo2Track.h"
// Histogramming 
#include "TH2D.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "AIDA/IHistogram2D.h"




/** @class CaloPhotonIdAlg CaloPhotonIdAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-02-27
 */
class CaloPhotonIdAlg: public GaudiAlgorithm {
public:

  enum TYPE {
    SIGNAL, BACKGROUND, SIGNAL_SPD, BACKGROUND_SPD
  };

  /// Standard constructor
  CaloPhotonIdAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~CaloPhotonIdAlg(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute(); ///< Algorithm execution

  /// callback function invoked by the UpdateManagerSvc
  StatusCode i_updateDLL();

protected:
  StatusCode initializeWithCondDB();
  StatusCode initializeWithoutCondDB();

private:
  double likelihood(const LHCb::CaloHypo* hypo) ;

  inline double dLL(const double, const double, const double, const double,
                    const CaloPhotonIdAlg::TYPE, const int) ;

  StatusCode evalParam(const LHCb::CaloHypo*, double &, double &, double &, int &, double &, unsigned int &) const;

  double evalLikelihood(double, double, double, int, double, double, unsigned int) ;

  TH2D *locateHistoOnDisk(std::string);

  TH2D *locateHistoOnDB(std::string);

private:

  bool m_tracking; // use tracking

  bool m_extrapol;
  bool m_seed;
  bool m_neig;

  std::string m_type;

  bool m_pdfEPrs;
  bool m_pdfChi2;
  bool m_pdfSeed;

  bool m_dlnL;

  bool m_isRunnable;

  // 

  std::vector<std::string> m_inputs;
  std::string m_output;

  // Condition dbase
  bool m_useCondDB; ///< if true - use CondDB, otherwise get the DLLs via THS from a root file
  std::string m_conditionName;
  Condition * m_cond;


  ICaloHypoEstimator* m_estimator ;
  
  // Photon Pdf

  std::string m_histo_path;

  std::string m_title_Sig_EPrs_10;
  std::string m_title_Sig_EPrs_11;
  std::string m_title_Sig_EPrs_12;
  std::string m_title_Sig_EPrs_15;
  std::string m_title_Sig_EPrs_16;
  std::string m_title_Sig_EPrs_17;

  std::string m_title_Sig_Chi2_20;
  std::string m_title_Sig_Chi2_21;
  std::string m_title_Sig_Chi2_22;
  std::string m_title_Sig_Chi2_25;
  std::string m_title_Sig_Chi2_26;
  std::string m_title_Sig_Chi2_27;

  std::string m_title_Sig_Seed_30;
  std::string m_title_Sig_Seed_31;
  std::string m_title_Sig_Seed_32;
  std::string m_title_Sig_Seed_35;
  std::string m_title_Sig_Seed_36;
  std::string m_title_Sig_Seed_37;

  std::string m_title_Bkg_EPrs_110;
  std::string m_title_Bkg_EPrs_111;
  std::string m_title_Bkg_EPrs_112;
  std::string m_title_Bkg_EPrs_115;
  std::string m_title_Bkg_EPrs_116;
  std::string m_title_Bkg_EPrs_117;

  std::string m_title_Bkg_Chi2_120;
  std::string m_title_Bkg_Chi2_121;
  std::string m_title_Bkg_Chi2_122;
  std::string m_title_Bkg_Chi2_125;
  std::string m_title_Bkg_Chi2_126;
  std::string m_title_Bkg_Chi2_127;

  std::string m_title_Bkg_Seed_130;
  std::string m_title_Bkg_Seed_131;
  std::string m_title_Bkg_Seed_132;
  std::string m_title_Bkg_Seed_135;
  std::string m_title_Bkg_Seed_136;
  std::string m_title_Bkg_Seed_137;

  const TH2D *m_Sig_EPrs_10;
  const TH2D *m_Sig_EPrs_11;
  const TH2D *m_Sig_EPrs_12;
  const TH2D *m_Sig_EPrs_15;
  const TH2D *m_Sig_EPrs_16;
  const TH2D *m_Sig_EPrs_17;

  const TH2D *m_Sig_Chi2_20;
  const TH2D *m_Sig_Chi2_21;
  const TH2D *m_Sig_Chi2_22;
  const TH2D *m_Sig_Chi2_25;
  const TH2D *m_Sig_Chi2_26;
  const TH2D *m_Sig_Chi2_27;

  const TH2D *m_Sig_Seed_30;
  const TH2D *m_Sig_Seed_31;
  const TH2D *m_Sig_Seed_32;
  const TH2D *m_Sig_Seed_35;
  const TH2D *m_Sig_Seed_36;
  const TH2D *m_Sig_Seed_37;

  const TH2D *m_Bkg_EPrs_110;
  const TH2D *m_Bkg_EPrs_111;
  const TH2D *m_Bkg_EPrs_112;
  const TH2D *m_Bkg_EPrs_115;
  const TH2D *m_Bkg_EPrs_116;
  const TH2D *m_Bkg_EPrs_117;

  const TH2D *m_Bkg_Chi2_120;
  const TH2D *m_Bkg_Chi2_121;
  const TH2D *m_Bkg_Chi2_122;
  const TH2D *m_Bkg_Chi2_125;
  const TH2D *m_Bkg_Chi2_126;
  const TH2D *m_Bkg_Chi2_127;

  const TH2D *m_Bkg_Seed_130;
  const TH2D *m_Bkg_Seed_131;
  const TH2D *m_Bkg_Seed_132;
  const TH2D *m_Bkg_Seed_135;
  const TH2D *m_Bkg_Seed_136;
  const TH2D *m_Bkg_Seed_137;
};

// ============================================================================
inline double CaloPhotonIdAlg::dLL(const double energy, const double eprs,
    const double chi2, const double seed, const CaloPhotonIdAlg::TYPE type,
    const int area) {
  const TH2D* histo_eprs = 0;
  const TH2D* histo_chi2 = 0;
  const TH2D* histo_seed = 0;

  const double epsilon = 1.e-20;

  switch (area) {
  case 0:
    switch (type) {
    case SIGNAL:
      histo_eprs = m_Sig_EPrs_10;
      histo_chi2 = m_Sig_Chi2_20;
      histo_seed = m_Sig_Seed_30;
      break;
    case SIGNAL_SPD:
      histo_eprs = m_Sig_EPrs_15;
      histo_chi2 = m_Sig_Chi2_25;
      histo_seed = m_Sig_Seed_35;
      break;
    case BACKGROUND:
      histo_eprs = m_Bkg_EPrs_110;
      histo_chi2 = m_Bkg_Chi2_120;
      histo_seed = m_Bkg_Seed_130;
      break;
    case BACKGROUND_SPD:
      histo_eprs = m_Bkg_EPrs_115;
      histo_chi2 = m_Bkg_Chi2_125;
      histo_seed = m_Bkg_Seed_135;
      break;
    default:
      Error("Invalid cluster type");
      return epsilon;
    }
    break;
  case 1:
    switch (type) {
    case SIGNAL:
      histo_eprs = m_Sig_EPrs_11;
      histo_chi2 = m_Sig_Chi2_21;
      histo_seed = m_Sig_Seed_31;
      break;
    case SIGNAL_SPD:
      histo_eprs = m_Sig_EPrs_16;
      histo_chi2 = m_Sig_Chi2_26;
      histo_seed = m_Sig_Seed_36;
      break;
    case BACKGROUND:
      histo_eprs = m_Bkg_EPrs_111;
      histo_chi2 = m_Bkg_Chi2_121;
      histo_seed = m_Bkg_Seed_131;
      break;
    case BACKGROUND_SPD:
      histo_eprs = m_Bkg_EPrs_116;
      histo_chi2 = m_Bkg_Chi2_126;
      histo_seed = m_Bkg_Seed_136;
      break;
    default:
      Error("Invalid cluster type");
      return epsilon;
    }
    break;
  case 2:
    switch (type) {
    case SIGNAL:
      histo_eprs = m_Sig_EPrs_12;
      histo_chi2 = m_Sig_Chi2_22;
      histo_seed = m_Sig_Seed_32;
      break;
    case SIGNAL_SPD:
      histo_eprs = m_Sig_EPrs_17;
      histo_chi2 = m_Sig_Chi2_27;
      histo_seed = m_Sig_Seed_37;
      break;
    case BACKGROUND:
      histo_eprs = m_Bkg_EPrs_112;
      histo_chi2 = m_Bkg_Chi2_122;
      histo_seed = m_Bkg_Seed_132;
      break;
    case BACKGROUND_SPD:
      histo_eprs = m_Bkg_EPrs_117;
      histo_chi2 = m_Bkg_Chi2_127;
      histo_seed = m_Bkg_Seed_137;
      break;
    default:
      Error("Invalid cluster type");
      return epsilon;
    }
    break;
  default:
    Error("Invalid area");
    return epsilon;
  }

  if (0 == histo_eprs) {
    warning() << "Histogram EPrs is not found." << endmsg;
    warning() << "Stop evaluating the Dll." << endmsg;
    m_isRunnable=false;
    return -9999.;
  }
  if (0 == histo_chi2) {
    warning() << "Histogram Chi2 is not found." << endmsg;
    warning() << "Stop evaluating the Dll." << endmsg;
    m_isRunnable=false;
    return -9999.;
  }
  if (0 == histo_seed) {
    warning() << "Histogram Seed is not found." << endmsg;
    warning() << "Stop evaluating the Dll." << endmsg;
    m_isRunnable=false;
    return -9999.;
  }

  if (energy < epsilon)
    return epsilon;
  double e = log(1.4 * energy) - 6.2;
  if (e > 5.5) e=5.5;

  const int ieprs = const_cast<TH2D *> (histo_eprs)->FindBin(eprs, e);
  const int ichi2 = const_cast<TH2D *> (histo_chi2)->FindBin(chi2, e);
  const int iseed = const_cast<TH2D *> (histo_seed)->FindBin(seed, e);
  
  double tmp = 
    histo_eprs->GetBinContent(ieprs)*
    histo_seed->GetBinContent(iseed);
 
  if (m_tracking) tmp*=histo_chi2->GetBinContent(ichi2);
  
  return tmp;
}

#endif // CALOPHOTONIDALG_H
