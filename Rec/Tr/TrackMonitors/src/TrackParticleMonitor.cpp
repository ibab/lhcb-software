#include "GaudiKernel/AlgFactory.h"
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
#include <boost/math/special_functions/gamma.hpp>

namespace {

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
  const GaudiAlg::HistoID momID("momentum");
  const std::string momTitle = "momentum [GeV]";
  const GaudiAlg::HistoID pdifID("momdif");
  const std::string pdifTitle = "p_{pos} - p_{neg} [GeV]";
  const GaudiAlg::HistoID ptID("pt");
  const std::string ptTitle = "pt [GeV]";
  const GaudiAlg::HistoID etaID("eta");
  const std::string etaTitle = "eta";
  const GaudiAlg::HistoID asymID("asym");
  const std::string asymTitle = "momentum asymmetry";
  const GaudiAlg::HistoID phimattID("phimatt");
  const std::string phimattTitle = "decay plane angle w.r.t. magnetic field";
  const GaudiAlg::HistoID phiangleID("phiangle");
  const std::string phiangleTitle = "decay plane azimuthal angle";
  const GaudiAlg::HistoID openingangleID("openingangle");
  const std::string openingangleTitle = "opening angle";
  const GaudiAlg::HistoID momH2ID("massVersusMomH2");
  const std::string momH2Title = "mass vs. momentum [GeV]";
  const GaudiAlg::HistoID pdifH2ID("massVersusMomDifH2");
  const std::string pdifH2Title = "mass vs. p_{A} - p_{B} [GeV]";
  const GaudiAlg::HistoID ptH2ID("massVersusPtH2");
  const std::string ptH2Title = "mass vs. pt [GeV]";
  const GaudiAlg::HistoID etaH2ID("massVersusEtaH2");
  const std::string etaH2Title = "mass [GeV] vs. eta";
  const GaudiAlg::HistoID tyH2ID("massVersusTyH2");
  const std::string tyH2Title = "mass [GeV] vs. ty";
  const GaudiAlg::HistoID asymH2ID("massVersusMomAsymH2");
  const std::string asymH2Title = "mass [GeV] vs. momentum asymmetry";
  const GaudiAlg::HistoID phimattH2ID("massVersusPhiMattH2");
  const std::string phimattH2Title = "mass [GeV] vs. Matt's phi";
  const GaudiAlg::HistoID phiangleH2ID("massVersusPhiH2");
  const std::string phiangleH2Title = "mass [GeV] vs. phi";
  const GaudiAlg::HistoID momProID("massVersusMom");
  const std::string momProTitle = "mass vs. momentum [GeV]";
  const GaudiAlg::HistoID pdifProID("massVersusMomDif");
  const std::string pdifProTitle = "mass vs. p_{A} - p_{B} [GeV]";
  const GaudiAlg::HistoID ptProID("massVersusPt");
  const std::string ptProTitle = "mass vs. pt [GeV]";
  const GaudiAlg::HistoID etaProID("massVersusEta");
  const std::string etaProTitle = "mass [GeV] vs. eta";
  const GaudiAlg::HistoID tyProID("massVersusTy");
  const std::string tyProTitle = "mass [GeV] vs. ty";
  const GaudiAlg::HistoID asymProID("massVersusMomAsym");
  const std::string asymProTitle = "mass [GeV] vs. momentum asymmetry";
  const GaudiAlg::HistoID phimattProID("massVersusPhiMatt");
  const std::string phimattProTitle = "mass [GeV] vs. Matt's phi";
  const GaudiAlg::HistoID phiangleProID("massVersusPhi");
  const std::string phiangleProTitle = "mass [GeV] vs. phi";
  const GaudiAlg::HistoID tophalfID("massPositiveY");
  const std::string tophalfTitle = "mass [GeV] for y>0";
  const GaudiAlg::HistoID bothalfID("massNegativeY");
  const std::string bothalfTitle = "mass [GeV] for y<0";  

  // Variable binning of eta distribution requires regularized gamma function and its inverse
  double GammaReg(double k, double q) 
  {
    return boost::math::gamma_q<double, double>(k,q);
  }
  
  // Modified from Num. Snip. 18.1 (http://numericalsnippets.com/noi/18.1)
  double InvGammaReg(double k, double g,
                     double lower = 0.0, double upper = 100000.0, // upper/lower bound for binary search
                     double prec = 1E-3, int level = 1, int max_level = 100) // acceptance criteria, can be loose
  {
    // EDGE CASES
    if (g == 0.0)
      return -999.999; // this is an error
    if (g > 1.0 or g < 0.0)
      return -1.0;
    if(level > max_level)
      return -1.0;

    // CLOSE ENOUGH
    double n = (upper+lower)/2.0;
    if (fabs(upper-lower) < prec)
      return n;

    // RAISE LOWER BOUND / DROP UPPER BOUND
    double m = GammaReg(k, n);
    if(m > g)
      return InvGammaReg(k,g,n,upper,prec,level+1,max_level);
    return InvGammaReg(k,g,lower,n,prec,level+1,max_level);
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
  double m_minMass;
  double m_maxMass;
  int m_binsMass;
  double m_maxMom;
  double m_decayMom; 
  double m_threshMom;
  int m_binsMom;
  int m_binsMomDif;
  double m_maxPt;
  double m_decayPt;
  double m_threshPt;
  int m_binsPt;
  double m_minEta;
  double m_maxEta;
  double m_kEta;
  double m_decayEta;
  int m_binsEta;
  double m_maxTy;
  int m_binsTy;
  int m_binsMomAsym;
  int m_binsPhiMatt;
  int m_binsPhiAngle;
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
      m_decayMom(0),
      m_threshMom(0),
      m_binsMom(20),
      m_binsMomDif(20),
      m_maxPt(0),
      m_decayPt(0),
      m_threshPt(0),
      m_binsPt(20),
      m_minEta(2),
      m_maxEta(7),
      m_kEta(1),
      m_decayEta(0),
      m_binsEta(10),
      m_maxTy(0.2),
      m_binsTy(40),
      m_binsMomAsym(20),
      m_binsPhiMatt(20),
      m_binsPhiAngle(12)
{
  declareProperty( "InputLocation", m_inputLocation = "" );
  declareProperty( "MinMass", m_minMass, "Mass window minimum in [MeV]" );
  declareProperty( "MaxMass", m_maxMass, "Mass window maximum in [MeV]" );
  declareProperty( "BinsMass", m_binsMass, "Number of mass bins in all 2D histos" );
  declareProperty( "MaxMom", m_maxMom, "Maximum p in [MeV] for all histos (default: depends on particle mass)" );
  declareProperty( "DecayMom", m_decayMom,
                   "Fall rate of momentum distribution in 1/[MeV] (set for optional variable binning in M vs. p)");
  declareProperty( "ThreshMom", m_threshMom, "Start variable momentum binning in [MeV] ([0,this] makes up lowest bin)");
  declareProperty( "BinsMom", m_binsMom, "Number of momentum bins in M vs. p (default: 20)");
  declareProperty( "BinsMomDif", m_binsMomDif, "Number of pdif bins in M vs. pdif (only flat binning)" );
  declareProperty( "MaxPt", m_maxPt, "Maxmum pt in [MeV] for all histos (default: depends on particle mass)" );
  declareProperty( "DecayPt", m_decayPt, "Fall rate of pt distribution in 1/[MeV] (for variable binning in M vs. pt)");
  declareProperty( "ThreshPt", m_threshPt, "Start variable pt binning in [MeV] ([0,this] makes up lowest bin)");
  declareProperty( "BinsPt", m_binsPt, "Number of pt bins in M vs. pt (default: 20)");
  declareProperty( "MinEta", m_minEta, "Minimum eta (default: 2)");
  declareProperty( "MaxEta", m_maxEta, "Maximum eta (default: 7)");
  declareProperty( "KEta", m_kEta,
                   "k-factor of eta's gamma-like distribution (set for optional variable binning in M vs. eta)" );
  declareProperty( "DecayEta", m_decayEta,
                   "Fall rate of eta's gamma-like distribution (set for optional variable binning in M vs. eta)");
  declareProperty( "BinsEta", m_binsEta, "Number of eta bins in M vs. eta (default: 10)" );
  declareProperty( "MaxTy", m_maxTy, "Maximum ty (track verticle slope) for all histos (default: 0.2)" );
  declareProperty( "BinsTy", m_binsTy, "Number of ty (track verticle slope) bins in M vs. ty (default: 40)" );
  declareProperty( "BinsMomAsym", m_binsMomAsym, "Number of pasym bins in M vs. pasym (only flat binning)" );
  declareProperty( "BinsPhiMatt", m_binsPhiMatt,
                   "Number of 'Matt's phi' (decay plane angle w.r.t. magnetic field) bins in M vs. 'Matt's phi' (default: 20)" );
  declareProperty( "BinsPhiAngle", m_binsPhiAngle,
                   "Number of 'phi' (decay plane azimuthal angle) bins in M vs.'phi' (default: 12)" );

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

  // SWITCH TO DEFAULT MOM / PT MAXES
  // If user doesn't set m_maxMom or m_maxPt, use defaults
  // Also catch some sanity errors
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
  if (m_kEta < 1.0)
    m_kEta = 1.0;
  if (m_binsMomDif % 2 != 0)
    m_binsMomDif += 1;
  

  // HISTOGRAM SETTINGS FOR TRACKS
  // Convert everything to GeV
  double mmin = m_minMass/Gaudi::Units::GeV;
  double mmax = m_maxMass/Gaudi::Units::GeV;
  double pthresh = m_threshMom/Gaudi::Units::GeV;
  double plambda = m_decayMom*Gaudi::Units::GeV;
  double pmax = m_maxMom/Gaudi::Units::GeV;
  double ptthresh = m_threshPt/Gaudi::Units::GeV;
  double ptlambda = m_decayPt*Gaudi::Units::GeV;
  double ptmax = m_maxPt/Gaudi::Units::GeV;
  double pdifmin = -0.5*pmax;
  double pdifmax = 0.5*pmax;
  double trackChi2, vertexChi2Max;
  double trackPMax, trackPtMax;
  double vtxX, vtxY, vtxZ;
  if (mmax < 2.3) {
    trackPMax = 200;
    trackPtMax = 20;
    trackChi2 = 5.0; vertexChi2Max = 5.0;
    vtxX = 2.5; vtxY = 2.0; vtxZ = 200.0;
  } else {
    trackPMax = 20*mmax;
    trackPtMax = 3*mmax;
    trackChi2 = 10.0; vertexChi2Max = 20;
    vtxX = 2.0; vtxY = 2.0; vtxZ = 200.0;
  }

  // CALCULATE VARIABLE BIN SIZES
  std::vector<double> massEdges(m_binsMass+1,0);
  std::vector<double> pEdges(m_binsMom+1,0);
  std::vector<double> ptEdges(m_binsPt+1,0);
  std::vector<double> etaEdges(m_binsEta+1,0);

  // massEdges ...
  {
    massEdges[0] = mmin;
    int n = m_binsMass;
    double p = 1.0;
    double dp = 1.0 / n;
    for (int i = 1; i < n + 1; i++) {
      p -= dp;
      massEdges[i] = mmin + (mmax-mmin)*(1-p);
    }

    // Should already be m_maxMom, but with machine imprecision...
    massEdges[m_binsMass] = mmax;
  }
  debug() << "MASS BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMass+1; i++)
    debug() << "BIN " << i << ": " << massEdges[i] << std::endl;
  debug() << endmsg;

  // pEdges ...
  {
    pEdges[0] = 0;
    int offset;
    int n;
    if (pthresh != 0) {
      pEdges[1] = pthresh;
      offset = 2;
      n = m_binsMom-1;
    } else {
      offset = 1;
      n = m_binsMom;
    }

    double p = 1.0;
    double dp;
    
    if (plambda != 0) {
      dp = (1.0 - exp(-1.0*plambda*(pmax-pthresh))) / n;
      for (int i = offset; i < n + offset; i++) {
        p -= dp;
        pEdges[i] = pthresh + log(1.0/p)/plambda;
      }
    } else {
      dp = 1.0 / n;
      for (int i = offset; i < n + offset; i++) {
        p -= dp;
        pEdges[i] = pthresh + (pmax-pthresh)*(1-p);
      }
    }
    // Should already be m_maxMom, but with machine imprecision...
    pEdges[m_binsMom] = pmax;
  }
  debug() << "P BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsMom+1; i++)
    debug() << "BIN " << i << ": " << pEdges[i] << std::endl;
  debug() << endmsg;

  //  ptEdges ...
  {
    ptEdges[0] = 0;
    int offset;
    int n;
    if (ptthresh != 0) {
      ptEdges[1] = ptthresh;
      offset = 2;
      n = m_binsPt-1;
    } else {
      offset = 1;
      n = m_binsPt;
    }

    double p = 1.0;
    double dp;
    
    if (ptlambda != 0) {
      dp = (1.0 - exp(-1.0*ptlambda*(ptmax-ptthresh))) / n;
      for (int i = offset; i < n + offset; i++) {
        p -= dp;
        ptEdges[i] = ptthresh + log(1.0/p)/ptlambda;
      }
    } else {
      dp = 1.0 / n;
      for (int i = offset; i < n + offset; i++) {
        p -= dp;
        ptEdges[i] = ptthresh + (ptmax-ptthresh)*(1-p);
      }
    }
    // Should already be ptmax, but with machine imprecision...
    ptEdges[m_binsPt] = ptmax;
  }
  debug() << "PT BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsPt+1; i++)
    debug() << "BIN " << i << ": " << ptEdges[i] << std::endl;
  debug() << endmsg;

  //  etaEdges ...
  {
    int n = m_binsEta;
    double k = m_kEta;
    double L = m_decayEta;
    double m = m_minEta;
    double M = m_maxEta;
    double p = GammaReg(k,L*m);
    double dp;

    etaEdges[0] = m;
    if (L != 0) {
      dp = (GammaReg(k,L*m)-GammaReg(k,L*M)) / n;
      for (int i = 1; i < n + 1; i++) {
        p -= dp;
        etaEdges[i] = InvGammaReg(k,p)/L;
      }
    } else {
      dp = 1.0 / n;
      for (int i = 1; i < n + 1; i++) {
        p -= dp;
        etaEdges[i] = m + (M-m)*(1-p);
      }
    }
    // Should already be m_maxEta, but with machine imprecision...
    etaEdges[m_binsEta] = M;
  }
  debug() << "ETA BIN EDGES:" << std::endl;
  for (int i = 0; i < m_binsEta+1; i++)
    debug() << "BIN " << i << ": " << etaEdges[i] << std::endl;
  debug() << endmsg;

  // CREATE HISTOGRAMS
  setHistoTopDir("Track/");

  // 1D histograms (defaults to 100 bins)
  book(multID,multTitle,-0.5,10.5,11);
  book(trackChi2ID,trackChi2Title,0,trackChi2);
  book(trackPID,trackPTitle,0,trackPMax);
  book(trackPtID,trackPtTitle,0,trackPtMax);
  book(vertexChi2ProbID,vertexChi2ProbTitle,0,1);
  book(vertexChi2ID,vertexChi2Title,0,vertexChi2Max);
  book(vtxxID,vtxxTitle,-vtxX,vtxX);
  book(vtxyID,vtxyTitle,-vtxY,vtxY);
  book(vtxzID,vtxzTitle,-vtxZ,vtxZ);
  book(vtxxerrID,vtxxerrTitle,0,0.2,50);
  book(vtxyerrID,vtxyerrTitle,0,0.2,50);
  book(vtxzerrID,vtxzerrTitle,0,2.5,50);
  book(massID,massTitle,mmin,mmax);
  book(masspullID,masspullTitle,-5,5);
  book(momID,momTitle,0,pmax);
  book(pdifID,pdifTitle,pdifmin,pdifmax);
  book(ptID,ptTitle,0,ptmax);
  book(etaID,etaTitle,m_minEta,m_maxEta);
  book(asymID,asymTitle,-1,1,m_binsMomAsym);
  book(phimattID,phimattTitle,0,M_PI);
  book(phiangleID,phiangleTitle,0,M_PI);
  book(openingangleID,openingangleTitle,0,0.3);
  book(tophalfID,tophalfTitle,mmin,mmax,m_binsMass);
  book(bothalfID,bothalfTitle,mmin,mmax,m_binsMass);

  // 2D histograms
  book2D(momH2ID,momH2Title,pEdges,massEdges);
  book2D(pdifH2ID,pdifH2Title,pdifmin,pdifmax,m_binsMomDif,mmin,mmax,m_binsMass);
  book2D(ptH2ID,ptH2Title,ptEdges,massEdges);
  book2D(etaH2ID,etaH2Title,etaEdges,massEdges);
  book2D(tyH2ID,tyH2Title,-m_maxTy,m_maxTy,m_binsTy,mmin,mmax,m_binsMass);
  book2D(asymH2ID,asymH2Title,-1,1,m_binsMomAsym,mmin,mmax,m_binsMass);
  book2D(phimattH2ID,phimattH2Title,0,M_PI,m_binsPhiMatt,mmin,mmax,m_binsMass);
  book2D(phiangleH2ID,phiangleH2Title,-M_PI,M_PI,m_binsPhiAngle,mmin,mmax,m_binsMass);

  // Profiles
  bookProfile1D(momProID,momProTitle,pEdges);
  bookProfile1D(pdifProID,pdifProTitle,pdifmin,pdifmax,m_binsMomDif);
  bookProfile1D(ptProID,ptProTitle,ptEdges);
  bookProfile1D(etaProID,etaProTitle,etaEdges);
  bookProfile1D(tyProID,tyProTitle,-m_maxTy,m_maxTy,m_binsTy);
  bookProfile1D(asymProID,asymProTitle,-1,1,m_binsMomAsym);
  bookProfile1D(phimattProID,phimattProTitle,0,M_PI,m_binsPhiMatt);
  bookProfile1D(phiangleProID,phiangleProTitle,-M_PI,M_PI,m_binsPhiAngle);
  

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
  //debug() << "particles: " << particles.size() << endmsg;

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
    double ty = p4.y() / p4.z();

    // DECAY PLANE ANGLES
    Gaudi::XYZVector norm = p3A.Cross( p3B ).Unit();
    double phiangle = std::atan2( norm.y(), norm.x());
    double phimatt  = std::acos( norm.y() );
    double openingangle = acos( p3A.Dot(p3B)/(ppos*pneg) );

    // 1D histograms
    histo(massID)->fill(mass);
    histo(masspullID)->fill(masspull);
    histo(momID)->fill(mom);
    histo(pdifID)->fill(pdif);
    histo(ptID)->fill(pt);
    histo(etaID)->fill(eta);
    histo(asymID)->fill(asym);
    histo(phimattID)->fill(phimatt);
    histo(phiangleID)->fill(phiangle);
    histo(openingangleID)->fill(openingangle);

    // 2D histograms
    histo2D(momH2ID)->fill(mom,mass);
    histo2D(pdifH2ID)->fill(pdif,mass);
    histo2D(ptH2ID)->fill(pt,mass);
    histo2D(etaH2ID)->fill(eta,mass);
    histo2D(tyH2ID)->fill(ty,mass);
    histo2D(asymH2ID)->fill(asym,mass);
    histo2D(phimattH2ID)->fill(phimatt,mass);
    histo2D(phiangleH2ID)->fill(phiangle,mass);
    
    // Profiles
    profile1D(momProID)->fill(mom,mass);
    profile1D(pdifProID)->fill(pdif,mass);
    profile1D(ptProID)->fill(pt,mass);
    profile1D(etaProID)->fill(eta,mass);
    profile1D(tyProID)->fill(ty,mass);
    profile1D(asymProID)->fill(asym,mass);
    profile1D(phimattProID)->fill(phimatt,mass);
    profile1D(phiangleProID)->fill(phiangle,mass);
    
    // Mass in top/bottom of detector
    if( p3A.y()<0 && p3B.y()<0) 
      plot( mass, bothalfID, bothalfTitle);
    else if (p3A.y()>0 && p3B.y()>0) 
      plot( mass, tophalfID, tophalfTitle);

    BOOST_FOREACH( const LHCb::State* s, states) delete s;
  }
  return StatusCode::SUCCESS;
}
