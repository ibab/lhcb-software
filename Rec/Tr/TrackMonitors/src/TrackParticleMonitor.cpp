#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "Event/ChiSquare.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "TrackKernel/TrackTraj.h"
#include "Event/Particle.h"
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackKernel/TrackStateVertex.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

#include <boost/foreach.hpp>

class TrackParticleMonitor;

namespace {

  typedef double(TrackParticleMonitor::*PDF)(double);

  // HISTO IDS / TITLES
  const GaudiAlg::HistoID multID("multiplicity");
  const std::string multTitle = "multiplicity";
  const GaudiAlg::HistoID trackChi2ID("trackChi2");
  const std::string trackChi2Title = "track chi2/dof";
  const GaudiAlg::HistoID trackPID("trackP");
  const std::string trackPTitle = "track momentum";
  const GaudiAlg::HistoID trackPtID("trackPt");
  const std::string trackPtTitle = "track pT";
  const GaudiAlg::HistoID vertexChi2ProbID("chi2prob");
  const std::string vertexChi2ProbTitle = "vertex chi2prob";
  const GaudiAlg::HistoID vertexChi2ID("chi2");
  const std::string vertexChi2Title = "vertex chi2";
  const GaudiAlg::HistoID vtxxID("vtxx");
  const std::string vtxxTitle = "vertex x";
  const GaudiAlg::HistoID vtxyID("vtxy");
  const std::string vtxyTitle = "vertex y";
  const GaudiAlg::HistoID vtxzID("vtxz");
  const std::string vtxzTitle = "vertex z";  
  const GaudiAlg::HistoID vtxxerrID("vtxxerr");
  const std::string vtxxerrTitle = "vertex x error";
  const GaudiAlg::HistoID vtxyerrID("vtxyerr");
  const std::string vtxyerrTitle = "vertex y error";
  const GaudiAlg::HistoID vtxzerrID("vtxzerr");
  const std::string vtxzerrTitle = "vertex z error";
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

  void setBinThresh(int N, std::vector<double> &thresh, double min, double max, TrackParticleMonitor* tpm, PDF func)
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
  
  // Descends to find all proto tracks making up particle
  void addTracks( const LHCb::Particle& p,
                  std::vector<const LHCb::Track*>& tracks,
                  std::vector<double>& masshypos)
  {
    if( p.proto() && p.proto()->track() ) {
      tracks.push_back(p.proto()->track() );
      masshypos.push_back(p.momentum().M());
    } else {
      BOOST_FOREACH( const LHCb::Particle* dau, p.daughters() )
        addTracks( *dau, tracks, masshypos );
    }
  }
}

class TrackParticleMonitor : public GaudiHistoAlg 
{
public:

  /** Standard construtor */
  TrackParticleMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackParticleMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm initialize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  std::string m_inputLocation; // Input Tracks container location
  ILHCbMagnetSvc* m_magfieldsvc;
  ToolHandle<ITrackStateProvider> m_stateprovider;
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
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackParticleMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TrackParticleMonitor::TrackParticleMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
    : GaudiHistoAlg( name , pSvcLocator ), 
      m_magfieldsvc(0),
      m_stateprovider("TrackStateProvider"),
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
      m_binsOpenAngle(10)
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
}

//=============================================================================
// Destructor
//=============================================================================
TrackParticleMonitor::~TrackParticleMonitor() 
{
}

StatusCode TrackParticleMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
 
  // SETUP SERVICES
  m_magfieldsvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
  m_propertysvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true);
  m_stateprovider.retrieve().ignore();

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

  // HISTOGRAM SETTINGS FOR TRACKS
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

  double trackChi2, vertexChi2Max;
  double trackM_Pmax, trackM_Ptmax;
  double vtxX, vtxY, vtxZ;
  if (m_maxMass < 2.3) {
    trackM_Pmax = 200;
    trackM_Ptmax = 20;
    trackChi2 = 5.0; vertexChi2Max = 5.0;
    vtxX = 2.5; vtxY = 2.0; vtxZ = 200.0;
  } else {
    trackM_Pmax = 20*m_maxMass;
    trackM_Ptmax = 3*m_maxMass;
    trackChi2 = 10.0; vertexChi2Max = 20;
    vtxX = 2.0; vtxY = 2.0; vtxZ = 200.0;
  }


  // CALCULATE VARIABLE BIN SIZES
  
  // massEdges ...
  std::vector<double> massEdges(m_binsMass+1,0);
  setBinThresh(m_binsMass,massEdges,m_minMass,m_maxMass,this,&TrackParticleMonitor::massPDF);
  debug() << "MASS BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMass+1; i++)
    debug() << "BIN " << i << ": " << massEdges[i] << std::endl;
  debug() << endmsg;

  // pEdges ...
  std::vector<double> pEdges(m_binsMom+1,0);
  setBinThresh(m_binsMom,pEdges,0,m_maxMom,this,&TrackParticleMonitor::momPDF);
  debug() << "P BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMom+1; i++)
    debug() << "BIN " << i << ": " << pEdges[i] << std::endl;
  debug() << endmsg;

  //  ptEdges ...
  std::vector<double> ptEdges(m_binsPt+1,0);
  setBinThresh(m_binsPt,ptEdges,0,m_maxPt,this,&TrackParticleMonitor::ptPDF);
  debug() << "PT BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsPt+1; i++)
    debug() << "BIN " << i << ": " << ptEdges[i] << std::endl;
  debug() << endmsg;

  //  pdifEdges ...
  std::vector<double> pdifEdges(2*m_binsMomDif+1,0);
  std::vector<double> pdifPosEdges(m_binsMomDif+1,0);
  setBinThresh(m_binsMomDif,pdifPosEdges,0,m_maxMomDif,this,&TrackParticleMonitor::pdifPDF);
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
  setBinThresh(m_binsMomAsym,asymPosEdges,0,1,this,&TrackParticleMonitor::asymPDF);
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
  setBinThresh(m_binsEta,etaEdges,m_minEta,m_maxEta,this,&TrackParticleMonitor::etaPDF);
  debug() << "ETA BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsEta+1; i++)
    debug() << "BIN " << i << ": " << etaEdges[i] << std::endl;
  debug() << endmsg;

  //  txEdges and tyEdges ...
  std::vector<double> txEdges(2*m_binsTx+1,0);
  std::vector<double> txPosEdges(m_binsTx+1,0);
  setBinThresh(m_binsTx,txPosEdges,0,m_maxTx,this,&TrackParticleMonitor::txPDF);
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
  setBinThresh(m_binsTy,tyPosEdges,0,m_maxTy,this,&TrackParticleMonitor::tyPDF);
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
  setBinThresh(m_binsOpenAngle,openangleEdges,m_minOpenAngle,m_maxOpenAngle,this,&TrackParticleMonitor::openanglePDF);
  debug() << "OPENANGLE BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsOpenAngle+1; i++)
    debug() << "BIN " << i << ": " << openangleEdges[i] << std::endl;
  debug() << endmsg;

  // CREATE HISTOGRAMS
  setHistoTopDir("Track/");

  // Various track/vertex histograms
  book(multID,multTitle,-0.5,10.5,11);
  book(trackChi2ID,trackChi2Title,0,trackChi2);
  book(trackPID,trackPTitle,0,trackM_Pmax);
  book(trackPtID,trackPtTitle,0,trackM_Ptmax);
  book(vertexChi2ProbID,vertexChi2ProbTitle,0,1);
  book(vertexChi2ID,vertexChi2Title,0,vertexChi2Max);
  book(vtxxID,vtxxTitle,-vtxX,vtxX);
  book(vtxyID,vtxyTitle,-vtxY,vtxY);
  book(vtxzID,vtxzTitle,-vtxZ,vtxZ);
  book(vtxxerrID,vtxxerrTitle,0,0.2,50);
  book(vtxyerrID,vtxyerrTitle,0,0.2,50);
  book(vtxzerrID,vtxzerrTitle,0,2.5,50);

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

  // RETURN
  return sc;
}

    

StatusCode TrackParticleMonitor::finalize()
{
  m_stateprovider.release().ignore();
  return  GaudiHistoAlg::finalize();
}

  
StatusCode TrackParticleMonitor::execute()
{ 

  LHCb::Particle::Range particles  = get<LHCb::Particle::Range>(m_inputLocation);
  histo(multID)->fill(particles.size());
  //info() << "particles: " << particles.size() << endmsg;

  BOOST_FOREACH( const LHCb::Particle* particle, particles) {
    
    std::vector< const LHCb::Track* > tracks;
    std::vector< double > masshypos;
    addTracks(*particle, tracks, masshypos);
    
    std::vector< const LHCb::State* > states;
    double z = particle->referencePoint().z();
    //double z= particle->endVertex()->position().z();

    BOOST_FOREACH( const LHCb::Track* track, tracks) {
      histo(trackChi2ID)->fill(track->chi2PerDoF());
      histo(trackPID)->fill(track->p()/Gaudi::Units::GeV);
      histo(trackPtID)->fill(track->pt()/Gaudi::Units::GeV);
      
      LHCb::State* state = new LHCb::State();
      m_stateprovider->stateFromTrajectory(*state,*track,z).ignore();
      states.push_back(state);
    }
    
    double pdgmass(0),pdgwidth(0);
    const LHCb::ParticleProperty* prop = m_propertysvc->find( particle->particleID() );
    if( prop ) {
      pdgmass  = prop->mass()/Gaudi::Units::GeV;
      pdgwidth = prop->width()/Gaudi::Units::GeV;
    }

    LHCb::TrackStateVertex vertex( states );
    vertex.fit();
    double chi2 = vertex.chi2();
    double chi2prob = LHCb::ChiSquare(chi2,vertex.nDoF()).prob();
    histo(vertexChi2ProbID)->fill(chi2prob);
    histo(vertexChi2ID)->fill(chi2);
    histo(vtxxID)->fill(vertex.position().x());
    histo(vtxyID)->fill(vertex.position().y());
    histo(vtxzID)->fill(vertex.position().z());
    const Gaudi::SymMatrix3x3& poscov = vertex.covMatrix();
    histo(vtxxerrID)->fill(std::sqrt(poscov(0,0)));
    histo(vtxyerrID)->fill(std::sqrt(poscov(1,1)));
    histo(vtxzerrID)->fill(std::sqrt(poscov(2,2)));

    // assume that there are at least two tracks
    LHCb::State stateA = vertex.state(0);
    LHCb::State stateB = vertex.state(1);
    
    if( (m_magfieldsvc->isDown() && stateA.qOverP() > 0 ) ||
        (!m_magfieldsvc->isDown() && stateA.qOverP() < 0 ) )
      std::swap( stateA, stateB );
    
    // DAUGHTERS
    Gaudi::XYZVector p3A = stateA.momentum()/Gaudi::Units::GeV;
    double ppos = p3A.R();
    Gaudi::XYZVector p3B = stateB.momentum()/Gaudi::Units::GeV;
    double pneg = p3B.R();
    double pdif = ppos - pneg;
    double asym = pdif / (ppos + pneg);

    // MOTHER
    Gaudi::LorentzVector p4 = vertex.p4(masshypos)/Gaudi::Units::GeV;
    double mass = p4.M();
    double merr = vertex.massErr(masshypos)/Gaudi::Units::GeV;
    double masspull = (mass-pdgmass)/std::sqrt(merr*merr+pdgwidth*pdgwidth);
    double mom  = p4.P();
    double pt = p4.Pt();
    double eta = p4.Eta();
    double tx = p4.x() / p4.z();
    double ty = p4.y() / p4.z();

    // DECAY PLANE ANGLES
    Gaudi::XYZVector norm = p3A.Cross( p3B ).Unit();
    double phiangle = std::atan2( norm.y(), norm.x());
    double phimatt  = std::acos( norm.y() );
    double openangle = acos( p3A.Dot(p3B)/(ppos*pneg) );

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
    
    // Memory management
    BOOST_FOREACH( const LHCb::State* s, states) delete s;
  }
  return StatusCode::SUCCESS;
}
