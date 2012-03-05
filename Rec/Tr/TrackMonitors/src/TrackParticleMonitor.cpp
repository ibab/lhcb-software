#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "Event/TwoProngVertex.h"
#include "Event/FitNode.h"
#include "Event/TrackFitResult.h"
#include "Event/ChiSquare.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "TrackKernel/TrackTraj.h"
#include "Event/Particle.h"
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackKernel/TrackStateVertex.h"

#include <boost/foreach.hpp>

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
  ILHCbMagnetSvc* m_magfieldsvc ;
  ToolHandle<ITrackStateProvider> m_stateprovider ;
  double m_minMass ;
  double m_maxMass ;
} ;

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
    m_minMass(0*Gaudi::Units::GeV),
    m_maxMass(120.0*Gaudi::Units::GeV)
{
  declareProperty( "InputLocation", m_inputLocation = "" ) ;
  declareProperty( "MinMass", m_minMass ) ;
  declareProperty( "MaxMass", m_maxMass ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackParticleMonitor::~TrackParticleMonitor() 
{
}

StatusCode TrackParticleMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_magfieldsvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
  // LHCb::IParticlePropertySvc* propertysvc = 
  //     svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  m_stateprovider.retrieve().ignore() ;
  return sc ;
}

StatusCode TrackParticleMonitor::finalize()
{
  m_stateprovider.release().ignore() ;
  return  GaudiHistoAlg::finalize() ;
}

  
namespace {
  void addTracks( const LHCb::Particle& p,
		  std::vector<const LHCb::Track*>& tracks,
		  std::vector<double>& masshypos)
  {
    if( p.proto() && p.proto()->track() ) {
      tracks.push_back(p.proto()->track() ) ;
      masshypos.push_back(p.momentum().M()) ;
    } else {
      BOOST_FOREACH( const LHCb::Particle* dau,
		     p.daughters() )
	addTracks( *dau, tracks, masshypos ) ;
    }
  }
}

StatusCode TrackParticleMonitor::execute()
{ 
  setHistoTopDir("Track/") ;

  LHCb::Particle::Range particles  = get<LHCb::Particle::Range>(m_inputLocation) ;
  plot(particles.size(),"multiplicity","multiplicity",-0.5,10.5,11) ;
  //debug() << "particles: " << particles.size() << endmsg ;
  
  double trackPMax(20*m_maxMass), trackPtMax(3*m_maxMass), 
    vtxX(2.), vtxY(2.), vtxZ(200.),
    momMax(50*m_maxMass), ptMax(3*m_maxMass),
    trackChi2(10), vertexChi2(20) ;

  if (m_maxMass < 2.3*Gaudi::Units::GeV) {
    trackPMax= 200*Gaudi::Units::GeV;
    trackPtMax= 20*Gaudi::Units::GeV;
    vtxX = 2.5; vtxY = 2.; vtxZ = 200.;
    momMax = 200*Gaudi::Units::GeV;
    ptMax = 30*Gaudi::Units::GeV;
    trackChi2 = 5;
    vertexChi2 = 5.;
  }

  BOOST_FOREACH( const LHCb::Particle* particle, particles) {
    
    std::vector< const LHCb::Track* > tracks ;
    std::vector< double > masshypos ;
    addTracks(*particle, tracks, masshypos) ;
    
    std::vector< const LHCb::State* > states ;
    double z = particle->referencePoint().z() ;
    //double z= particle->endVertex()->position().z() ;

    BOOST_FOREACH( const LHCb::Track* track, tracks) {
      plot( track->chi2PerDoF(), "trackchi2","track chi2/dof", 0, trackChi2 ) ;
      plot( track->p()/Gaudi::Units::GeV, "trackP","track momentum", 0, trackPMax/Gaudi::Units::GeV ) ;
      plot( track->pt()/Gaudi::Units::GeV, "trackPt","track pT", 0, trackPtMax/Gaudi::Units::GeV ) ;
      
      LHCb::State* state = new LHCb::State() ;
      m_stateprovider->stateFromTrajectory(*state,*track,z).ignore() ;
      states.push_back(state) ;
    }
        
    LHCb::TrackStateVertex vertex( states ) ;
    vertex.fit() ;
    double chi2 = vertex.chi2() ;
    double chi2prob = LHCb::ChiSquare(chi2,vertex.nDoF()).prob() ;
    plot( chi2prob, "chi2prob","vertex chi2prob", 0,1 ) ;
    plot( chi2, "chi2","vertex chi2", 0, vertexChi2 ) ;
    plot( vertex.position().x(), "vtxx", "vertex x", -vtxX,vtxX) ;
    plot( vertex.position().y(), "vtxy", "vertex y", -vtxY,vtxY) ;
    plot( vertex.position().z(), "vtxz", "vertex z", -vtxZ,vtxZ) ;
    
    // assume that there is at least two tracks
    LHCb::State stateA = vertex.state(0) ;
    LHCb::State stateB = vertex.state(1) ;
    
    if( (m_magfieldsvc->isDown() && stateA.qOverP() > 0 ) ||
	(!m_magfieldsvc->isDown() && stateA.qOverP() < 0 ) )
      std::swap( stateA, stateB ) ;
    
    Gaudi::XYZVector p3A = stateA.momentum()/Gaudi::Units::GeV ;
    Gaudi::XYZVector p3B = stateB.momentum()/Gaudi::Units::GeV ;
    
    Gaudi::LorentzVector p4 = vertex.p4(masshypos)/Gaudi::Units::GeV ;
    double mass = p4.M() ;
    double mom  = p4.P() ;

    double ppos = p3A.R() ;
    double pneg = p3B.R() ;
    double pdif = ppos - pneg ;
    //    double costheta = pdif / (ppos + pneg) ;
    double asym = pdif / (ppos + pneg) ;
    
    Gaudi::XYZVector norm = p3A.Cross( p3B ).Unit() ;
    double phiangle = std::atan2( norm.y(), norm.x()) ;
    double phimatt  = std::acos( norm.y() ) ;

    const double mmin = m_minMass/Gaudi::Units::GeV ;
    const double mmax = m_maxMass/Gaudi::Units::GeV ;
    
    plot( mass, "mass", "mass", mmin, mmax) ;
    plot( mom, "momentum", "momentum [GeV]", 0, momMax/Gaudi::Units::GeV ) ;
    plot( asym,"momentum asymmetry",-1,1,20) ;
    plot( pdif, "momdif", "p_{pos} - p_{neg}",-momMax/Gaudi::Units::GeV/2, momMax/Gaudi::Units::GeV/2) ;
    plot( p4.Pt(), "pt", "Pt", 0, ptMax/Gaudi::Units::GeV ) ;
    plot( p4.Eta(), "eta", "eta", 2, 7 ) ;
    
    plot2D( phimatt, mass, "massVersusPhiMattH2", "mass versus Matt's phi", 0,M_PI,mmin,mmax,20,50) ;
    plot2D( phiangle, mass, "massVersusPhiH2", "mass versus phi", -M_PI,M_PI,mmin,mmax,12,50) ;
    plot2D( pdif, mass, "massVersusMomDifH2", "mass versus p_{A} - p_{B}",-momMax/2,momMax/2,mmin,mmax,20,50) ;
    plot2D( asym, mass, "massVersusMomAsymH2", "mass versus momentum asymmetry", -1,1,mmin,mmax,20,50) ;
    plot2D( mom, mass, "massVersusMomH2", "mass versus momentum",0,momMax,mmin,mmax,20,50) ;
    profile1D( phimatt, mass, "massVersusPhiMatt", "mass versus Matt's phi", 0,M_PI,20) ;
    profile1D( phiangle, mass, "massVersusPhi", "dimuon mass versus phi",-M_PI,M_PI,12) ;
    profile1D( pdif, mass, "massVersusMomDif", "mass versus p_{A} - p_{B}",-momMax/2,momMax/2,20) ;
    profile1D( asym, mass, "massVersusMomAsym", "mass versus momentum asymmetry", -1,1,20) ;
    profile1D( mom, mass, "massVersusMom", "mass versus momentum",0,momMax,20) ;
    profile1D( p4.y() / p4.z(), mass, "massVersusTy", "mass versus ty",-0.2,0.2,40) ;

    if( p3A.y()<0 && p3B.y()<0) 
      plot( mass, "massNegativeY", "mass for y<0",mmin,mmax,50) ;
    else if (p3A.y()>0 && p3B.y()>0) 
      plot( mass, "massPositiveY", "mass for y>0",mmin,mmax,50) ;

    BOOST_FOREACH( const LHCb::State* s, states) delete s ;
  }
  return StatusCode::SUCCESS ;
}

