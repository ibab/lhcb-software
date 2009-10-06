#ifndef CALOMONIDST_CALOEMUMONITOR_H
#define CALOMONIDST_CALOEMUMONITOR_H 1
// ============================================================================
// $Id: CaloEMuMonitor.h,v 1.5 2009-10-06 12:22:48 cattanem Exp $
// Include files
// ============================================================================

// from Gaudi
#include "Kernel/Particle2MCLinker.h"

// TrEvent
#include "Event/Track.h"
#include "Event/ProtoParticle.h"

#include "CaloUtils/Calo2Track.h"

#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
#include "CaloUtils/Calo2Track.h"

#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"

#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiUtils/HistoStats.h"


#include "GaudiAlg/GaudiTupleAlg.h"
#include "CaloInterfaces/ICaloTrackMatch.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "CaloUtils/ICaloElectron.h"

#include "CaloMoniAlg.h"

#include "AIDA/IAxis.h"

// #include "AIDA/ITree.h"
// #include <AIDA/IAnalysisFactory.h>
// #include <AIDA/ITreeFactory.h>

// boost
#include "boost/lexical_cast.hpp"
#include "boost/assign/list_of.hpp"

// =====================================================================
/** @file
 *
 *  Header file for class CaloEMuMonitor - another algorithm to monitor
 *  the quality of Calo Electron and Muon PID. See detailed description in
 *  <a href="http://cdsweb.cern.ch/record/1209003">LHCb-INT-2009-022</a>
 *
 *  @author Dmitry Golubkov
 *  @date   2009-03-24  */
// =====================================================================
namespace LHCb {
  class IParticlePropertySvc;
}

/** @class CaloEMuMonitor
 * 
 *  Another algorithm to monitor the quality of the Calo Electron and Muon PID.
 *  A few main PID-related distributions are accumulated in execute() and in
 *  the finalize() their parameters are compared with the reference values.
 *  <br>Original approach based on Check_PIDe (2007-05-30) by Victor Egorychev.
 *
 *  The values and errors of the reference parameters can be changed via declareProperty(),
 *  each monitoring histogram has 3 following properties (arrays of doubles):
 *  <ol><li> xHistogramName_histo  - histogram definition (n_bins, min, max, min_number_entries)
 *      <li> xHistogramName_params - reference values (mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow)
 *      <li> xHistogramName_errors - parameter errors (negative error disables checking the corresponding value)
 *  </ol>
 *  here the 'x' before the HistogramName stands for 'e' or 'mu' for electron- and muon-related monitoring histograms.
 *
 *  At present the following monitoring histograms are defined:
 *  <ul><li>eCaloEcalChi2     - electron ProtoParticle::CaloEcalChi2
 *      <li>eCaloEcalChi2Tanh - electron tanh(CaloEcalChi2 / Chi2eNormalization)
 *      <li>eCaloTrMatch      - electron ProtoParticle::CaloTrMatch
 *      <li>eChi2             - electron (E-p)^{2}/(cov(E,E)+cov(p,p))
 *      <li>ePull             - electron (E-p) pull
 *      <li>ePrseE            - electron ProtoParticle::CaloPrsE
 *      <li>muCaloEcalE       - muon ProtoParticle::CaloEcalE
 *      <li>muCaloHcalE       - muon ProtoParticle::CaloHcalE
 *  </ul>
 *
 *  The algorithm prints a Warning or an Error at finalize() if the difference in sigmas between the parameter
 *  and the reference value exceeds a treshlold (the threshold number of sigmas is steered by the NSigmaThreshold
 *  property).
 *  The tests are performed only for running over more than 'nEventMin' events.
 *
 *  @author Dmitry Golubkov
 *  @date   2009-03-24  */
class CaloEMuMonitor : public CaloMoniAlg {
public: 
  CaloEMuMonitor(const std::string& name, ISvcLocator* pSvcLocator ); ///< Standard constructor

  virtual ~CaloEMuMonitor() { }          ///< Destructor

  virtual StatusCode initialize   ();    ///< Algorithm init
  virtual StatusCode execute      ();    ///< Algorithm execution
  virtual StatusCode finalize     ();    ///< Algorithm finalization

protected:

  /// select/reject electron candidates
  bool acceptE(const LHCb::ProtoParticle *proto) const;
  /// select/reject muon candidates
  bool acceptMu(const LHCb::ProtoParticle *proto) const;
  /// select/reject track candidates
  virtual bool acceptTrack(const LHCb::ProtoParticle *proto);
  /// return the threshold value in sigmas for Warning (level=0) and Error (level=1)
  double nSigmaThreshold(unsigned int level) const {
    if ( level < m_nSigmaThreshold.size() ) return m_nSigmaThreshold[level];
    return 0;
  }
  /// compare parameters of the PID-related histograms with the reference values
  virtual void check();

  /// storage for the monitored histogram and its reference parameters
  class RefPar {
  public:
    RefPar() { }               ///< default constructor required by the CaloEMuChecker::m_refPar
    RefPar(CaloEMuMonitor *ptr) : v_par(8), v_err(8), v_def(4), status(-2), ndof(0) { caloMoniAlgPtr = ptr; } ///< constructor
    std::vector<double> v_par; ///< reference values: mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
    std::vector<double> v_err; ///< errors on the reference values: mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
    std::vector<double> v_def; ///< monitornig histogram definition: nbin, min, max, minimal number of entries

    double mean        () const { return v_par[0]; }  ///< reference mean
    double meanErr     () const { return v_err[0]; }  ///< error on the reference mean

    double rms         () const { return v_par[1]; }  ///< reference RMS
    double rmsErr      () const { return v_err[1]; }  ///< error on the reference RMS

    double first_bin   () const { return v_par[2]; }  ///< reference ratio of the last bin to the total entries (including underflow+overflow)
    double first_binErr() const { return v_err[2]; }  ///< error on the reference raio of the last bin to the total entries

    double skewness    () const { return v_par[3]; }  ///< reference skewness
    double skewnessErr () const { return v_err[3]; }  ///< error on the reference skewness

    double kurtosis    () const { return v_par[4]; }  ///< reference kurtosis
    double kurtosisErr () const { return v_err[4]; }  ///< error on the reference kurtosis

    double last_bin    () const { return v_par[5]; }  ///< reference ratio of the last bin to the total entries (including underflow+overflow)
    double last_binErr () const { return v_err[5]; }  ///< error on the reference raio of the last bin to the total entries

    double underflow   () const { return v_par[6]; }  ///< reference ratio of underflow to the total entries (including underflow+overflow)
    double underflowErr() const { return v_err[6]; }  ///< error on the reference ratio of underflow to the total entries

    double overflow    () const { return v_par[7]; }  ///< reference ratio of overflow to the total entries (including underflow+overflow)
    double overflowErr () const { return v_err[7]; }  ///< error on the reference ratio of overflow to the total entries

    /** Minimal number of entries required in a histogram.
      *
      * @return minimal number of entries (including underflow and overflow)
      * for the monitored histogram to be analysed. The number is set in the 4th parameter.  */
    double minStat     () const { return v_def[3]; }

    /** Compare parameters of the monitored histogram with reference values.
      *
      * @return -1 = comparison could not be done (missing histogram, too low statistics etc.)
      * @return  0 = OK
      * @return  1 = Warning (difference in sigmas exceeded the first threshold NSigmaThreshold)
      * @return  2 = Error (difference in sigmas exceeded the second threshold NSigmaThreshold)  */
    int    check       ();

    std::string hname;   ///< name of the monitoring histogram
    std::string htitle;  ///< title for the monitoring histogram

    std::string msg;     ///< message regarding the results of the check() function 

    /// create the monitored histogram
    void hbook1();

    AIDA::IHistogram1D *histo;  ///< pointer to the monitored histogram
  private:
    /// significance of the difference between the histogram mean and the reference mean()
    double diffMean    ();
    /// significance of the difference between the hisotgram RMS and the reference rms()
    double diffRms     ();
    /// significance of the difference between the ratio of the first bin to total entries and the reference first_bin()
    double diffFirstBin();
    /// significance of the difference between the ratio of the last bin to total entries and the reference last_bin()
    double diffLastBin ();
    /// significance of the difference between the ratio of the underflow  to the total entries and the reference underflow()
    // bool diffUnderflow(void);
    /// significance of the difference between the ratio of the overflow to the total entries and the reference overflow()
    double diffOverflow();
    /// significance of the difference between the histogram skewness and the reference skewness()
    bool diffSkewness (void);
    /// significance of the difference between the histogram kurtosis and the reference kurtosis()
    bool diffKurtosis (void);

    /** Calculate binomial error of the ratio divident/divisor.
     *
     *  @return binomial error sigma(divident/divisor) = sqrt(divident/divisor * (1.-divident/divisor) / divisor)  */
    static double binomialError(double divident, double divisor);
    /// update the status of the monitored histogram having observed sig difference from a reference value
    void   updateStatus(double sig);

    CaloEMuMonitor *caloMoniAlgPtr;  ///< pointer to the monitor algorithm

    /** status of the comparison with the reference values
     *
     *  -2 = status was not evaluated
     *  -1 = status could not be evaluated (e.g. low statistics)
     *   0 = OK
     *   1 = Warning
     *   2 = Error  */
    int status;
    std::string tmp_msg;  ///< temporary storage for messages
    int ndof;             ///< number of degrees of freedom of the parameter comparison chi2-like sum
  };


  /// intermediate storage for monitored variables
  class MoniVars {
  public:
    virtual ~MoniVars() { }

    float chi2e;  ///< 3D chi2 for Track and Calo matching (equivalent to CaloElectronMatch)
    float tanhe;  ///< tahn(chi2e/Chi2eNormalization) 

    int   ttype;  ///< track type

    bool  inecal; ///< Acceptance flag for Ecal
    bool  inhcal; ///< Acceptance flag for Hcal
    bool  inbrem; ///< Acceptance flag for Brem
    bool  inprs ; ///< Acceptance flag for Prs

    float epide;  ///< Ecal-based DLL for electron-ID
    float hpide;  ///< Hcal-based DLL for electron-ID
    float ppide;  ///< Prs-based DLL for electron-ID

    float epidm;  ///< Ecal-based DLL for muon-ID
    float hpidm;  ///< Hcal-based DLL for muon-ID

    float chi2b;  ///< 3D chi2 for Track/CaloHypo(Brem)

    float chi23d; ///< 3D chi2 for Track/CaloHypo(e) matching, -999. coresponds to 10000. for chi21, chi2e, chi23
    float chi2d;  ///< 2D chi2 for Track/CaloCluster matching (neutral)

    float spdd ;  ///< LHCb::ProtoParticle::CaloChargedSpd
    float prsd ;  ///< LHCb::ProtoParticle::CaloChargedPrs
    float ecalc;  ///< LHCb::ProtoParticle::CaloChargedEcal

    float spde ;  ///< LHCb::ProtoParticle::CaloSpdE
    float prse ;  ///< LHCb::ProtoParticle::CaloPrsE
    float ecale;  ///< LHCb::ProtoParticle::CaloEcalE
    float hcale;  ///< LHCb::ProtoParticle::CaloHcalE

    float p;         ///< track momentum
    float pt;        ///< track pT

    float eoverp;    ///< E/p
    float echi2;     ///< energy contribution to 3D chi2: (E-p)^{2}/(cov(E,E)+cov(p,p))
    float cov_track; ///< cov(p,p), track momentum error^2
    float cov_hypo;  ///< cov(E,E), cluster charged hypothesis energy error^2
    float e_hypo;    ///< E, cluster charged hypothesis (CaloHypo::EmCharged) energy
    float p_state;   ///< momentum of the track at the state extrapolated to the CaloPlane::ShowerMax
    float epull;     ///< energy pull: (E-p) / sqrt(cov(E,E)+cov(p,p))
    float p_rel_err; ///< relative error of the momentum: sqrt(cov(p,p))/p
    float e_rel_err; ///< relative error of the cluster energy: sqrt(cov(E,E))/E

    int   h_ndigit;  ///< number of digits linked ot the CaloHypo::EmCharged

    bool  muCandidate; ///< bool flag, true if the track was accepted as a muon candidate
    bool  eCandidate;  ///< bool flag, true if the track was accepted as an electron candidate
    float rdlle;       ///< ProtoParticle::RichDLLe
    float rdllmu;      ///< ProtoParticle::RichDLLmu
    bool  ismuon;      ///< ProtoParticle->muonPID->IsMuon()
  };

  /// pointer to an intermediate storage used for filling the monitoring histograms
  MoniVars *m_var;


  Gaudi::Plane3D       m_plane;  ///< Ecal ShowerMax plane
  ITrackExtrapolator  *m_ext;    ///< track extrapolator tool
  DeCalorimeter       *m_calo;   ///< Ecal

  /// Accessor for ParticlePropertySvc
  LHCb::IParticlePropertySvc* m_ppSvc;

  std::vector<double> m_nSigmaThreshold; ///< threshold values (in sigmas) corresponding to a Warning and an Error
  bool  m_extraHistograms;  ///< true/false = extra histograms should/should_not be produced

  int   m_eID;              ///< electron PDG code
  int   m_muID;             ///< muon PDG code
  int   m_JPsiID;           ///< J/Psi PDG code


  unsigned long m_nEventMin;///< minimal number of events to check
  float m_chi2eNorm;        ///< normalization factor for the tanh(chi2e / m_chi2eNorm) histogram
  float m_minPt;            ///< minimal pT of the track
  float m_maxPt;            ///< maximal pT of the track
  int   m_minTrackType;     ///< minimal track type (LHCb::Track::Types)
  float m_RichDLLe;         ///< minimal ProtoParticle::RichDLLe for electron identification
  float m_maxEHcalE;        ///< maximal ProtoParticle::CaloHcalE for electron identification



  /// complete the initialization of RefPar (book the histograms)
  virtual void  bookMonitoringHistograms();
  /// book additional useful histograms (although not monitored)
  virtual void  bookExtraHistograms();
  /// fill m_var for the selected track before histogramming
  virtual void  fillMVar(const LHCb::ProtoParticle* proto);
  /// fill monitoring histograms from m_var
  virtual void  fillMonitoringHistograms();
  /// fill additional histograms from m_var
  virtual void  fillExtraHistograms();
  /// declare a few basic properties - to be called from the constructor
  void declareBasicProperties();

  std::map<std::string,RefPar> m_refPar;  ///< the map of the monitored histograms + references
  std::string m_RefSample;                ///< description of the reference sample used
private:
};

// ============================================================================
// The END
// ============================================================================
#endif // CALOMONIDST_CALOEMUMONITOR_H
