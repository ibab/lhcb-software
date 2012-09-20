
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Linker/LinkedTo.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "Event/MCHit.h"

#include "Event/RecVertex.h"

#include "Event/VPCluster.h"
#include "Event/VPLiteMeasurement.h"
// local
#include "VPV0s.h"

#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ParticleID.h"
#include "TrackKernel/TrackTraj.h"

#include "Kernel/ITrajPoca.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackInterpolator.h"
#include <iterator>

// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : VPV0s
//
// 2006-05-11 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VPV0s )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPV0s::VPV0s( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_extrapolator("TrackMasterExtrapolator",this),
    m_interpolator("TrackInterpolator",this),
    m_zmin(-100*Gaudi::Units::cm),
    m_zmax( 300*Gaudi::Units::cm),
    m_distanceCutUpstream(5*Gaudi::Units::mm),
    m_distanceCutLong(1*Gaudi::Units::mm),
    m_deltaZCut(5*Gaudi::Units::cm),
    m_deltaZSignificanceCut(5),
    m_maxChi2V0Vertex(25),     // 1 dof
    m_maxChi2PVConstraint(20), // 2 dofs
    m_minDecayLengthSignificance(5),
    m_correctBFlight(true),
    m_sigmaBFlightX(0.5*Gaudi::Units::mm), // approx RMS of flight length of B in X
    m_sigmaBFlightY(0.5*Gaudi::Units::mm), // approx RMS of flight length of B in Y
    m_sigmaBFlightZ(10*Gaudi::Units::mm),  // approx RMS of flight length of B in Z
    m_ksmasscut(40*Gaudi::Units::MeV),
    m_lambdamasscut(20*Gaudi::Units::MeV),
    m_useExtrapolator(true),
    m_excludePVTracks(true),
    m_rejectUpstreamHits(true),
    m_addExtraInfo(false)
{
  declareProperty( "NtupleName"           , m_tupleName = "V0s"    );
  declareProperty( "V0Container", m_v0ContainerName = "Rec/Track/Forward" ) ;
  declareProperty( "KsMassCut", m_ksmasscut) ;
  declareProperty( "LambdaMassCut", m_lambdamasscut) ;
  declareProperty( "UseExtrapolator", m_useExtrapolator) ;
  declareProperty( "ExcludePVTracks", m_excludePVTracks) ;
  declareProperty( "MaxNumCommonHits", m_maxNumCommonHits = 0 ) ;
  declareProperty( "MinDeltaZ", m_deltaZCut) ;
  declareProperty( "MinDecayLengthSignificance",m_minDecayLengthSignificance) ;
  declareProperty( "MaxChi2V0Vertex", m_maxChi2V0Vertex) ;
  declareProperty( "MaxChi2PVConstraint", m_maxChi2PVConstraint ) ;
  declareProperty( "RejectUpstreamHits", m_rejectUpstreamHits ) ;
  declareProperty( "AddExtraInfo", m_addExtraInfo ) ;
  declareProperty( "Interpolator", m_interpolator ) ;
  declareProperty( "Extrapolator", m_extrapolator ) ;
  declareProperty( "MaxDistanceLong", m_distanceCutLong ) ;
  declareProperty( "MaxDistanceUpstream", m_distanceCutLong ) ;

}
//=============================================================================
// Destructor
//=============================================================================
VPV0s::~VPV0s() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPV0s::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_positiontool =  tool<IVPClusterPosition>("VPClusterPosition");
  m_linkTool = tool<ILHCbIDsToMCHits>("LHCbIDsToMCHits","IDsToMCHits",this);
  m_vP = getDet<DeVP>( DeVPLocation::Default );


  m_magfieldsvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_pocatool = tool<ITrajPoca>( "TrajPoca" );
  m_vertexer = tool<ITrackVertexer>( "TrackVertexer" );
  sc = m_extrapolator.retrieve() ;
  if( sc.isFailure() ) {
    error() << "Cannot retrieve extrapolator" << endmsg ;
    return sc; 
  }
  sc = m_interpolator.retrieve() ;
  if( sc.isFailure() ) {
    error() << "Cannot retrieve interpolator" << endmsg ;
    return sc; 
  }
  

  LHCb::IParticlePropertySvc* propertysvc = 
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  m_ksProperty     = propertysvc->find( "KS0" ) ;
  m_lambdaProperty = propertysvc->find( "Lambda0" ) ;
  m_pionProperty   = propertysvc->find( "pi+" ) ;
  m_protonProperty = propertysvc->find( "p+" ) ;
  if( m_ksProperty==0 ||  m_lambdaProperty==0 || m_pionProperty==0 || m_protonProperty==0) {
    error() << "Did not find all properties." << endmsg ;
    sc = StatusCode::FAILURE ;
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPV0s::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_v0ContainerName );
  verbose()<<m_v0ContainerName<<" size: "<<tracks->size()<<endreq;
  
  LinkedTo<LHCb::MCParticle, LHCb::Track> link( evtSvc(), msgSvc(), m_v0ContainerName);

  LinkedTo<LHCb::MCParticle, LHCb::Track> link1( evtSvc(), msgSvc(), m_v0ContainerName);

  // Get the primary vertices. Locate the one that's most downstream.
  const LHCb::RecVertices* pvcontainer = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Primary ) ;
  if( pvcontainer->empty() ) 
    return Warning("No primary verticesfound. Skip V0 search",StatusCode::SUCCESS,0) ;

  double zprimary = -9999 ;
  for( LHCb::RecVertices::const_iterator ipv = pvcontainer->begin() ; 
       ipv != pvcontainer->end(); ++ipv ) 
    zprimary = std::max(zprimary, (*ipv)->position().z()) ;
  
  m_clusters = get<LHCb::VPLiteCluster::VPLiteClusters>(LHCb::VPLiteClusterLocation::Default );


  typedef std::vector<const LHCb::Track*> TrackContainer ;
  std::vector<const LHCb::MCVertex*> MothVertex ;
  TrackContainer postracks,negtracks ;

  std::vector< std::pair< int , int > > mcMotherKey ;
  
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT ; ++itT ) {
    LHCb::Track* tr = *itT;
    LHCb::MCParticle* part = link.first( tr );
    while( 0 != part ) {
      Gaudi::XYZPoint mcPosition = part->originVertex()->position();
      if( part->particleID().abspid()!=211 ){
        part = link.next();
        continue;
      }
      if( part->mother() == NULL ){
        part = link.next();
        continue;
      }
      if( part->mother()->particleID().abspid()!= 310 ){
        part = link.next();
        continue;
      }
      if ( mcPosition.z() > 700. ) {  // ignore tracks created after the Velo.
        part = link.next();
        continue;
      }
      always()<<"HERRRRRRRRRRRRRE THERE IS A V0 DAUG "<< part->particleID().pid()<<" from "<<part->mother()->key()<<endreq;
      bool haveKey = false;
      for ( int ii = 0 ; ii <(int) mcMotherKey.size(); ii++){
        if( mcMotherKey[ii].first == part->mother()->key()){
          mcMotherKey[ii].second = mcMotherKey[ii].second + 1;
          haveKey = true ;
        }
      }
      if (haveKey == false ){
        mcMotherKey.push_back( std::make_pair(part->mother()->key(),1));
      }
      
      part = link.next();
    }
  }

  //bool hasDouble = false;
  
  for ( int ii = 0 ; ii < (int)mcMotherKey.size(); ii++){
    if( mcMotherKey[ii].second == 2) {
      for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT ; ++itT ) {
        LHCb::Track* tr = *itT;
        LHCb::MCParticle* part = link.first( tr );
        while( 0 != part ) {
          Gaudi::XYZPoint mcPosition = part->originVertex()->position();
          if( part->particleID().abspid()!=211 ){
            part = link.next();
            continue;
          }
          if( part->mother() == NULL ){
            part = link.next();
            continue;
          }
          if( part->mother()->key()!=  mcMotherKey[ii].first ){
            part = link.next();
            continue;
          }
          if ( mcPosition.z() > 700. ) {  // ignore tracks created after the Velo.
            part = link.next();
            continue;
          }      
          if( tr->firstState().qOverP()>0 ){ 
            postracks.push_back(tr) ;
          }
          else{   
            negtracks.push_back(tr) ;
          }
          MothVertex.push_back(part->originVertex  () );
          part = link.next();
        }
      }
    }
  }
  
  if (negtracks.size()!=postracks.size()){
    
    always()<<"+- assymetry...."<<endreq;
    return StatusCode::SUCCESS;
  }

  if (negtracks.size() == 0)return StatusCode::SUCCESS;

  always()<<"Two tracks from one V0"<<endreq;


  // turn them into trajectories
  typedef std::vector<LHCb::TrackTraj> TrackTrajContainer ;
  TrackTrajContainer postrajs,negtrajs ;
  for( TrackContainer::const_iterator ipos = postracks.begin() ;
       ipos != postracks.end(); ++ipos) 
    postrajs.push_back(LHCb::TrackTraj(**ipos,m_magfieldsvc)) ;
  for( TrackContainer::const_iterator ineg = negtracks.begin() ;
       ineg != negtracks.end(); ++ineg) 
    negtrajs.push_back(LHCb::TrackTraj(**ineg,m_magfieldsvc)) ;
  
  const double pimass = m_pionProperty->mass() ;
  const double ksmass  = m_ksProperty->mass() ;
  
  for (int jj = 0 ; jj <(int) postracks.size() ; jj ++){
    const LHCb::Track *ipos = postracks[jj];
    const LHCb::Track *ineg = negtracks[jj];
    const LHCb::MCVertex* theMothV =  MothVertex[jj];

    if( (ipos)->nCommonLhcbIDs(*ineg) <= m_maxNumCommonHits ) {
        

        always()<<"inside loop"<<endreq;
        const LHCb::TrackTraj& postraj = postrajs[jj] ;
        const LHCb::TrackTraj& negtraj = negtrajs[jj] ;
	
        // seed the z position with the intersection in the yz-plane
        double mupos = postraj.beginRange() + 10*Gaudi::Units::mm ;
        double muneg = negtraj.beginRange() + 10*Gaudi::Units::mm ;
        double ypos  = postraj.position(mupos).y() ;
        double yneg  = negtraj.position(muneg).y() ;
        double typos = postraj.direction(mupos).y() ;
        double tyneg = negtraj.direction(muneg).y() ;
        double z_yz = - (ypos - yneg + muneg*tyneg - mupos*typos) / ( typos - tyneg ) ;
        mupos = muneg = z_yz ;
        
        always()<<"Before minimize"<<endreq;
            
        // Calls pocatool
        Gaudi::XYZVector deltaX; 
        StatusCode sc = m_pocatool->minimize( postraj,mupos,negtraj,muneg,deltaX,0.001*Gaudi::Units::mm );
        if( sc.isSuccess() ) {
          double distance = deltaX.R() ;
          double z = 0.5*(mupos+muneg) ;
            always()<<"Before cut distance and Z position"<<endreq;
            
          // make the cut on the distance and the z-position
          //if( (distance < m_distanceCutLong ||
          ////     (distance < m_distanceCutUpstream && !isVeloCombi)) &&
          //    zprimary + m_deltaZCut < z && z< m_zmax ) {
            //if ()  
            always()<<"Passed distance and Z position"<<endreq;
            
            
            // now make an invariant mass cut
            Gaudi::XYZVector mompos = postraj.momentum(mupos) ;
            Gaudi::XYZVector momneg = negtraj.momentum(muneg) ;
            Gaudi::LorentzVector p4pos(mompos.X(),mompos.Y(),mompos.Z(),
                                       std::sqrt(mompos.Mag2()+pimass*pimass)) ;
            Gaudi::LorentzVector p4neg(momneg.X(),momneg.y(),momneg.Z(),
                                       std::sqrt(momneg.Mag2()+pimass*pimass)) ;
            double pipimass = (p4pos+p4neg).M() ;
            bool iskscandidate = fabs(pipimass - ksmass) < m_ksmasscut ;
            always()<<"Before Ks"<<endreq;
            
            if( iskscandidate ) {

              always()<<"Is ks"<<endreq;
              // Determine the states passed to the vertexer. At this
              // stage combinatorics is small enough that we can
              // eventually use a real extrapolator.
              LHCb::State posstate,negstate ;
              if( m_useExtrapolator ) {
                // If we can, we use interpolators, because those can do
                // this correctly even if we are between nodes. (The
                // extrapolators are guaranteed to be wrong half the
                // time!)
                StatusCode sc ;
                if( (ipos)->nodes().empty() ) sc = m_extrapolator->propagate( *ipos, z, posstate ) ;
                else                           sc = m_interpolator->interpolate( *ipos, z, posstate ) ;
                if(!sc.isSuccess() ) {
                  Warning("Extrapolation failed. Rely on trajectory interpolation.",StatusCode::SUCCESS,0).ignore() ;
                  posstate = postraj.state(z) ;
                }
                if( (ineg)->nodes().empty() ) sc = m_extrapolator->propagate( *ineg, z, negstate ) ;
                else                           sc = m_interpolator->interpolate( *ineg, z, negstate ) ;
                if(!sc.isSuccess() ) {
                  Warning("Extrapolation failed. Rely on trajectory interpolation.",StatusCode::SUCCESS,0).ignore() ;
                  negstate = negtraj.state(z) ;
                }
              } else {
                posstate = postraj.state(z) ;
                negstate = negtraj.state(z) ;
              }
              
              // finally, create the vertex and cut on the significance
              
              always()<<"before fit"<<endreq;
              LHCb::TwoProngVertex* vertex = m_vertexer->fit(posstate,negstate ) ;
              if( vertex == 0 ) {
                Warning("TrackVertexer Failure", StatusCode::SUCCESS, 0 ).ignore() ;
              } 
              else if( vertex->chi2() < m_maxChi2V0Vertex ) {
                always()<<"after fit"<<endreq;
                // one last chec: test that there are no hits upstream of the vertex on either track
                const LHCb::State* mstatepos = (ipos)->stateAt(LHCb::State::FirstMeasurement ) ;
                const LHCb::State* mstateneg = (ineg)->stateAt(LHCb::State::FirstMeasurement ) ;
                bool hasUpstreamHits = false ;
                if( m_rejectUpstreamHits || m_addExtraInfo ) {
                  hasUpstreamHits =
                    (mstatepos && mstatepos->z() < vertex->position().z() ) ||
                    (mstateneg && mstateneg->z() < vertex->position().z() ) ;
                }
                
                if( m_addExtraInfo ) {
                  vertex->addInfo(100,distance) ;
                  vertex->addInfo(101,z-zprimary) ;
                  vertex->addInfo(102,pipimass) ;
                  vertex->addInfo(105,double(hasUpstreamHits)) ;
                }
                vertex->addToTracks(ipos) ;
                vertex->addToTracks(ineg) ;
                if(iskscandidate) {
                  LHCb::ParticleID pid = LHCb::ParticleID(m_ksProperty->pdgID());
                  vertex->addPID( pid ) ;
                }
                Tuple tPoint = nTuple( "V0Tuple" );
                tPoint->column( "mc_vx"   ,theMothV->position().x() );
                tPoint->column( "mc_vy"   , theMothV->position().y() );
                tPoint->column( "mc_vz"   , theMothV->position().z() );
                tPoint->column( "vx"   , vertex->position().x() );
                tPoint->column( "vy"   , vertex->position().y() );
                tPoint->column( "vz"   , vertex->position().z() );
                tPoint->column( "vchi2"   , vertex->chi2() );
                tPoint->column( "vchi2ndof"   , vertex->chi2PerDoF() );
                tPoint->column( "tr1chi2n"   , vertex->trackA()->chi2PerDoF() );
                tPoint->column( "tr2chi2n"   , vertex->trackB()->chi2PerDoF() );
                tPoint->column( "evy"   , vertex->covMatrix  () (1,1) );
                tPoint->column( "evx"   , vertex->covMatrix () (0,0) );
                tPoint->column( "evz"   , vertex->covMatrix  () (2,2) );
                tPoint->column( "eta",(p4pos+p4neg).eta());
                
                tPoint->column( "mass",pipimass);
                tPoint->write();
                always()<<"SSSSSSSSSSSSAAAAAAAAAAAAAAAAAVVVVVVVVVVVVVVEEEEEEEEEEEDDDDDDDDDDDDD"<<endreq;
              }
              else {
                delete vertex ;
              }
          }
        } 
        else {
          Warning("TrajPoca Failure",StatusCode::SUCCESS,0).ignore() ;
        }
      }
  
  }
  



//       Gaudi::XYZVector mcSlopes  = part->momentum().Vect();
//       mcSlopes /= mcSlopes.z();
//       double mcMomentum = part->momentum().P();
//       double mcTransMomentum = part->momentum().Pt();
//       int charge = part->particleID().threeCharge()/3;
//       mcMomentum *= charge;


//       Tuple tPoint = nTuple( m_tupleName );

      
//       tPoint->column( "x"   , positionForIp.x() );
//       tPoint->column( "y"   , positionForIp.y() );
//       tPoint->column( "z"   , positionForIp.z() );
//       tPoint->column( "tx"  , slopes.x() );
//       tPoint->column( "ty"  , slopes.y() );
//       tPoint->column( "p"   , momentum );
//       tPoint->column( "dp"  , momentum-mcMomentum );
//       tPoint->column( "pt"   , transmomentum );
//       tPoint->column( "chi2ndof" , tr->chi2PerDoF() );
//       tPoint->column( "nhits" , tr->nLHCbIDs () );
//       tPoint->column( "mcpt" ,  mcTransMomentum);
//       tPoint->column( "ip"   , ip );
//       tPoint->column( "Extrapolation_dx"  , position.x() - mcPosition.x() );
//       tPoint->column( "Extrapolation_dy"  , position.y() - mcPosition.y() );
//       tPoint->column( "firstMeas"  ,firstposition.z()- mcPosition.z() );
//       tPoint->column( "Extrapolation_dtx" , slopes.x() - mcSlopes.x() );
//       tPoint->column( "Extrapolation_dty" , slopes.y() - mcSlopes.y() );
//       tPoint->column( "mctx" , mcSlopes.x() );
//       tPoint->column( "mcty" , mcSlopes.y() );
//       tPoint->write();
//       part = link.next();
//     }


  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VPV0s::finalize() {

  debug() << "==> Finalize" << endmsg;

  m_extrapolator.release().ignore() ;
  m_interpolator.release().ignore() ;
  
  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
//=============================================================================
// Check how well this V0 candidate fits to the PV
//=============================================================================

inline Gaudi::Vector3 transform( const Gaudi::XYZVector& vec)
{
  return Gaudi::Vector3(vec.X(),vec.Y(),vec.Z()) ;
}

void
VPV0s::constrainToVertex(const Gaudi::XYZPoint& pos,
				 const Gaudi::LorentzVector& p4,
				 const Gaudi::SymMatrix7x7& cov7,
				 const LHCb::RecVertex& pv,
				 double& chi2,
				 double& decaylength,
				 double& decaylengtherr) const
{
  // This calculation is basically a 1-iteration beamspot fit. The
  // constraint is 
  //
  //    r = x - lambda p/|p| - xbs
  // 
  // where x and p are the position of the decay vertex of the
  // candidate and its momentum, lambda is the decaylength and xbs
  // the position of the beamspot. The covariance in the constraint
  // is
  //
  //    V = Vbs + Vxx - a * Vxp - a Vxp^T + a^2 * Vpp
  //
  // where a=lambda/|p|^2. It needs an initial estimate for the
  // flightlength, for which we simply take the projection of deltaX
  // on the direction. We now minimize  the chisquare contribution 
  // 
  //     chi^2 = r^T V^{-1} r
  //
  // for lambda.
  
  Gaudi::Vector3 dx = transform(pos - pv.position()) ;
  Gaudi::XYZVector p3 = p4.Vect() ;
  double p3mag = p3.R() ; 
  Gaudi::Vector3 dir = transform(p3.Unit()) ;
  
  Gaudi::SymMatrix3x3 W = pv.covMatrix() ; // we'll repace this with a constant error that contains B motion
  
  if(m_correctBFlight) {
    // For determining whether this candidate is compatible with the
    // PV, we want to take into account the the Ks may come from a
    // B. So, we need to add something to the PV vertex. However, we
    // want to count the B flight length in the decay length, so we
    // don't want to add the B flight length to the PV error in z. The
    // trick is to remove the contribution of B-flight along the V0
    // direction.

    static Gaudi::SymMatrix3x3 covBFlight ;
    covBFlight(0,0) = m_sigmaBFlightX*m_sigmaBFlightX  ;
    covBFlight(1,1) = m_sigmaBFlightY*m_sigmaBFlightY  ;
    covBFlight(2,2) = m_sigmaBFlightZ*m_sigmaBFlightZ  ;

    // now project out the part perpendicular to the direction. 
    //   W +=  (1-P) * covB * (1-P)
    // where P is the projection matrix
    //   P_ij = dir_i * dir_j
    // 
    // I am sure that there is something left to optimize here ...
    Gaudi::SymMatrix3x3 UnitMinusP ;
    UnitMinusP(0,0) = UnitMinusP(1,1) = UnitMinusP(2,2) = 1 ;
    for( size_t irow = 0; irow<3; ++irow )
      for( size_t jrow = 0; jrow<=irow; ++jrow )
     	UnitMinusP(irow,jrow) -= dir(irow) * dir(jrow) ;
    
    // here we could use that W is diagonal. that saves a lot of time
    //W +=  ROOT::Math::Similarity(covBFlight,UnitMinusP) ;
    for( size_t irow = 0; irow<3; ++irow ) 
      for( size_t jrow = 0; jrow<=irow; ++jrow )
	for( size_t krow = 0; krow<3; ++krow )
	  W(irow,jrow) += UnitMinusP(irow,krow) * covBFlight(krow,krow) *  UnitMinusP(krow,jrow) ;
    
  }

  //double a = (ROOT::Math::Transpose(dir)*dx)/p3mag  ;
  double a = ROOT::Math::Dot(dir,dx)/p3mag  ;
  for(size_t row=0; row<3; ++row)
    for(size_t col=0; col<=row; ++col)
      W(row,col) += 
	cov7(row,col)
	+ a*a * cov7(row+3,col+3)
	- a * (cov7(row+3,col) + cov7(col+3,row)) ;
  
  int OK = W.Invert() ;
  if( !OK ) error() << "inversion error in constrainToVertex" << endmsg ;
  
  double halfdChi2dLam2 = ROOT::Math::Similarity(W,dir) ;
  decaylength    = ROOT::Math::Dot(dir,W*dx)/halfdChi2dLam2 ;
  decaylengtherr = std::sqrt(1/halfdChi2dLam2) ;
  
  Gaudi::Vector3 res = dx - decaylength * dir ;
  
  chi2 = ROOT::Math::Similarity(W,res) ;
}

bool
VPV0s::hasV0Topology(LHCb::TwoProngVertex& vertex,
			     const LHCb::RecVertices& pvs) const
{
  // returns true if V0 candidate accepted. we veto two types of background:
  // * V0 candidates that do not point to any PV ( chi2 > m_maxChi2PVConstraint)
  // * V0 candidates that point to a PV but with too small decay length

  bool isFromPV = false ;
  double chi2(0),decaylength(0),decaylengtherr(1) ;

  if(pvs.size()>0) {
    // first find the best fitting primary vertex
    const Gaudi::XYZPoint& pos = vertex.position() ;
    Gaudi::SymMatrix7x7 cov7x7  = vertex.covMatrix7x7(0,0) ;
    Gaudi::LorentzVector p4     = vertex.momentum(0,0) ;
    for( LHCb::RecVertices::const_iterator ipv = pvs.begin() ;
	 ipv != pvs.end(); ++ipv ) {
      double tmpchi2, tmpdecaylength, tmpdecaylengtherr ;
      constrainToVertex(pos,p4,cov7x7,**ipv,tmpchi2, tmpdecaylength, tmpdecaylengtherr) ;
      // is this the best fitting PV?
      if( ipv == pvs.begin() || tmpchi2 < chi2 ) {
	chi2 = tmpchi2 ;
	decaylength = tmpdecaylength ;
	decaylengtherr = tmpdecaylengtherr ;
      }
      // veto this candidate if it is compatible with this PV
      isFromPV = isFromPV ||
	( tmpchi2 < m_maxChi2PVConstraint &&
	  tmpdecaylength / tmpdecaylengtherr < m_minDecayLengthSignificance ) ;
    }
  }
  if(m_addExtraInfo) {
    vertex.addInfo( 1000, chi2 ) ;
    vertex.addInfo( 1001, decaylength/decaylengtherr ) ;
  }
  // select this one if it is not compatible with any PV, but still points to a PV:  
  return !isFromPV && chi2 < m_maxChi2PVConstraint ;
}
