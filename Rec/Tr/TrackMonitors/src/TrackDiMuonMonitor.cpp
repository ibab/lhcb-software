#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/TwoProngVertex.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "gsl/gsl_cdf.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "TrackKernel/TrackTraj.h"
#include "Kernel/ITrajPoca.h"
#include "TrackInterfaces/ITrackVertexer.h"

class TrackDiMuonMonitor : public GaudiHistoAlg 
{
public:

   /** Standard construtor */
  TrackDiMuonMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackDiMuonMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  bool accept(const LHCb::TwoProngVertex& vertex) ;
  bool overlap( const LHCb::Track& rhs, const LHCb::Track& lhs) const ;
private:
  
  std::string m_trackInputLocation; // Input Tracks container location
  std::string m_dimuonOutputLocation ;
  IMagneticFieldSvc* m_magfieldsvc ;
  ITrajPoca* m_pocatool ;
  ITrackVertexer* m_vertexer ;
  double m_maxDistance ;
  double m_minMass ;
  double m_maxMass ;
  double m_minJPsiMass ;
  double m_maxJPsiMass ;
  double m_maxChi2TwoProngVertex ;
  double m_muonmass ;  

} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackDiMuonMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackDiMuonMonitor::TrackDiMuonMonitor( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_maxDistance( 5*Gaudi::Units::mm),
    m_minMass(2.6*Gaudi::Units::GeV),
    m_maxMass(4.0*Gaudi::Units::GeV),
    m_minJPsiMass(3.065*Gaudi::Units::GeV),
    m_maxJPsiMass(3.125*Gaudi::Units::GeV),
    m_maxChi2TwoProngVertex(25),
    m_muonmass(0.)
{
  declareProperty( "TrackLocation", m_trackInputLocation = LHCb::TrackLocation::Muon );
  declareProperty( "DiMuonLocation", m_dimuonOutputLocation = "Rec/Vertex/DiMuon" ) ;
  declareProperty( "MaxChi2TwoProngVertex", m_maxChi2TwoProngVertex) ;
  declareProperty( "MinMass", m_minMass ) ;
  declareProperty( "MaxMass", m_maxMass ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackDiMuonMonitor::~TrackDiMuonMonitor() 
{
}

StatusCode TrackDiMuonMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_magfieldsvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_pocatool = tool<ITrajPoca>( "TrajPoca" );
  m_vertexer = tool<ITrackVertexer>( "TrackVertexer" );
  LHCb::IParticlePropertySvc* propertysvc = 
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  if( propertysvc ) {
    const LHCb::ParticleProperty* muon = propertysvc->find( "mu+" ) ;
    m_muonmass   = muon ? muon->mass() : 0 ;
  } else {
    sc = StatusCode::FAILURE ;
  }
  return sc ;
}
  
StatusCode TrackDiMuonMonitor::execute()
{ 
  setHistoTopDir("Track/") ;

  // Get the Tracks
  const LHCb::Tracks* muontracks = getIfExists<LHCb::Tracks>( m_trackInputLocation );

  // Very ugly: take care this doesn't crash if muonID is not running.
  if( NULL == muontracks ) 
    return Warning("No muon tracks at given location",StatusCode::SUCCESS,0) ;

  // Sort them by charge, make some cuts
  typedef std::vector<const LHCb::Track*> TrackContainer ;
  TrackContainer postracks,negtracks ;
  for( LHCb::Tracks::const_iterator it = muontracks->begin() ;
       it != muontracks->end(); ++it) {
    const LHCb::Track* track = *it ;
    // require tracks with T and (TT or Velo)
    bool accepted = track->hasT() && (track->hasVelo()||track->hasTT()) ;
    if(!accepted && track->ancestors().size()>0) {
      // muon track: take the first ancestor
      track = track->ancestors().front() ;
    } else {
      track = 0 ;
    }
    if(track) {
      if( track->firstState().qOverP()>0 ) postracks.push_back(track) ;
      else                                 negtracks.push_back(track) ;
    }
  }
  
  // turn them into trajectories
  typedef std::vector<LHCb::TrackTraj> TrackTrajContainer ;
  TrackTrajContainer postrajs,negtrajs ;
  for( TrackContainer::const_iterator ipos = postracks.begin() ;
       ipos != postracks.end(); ++ipos) 
    postrajs.push_back(LHCb::TrackTraj(**ipos,m_magfieldsvc)) ;
  for( TrackContainer::const_iterator ineg = negtracks.begin() ;
       ineg != negtracks.end(); ++ineg) 
    negtrajs.push_back(LHCb::TrackTraj(**ineg,m_magfieldsvc)) ;
  
  counter("numcombinations") += postracks.size()*negtracks.size() ;
  counter("numpostracks") += postracks.size() ;
  counter("numnegtracks") += negtracks.size() ;
  
  // Create the output container 
  typedef KeyedContainer<LHCb::TwoProngVertex, Containers::HashMap> TwoProngVertices;
  TwoProngVertices* dimuoncontainer = new TwoProngVertices() ;
  
  for( TrackContainer::iterator ipos = postracks.begin() ;
       ipos != postracks.end(); ++ipos) 
    for( TrackContainer::iterator ineg = negtracks.begin() ;
         ineg != negtracks.end(); ++ineg) 
      if( !overlap(**ipos,**ineg) ) {
	const LHCb::TrackTraj& postraj = postrajs[std::distance(postracks.begin(),ipos)] ;
	const LHCb::TrackTraj& negtraj = negtrajs[std::distance(negtracks.begin(),ineg)] ;
	
	double z_seed = postraj.beginRange() + 1*Gaudi::Units::mm ;
	double mupos(z_seed),muneg(z_seed) ;
	
	// Calls pocatool
	Gaudi::XYZVector deltaX; 
	StatusCode sc = m_pocatool->minimize( postraj,mupos,negtraj,muneg,deltaX,0.001*Gaudi::Units::mm );

	if( sc.isSuccess() ) {
	  double distance = deltaX.R() ;
	  double z = 0.5*(mupos+muneg) ;
	  if( distance < m_maxDistance ) {
	    // now make an invariant mass cut
	    Gaudi::XYZVector mompos = postraj.momentum(mupos) ;
	    Gaudi::XYZVector momneg = negtraj.momentum(muneg) ;
	    double mumass2 = m_muonmass*m_muonmass ;
	    Gaudi::LorentzVector p4pos(mompos.X(),mompos.Y(),mompos.Z(),
				       std::sqrt(mompos.Mag2()+mumass2)) ;
	    Gaudi::LorentzVector p4neg(momneg.X(),momneg.y(),momneg.Z(),
				       std::sqrt(momneg.Mag2()+mumass2)) ;
	    double dimuonmass = (p4pos+p4neg).M() ;

	    if( m_minMass < dimuonmass && dimuonmass < m_maxMass ) {
	      
	      // finally, create the vertex and cut on the chisquare
	      LHCb::State posstate = postraj.state(z) ;
	      LHCb::State negstate = negtraj.state(z) ;
	      LHCb::TwoProngVertex* vertex = m_vertexer->fit(posstate,negstate ) ;
	      vertex->addToTracks(*ipos) ;
	      vertex->addToTracks(*ineg) ;

	      if( vertex && accept( *vertex ) ) {
		dimuoncontainer->add( vertex ) ;
	      } else {
		delete vertex ;
	      }
	    }
	  }
	} else {
	  Warning("TrajPoca Failure",StatusCode::SUCCESS,0).ignore() ;
	}
      }

  counter("numselected") += dimuoncontainer->size() ;
  
  plot(dimuoncontainer->size(),"multiplicity","J/psi candidate multiplicity",-0.5,20.5,21) ;

  if( !m_dimuonOutputLocation.empty() ) 
    put(dimuoncontainer, m_dimuonOutputLocation) ;
  else
    delete dimuoncontainer ;

  return StatusCode::SUCCESS;
}
	      
//=============================================================================
// Check of two tracks do not have two much overlap to be combined as TwoProng
//=============================================================================

bool TrackDiMuonMonitor::overlap( const LHCb::Track& trackA,
				  const LHCb::Track& trackB ) const
{
  // for now, just look at common ancestors
  bool rc = false ;
  for( SmartRefVector<LHCb::Track>::const_iterator iancA = trackA.ancestors().begin() ;
       iancA != trackA.ancestors().end() && !rc; ++iancA) 
    for( SmartRefVector<LHCb::Track>::const_iterator iancB = trackB.ancestors().begin() ;
	 iancB != trackB.ancestors().end() && !rc; ++iancB) 
      rc = *iancA == *iancB ;
  return rc ;
}


bool TrackDiMuonMonitor::accept( const LHCb::TwoProngVertex& vertex )
{
  bool rc(false) ;
  static double chi2max =  gsl_cdf_chisq_Qinv (1e-6,1); 
  double chi2 = vertex.chi2() ;
  double chi2prob = chi2<chi2max ?  gsl_cdf_chisq_Q(chi2,1) : 0 ;
  plot( chi2prob, "chi2prob","chi2prob", 0,1 ) ;
  
  if( vertex.chi2() < m_maxChi2TwoProngVertex ) {
    Gaudi::LorentzVector p4A = vertex.p4A( m_muonmass ) ;
    Gaudi::LorentzVector p4B = vertex.p4B( m_muonmass ) ;
    Gaudi::LorentzVector p4 = p4A + p4B ;
    double mass = p4.M() ;
    plot( mass, "mass", "dimuon mass", m_minMass, m_maxMass ) ;
    
    if( m_minJPsiMass < mass && mass < m_maxJPsiMass ) {
      
      plot( vertex.position().x(), "vxJPsi", "J/psi candidate vertex x", -2,2) ;
      plot( vertex.position().y(), "vyJPsi", "J/psi candidate vertex y", -2,2) ;
      plot( vertex.position().z(), "vzJPsi", "J/psi candidate vertex z", -200,200) ;
      plot( chi2prob, "chi2probJPsi","J/psi candidate chi2prob", 0,1 ) ;
      plot( mass, "massJPsi", "J/psi candidate mass", 
	    3.05*Gaudi::Units::GeV, 3.15*Gaudi::Units::GeV ) ;
      const LHCb::Track* postrack = vertex.trackA() ;
      const LHCb::Track* negtrack = vertex.trackB() ;
      if( postrack->firstState().qOverP() < 0 ) std::swap( postrack,negtrack) ;
      double ppos = postrack->firstState().momentum().R() ;
      double pneg = negtrack->firstState().momentum().R() ;
      double pdif = ppos - pneg ;
      profile1D( pdif, mass, "massVersusMomDif", "dimuon mass versus p_{pos} - p_{neg}",
		 -50*Gaudi::Units::GeV, 50*Gaudi::Units::GeV,20,"",
		 3.065*Gaudi::Units::GeV, 3.125*Gaudi::Units::GeV ) ;
      plot( p4.P() , "momJPsi", "JPsi candidate momentum", 0, 100*Gaudi::Units::GeV ) ;
      plot( pdif , "momdifJPsi", "p_{pos} - p_{neg} for JPsi candidates",-50*Gaudi::Units::GeV, 50*Gaudi::Units::GeV) ;
      plot( p4.Pt(), "ptJPsi", "JPsi candidate Pt", 0, 10*Gaudi::Units::GeV ) ;
      plot( p4.Eta(), "etaJPsi", "JPsi candidate eta", 2, 5 ) ;
      
      rc = true ;
    }
  }
  return rc ;
}
