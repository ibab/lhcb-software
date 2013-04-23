#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "Event/ODIN.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "Event/Particle.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include <boost/foreach.hpp>
#include <stdlib.h>
#include <fstream>
#include <sstream>

class MassBiasesMonitor;

namespace {

  typedef double(MassBiasesMonitor::*PDF)(double);

  // HISTO IDS / TITLES
  const GaudiAlg::HistoID multID("multiplicity");
  const std::string multTitle = "multiplicity";

  const GaudiAlg::HistoID massID("mass");
  const std::string massTitle("mass [GeV]");
  const GaudiAlg::HistoID masspullID("masspull");
  const std::string masspullTitle("mass pull");
  const GaudiAlg::HistoID tophalfID("massPositiveY");
  const std::string tophalfTitle = "mass [GeV] for y>0";
  const GaudiAlg::HistoID bothalfID("massNegativeY");
  const std::string bothalfTitle = "mass [GeV] for y<0";

  const GaudiAlg::HistoID momID("momentum");
  const std::string momTitle = "momentum [GeV]";
  const GaudiAlg::HistoID ptID("pt");
  const std::string ptTitle = "pt [GeV]";
  const GaudiAlg::HistoID pdifID("momdif");
  const std::string pdifTitle = "p_{pos} - p_{neg} [GeV]";
  const GaudiAlg::HistoID asymID("asym");
  const std::string asymTitle = "momentum asymmetry";
  const GaudiAlg::HistoID etaID("eta");
  const std::string etaTitle = "eta";
  const GaudiAlg::HistoID txID("tx");
  const std::string txTitle = "tx";
  const GaudiAlg::HistoID tyID("ty");
  const std::string tyTitle = "ty";
  const GaudiAlg::HistoID phimattID("phimatt");
  const std::string phimattTitle = "decay plane angle w.r.t. magnetic field";
  const GaudiAlg::HistoID phiangleID("phiangle");
  const std::string phiangleTitle = "decay plane azimuthal angle";
  const GaudiAlg::HistoID openangleID("openingangle");
  const std::string openangleTitle = "opening angle";

  const GaudiAlg::HistoID momH2ID("massVersusMomH2");
  const std::string momH2Title = "mass vs. momentum [GeV]";
  const GaudiAlg::HistoID ptH2ID("massVersusPtH2");
  const std::string ptH2Title = "mass vs. pt [GeV]";
  const GaudiAlg::HistoID pdifH2ID("massVersusMomDifH2");
  const std::string pdifH2Title = "mass vs. p_{A} - p_{B} [GeV]";
  const GaudiAlg::HistoID asymH2ID("massVersusMomAsymH2");
  const std::string asymH2Title = "mass [GeV] vs. momentum asymmetry";
  const GaudiAlg::HistoID etaH2ID("massVersusEtaH2");
  const std::string etaH2Title = "mass [GeV] vs. eta";
  const GaudiAlg::HistoID txH2ID("massVersusTxH2");
  const std::string txH2Title = "mass [GeV] vs. tx";
  const GaudiAlg::HistoID tyH2ID("massVersusTyH2");
  const std::string tyH2Title = "mass [GeV] vs. ty";
  const GaudiAlg::HistoID phimattH2ID("massVersusPhiMattH2");
  const std::string phimattH2Title = "mass [GeV] vs. Matt's phi";
  const GaudiAlg::HistoID phiangleH2ID("massVersusPhiH2");
  const std::string phiangleH2Title = "mass [GeV] vs. phi";
  const GaudiAlg::HistoID openangleH2ID("massVersusOpenAngleH2");
  const std::string openangleH2Title = "mass [GeV] vs. opening angle";
  const GaudiAlg::HistoID timeH2ID("massVersusTimeH2");
  const std::string timeH2Title = "mass vs. time [GeV]";

  const GaudiAlg::HistoID momProID("massVersusMom");
  const std::string momProTitle = "mass vs. momentum [GeV]";
  const GaudiAlg::HistoID ptProID("massVersusPt");
  const std::string ptProTitle = "mass vs. pt [GeV]";
  const GaudiAlg::HistoID pdifProID("massVersusMomDif");
  const std::string pdifProTitle = "mass vs. p_{A} - p_{B} [GeV]";
  const GaudiAlg::HistoID asymProID("massVersusMomAsym");
  const std::string asymProTitle = "mass [GeV] vs. momentum asymmetry";
  const GaudiAlg::HistoID etaProID("massVersusEta");
  const std::string etaProTitle = "mass [GeV] vs. eta";
  const GaudiAlg::HistoID txProID("massVersusTx");
  const std::string txProTitle = "mass [GeV] vs. tx";
  const GaudiAlg::HistoID tyProID("massVersusTy");
  const std::string tyProTitle = "mass [GeV] vs. ty";
  const GaudiAlg::HistoID phimattProID("massVersusPhiMatt");
  const std::string phimattProTitle = "mass [GeV] vs. Matt's phi";
  const GaudiAlg::HistoID phiangleProID("massVersusPhi");
  const std::string phiangleProTitle = "mass [GeV] vs. phi";
  const GaudiAlg::HistoID openangleProID("massVersusOpenAngle");
  const std::string openangleProTitle = "mass [GeV] vs. opening angle";
  const GaudiAlg::HistoID timeProID("massVersusTime");
  const std::string timeProTitle = "mass vs. time [GeV]";

  // PARSE IOV DICTIONARY
  // ../data contains files listing IOVs with info about start/stop dates/runs
  // Instead of recompiling to update / fix this, parse the .txt file

  const char* raw = getenv("MASSMONITORROOT");
  const std::string rootPath = raw?raw:"";
  const std::string default_iov_file = rootPath+ "/data/LHCbStatus.txt";

  const std::string default_iov_data =
    "MagDown         2011-03-13      2011-03-21      1617-1640       87219           87773           2639.85\n"
    "MagDown         2011-03-22      2011-04-14      1644-1647       87849           87977           4918.50\n"
    "MagUp           2011-04-15      2011-04-25      1711-1737       89333           90207           44738.68\n"
    "MagDown	       2011-04-26	     2011-06-10      1739-1862       90256           93282           195775.28\n"
    "MagUp           2011-06-11      2011-06-28      1863-1901       93398           94386           140419.58\n"
    "MagUp           2011-07-07      2011-07-27      1936-1979       95948           97028           71239.42\n"
    "MagDown         2011-07-27      2011-08-17      1982-2029       97114           98882           192363.51\n"
    "MagUp           2011-08-17      2011-08-22      2030-2040       98900           100256          36805.67\n"
    "MagUp           2011-09-07      2011-09-16      2083-2110       101373          101862          67994.42\n"
    "MagDown         2011-09-16      2011-09-28      2117-2160       101891          102452          99523.48\n"
    "MagUp           2011-09-28      2011-10-05      2165-2182       102499          102907          83023.29\n"
    "MagDown         2011-10-05      2011-10-21      2186-2234       103049          103863          113943.49\n"
    "MagUp           2011-10-22      2011-10-31      2239-2267       103936          104414          54280.49\n"
    "MagDown         2012-04-01      2012-04-17      2469-2523       111181          112916          48250.48\n"
    "MagDown         2012-04-17      2012-05-01      2533-2536       113013          113146          19635.33\n"
    "MagUp           2012-05-01      2012-05-02      2574-2580       114205          114287          3366.76\n"
    "MagDown         2012-05-02      2012-05-16      2583-2630       114316          115464          107426.52\n"
    "MagUp           2012-05-16      2012-05-31      2632-2686       115518          117103          163044.70\n"
    "MagDown         2012-05-31      2012-06-11      2691-2720       117192          118286          152351.70\n"
    "MagUp           2012-06-11      2012-07-02      2723-2739       118326          118880          121009.77\n"
    "MagUp           2012-07-02      2012-07-20      2795-2856       119956          122520          95125.17\n"
    "MagUp           2012-07-20      2012-07-25      2858-2875       122540          123803          94469.95\n"
    "MagDown         2012-07-25      2012-08-10      2880-2934       123910          125115          205270.63\n"
    "MagUp		       2012-08-10	     2012-08-28	     2957-3011       125566          126676          194361.06\n"
    "MagDown		     2012-08-28	     2012-09-15	     3015-3047       126824          128110          144160.58\n"
    "MagUp           2012-09-15	     2012-09-17	     3067-3160       128411          128492          156105.81\n"
    "MagDown		     2012-10-12	     2012-10-24	     3169-3214       130316          130861          140446.12\n"
    "MagUp		       2012-10-24	     2012-11-08	     3220-3272       130911          131940          185258.63\n"
    "MagDown         2012-11-08      2012-12-03      3273-3363       131973          133587          233169.75\n"
    "MagUp           2012-12-03      2012-12-08      3370-3378       133624          133785          16035.34\n";

  void setBinThresh(int N, std::vector<double> &thresh, double min, double max, MassBiasesMonitor* tpm, PDF func)
  {

    // First, numerically integrate to find normalization
    // Do this in 5N bins, which should be enough within error
    double norm;
    {
      norm = 0;
      double delta = (max-min)/(5*N);
      for (int i = 0; i < 5*N; i++) {
        norm += ((tpm->*func)(min + i*delta) + 4*(tpm->*func)(min + (i+0.5)*delta) + (tpm->*func)(min + (i+1)*delta))*delta/6.0;
      }
    }

    double delta = 1.0/N;
    double high;
    double low;
    double next;
    double x = min;
    thresh[0]= x;
    for (int i = 1; i < N; i++) {
      high = max;
      low = x;
      next = (high + low)/2.0;
      for (int k = 0; k < 50; k++) {
        double diff = ((tpm->*func)(x) + 4*(tpm->*func)((x + next)/2.0) + (tpm->*func)(next))*(next-x)/(6.0*norm);
        if (fabs(diff-delta) <= 1E-4)
          break;
        if (diff > delta) {
          high = next;
          next = (next+low)/2.0;
        } else {
          low = next;
          next = (next+high)/2.0;
        }
      }
      x = next;
      thresh[i] = x;
    }
    thresh[N] = max;
  }

}

class MassBiasesMonitor : public GaudiHistoAlg
{
public:

  /** Standard construtor */
  MassBiasesMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~MassBiasesMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm initialize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  std::string m_inputLocation; // Input Tracks container location
  ILHCbMagnetSvc* m_magfieldsvc;
  const LHCb::IParticlePropertySvc* m_propertysvc;

  // PROPERTIES
  // Mass
  double m_minMass; // initialized as MeV -> converted to GeV
  double m_maxMass; // etc
  int m_binsMass;
  double massPDF(double mass)
  {
    if (mass >= 0)
      return 1;
    return 0;
  };

  // P
  double m_maxMom;
  double m_threshMom;
  double m_riseMom;
  double m_fallMom;
  int m_binsMom;
  double momPDF(double mom)
  {
    if (m_threshMom == 0 and m_riseMom == 0 and m_fallMom == 0) // defaults
      return 1; // flat
    if (mom < m_threshMom)
      return 0;
    if (m_riseMom == 0) // just exponential
      return exp(-m_fallMom*mom);
    return (1-exp(-m_riseMom*(mom-m_threshMom)))*exp(-m_fallMom*mom);
  };

  // PT
  double m_maxPt;
  double m_threshPt;
  double m_risePt;
  double m_fallPt;
  int m_binsPt;
  double ptPDF(double pt)
  {
    if (m_threshPt == 0 and m_risePt == 0 and m_fallPt == 0) // defaults
      return 1; // flat
    if (pt < m_threshPt)
      return 0;
    if (m_risePt == 0) // just exponential
      return exp(-m_fallPt*pt);
    return (1-exp(-m_risePt*(pt-m_threshPt)))*exp(-m_fallPt*pt);
  };

  // PDIF
  double m_maxMomDif;
  double m_fallMomDif;
  int m_binsMomDif;
  double pdifPDF(double pdif)
  {
    if (m_fallMomDif == 0) // defaults
      return 1; // flat
    return exp(-m_fallMomDif*fabs(pdif));
  };

  // MOM ASYM
  bool m_powMomAsym;
  int m_binsMomAsym;
  double asymPDF(double a)
  {
    if (m_powMomAsym == 0) // defaults
      return 1; // flat
    return pow(1-a*a,m_powMomAsym);
  }

  // ETA
  double m_minEta;
  double m_maxEta;
  double m_kEta;
  double m_fallEta;
  int m_binsEta;
  double etaPDF(double eta)
  {
    if (m_kEta == 1 and m_fallEta == 0) // defaults
      return 1;
    return pow(eta,m_kEta-1)*exp(-m_fallEta*eta);
  };

  // TX AND TY
  double m_maxTx;
  double m_fallTx;
  int m_binsTx;
  double txPDF(double tx)
  {
    if (m_fallTx == 0)
      return 1;
    return exp(-m_fallTx*fabs(tx));
  }

  double m_maxTy;
  double m_fallTy;
  int m_binsTy;
  double tyPDF(double ty)
  {
    if (m_fallTy == 0)
      return 1;
    return exp(-m_fallTy*fabs(ty));
  }

  // DECAY PLANE ANGLES
  int m_binsPhiMatt;
  int m_binsPhiAngle;

  // OPENING ANGLE
  double m_minOpenAngle;
  double m_maxOpenAngle;
  double m_kOpenAngle;
  double m_fallOpenAngle;
  int m_binsOpenAngle;
  double openanglePDF(double oa)
  {
    if (m_kOpenAngle == 1 and m_fallOpenAngle == 0) // defaults
      return 1;
    return pow(oa,m_kOpenAngle+1)*exp(-m_fallOpenAngle*oa);
  };

  // IOVs
  std::string m_iovFilePath;
  std::string m_iovData;
  std::vector<std::string> m_IOVConditions;
  std::vector<std::string> m_IOVStartDates;
  std::vector<std::string> m_IOVEndDates;
  std::vector<std::string> m_IOVFillRanges;
  std::vector<int> m_IOVStartRuns;
  std::vector<int> m_IOVEndRuns;
  std::vector<double> m_IOVLuminosities;

};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MassBiasesMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MassBiasesMonitor::MassBiasesMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
    : GaudiHistoAlg( name , pSvcLocator ),
      m_magfieldsvc(0),
      m_propertysvc(0),

      m_minMass(0*Gaudi::Units::GeV),
      m_maxMass(120.0*Gaudi::Units::GeV),
      m_binsMass(50),

      m_maxMom(0),
      m_threshMom(0),
      m_riseMom(0),
      m_fallMom(0),
      m_binsMom(20),

      m_maxPt(0),
      m_threshPt(0),
      m_risePt(0),
      m_fallPt(0),
      m_binsPt(20),

      m_maxMomDif(0),
      m_fallMomDif(0),
      m_binsMomDif(5),

      m_powMomAsym(0),
      m_binsMomAsym(5),

      m_minEta(2),
      m_maxEta(7),
      m_kEta(1),
      m_fallEta(0),
      m_binsEta(10),

      m_maxTx(0.2),
      m_fallTx(0),
      m_binsTx(20),
      m_maxTy(0.2),
      m_fallTy(0),
      m_binsTy(20),

      m_binsPhiMatt(20),
      m_binsPhiAngle(12),

      m_minOpenAngle(0),
      m_maxOpenAngle(0.3),
      m_kOpenAngle(1),
      m_fallOpenAngle(0),
      m_binsOpenAngle(10),

      m_iovData("")
{
  declareProperty( "InputLocation", m_inputLocation = "" );

  declareProperty( "MinMass", m_minMass, "Mass window min in MeV (default: 0)" );
  declareProperty( "MaxMass", m_maxMass, "Mass window max in MeV (default: 120GeV" );
  declareProperty( "BinsMass", m_binsMass, "# mass bins in all 2D histos (default: 50)" );

  declareProperty( "MaxMom", m_maxMom, "Max momentum in MeV for all histos (default: particle dependent)" );
  declareProperty( "ThreshMom", m_threshMom, "threshold momentum in MeV (optional)");
  declareProperty( "RiseMom", m_riseMom,"exp rise rate from threshold in 1/MeV (optional)");
  declareProperty( "FallMom", m_fallMom,"exp fall rate in 1/MeV (optional)");
  declareProperty( "BinsMom", m_binsMom, "# momentum bins in M vs. p (default: 20)");

  declareProperty( "MaxPt", m_maxPt, "Max transverse momentum in MeV for all histos (default: particle dependent)" );
  declareProperty( "ThreshPt", m_threshPt, "threshold transverse momentum in MeV (optional)");
  declareProperty( "RisePt", m_risePt,"exp rise rate from threshold in 1/MeV (optional)");
  declareProperty( "FallPt", m_fallPt,"exp fall rate in 1/MeV (optional)");
  declareProperty( "BinsPt", m_binsPt, "# transverse momentum bins in 2D histo (default: 20)");

  declareProperty( "MaxMomDif", m_maxMomDif, "Max (p+ - p-) magnitude in MeV for all histos (default: particle dependent)" );
  declareProperty( "FallMomDif", m_fallMomDif, "exp fall rate in 1/MeV (optional)");
  declareProperty( "BinsMomDif", m_binsMomDif, "# (p+ - p-) bins (pos/neg each) in 2D histo (default: 5)" );

  declareProperty( "PowMomAsym", m_powMomAsym, "power of (p+ - p-)/(p+ + p-) shape");
  declareProperty( "BinsMomAsym", m_binsMomAsym, "# (p+ - p-)/(p+ + p-) bins (pos/neg each) in 2D histo (default: 5)" );

  declareProperty( "MinEta", m_minEta, "Min eta (default: 2)");
  declareProperty( "MaxEta", m_maxEta, "Max eta (default: 7)");
  declareProperty( "KEta", m_kEta,"rise power (optional)" );
  declareProperty( "FallEta", m_fallEta,"exp fall rate (optional)");
  declareProperty( "BinsEta", m_binsEta, "# eta bins in 2D histo (default: 10)" );

  declareProperty( "MaxTx", m_maxTx, "Max tx (vertical slope) for all histos (default: 0.2)" );
  declareProperty( "FallTx", m_fallTx, "exp fall rate (optional)");
  declareProperty( "BinsTx", m_binsTx, "# tx (vertical slope) bins (pos/neg each) in 2D histo (default: 40)" );
  declareProperty( "MaxTy", m_maxTy, "Max ty (horizontal slope) for all histos (default: 0.2)" );
  declareProperty( "FallTy", m_fallTy, "exp fall rate (optional)");
  declareProperty( "BinsTy", m_binsTy, "# ty (horizontal slope) bins (pos/neg each) in 2D histo (default: 40)" );

  declareProperty( "BinsPhiMatt", m_binsPhiMatt,"# 'Matt's phi' (decay plane angle w.r.t. B) bins (default: 20)" );
  declareProperty( "BinsPhiAngle", m_binsPhiAngle,"# 'phi' (decay plane azimuthal angle) bins (default: 12)" );

  declareProperty( "MinOpenAngle", m_minOpenAngle, "Min opening angle (default: 0)");
  declareProperty( "MaxOpenAngle", m_maxOpenAngle, "Max opening angle (default: 0.3)");
  declareProperty( "KOpenAngle", m_kOpenAngle,"rise power (optional)" );
  declareProperty( "FallOpenAngle", m_fallOpenAngle,"exp fall rate (optional)");
  declareProperty( "BinsOpenAngle", m_binsOpenAngle, "# eta bins in 2D histo (default: 10)" );

  declareProperty( "IOVFilePath", m_iovFilePath, "Path to file defining IOVs (default: data/LHCbStatus.txt)");
  declareProperty( "IOVData", m_iovData,
                   "Tabular data containing IOV definitions - "
                   "Condition / StartDate / EndDate / FillRange / StartRun / EndRun / Int. Luminosity (/nb)"
                   "(default: reproduced in data/LHCbStatus.txt");
}



//=============================================================================
// Destructor
//=============================================================================
MassBiasesMonitor::~MassBiasesMonitor()
{
}

StatusCode MassBiasesMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // SETUP SERVICES

  m_magfieldsvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
  m_propertysvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true);

  // SWITCH TO DEFAULT MOM / PT / PDIF MAXES
  // If user doesn't set m_maxMom or m_maxPt, use defaults
  // Also catch some sanity errors while we're at it
  if (m_maxMom <= 0) {
    if (m_maxMass < 2.3*Gaudi::Units::GeV)
      m_maxMom = 200*Gaudi::Units::GeV;
    else
      m_maxMom = 50*m_maxMass;
  }
  if (m_maxPt <= 0) {
    if (m_maxMass < 2.3*Gaudi::Units::GeV)
      m_maxPt = 30*Gaudi::Units::GeV;
    else
      m_maxPt = 3*m_maxMass;
  }
  if (m_maxMomDif <= 0) {
    m_maxMomDif = 0.5*m_maxMom;
  }
  if (m_kEta < 1.0)
    m_kEta = 1.0;

  // HISTOGRAM SETTINGS
  /* Convert everything to GeV
     (The only reason they aren't passed in GeV is because MeV are default units)
     Is this great practice? Maybe not, but it's convenient
  */

  m_minMass /= Gaudi::Units::GeV;
  m_maxMass /= Gaudi::Units::GeV;

  m_maxMom /= Gaudi::Units::GeV;
  m_threshMom /= Gaudi::Units::GeV;
  m_riseMom *= Gaudi::Units::GeV;
  m_fallMom *= Gaudi::Units::GeV;

  m_maxPt /= Gaudi::Units::GeV;
  m_threshPt /= Gaudi::Units::GeV;
  m_risePt *= Gaudi::Units::GeV;
  m_fallPt *= Gaudi::Units::GeV;

  m_maxMomDif /= Gaudi::Units::GeV;
  m_fallMomDif *= Gaudi::Units::GeV;

  // CALCULATE VARIABLE BIN SIZES

  // massEdges ...
  std::vector<double> massEdges(m_binsMass+1,0);
  setBinThresh(m_binsMass,massEdges,m_minMass,m_maxMass,this,&MassBiasesMonitor::massPDF);
  debug() << "MASS BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMass+1; i++)
    debug() << "BIN " << i << ": " << massEdges[i] << std::endl;
  debug() << endmsg;

  // pEdges ...
  std::vector<double> pEdges(m_binsMom+1,0);
  setBinThresh(m_binsMom,pEdges,0,m_maxMom,this,&MassBiasesMonitor::momPDF);
  debug() << "P BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMom+1; i++)
    debug() << "BIN " << i << ": " << pEdges[i] << std::endl;
  debug() << endmsg;

  //  ptEdges ...
  std::vector<double> ptEdges(m_binsPt+1,0);
  setBinThresh(m_binsPt,ptEdges,0,m_maxPt,this,&MassBiasesMonitor::ptPDF);
  debug() << "PT BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsPt+1; i++)
    debug() << "BIN " << i << ": " << ptEdges[i] << std::endl;
  debug() << endmsg;

  //  pdifEdges ...
  std::vector<double> pdifEdges(2*m_binsMomDif+1,0);
  std::vector<double> pdifPosEdges(m_binsMomDif+1,0);
  setBinThresh(m_binsMomDif,pdifPosEdges,0,m_maxMomDif,this,&MassBiasesMonitor::pdifPDF);
  for (int i = 0; i <= m_binsMomDif; i++) {
    pdifEdges[m_binsMomDif+i] = pdifPosEdges[i];
    pdifEdges[m_binsMomDif-i] = -pdifPosEdges[i];
  }
  debug() << "PDIF BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsMomDif+1; i++)
    debug() << "BIN " << i << ": " << pdifEdges[i] << std::endl;
  debug() << endmsg;

  // asymEdges ...
  std::vector<double> asymEdges(2*m_binsMomAsym+1,0);
  std::vector<double> asymPosEdges(m_binsMomAsym+1,0);
  setBinThresh(m_binsMomAsym,asymPosEdges,0,1,this,&MassBiasesMonitor::asymPDF);
  for (int i = 0; i <= m_binsMomAsym; i++) {
    asymEdges[m_binsMomAsym+i] = asymPosEdges[i];
    asymEdges[m_binsMomAsym-i] = -asymPosEdges[i];
  }
  debug() << "ASYM BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsMomAsym+1; i++)
    debug() << "BIN " << i << ": " << asymEdges[i] << std::endl;
  debug() << endmsg;

  //  etaEdges ...
  std::vector<double> etaEdges(m_binsEta+1,0);
  setBinThresh(m_binsEta,etaEdges,m_minEta,m_maxEta,this,&MassBiasesMonitor::etaPDF);
  debug() << "ETA BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsEta+1; i++)
    debug() << "BIN " << i << ": " << etaEdges[i] << std::endl;
  debug() << endmsg;

  //  txEdges and tyEdges ...
  std::vector<double> txEdges(2*m_binsTx+1,0);
  std::vector<double> txPosEdges(m_binsTx+1,0);
  setBinThresh(m_binsTx,txPosEdges,0,m_maxTx,this,&MassBiasesMonitor::txPDF);
  for (int i = 0; i <= m_binsTx; i++) {
    txEdges[m_binsTx+i] = txPosEdges[i];
    txEdges[m_binsTx-i] = -txPosEdges[i];
  }
  debug() << "TX BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsTx+1; i++)
    debug() << "BIN " << i << ": " << txEdges[i] << std::endl;
  debug() << endmsg;

  std::vector<double> tyEdges(2*m_binsTy+1,0);
  std::vector<double> tyPosEdges(m_binsTy+1,0);
  setBinThresh(m_binsTy,tyPosEdges,0,m_maxTy,this,&MassBiasesMonitor::tyPDF);
  for (int i = 0; i <= m_binsTy; i++) {
    tyEdges[m_binsTy+i] = tyPosEdges[i];
    tyEdges[m_binsTy-i] = -tyPosEdges[i];
  }
  debug() << "TY BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsTy+1; i++)
    debug() << "BIN " << i << ": " << tyEdges[i] << std::endl;
  debug() << endmsg;

  // openangleEdges ...
  std::vector<double> openangleEdges(m_binsOpenAngle+1,0);
  setBinThresh(m_binsOpenAngle,openangleEdges,m_minOpenAngle,m_maxOpenAngle,this,&MassBiasesMonitor::openanglePDF);
  debug() << "OPENANGLE BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsOpenAngle+1; i++)
    debug() << "BIN " << i << ": " << openangleEdges[i] << std::endl;
  debug() << endmsg;

  // IOVs ...
  {
    std::stringstream input;
    if (m_iovData != "")
      input << m_iovData;
    else
      input << default_iov_data;
    std::string line;
    for (int i = 0; i < 6; i++)
      std::getline(input, line);
    while (std::getline(input, line))
    {
      std::istringstream ss(line);
      std::string condition, startDate, endDate, fillRange;
      int startRun, endRun;
      double luminosity;
      ss >> condition >> startDate >> endDate >> fillRange >> startRun >> endRun >> luminosity;
      m_IOVConditions.push_back(condition);
      m_IOVStartDates.push_back(startDate);
      m_IOVEndDates.push_back(endDate);
      m_IOVFillRanges.push_back(fillRange);
      m_IOVStartRuns.push_back(startRun);
      m_IOVEndRuns.push_back(endRun);
      m_IOVLuminosities.push_back(luminosity);
    }
  }
  // Create bins out of the starting run numbers, and the final run number
  std::vector<double> runEdges;
  runEdges.assign(m_IOVStartRuns.begin(),m_IOVStartRuns.end());
  runEdges.push_back(m_IOVEndRuns.back());

  // CREATE HISTOGRAMS
  setHistoTopDir("Biases/");

  // Multiplicity
  book(multID,multTitle,-0.5,10.5,11);

  // Mass histograms
  book(massID,massTitle,m_minMass,m_maxMass);
  book(masspullID,masspullTitle,-5,5);
  book(tophalfID,tophalfTitle,m_minMass,m_maxMass,m_binsMass);
  book(bothalfID,bothalfTitle,m_minMass,m_maxMass,m_binsMass);

  // 1D histograms of bias variables
  book(momID,momTitle,0,m_maxMom);
  book(ptID,ptTitle,0,m_maxPt);
  book(pdifID,pdifTitle,-1*m_maxMomDif,m_maxMomDif);
  book(asymID,asymTitle,-1,1,m_binsMomAsym);
  book(etaID,etaTitle,m_minEta,m_maxEta);
  book(txID,txTitle,-m_maxTx,m_maxTx);
  book(tyID,tyTitle,-m_maxTy,m_maxTy);
  book(phimattID,phimattTitle,0,M_PI);
  book(phiangleID,phiangleTitle,0,M_PI);
  book(openangleID,openangleTitle,m_minOpenAngle,m_maxOpenAngle);

  // 2D histograms of mass vs. bias variables
  book2D(momH2ID,momH2Title,pEdges,massEdges);
  book2D(ptH2ID,ptH2Title,ptEdges,massEdges);
  book2D(pdifH2ID,pdifH2Title,pdifEdges,massEdges);
  book2D(asymH2ID,asymH2Title,asymEdges,massEdges);
  book2D(etaH2ID,etaH2Title,etaEdges,massEdges);
  book2D(txH2ID,txH2Title,txEdges,massEdges);
  book2D(tyH2ID,tyH2Title,tyEdges,massEdges);
  book2D(phimattH2ID,phimattH2Title,0,M_PI,m_binsPhiMatt,m_minMass,m_maxMass,m_binsMass);
  book2D(phiangleH2ID,phiangleH2Title,-M_PI,M_PI,m_binsPhiAngle,m_minMass,m_maxMass,m_binsMass);
  book2D(openangleH2ID,openangleH2Title,openangleEdges,massEdges);
  book2D(timeH2ID,timeH2Title,runEdges,massEdges);

  // Profiles of mass vs. bias variables
  bookProfile1D(momProID,momProTitle,pEdges);
  bookProfile1D(ptProID,ptProTitle,ptEdges);
  bookProfile1D(pdifProID,pdifProTitle,pdifEdges);
  bookProfile1D(asymProID,asymProTitle,asymEdges);
  bookProfile1D(etaProID,etaProTitle,etaEdges);
  bookProfile1D(txProID,txProTitle,txEdges);
  bookProfile1D(tyProID,tyProTitle,tyEdges);
  bookProfile1D(phimattProID,phimattProTitle,0,M_PI,m_binsPhiMatt);
  bookProfile1D(phiangleProID,phiangleProTitle,-M_PI,M_PI,m_binsPhiAngle);
  bookProfile1D(openangleProID,openangleProTitle,openangleEdges);
  bookProfile1D(timeProID,timeProTitle,runEdges);

  // RETURN
  return sc;
}



StatusCode MassBiasesMonitor::finalize()
{
  return  GaudiHistoAlg::finalize();
}


StatusCode MassBiasesMonitor::execute()
{

  LHCb::Particle::Range particles  = get<LHCb::Particle::Range>(m_inputLocation);
  histo(multID)->fill(particles.size());

  BOOST_FOREACH( const LHCb::Particle* particle, particles) {

    double pdgmass(0),pdgwidth(0);
    const LHCb::ParticleProperty* prop = m_propertysvc->find( particle->particleID() );
    if( prop ) {
      pdgmass  = prop->mass()/Gaudi::Units::GeV;
      pdgwidth = prop->width()/Gaudi::Units::GeV;
    }

    // Get daughter particles
    /* Checks that there are exactly two signed daughters of opposite sign
       (otherwise some of the variables are ambiguous and this tool shouldn't be used)
       If this fails, returns a statusCode::Failure
    */

    const LHCb::Particle *dauA = 0;
    const LHCb::Particle *dauB = 0;
    int chargeA = 0;
    BOOST_FOREACH( const LHCb::Particle* dau, particle->daughters() ) {
      int c = dau->charge();
      if (dauA == 0) {
        if (c != 0) {
          dauA = dau;
          chargeA = c;
        }
      } else if (dauB == 0) {
        if ((c > 0) == (chargeA > 0))
          return Error("Unique Pos/Neg daughters not found", StatusCode::SUCCESS);
        if (c != 0)
          dauB = dau;
      } else {
        if (c != 0)
          return Error("Unique Pos/Neg daughters not found", StatusCode::SUCCESS);
      }
    }
    if (dauA == 0 or dauB == 0)
      return Error("Unique Pos/Neg daughters not found", StatusCode::SUCCESS);
    if( (m_magfieldsvc->isDown() && chargeA > 0 ) ||
        (!m_magfieldsvc->isDown() && chargeA < 0 ) )
      std::swap( dauA, dauB );

    // MOTHER VARIABLES
    double mass = particle->measuredMass()/Gaudi::Units::GeV;
    double merr = particle->measuredMassErr()/Gaudi::Units::GeV;
    double masspull = (mass-pdgmass)/std::sqrt(merr*merr+pdgwidth*pdgwidth);

    Gaudi::LorentzVector p4 = particle->momentum()/Gaudi::Units::GeV;
    double mom  = p4.P();
    double pt = p4.Pt();
    double eta = p4.Eta();
    double tx = p4.x() / p4.z();
    double ty = p4.y() / p4.z();

    // DAUGHTER VARIABLES
    Gaudi::LorentzVector p4A = dauA->momentum()/Gaudi::Units::GeV;
    Gaudi::XYZVector p3A = p4A.Vect();
    Gaudi::LorentzVector p4B = dauB->momentum()/Gaudi::Units::GeV;
    Gaudi::XYZVector p3B = p4B.Vect();
    double ppos = p3A.R();
    double pneg = p3B.R();
    double pdif = ppos - pneg;
    double asym = pdif / (ppos + pneg);

    // DECAY PLANE VARIABLES
    Gaudi::XYZVector norm = p3A.Cross( p3B ).Unit();
    double phiangle = std::atan2( norm.y(), norm.x());
    double phimatt  = std::acos( norm.y() );
    double openangle = acos( p3A.Dot(p3B)/(ppos*pneg) );

    // RUN NUMBER
    // Load the ODIN (copied from TupleToolEventInfo)
    const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default);
    if ( !odin )
      odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default,false);
    if ( !odin )
      return Error("Cannot load the ODIN data object", StatusCode::SUCCESS);
    int run = odin->runNumber();

    // Mass histograms
    histo(massID)->fill(mass);
    histo(masspullID)->fill(masspull);
    if( p3A.y()<0 && p3B.y()<0)
      plot( mass, bothalfID, bothalfTitle);
    else if (p3A.y()>0 && p3B.y()>0)
      plot( mass, tophalfID, tophalfTitle);

    // 1D histograms of bias variables
    histo(momID)->fill(mom);
    histo(ptID)->fill(pt);
    histo(pdifID)->fill(pdif);
    histo(asymID)->fill(asym);
    histo(etaID)->fill(eta);
    histo(txID)->fill(tx);
    histo(tyID)->fill(ty);
    histo(phimattID)->fill(phimatt);
    histo(phiangleID)->fill(phiangle);
    histo(openangleID)->fill(openangle);

    // 2D histograms of mass vs. bias variables
    histo2D(momH2ID)->fill(mom,mass);
    histo2D(ptH2ID)->fill(pt,mass);
    histo2D(pdifH2ID)->fill(pdif,mass);
    histo2D(asymH2ID)->fill(asym,mass);
    histo2D(etaH2ID)->fill(eta,mass);
    histo2D(txH2ID)->fill(tx,mass);
    histo2D(tyH2ID)->fill(ty,mass);
    histo2D(phimattH2ID)->fill(phimatt,mass);
    histo2D(phiangleH2ID)->fill(phiangle,mass);
    histo2D(openangleH2ID)->fill(openangle,mass);
    histo2D(timeH2ID)->fill(run,mass);

    // Profiles of mass vs. bias variables
    profile1D(momProID)->fill(mom,mass);
    profile1D(ptProID)->fill(pt,mass);
    profile1D(pdifProID)->fill(pdif,mass);
    profile1D(asymProID)->fill(asym,mass);
    profile1D(etaProID)->fill(eta,mass);
    profile1D(txProID)->fill(tx,mass);
    profile1D(tyProID)->fill(ty,mass);
    profile1D(phimattProID)->fill(phimatt,mass);
    profile1D(phiangleProID)->fill(phiangle,mass);
    profile1D(openangleProID)->fill(openangle,mass);
    profile1D(timeProID)->fill(run,mass);
  }
  return StatusCode::SUCCESS;
}
