
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

#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;

class MassBiasesMonitor;

namespace {

  // BIAS VARIABLE DISTRIBUTIONS

  typedef double(*DISTR)(double,vector<double>);

  double FLAT_FUNC(double, vector<double>)
  {
    return 1;
  }
  const DISTR FLAT_DISTR = &FLAT_FUNC;
  
  double THETA_FUNC(double x, vector<double>)
  {
    if (x < 0)
      return 0;
    return 1;
  }
  const DISTR THETA_DISTR = &THETA_FUNC;
  
  double EXPO_FUNC(double x, vector<double> params) 
  {
    double fall = params[0];
    return exp(-fall*fabs(x)); // exp(abs(x)) usually wanted
  };
  const DISTR EXPO_DISTR = &EXPO_FUNC;
  
  double RFEXPO_FUNC(double x, vector<double> params)
  {
    double thresh = params[0];
    double rise = params[1];
    double fall = params[2];
    if (fabs(x) < thresh)
      return 0;
    return (1-exp(-rise*(fabs(x)-thresh)))*exp(-fall*fabs(x));
  };
  const DISTR RFEXPO_DISTR = &RFEXPO_FUNC;

  double GAUS_FUNC(double x, vector<double> params)
  {
    double mu = params[0];
    double sigma = params[1];
    double z = (x-mu)/sigma;    
    return exp(-0.5*z*z);
  };
  const DISTR GAUS_DISTR = &GAUS_FUNC;

  double GAMMA_FUNC(double x, vector<double> params)
  {
    double k = params[0];
    double fall = params[1];    
    return pow(fabs(x),k-1)*exp(-fall*fabs(x));
  };
  const DISTR GAMMA_DISTR = &GAMMA_FUNC;
  
  double ASYM_FUNC(double x, vector<double> params)
  {
    double P = params[0];
    if (fabs(x) >= 1)
      return 0;
    return pow(1-x*x,P);
  }
  const DISTR ASYM_DISTR = &ASYM_FUNC;
  
  // ENUM -> DISTR MAP
  std::pair<string,DISTR> map2_data[] = {
    std::make_pair("FLAT",FLAT_DISTR)
    , std::make_pair("THETA",THETA_DISTR)
    , std::make_pair("EXPO",EXPO_DISTR)
    , std::make_pair("RFEXPO",RFEXPO_DISTR)
    , std::make_pair("GAUS",GAUS_DISTR)
    , std::make_pair("GAMMA",GAMMA_DISTR)
    , std::make_pair("ASYM",ASYM_DISTR)
  };  
  std::map<string,DISTR> getDistr(map2_data, map2_data + sizeof(map2_data)/sizeof(map2_data[0]));
  
  
  // HISTO IDS / TITLES
  const GaudiAlg::HistoID multID("multiplicity");
  const string multTitle = "multiplicity";

  const GaudiAlg::HistoID massID("mass");
  const string massTitle("mass [GeV]");
  const GaudiAlg::HistoID masspullID("masspull");
  const string masspullTitle("mass pull");
  const GaudiAlg::HistoID tophalfID("massPositiveY");
  const string tophalfTitle = "mass [GeV] for y>0";
  const GaudiAlg::HistoID bothalfID("massNegativeY");
  const string bothalfTitle = "mass [GeV] for y<0";

  const GaudiAlg::HistoID momID("momentum");
  const string momTitle = "momentum [GeV]";
  const GaudiAlg::HistoID ptID("pt");
  const string ptTitle = "pt [GeV]";
  const GaudiAlg::HistoID pdifID("momdif");
  const string pdifTitle = "p_{pos} - p_{neg} [GeV]";
  const GaudiAlg::HistoID asymID("asym");
  const string asymTitle = "momentum asymmetry";
  const GaudiAlg::HistoID etaID("eta");
  const string etaTitle = "eta";
  const GaudiAlg::HistoID txID("tx");
  const string txTitle = "tx";
  const GaudiAlg::HistoID tyID("ty");
  const string tyTitle = "ty";
  const GaudiAlg::HistoID phimattID("phimatt");
  const string phimattTitle = "decay plane angle w.r.t. magnetic field";
  const GaudiAlg::HistoID phiangleID("phiangle");
  const string phiangleTitle = "decay plane azimuthal angle";
  const GaudiAlg::HistoID openangleID("openingangle");
  const string openangleTitle = "opening angle";

  const GaudiAlg::HistoID momH2ID("massVersusMomH2");
  const string momH2Title = "mass vs. momentum [GeV]";
  const GaudiAlg::HistoID ptH2ID("massVersusPtH2");
  const string ptH2Title = "mass vs. pt [GeV]";
  const GaudiAlg::HistoID pdifH2ID("massVersusMomDifH2");
  const string pdifH2Title = "mass vs. p_{A} - p_{B} [GeV]";
  const GaudiAlg::HistoID asymH2ID("massVersusAsymH2");
  const string asymH2Title = "mass [GeV] vs. momentum asymmetry";
  const GaudiAlg::HistoID etaH2ID("massVersusEtaH2");
  const string etaH2Title = "mass [GeV] vs. eta";
  const GaudiAlg::HistoID txH2ID("massVersusTxH2");
  const string txH2Title = "mass [GeV] vs. tx";
  const GaudiAlg::HistoID tyH2ID("massVersusTyH2");
  const string tyH2Title = "mass [GeV] vs. ty";
  const GaudiAlg::HistoID phimattH2ID("massVersusPhiMattH2");
  const string phimattH2Title = "mass [GeV] vs. Matt's phi";
  const GaudiAlg::HistoID phiangleH2ID("massVersusPhiH2");
  const string phiangleH2Title = "mass [GeV] vs. phi";
  const GaudiAlg::HistoID openangleH2ID("massVersusOpenAngleH2");
  const string openangleH2Title = "mass [GeV] vs. opening angle";
  const GaudiAlg::HistoID iov2011H2ID("massVersus2011iovH2");
  const string iov2011H2Title = "mass vs. 2011 iovs [GeV]";
  const GaudiAlg::HistoID iov2012H2ID("massVersus2012iovH2");
  const string iov2012H2Title = "mass vs. 2012 iovs [GeV]";

  const GaudiAlg::HistoID momProID("massVersusMom");
  const string momProTitle = "mass vs. momentum [GeV]";
  const GaudiAlg::HistoID ptProID("massVersusPt");
  const string ptProTitle = "mass vs. pt [GeV]";
  const GaudiAlg::HistoID pdifProID("massVersusMomDif");
  const string pdifProTitle = "mass vs. p_{A} - p_{B} [GeV]";
  const GaudiAlg::HistoID asymProID("massVersusAsym");
  const string asymProTitle = "mass [GeV] vs. momentum asymmetry";
  const GaudiAlg::HistoID etaProID("massVersusEta");
  const string etaProTitle = "mass [GeV] vs. eta";
  const GaudiAlg::HistoID txProID("massVersusTx");
  const string txProTitle = "mass [GeV] vs. tx";
  const GaudiAlg::HistoID tyProID("massVersusTy");
  const string tyProTitle = "mass [GeV] vs. ty";
  const GaudiAlg::HistoID phimattProID("massVersusPhiMatt");
  const string phimattProTitle = "mass [GeV] vs. Matt's phi";
  const GaudiAlg::HistoID phiangleProID("massVersusPhi");
  const string phiangleProTitle = "mass [GeV] vs. phi";
  const GaudiAlg::HistoID openangleProID("massVersusOpenAngle");
  const string openangleProTitle = "mass [GeV] vs. opening angle";
  const GaudiAlg::HistoID iov2011ProID("massVersus2011iov");
  const string iov2011ProTitle = "mass vs. 2011 iov [GeV]";
  const GaudiAlg::HistoID iov2012ProID("massVersus2012iov");
  const string iov2012ProTitle = "mass vs. 2012 iov [GeV]";

  // PARSE IOV DICTIONARY
  // ../data contains files listing IOVs with info about start/stop dates/runs
  // Instead of recompiling to update / fix this, parse the .txt file

//   const char* raw = getenv("MASSMONITORROOT");
//   const string rootPath = raw?raw:"";
//   const string default_iov_file = rootPath+ "/data/LHCbStatus.txt";

  const string iov_data_2011 =
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
    "MagUp           2011-10-22      2011-10-31      2239-2267       103936          104414          54280.49\n";
  
  const string iov_data_2012 =
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

  vector<double> setBinThresh(int N, double min, double max, DISTR func, vector<double> params)
  {
    
    // First, numerically integrate to find normalization
    // Do this in 5N bins, which should be enough within error
    vector<double> thresh(N+1,0);
    double temp;
    double norm;
    {
      norm = 0;
      double delta = (max-min)/(5*N);
      for (int i = 0; i < 5*N; i++) {
        temp = (*func)(min + i*delta,params);
        temp += 4*(*func)(min + (i+0.5)*delta,params);
        temp += (*func)(min + (i+1)*delta,params);
        temp *= delta/6.0;
        norm += temp;
      }
    }

    double delta = 1.0/N;
    double high;
    double low;
    double next;
    double x = min;
    thresh[0] = x;
    for (int i = 1; i < N; i++) {
      high = max;
      low = x;
      next = (high + low)/2.0;
      for (int k = 0; k < 50; k++) {        
        double diff = ((*func)(x,params) + 4*(*func)((x + next)/2.0,params) + (*func)(next,params))*(next-x)/(6.0*norm);
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
    return thresh;
  }  

  vector<double> setSymBinThresh(int N, double max, DISTR func, vector<double> params)
  {
    vector<double> posEdges = setBinThresh(N,0,max,func,params);
    vector<double> edges(2*N+1,0);
    for (int i = 0; i <= N; i++) {
      edges[N+i] = posEdges[i];
      edges[N-i] = -posEdges[i];
    }
    return edges;
  }
  
}

class MassBiasesMonitor : public GaudiHistoAlg
{
public:
  
  /** Standard construtor */
  MassBiasesMonitor( const string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~MassBiasesMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm initialize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  string m_inputLocation; // Input Tracks container location
  ILHCbMagnetSvc* m_magfieldsvc;
  const LHCb::IParticlePropertySvc* m_propertysvc;

  // PROPERTIES
  // Mass
  int m_binsMass;
  double m_minMass; // initialized as MeV -> converted to GeV
  double m_maxMass; // etc
  // DISTR fixed to THETA

  // P
  int m_binsMom;
  double m_minMom;
  double m_maxMom;
  string m_momDistr;
  vector<double> m_momParams;

  // PT
  int m_binsPt;  
  double m_minPt;
  double m_maxPt;
  string m_ptDistr;
  vector<double> m_ptParams;

  // PDIF
  int m_binsPdif;
  double m_maxPdif;
  // Fixed to [-max,max]
  string m_pdifDistr;
  vector<double> m_pdifParams;

  // MOM ASYM
  int m_binsAsym;
  // Fixed to [-1,1]
  string m_asymDistr;
  vector<double> m_asymParams;
  
  // ETA
  int m_binsEta;
  double m_minEta;
  double m_maxEta;
  string m_etaDistr;
  vector<double> m_etaParams;

  // TX AND TY
  int m_binsTx;
  double m_maxTx;
  // Fixed to [-max,max]
  string m_txDistr;
  vector<double> m_txParams;

  int m_binsTy;
  double m_maxTy;
  // Fixed to [-max,max]
  string m_tyDistr;
  vector<double> m_tyParams;

  // DECAY PLANE ANGLES
  int m_binsPhiMatt;
  // DISTR fixed to FLAT
  // Fixed to [0,pi]
  int m_binsPhiAngle;
  // DISTR fixed to FLAT
  // Fixed to [0,pi]

  // OPENING ANGLE
  int m_binsOpenAngle;
  double m_minOpenAngle;
  double m_maxOpenAngle;
  string m_oangleDistr;
  vector<double> m_oangleParams;

  // 2011 IOVs
  string m_iov2011Data;
  vector<string> m_iov2011Conditions;
  vector<string> m_iov2011StartDates;
  vector<string> m_iov2011EndDates;
  vector<string> m_iov2011FillRanges;
  vector<int> m_iov2011StartRuns;
  vector<int> m_iov2011EndRuns;
  vector<double> m_iov2011Luminosities;

  // 2012 IOVs
  string m_iov2012Data;
  vector<string> m_iov2012Conditions;
  vector<string> m_iov2012StartDates;
  vector<string> m_iov2012EndDates;
  vector<string> m_iov2012FillRanges;
  vector<int> m_iov2012StartRuns;
  vector<int> m_iov2012EndRuns;
  vector<double> m_iov2012Luminosities;

};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MassBiasesMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MassBiasesMonitor::MassBiasesMonitor( const string& name,
                                              ISvcLocator* pSvcLocator)
    : GaudiHistoAlg( name , pSvcLocator ),
      m_magfieldsvc(0),
      m_propertysvc(0),

      m_binsMass(50),
      m_minMass(0*Gaudi::Units::GeV),
      m_maxMass(120.0*Gaudi::Units::GeV),

      m_binsMom(20),
      m_minMom(0),
      m_maxMom(0),
      m_momDistr("FLAT"),
      m_momParams(),
      
      m_binsPt(20),
      m_minPt(0),
      m_maxPt(0),
      m_ptDistr("FLAT"),
      m_ptParams(),

      m_binsPdif(5),
      m_maxPdif(0),
      m_pdifDistr("FLAT"),
      m_pdifParams(),

      m_binsAsym(5),
      m_asymDistr("FLAT"),
      m_asymParams(),

      m_binsEta(10),
      m_minEta(2),
      m_maxEta(7),
      m_etaDistr("FLAT"),
      m_etaParams(),

      m_binsTx(20),
      m_maxTx(0.2),
      m_txDistr("FLAT"),
      m_txParams(),

      m_binsTy(20),
      m_maxTy(0.2),
      m_tyDistr("FLAT"),
      m_tyParams(),

      m_binsPhiMatt(20),
      m_binsPhiAngle(12),

      m_binsOpenAngle(10),
      m_minOpenAngle(0),
      m_maxOpenAngle(0.3),
      m_oangleDistr("FLAT"),
      m_oangleParams(),

      m_iov2011Data(""),
      m_iov2012Data("")
{
  declareProperty( "InputLocation", m_inputLocation = "" );

  declareProperty( "BinsMass", m_binsMass, "# mass bins in all 2D histos (default: 50)" );
  declareProperty( "MinMass", m_minMass, "Mass window min in MeV (default: 0)" );
  declareProperty( "MaxMass", m_maxMass, "Mass window max in MeV (default: 120GeV" );

  declareProperty( "BinsMom", m_binsMom, "# momentum bins in M vs. p (default: 20)");
  declareProperty( "MinMom", m_minMom, "Min momentum (MeV) for all histos (default: 0)" );
  declareProperty( "MaxMom", m_maxMom, "Max momentum (MeV) for all histos (default: particle dependent)" );
  declareProperty( "MomDistr", m_momDistr, "Distribution to use for binning (default: FLAT; try RFEXPO)" );
  declareProperty( "MomParams", m_momParams, "Parameters for MomDistr" );

  declareProperty( "BinsPt", m_binsPt, "# transverse momentum bins in 2D histo (default: 20)");
  declareProperty( "MinPt", m_minPt, "Min transverse momentum (MeV) for all histos (default: 0)" );
  declareProperty( "MaxPt", m_maxPt, "Max transverse momentum (MeV) for all histos (default: particle dependent)" );
  declareProperty( "PtDistr", m_ptDistr, "Distribution to use for binning (default: FLAT; try GAMMA)" );
  declareProperty( "PtParams", m_ptParams, "Parameters for PtDistr" );

  declareProperty( "BinsMomDif", m_binsPdif, "# (p+ - p-) bins (pos/neg each) in 2D histo (default: 5)" );
  declareProperty( "MaxMomDif", m_maxPdif, "Max (p+ - p-) magnitude (MeV) for all histos (default: particle dependent)" );
  declareProperty( "MomDifDistr", m_pdifDistr, "Distribution to use for binning (default: FLAT; try EXPO)" );
  declareProperty( "MomDifParams", m_pdifParams, "Parameters for MomDifDistr" );

  declareProperty( "BinsAsym", m_binsAsym, "# (p+ - p-)/(p+ + p-) bins (pos/neg each) in 2D histo (default: 5)" );
  declareProperty( "AsymDistr", m_asymDistr, "Distribution to use for binning (default: FLAT; try ASYM)" );
  declareProperty( "AsymParams", m_asymParams, "Parameters for AsymDistr" );

  declareProperty( "BinsEta", m_binsEta, "# eta bins in 2D histo (default: 10)" );
  declareProperty( "MinEta", m_minEta, "Min eta (default: 2)");
  declareProperty( "MaxEta", m_maxEta, "Max eta (default: 7)");
  declareProperty( "EtaDistr", m_etaDistr, "Distribution to use for binning (default: FLAT; try GAMMA)" );
  declareProperty( "EtaParams", m_etaParams, "Parameters for EtaDistr" );

  declareProperty( "BinsTx", m_binsTx, "# tx (vertical slope) bins (pos/neg each) in 2D histo (default: 40)" );
  declareProperty( "MaxTx", m_maxTx, "Max tx (vertical slope) for all histos (default: 0.2)" );
  declareProperty( "TxDistr", m_txDistr, "Distribution to use for binning (default: FLAT; try EXPO)" );
  declareProperty( "TxParams", m_txParams, "Parameters for TxDistr" );

  declareProperty( "BinsTy", m_binsTy, "# ty (horizontal slope) bins (pos/neg each) in 2D histo (default: 40)" );
  declareProperty( "MaxTy", m_maxTy, "Max ty (horizontal slope) for all histos (default: 0.2)" );
  declareProperty( "TyDistr", m_tyDistr, "Distribution to use for binning (default: FLAT; try EXPO)" );
  declareProperty( "TyParams", m_tyParams, "Parameters for TyDistr" );

  declareProperty( "BinsPhiMatt", m_binsPhiMatt,"# 'Matt's phi' (decay plane angle w.r.t. B) bins (default: 20)" );
  declareProperty( "BinsPhiAngle", m_binsPhiAngle,"# 'phi' (decay plane azimuthal angle) bins (default: 12)" );

  declareProperty( "BinsOpenAngle", m_binsOpenAngle, "# eta bins in 2D histo (default: 10)" );
  declareProperty( "MinOpenAngle", m_minOpenAngle, "Min opening angle (default: 0)");
  declareProperty( "MaxOpenAngle", m_maxOpenAngle, "Max opening angle (default: 0.3)");
  declareProperty( "OpenAngleDistr", m_oangleDistr, "Distribution to use for binning (default: FLAT; try GAMMA)" );
  declareProperty( "OpenAngleParams", m_oangleParams, "Parameters for OpenAngleDistr" );

  declareProperty( "IOVData2011", m_iov2011Data,
                   "Tabular data containing 2011 IOV definitions - "
                   "Condition / StartDate / EndDate / FillRange / StartRun / EndRun / Int. Luminosity (/nb)"
                   "(default: reproduced in data/LHCbStatus.txt");
  declareProperty( "IOVData2012", m_iov2012Data,
                   "Tabular data containing 2012 IOV definitions - "
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
  if (m_maxPdif <= 0) {
    m_maxPdif = 0.5*m_maxMom;
  }

  // CALCULATE VARIABLE BIN SIZES
  /* Use setBinThresh to find bin edges
     Then, Convert everything to GeV if necessary
     (The only reason they aren't passed in GeV is because MeV are default units)
     Is this great practice? Maybe not, but it's convenient
  */

  // massEdges ...
  vector<double> empty;
  vector<double> massEdges = setBinThresh(m_binsMass,m_minMass,m_maxMass,THETA_DISTR,empty);
  m_minMass /= Gaudi::Units::GeV;
  m_maxMass /= Gaudi::Units::GeV;
  info() << "MASS BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMass+1; i++) {
    massEdges[i] /= Gaudi::Units::GeV;
    info() << "BIN " << i << ": " << massEdges[i] << std::endl;
  }
  info() << endmsg;

  // pEdges ...
  DISTR d = getDistr[m_momDistr];
  vector<double> pEdges = setBinThresh(m_binsMom,m_minMom,m_maxMom,d,m_momParams);
  m_minMom /= Gaudi::Units::GeV;
  m_maxMom /= Gaudi::Units::GeV;
  info() << "P BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMom+1; i++) {
    pEdges[i] /= Gaudi::Units::GeV;
    info() << "BIN " << i << ": " << pEdges[i] << std::endl;
  }
  info() << endmsg;

  //  ptEdges ...
  d = getDistr[m_ptDistr];
  vector<double> ptEdges = setBinThresh(m_binsPt,m_minPt,m_maxPt,d,m_ptParams);
  m_minPt /= Gaudi::Units::GeV;
  m_maxPt /= Gaudi::Units::GeV;
  info() << "PT BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsPt+1; i++) {
    ptEdges[i] /= Gaudi::Units::GeV;
    info() << "BIN " << i << ": " << ptEdges[i] << std::endl;
  }
  info() << endmsg;
  
  //  pdifEdges ...
  d = getDistr[m_pdifDistr];
  vector<double> pdifEdges = setSymBinThresh(m_binsPdif,m_maxPdif,d,m_pdifParams);
  m_maxPdif /= Gaudi::Units::GeV;
  info() << "PDIF BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsPdif+1; i++) {
    pdifEdges[i] /= Gaudi::Units::GeV;
    info() << "BIN " << i << ": " << pdifEdges[i] << std::endl;
  }
  info() << endmsg;

  // asymEdges ...
  d = getDistr[m_asymDistr];
  vector<double> asymEdges = setSymBinThresh(m_binsAsym,1,d,m_asymParams);
  info() << "ASYM BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsAsym+1; i++)
    info() << "BIN " << i << ": " << asymEdges[i] << std::endl;
  info() << endmsg;

  //  etaEdges ...
  d = getDistr[m_etaDistr];
  vector<double> etaEdges = setBinThresh(m_binsEta,m_minEta,m_maxEta,d,m_etaParams);
  info() << "ETA BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsEta+1; i++)
    info() << "BIN " << i << ": " << etaEdges[i] << std::endl;
  info() << endmsg;

  //  txEdges and tyEdges ...
  d = getDistr[m_txDistr];
  vector<double> txEdges = setSymBinThresh(m_binsTx,m_maxTx,d,m_txParams);
  info() << "TX BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsTx+1; i++)
    info() << "BIN " << i << ": " << txEdges[i] << std::endl;
  info() << endmsg;

  d = getDistr[m_tyDistr];
  vector<double> tyEdges = setSymBinThresh(m_binsTy,m_maxTy,d,m_tyParams);
  info() << "TY BIN EDGES:" << std::endl;
  for (int i = 0; i < 2*m_binsTy+1; i++)
    info() << "BIN " << i << ": " << tyEdges[i] << std::endl;
  info() << endmsg;

  // openangleEdges ...
  d = getDistr[m_oangleDistr];
  vector<double> openangleEdges = setBinThresh(m_binsOpenAngle,m_minOpenAngle,m_maxOpenAngle,d,m_oangleParams);
  info() << "OPENANGLE BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsOpenAngle+1; i++)
    info() << "BIN " << i << ": " << openangleEdges[i] << std::endl;
  info() << endmsg;

  // 2011 IOVs ...
  {
    std::stringstream input;
    if (m_iov2011Data != "")
      input << m_iov2011Data;
    else
      input << iov_data_2011;
    string line;
    for (int i = 0; i < 6; i++)
      std::getline(input, line);
    while (std::getline(input, line))
    {
      std::istringstream ss(line);
      string condition, startDate, endDate, fillRange;
      int startRun, endRun;
      double luminosity;
      ss >> condition >> startDate >> endDate >> fillRange >> startRun >> endRun >> luminosity;
      m_iov2011Conditions.push_back(condition);
      m_iov2011StartDates.push_back(startDate);
      m_iov2011EndDates.push_back(endDate);
      m_iov2011FillRanges.push_back(fillRange);
      m_iov2011StartRuns.push_back(startRun);
      m_iov2011EndRuns.push_back(endRun);
      m_iov2011Luminosities.push_back(luminosity);
    }
  }
  // Create bins out of the starting run numbers, and the final run number
  vector<double> iov2011Edges;
  iov2011Edges.assign(m_iov2011StartRuns.begin(),m_iov2011StartRuns.end());
  iov2011Edges.push_back(m_iov2011EndRuns.back());

  // 2012 IOVs ...
  {
    std::stringstream input;
    if (m_iov2012Data != "")
      input << m_iov2012Data;
    else
      input << iov_data_2012;
    string line;
//     for (int i = 0; i < 6; i++)
//       std::getline(input, line);
    while (std::getline(input, line))
    {
      std::istringstream ss(line);
      string condition, startDate, endDate, fillRange;
      int startRun, endRun;
      double luminosity;
      ss >> condition >> startDate >> endDate >> fillRange >> startRun >> endRun >> luminosity;
      m_iov2012Conditions.push_back(condition);
      m_iov2012StartDates.push_back(startDate);
      m_iov2012EndDates.push_back(endDate);
      m_iov2012FillRanges.push_back(fillRange);
      m_iov2012StartRuns.push_back(startRun);
      m_iov2012EndRuns.push_back(endRun);
      m_iov2012Luminosities.push_back(luminosity);
    }
  }
  // Create bins out of the starting run numbers, and the final run number
  vector<double> iov2012Edges;
  iov2012Edges.assign(m_iov2012StartRuns.begin(),m_iov2012StartRuns.end());
  iov2012Edges.push_back(m_iov2012EndRuns.back());

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
  book(pdifID,pdifTitle,-1*m_maxPdif,m_maxPdif);
  book(asymID,asymTitle,-1,1);
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
  book2D(iov2011H2ID,iov2011H2Title,iov2011Edges,massEdges);
  book2D(iov2012H2ID,iov2012H2Title,iov2012Edges,massEdges);
  

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
  bookProfile1D(iov2011ProID,iov2011ProTitle,iov2011Edges);
  bookProfile1D(iov2012ProID,iov2012ProTitle,iov2012Edges);  

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
    histo2D(iov2011H2ID)->fill(run,mass);
    histo2D(iov2012H2ID)->fill(run,mass);

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
    profile1D(iov2011ProID)->fill(run,mass);
    profile1D(iov2012ProID)->fill(run,mass);
  }
  return StatusCode::SUCCESS;
}
