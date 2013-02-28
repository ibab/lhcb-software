// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

//get the Header of the event
#include "Event/RecHeader.h"

//Use ODIN
#include "Event/ODIN.h"

//calorimeter
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"

// local
#include "RecVertices2Particles.h"

#include "DaVinciUtils/Functions.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertices2Particles
//
// 2009-april-14 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecVertices2Particles )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  RecVertices2Particles::RecVertices2Particles( const std::string& name,
                                                ISvcLocator* pSvcLocator)
    : DaVinciHistoAlgorithm ( name , pSvcLocator )
    , pi(3.1415926)
    , m_FromBL(false)
    , m_FromUpPV(false)
    , m_FromBeamSpot(false)
    , m_GeoInit(false)
    , m_PVnbtrks(10)
    , m_pt(400.)
{
  declareProperty("UsePartFromTES", m_UsePartFromTES = false );
  declareProperty("Prey", m_Prey = "~chi_10" );
  declareProperty("PreyMinMass", m_PreyMinMass = 0.0 );
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*Gaudi::Units::TeV );
  declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
  declareProperty("RMin", m_RMin = 0.3*Gaudi::Units::mm );
  declareProperty("RMax", m_RMax = 50.*Gaudi::Units::m );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks
  declareProperty("TrackMaxChi2oNDOF", m_TChi2 = 5. );
  declareProperty("RecVerticesLocation", m_RVLocation );
  declareProperty("IsolationDistance", m_Dist = 0.0*mm  );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
  declareProperty("RCutMethod", m_RCut = "FromUpstreamPV" );
  declareProperty("BeamLineLocation",
                  m_BLLoc = "/Event/BeamLine");
  declareProperty("PVnbtrks", m_PVnbtrks = 10 );
}
//=============================================================================
// Destructor
//=============================================================================
RecVertices2Particles::~RecVertices2Particles() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecVertices2Particles::initialize() {

  StatusCode sc = DaVinciHistoAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize the RecVertices2Particles algorithm" << endmsg;
  //RecVerticesLocation
  if( m_RVLocation.empty() ) m_RVLocation.push_back("Rec/Vertices/RV");

  //Prey IDs
  const ParticleProperty* Prey = ppSvc()->find( m_Prey );
  if ( !Prey ) { //
    err() << "Cannot find particle property for " << m_Prey << endmsg ;
    return StatusCode::FAILURE;
  }
  m_PreyID = Prey->particleID( );
  m_PreyPID = m_PreyID.pid ();
  //Get the pion mass
  const ParticleProperty* Ppion = ppSvc()->find( "pi+" );
  m_piMass = Ppion->mass();

  //Sanity checks
  if(  m_RemVtxFromDet > 0 && m_RemVtxFromDet!= 5 ){
    info()<<" RemVtxFromDet = "
          << m_RemVtxFromDet <<" is not supported"
          << endmsg;
  }

  //Set the value of m_FromBL and m_FromUpPV according to what is given in
  //  m_RCut
  if( !m_RCut.empty() ){
    if( m_RCut.find("FromBeamLine") != string::npos ) m_FromBL = true;
    if( m_RCut.find("UpstreamPV") != string::npos ) m_FromUpPV = true;
    if( m_RCut.find("BeamSpot") != string::npos ) m_FromBeamSpot = true;
  }

  if( context() == "Info" ){
    info()<<"--------------------------------------------------------"<<endmsg;
    info()<<"RecVertices2Particles takes every RecVertex from "
          << m_RVLocation <<" and turns them into Particles if they satisfies "
          <<"the following criterias :"<< endmsg ;
    info()<<"RecVertex has no backward tracks"<< endmsg ;
    if( m_RemVtxFromDet == 5 )
      info()<<"Remove RecVertex if in region arround the RFFoil and sensors,"
            <<"positionned w.r.t the velo sensors"<< endmsg;
    info()<<"Remove tracks with Chi2/ndof > "<< m_TChi2<< endmsg;
    info()<<"Reconstructed Mass of the RecVertex"<< endmsg;
    info()<<"Min Mass : " << m_PreyMinMass/GeV <<" GeV"<< endmsg;
    info()<<"Max Mass : " << m_PreyMaxMass/GeV <<" GeV"<< endmsg;
    info()<<"Min sum of daughters's pT : "<< m_SumPt/GeV <<" GeV"<< endmsg;
    info()<<"Minimum number of tracks at the RecVertex : "
          << m_nTracks <<" tracks."<< endmsg;
    info()<< "The radial displacement is ";
    if( m_FromBL ){
      info()<< "computed with respect to the beam line given at "
            << m_BLLoc << endmsg;
      if( m_FromUpPV )
        info()<< "If no beam line info are found, it will then be ";
    }
    if( m_FromUpPV ){
      info()<< "computed with respect to the upstream PV."<< endmsg;
      info()<< "Min nb of tracks on the upPV candidate : "
            << m_PVnbtrks << endmsg;
    }
    if( !m_FromUpPV && !m_FromBL ){
      info()<< "computed with respect to (0,0,z) in the global LHCb frame"
            << endmsg;
      info()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!"
            << endmsg;
    }
    info()<<"Min R    : " << m_RMin/mm <<" mm"<< endmsg ;
    info()<<"Max R    : " << m_RMax/mm <<" mm"<< endmsg ;
    info()<<"Particles will be made with "<< m_Prey <<" id" << endmsg ;
    info()<<"Vertex fitter used : "<< m_Fitter << endmsg ;
    info()<<"When trying to associate tracks participating to the "
          <<"reconstructed vertex with a Particle, ";
    if( m_UsePartFromTES ){
      info()<<"particles from the TES will be used.";
    } else {
      info()<<"association will be done with protoparticles and a Particle"
            <<" made with best PID.";
    }; info()<< endmsg ;
    info()<<"--------------------------------------------------------"<<endmsg;
  }

  if( m_RemVtxFromDet != 0 ){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();
  }

  //Initialize the beam line
  m_BeamLine = new Particle();

  //Set beam line to z axis
  if( m_RCut.empty() ){
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( 0., 0., 0. ) );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  }

  if( m_RCut =="FromBeamSpot" ){
    registerCondition("/dd/Conditions/Online/Velo/MotionSystem",&RecVertices2Particles::UpdateBeamSpot);
    this->UpdateBeamSpot();
  }

  if( !m_UsePartFromTES ){

    m_protoMap.push_back( ProtoPair( ppSvc()->find( "pi+" ),
                                     tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "pion", this ) ) );
    m_protoMap.push_back( ProtoPair( ppSvc()->find( "e+" ),
                                     tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "electron", this ) ) );
    m_protoMap.push_back( ProtoPair( ppSvc()->find( "K+" ),
                                     tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "kaon", this ) ) );
    m_protoMap.push_back( ProtoPair( ppSvc()->find( "p+" ),
                                     tool<IProtoParticleFilter>( "ProtoParticleCALOFilter", "proton", this ) ) );
    m_protoMap.push_back( ProtoPair( ppSvc()->find( "mu+" ),
                                     tool<IProtoParticleFilter>( "ProtoParticleMUONFilter", "muon", this ) ) );
  }

  m_materialVeto = tool< IMatterVeto >( "MatterVetoTool" , "MatterVetoTool" );

  m_veloProtoPartLocation = "Strip/"+this->name()+"/VeloProtoP";
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RecVertices2Particles::execute() {

  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.

  ++counter("Processed evt nb");
  if( msgLevel(MSG::DEBUG) )
    debug() << "==> Execute the RecVertices2Particles algorithm, event "
            << counter("Processed evt nb").flag() << endmsg;

  //Clear stuff
  PVs.clear();

  //make sure geometry is inityialised on event basis
  m_GeoInit = false;

  //Set the beam line first, it may be used by GetUpstreamPV()
  if( m_FromBL ){
    if( exist<Particle::Range>( m_BLLoc ) ){
      const Particle::Range BL = get<Particle::Range>( m_BLLoc );
      const LHCb::Particle* tmp = *(BL.begin());
      m_BeamLine->setReferencePoint( tmp->referencePoint() );
      m_BeamLine->setMomentum( tmp->momentum() );
      if( msgLevel(MSG::DEBUG) )
        debug()<<"Beam line position "<< m_BeamLine->referencePoint()
               <<" direction " << m_BeamLine->momentum() << endmsg;
    } else {
      if( msgLevel(MSG::DEBUG) )
        debug()<<"No Beam line found at "<< m_BLLoc << endmsg;
      if( !m_FromUpPV ) return StatusCode::SUCCESS;
    }
  }

  //Require a upstream PV for both FromBeamLine and FromUpstreamPV methods.
  const RecVertex * UpPV = GetUpstreamPV();
  if( UpPV==NULL ){
    if(msgLevel(MSG::DEBUG))
      debug() <<"No PV match the requirements" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Set up the upstream PV
  if( m_FromUpPV ){
    m_BeamLine->setReferencePoint( UpPV->position() );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
    if(msgLevel(MSG::DEBUG))
      debug() <<"Upstream PV position "<< UpPV->position() << endmsg;
  }

  // Set up the beam spot
  if( m_FromBeamSpot ){
    Gaudi::XYZPoint beamSpot( m_beamSpotX,m_beamSpotY,UpPV->position().z());
    m_BeamLine->setReferencePoint(beamSpot );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  }

  //Retrieve Reconstructed Vertices
  RecVertex::ConstVector RV;
  GetRecVertices( RV );
  if( RV.size() < 1 ) return StatusCode::SUCCESS;

  int nbRecParts(0);


  LHCb::ProtoParticles* veloTrProtoP = new LHCb::ProtoParticles();
  put( veloTrProtoP , m_veloProtoPartLocation );


  /////////////////////////////////////////////////////////////
  // Main loop over the candidates. Turn them into particles
  /////////////////////////////////////////////////////////////

  for(RecVertex::ConstVector::const_iterator i = RV.begin() ; i != RV.end() ; ++i) {
    const RecVertex* rv = *i;
    // do not keep if upstream the upstream PV
    if( rv->position().z() < UpPV->position().z() ) continue;

    const SmartRefVector< Track > & Tracks = rv->tracks();
    int size = Tracks.size();
    double r = GetRFromBL( rv->position() ); //R to beam line
    // do not keep if R not in range or if number of tracks is too small
    if( size < m_nTracks || r < m_RMin || r > m_RMax ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV did not passed the cuts  --> disguarded !"<< endmsg;
      continue;
    }

    //Cut on the sum pt of daughter tracks
    double sumpt(0);
    int nbtrks(0);
    GetSumPtNbGoodTrks( rv, sumpt, nbtrks );
    if( sumpt < m_SumPt || nbtrks < m_nTracks ){
      if( msgLevel(MSG::DEBUG) ){
        if( sumpt < m_SumPt )
          debug() <<"RV has sumpt "<< sumpt/GeV <<" < "<< m_SumPt/GeV
                  <<" --> disguarded !"<< endmsg;
        else if( nbtrks < m_nTracks )
          debug() <<"RV has have number of tracks "<< nbtrks <<" < "
                  << m_nTracks <<" after low Chi2/ndof tracks removal."
                  <<" --> disguarded !"<< endmsg;
      }
      continue;
    }

    // initialize Geometrical information if needed
    //if( m_RemVtxFromDet==5 ) InitialiseGeoInfo();

    //Turn it into a Particle !
    //Will put in info 51: if the particle is in detector 52: the distance to beamline

    if( !RecVertex2Particle( rv, nbRecParts , r, *veloTrProtoP ) ) continue;

  }
  if( msgLevel(MSG::DEBUG) )
    debug()<<"# of Preys " << nbRecParts << endmsg;
  if( context() == "Info" ) plot( nbRecParts, "NbofPreys", 0, 20 );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RecVertices2Particles::finalize() {

  delete m_BeamLine;

  debug() << "==> Finalize" << endmsg;
  return DaVinciHistoAlgorithm::finalize();
}

//============================================================================
// Get the upstream Primary vertex
//============================================================================
const RecVertex * RecVertices2Particles::GetUpstreamPV(){
  GetPVs();
  if( PVs.empty() ){ return NULL; } else { return *(PVs.begin());}
}

//============================================================================
// Get candidate PVs
//============================================================================
void RecVertices2Particles::GetPVs(){

  //The PVs container is emptied at the begining of the execution
  if( !PVs.empty() ) return;
  const RecVertex::Range & PVCs = this->primaryVertices();
  if( PVCs.empty() ) return;

  for ( RecVertex::Range::const_iterator i = PVCs.begin();
        i != PVCs.end() ; ++i ){
    const RecVertex* pv = *i;
    //Apply some cuts
    if( m_FromBL ){
      double rho = GetRFromBL( pv->position() );
      if( rho > m_RMin ) continue;
    }
    else if( abs(pv->position().x()>1.5*mm) || abs(pv->position().y()>1.5*mm)){
      continue;}
    double z = pv->position().z();
    if( abs(z) > 400*mm ) continue;
    if( !HasBackAndForwardTracks( pv ) ) continue;
    if( msgLevel( MSG::DEBUG ) )
      debug() <<"PV candidate : nb of tracks "<< pv->tracks().size() << endmsg;
    if( pv->tracks().size() < m_PVnbtrks ) continue;
    PVs.push_back(pv);
  }
  sort( PVs.begin(), PVs.end(), RecVertices2Particles::CondRVz() );
}

//=============================================================================
// Get the reconstructed vertices
//=============================================================================
void RecVertices2Particles::GetRecVertices( RecVertex::ConstVector & RV )
{
  vector<string>::iterator itName;
  for( itName = m_RVLocation.begin(); itName != m_RVLocation.end();
       ++itName ) {
    const RecVertex::Range tRV = get<RecVertex::Range>( *itName );
    if(tRV.empty()) {
      warning() << "No reconstructed vertices found at location: "
                << *itName << endmsg;
      continue;
    }
    for(RecVertex::Range::const_iterator i = tRV.begin();  i != tRV.end(); ++i ) {
      // check that it only have forward tracks
      if( ! HasBackwardTracks(*i) ) RV.push_back( *i );
    }
  }
  unsigned int size = RV.size();
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Reconstructed Vertices " << size << endmsg;
  if( context() == "Info" ) plot( size,"NbofRecVtx", 0,50 );
}


//=============================================================================
// Get radial distance to Beam Line
//=============================================================================
double RecVertices2Particles::GetRFromBL( const Gaudi::XYZPoint& p ){

  //intersection of the beam line with the XY plane,
  //find the lambda parameter of the line.
  double lambda = (p.z() - m_BeamLine->referencePoint().z()) /
    m_BeamLine->momentum().z();

  //find x and y of intersection point
  double x = m_BeamLine->referencePoint().x()
    + lambda * m_BeamLine->momentum().x();
  double y = m_BeamLine->referencePoint().y()
    + lambda * m_BeamLine->momentum().y();

  //return distance to the intersection point
  x -= p.x(); y -= p.y();
  return sqrt( x*x + y*y );
}

//=============================================================================
//  Loop on the daughter track to see if there is at least one backward track
//  and one forward tracks
//=============================================================================
bool RecVertices2Particles::HasBackAndForwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = RV->tracks().end();
  bool back = false;
  bool forw = false;
  for( ; i != iend; ++i ){
    if ( (*i)->checkFlag( Track::Backward ) ){ back = true;}
    else { forw = true;}
    if( back && forw ) return true;
  }
  return false;
}

//=============================================================================
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool RecVertices2Particles::HasBackwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = RV->tracks().end();
  for( ; i != iend; ++i ){
    if ( (*i)->checkFlag( Track::Backward ) ) return true;
  }
  return false;
}

//=============================================================================
// Compute the number and sum pT of daughter tracks with chi2/ndof < Max
//=============================================================================
void RecVertices2Particles::GetSumPtNbGoodTrks( const LHCb::RecVertex * rv,
                                                double & pttot, int & nb  ){
  pttot = 0;
  nb = 0;
  SmartRefVector< Track >::const_iterator iend = rv->tracks().end();
  for( SmartRefVector< Track >::const_iterator i = rv->tracks().begin();
       i < iend; ++i ){
    if( i->target()->chi2PerDoF() > m_TChi2 ) continue;
    pttot += i->target()->pt();
    nb++;
  }
}


//=============================================================================
//  Remove RV if found to be in detector material
//
//  if = 0  : disabled
//  if = 1  : deprecated
//  if = 2  : deprecated
//  if = 3  : deprecated
//  if = 4  : deprecated
//  if = 5  : use data tunned parametrisation of the detector material.

//=============================================================================
bool RecVertices2Particles::IsAPointInDet( const Particle & P, int mode ){

  if( mode < 1 ) return false;
  const Vertex * RV = P.endVertex();
  if ( mode != 5 ){
    info()<< "This material cut is not used anymore..."<< endmsg;
    return false;
  }
  else if( mode == 5 ){
    const Gaudi::XYZPoint & point = RV->position();
    Gaudi::XYZPoint posloc;
    bool inMat = false;
    //move to local Velo half frame
    if( point.x() < 2. ){ //right half
      posloc = m_toVeloRFrame * point;
      inMat = inMat || IsInMaterialBoxRight(posloc);
    }
    if (inMat) return inMat;
    if( point.x() > -2. ){ //left half
      posloc = m_toVeloLFrame * point;
      inMat = inMat || IsInMaterialBoxLeft(posloc);
    }
    return inMat;
  }
  return false;
}
//=============================================================================
// Check if a point is in a region containing RFFoil and sensors in the Left
// halfbox frame
//=============================================================================

bool RecVertices2Particles::IsInMaterialBoxLeft(const Gaudi::XYZPoint& point){
  // First get the z bin
  int regModIndex(0);
  double downlimit(-1000.),uplimit(-1000.);
  if(int(m_LeftSensorsCenter.size())-1<2)return false;
  for(int mod = 0 ; mod != int(m_LeftSensorsCenter.size())-1; mod++){
    downlimit=uplimit;
    uplimit=(m_LeftSensorsCenter[mod].z()+(m_LeftSensorsCenter[mod+1].z()-
                                           m_LeftSensorsCenter[mod].z())/2);
    if( point.z()>downlimit && point.z()<uplimit ){
      regModIndex=mod;
      continue;
    }
  }
  if(point.z()<800. && point.z()>uplimit)regModIndex=m_LeftSensorsCenter.size()-1;
  // Is in vaccum clean cylinder?
  double r = sqrt(pow(point.x()-m_LeftSensorsCenter[regModIndex].x(),2)+pow(point.y()-m_LeftSensorsCenter[regModIndex].y(),2));

  if ( (r<5. && point.z()<370.) || (r<4.3 && point.z()>370.) ){
    return false;
  }
  // Is in the module area
  double halfModuleBoxThickness(1.75);
  if (point.z()<m_LeftSensorsCenter[regModIndex].z()+halfModuleBoxThickness
      && point.z()>m_LeftSensorsCenter[regModIndex].z()-halfModuleBoxThickness)
    return true;

  // depending on z:
  // in the region of small corrugation
  if(point.z()<290. && point.x()-m_LeftSensorsCenter[regModIndex].x()>4){
    // first rather large region, rather small r
    float smallerCyl = 8.;
    float RsmallerCyl = 7.;
    float largerCyl = 11.;
    float RlargerCyl = 9.;

    if(fabs(point.z()-m_LeftSensorsCenter[regModIndex].z())>smallerCyl
       && r < RsmallerCyl ) return false;
    if(fabs(point.z()-m_LeftSensorsCenter[regModIndex].z())>largerCyl
       && r < RlargerCyl ) return false;
  }
  // Is clearly outside RFFoil part
  if(r<12.5 && point.z()<440.) return true;
  if(fabs(point.x()-m_LeftSensorsCenter[regModIndex].x())<5.5 &&
     point.z()<440.) return true;
  if(fabs(point.x()-m_LeftSensorsCenter[regModIndex].x())<8.5 &&
     point.z()>440.) return true;
  return false;

}
//=============================================================================
// Initialize the geometric infos
//=============================================================================

void RecVertices2Particles::InitialiseGeoInfo(){
  if( m_GeoInit ) return; //no need to do it more than once per event.

  //get the Velo geometry
  string velo = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo";
  const IDetectorElement* lefthalv = getDet<IDetectorElement>( velo+"Left" );
  const IDetectorElement* righthalv =  getDet<IDetectorElement>( velo + "Right" );
  const IGeometryInfo* halflgeominfo = lefthalv->geometry();
  const IGeometryInfo* halfrgeominfo = righthalv->geometry();
  Gaudi::XYZPoint localorigin(0,0,0);
  Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin);
  Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin);
  if( msgLevel( MSG::DEBUG ) )
    debug() <<"Velo global right half center "
            << rightcenter <<", left half center "<< leftcenter << endmsg;
  //matrix to transform to local velo frame
  m_toVeloRFrame = halfrgeominfo->toLocalMatrix() ;
  //m_toGlobalFrame = halfgeominfo->toGlobalMatrix();
  m_toVeloLFrame = halflgeominfo->toLocalMatrix() ;
  if(m_RemVtxFromDet==5 ){
    m_LeftSensorsCenter.clear();
    m_RightSensorsCenter.clear();
    DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );
    std::vector< DeVeloRType * >::const_iterator iLeftR= velo->leftRSensorsBegin() ;
    for(;iLeftR!=velo->leftRSensorsEnd();iLeftR++){
      if((*iLeftR)->isPileUp())continue;
      const Gaudi::XYZPoint localCenter(0.,0.,0.);
      const Gaudi::XYZPoint halfBoxRCenter =
        (*iLeftR)->localToVeloHalfBox (localCenter);
      const DeVeloPhiType * phisens = (*iLeftR)->associatedPhiSensor () ;
      if(!(*iLeftR)->isPileUp()){
        const Gaudi::XYZPoint halfBoxPhiCenter =
          phisens->localToVeloHalfBox (localCenter);
        Gaudi::XYZPoint halfBoxCenter(halfBoxRCenter.x()+(halfBoxPhiCenter.x()-halfBoxRCenter.x())/2,
                                      halfBoxRCenter.y()+(halfBoxPhiCenter.y()-halfBoxRCenter.y())/2,
                                      halfBoxRCenter.z()+(halfBoxPhiCenter.z()-halfBoxRCenter.z())/2);
        m_LeftSensorsCenter.push_back(halfBoxCenter);
      }
    }
    std::vector< DeVeloRType * >::const_iterator iRightR =
      velo->rightRSensorsBegin() ;
    for(;iRightR!=velo->rightRSensorsEnd();iRightR++){
      const Gaudi::XYZPoint localCenter(0.,0.,0.);
      const Gaudi::XYZPoint halfBoxRCenter =
        (*iRightR)->localToVeloHalfBox (localCenter);
      const DeVeloPhiType * phisens = (*iRightR)->associatedPhiSensor () ;
      if(!(*iRightR)->isPileUp()){
        const Gaudi::XYZPoint halfBoxPhiCenter =
          phisens->localToVeloHalfBox (localCenter);
        Gaudi::XYZPoint halfBoxCenter(halfBoxRCenter.x()+(halfBoxPhiCenter.x()-halfBoxRCenter.x())/2,
                                      halfBoxRCenter.y()+(halfBoxPhiCenter.y()-halfBoxRCenter.y())/2,
                                      halfBoxRCenter.z()+(halfBoxPhiCenter.z()-halfBoxRCenter.z())/2);
        m_RightSensorsCenter.push_back(halfBoxCenter);
      }
    }
  }
}

//=============================================================================
// Check if a point is in a region containing RFFoil and sensors in the Right
// halfbox frame
//=============================================================================

bool RecVertices2Particles::IsInMaterialBoxRight(const Gaudi::XYZPoint& point){
  // First get the z bin
  int regModIndex(0);
  double downlimit(-1000.),uplimit(-1000.);
  if(int(m_RightSensorsCenter.size())-1<2)return false;
  for (int mod = 0 ; mod != int(m_RightSensorsCenter.size())-1; mod++){
    downlimit=uplimit;
    uplimit=(m_RightSensorsCenter[mod].z()+(m_RightSensorsCenter[mod+1].z()-m_RightSensorsCenter[mod].z())/2);
    if( point.z()>downlimit && point.z()<uplimit ){
      regModIndex=mod;
      continue;
    }
  }
  if(point.z()<800. && point.z()>uplimit)
    regModIndex=m_RightSensorsCenter.size()-1;
  // Is in vaccum clean cylinder?
  double r = sqrt(pow(point.x()-m_RightSensorsCenter[regModIndex].x(),2)+pow(point.y()-m_RightSensorsCenter[regModIndex].y(),2));

  // inner cylinder
  if ( (r<5. && point.z()<390.) || (r<4.3 && point.z()>390.) ){
    return false;
  }
  // is in the module area
  double halfModuleBoxThickness(1.75);
  if (point.z()<m_RightSensorsCenter[regModIndex].z()+halfModuleBoxThickness
      && point.z()>m_RightSensorsCenter[regModIndex].z()-halfModuleBoxThickness) return true;
  // depending on z:
  // in the region of small corrugation
  if(point.z()<300. && point.x()-m_RightSensorsCenter[regModIndex].x()<-4){
    // first rather large region, rather small r
    float smallerCyl = 8.;
    float RsmallerCyl = 7.;
    float largerCyl = 11.;
    float RlargerCyl = 9.;

    if (fabs(point.z()-m_RightSensorsCenter[regModIndex].z())>smallerCyl
        && r < RsmallerCyl ) return false;
    if (fabs(point.z()-m_RightSensorsCenter[regModIndex].z())>largerCyl
        && r < RlargerCyl ) return false;
  }
  // Is clearly outside RFFoil part
  if (r<12.5 && point.z()<450. ) return true;
  if (point.z()<450. && fabs(point.x()-m_RightSensorsCenter[regModIndex].x())<5.5)return true;
  if (fabs(point.x()-m_RightSensorsCenter[regModIndex].x())<8.5 && point.z()>450.) return true;
  return false;
}
//=============================================================================
// Apply some cuts on the Particle or Vertex
//=============================================================================
bool RecVertices2Particles::TestMass( Particle & part ){
  double mass = part.measuredMass();
  if( mass > m_PreyMinMass && mass < m_PreyMaxMass ) return true;
  return false;
}
bool RecVertices2Particles::TestMass( const Particle * part ){
  double mass = part->measuredMass();
  if( mass > m_PreyMinMass && mass < m_PreyMaxMass ) return true;
  return false;
}
//=============================================================================
// Apply some quality cuts on the track
//=============================================================================
bool RecVertices2Particles::TestTrack( const Track * t ){

  //If the track has chi^2/ndof greater than max, remove it !
  if( t->chi2PerDoF() > m_TChi2 ) return false;

  //We want to avoid tracks with "infinite" momentum, i.e. straight lines.
  //When Q/P ~ err( Q/P ), P become arbitrary...
  //Velo tracks gets default momentum, so are safe to keep.
  if( t->checkType(Track::Velo) ) return true;
  double sQoP = t->firstState().qOverP() / t->firstState().errQOverP2();
  if( fabs(sQoP) < 20 ) return false;
  return true;
}


//=============================================================================
// Turn RecVertices into Particles (from Parts) saved in RecParts
//=============================================================================
bool RecVertices2Particles::RecVertex2Particle( const RecVertex* rv,
                                                int& nbRecParts , double r, LHCb::ProtoParticles& veloProtos ){

  //Retrieve data Particles from Desktop.
  Particle::ConstVector Parts;
  Parts = this->i_particles();

  Gaudi::LorentzVector mom;
  SmartRefVector< Track >::const_iterator iVtx = rv->tracks().begin();
  int endkey = rv->tracks().back()->key();

  //Create an decay vertex
  const Gaudi::XYZPoint point = rv->position();
  Vertex tmpVtx = Vertex( point );
  tmpVtx.setNDoF( rv->nDoF());
  tmpVtx.setChi2( rv->chi2());
  tmpVtx.setCovMatrix( rv->covMatrix() );
  //Create a particle
  Particle tmpPart = Particle( m_PreyID );

  std::vector<const Particle *> newParticlesPointer ;

  if( !m_UsePartFromTES ){
    // Load the ProtoParticles
    const ProtoParticles * pps =
      get<ProtoParticles>(ProtoParticleLocation::Charged);
    for ( ProtoParticles::const_iterator ipp = pps->begin();
          pps->end() != ipp; ++ipp ){
      const ProtoParticle * pp = *ipp;
      if( pp->track() == NULL ) continue;
      while( ((*iVtx)->key() < pp->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }
      if( (*iVtx)->key() == pp->key() ){
        //Make a Particle with best PID
        if( !TestTrack( *iVtx ) ) continue;
        const Particle * part = MakeParticle( pp );
        tmpVtx.addToOutgoingParticles ( part );
        tmpPart.addToDaughters( part );
        mom += part->momentum();
        newParticlesPointer.push_back(part);
        continue;
      }
    }
    for(SmartRefVector< Track >::const_iterator itr = rv->tracks().begin(); itr != rv->tracks().end();itr++){
      const Track* p = (*itr);
      if ((*itr)->type()!= LHCb::Track::Velo) continue;
      double sx = p->slopes().x(); double sy = p->slopes().y();
      double pz = m_pt/sqrt( sx*sx + sy*sy );
      double e = std::sqrt( m_piMass*m_piMass + m_pt*m_pt + pz*pz );
      // new particle pointer will be handle by the markNewTree function or deleted if the
      Particle* pion = new Particle();
      // new ProtoParticle --> should be written in the TES
      ProtoParticle* protoPion = new ProtoParticle() ;
      protoPion->setTrack(p);
      veloProtos.insert(protoPion);
      pion->setProto(protoPion);
      const Gaudi::LorentzVector mompi = Gaudi::LorentzVector(sx*pz, sy*pz, pz,e );
      pion->setMomentum(mompi);
      tmpVtx.addToOutgoingParticles ( pion );
      tmpPart.addToDaughters( pion );
      mom += pion->momentum();
      newParticlesPointer.push_back(pion);
    }
  }
  else {
    //Find all particles that have tracks in RecVertex
    Particle::ConstVector::const_iterator jend = Parts.end();
    for ( Particle::ConstVector::const_iterator j = Parts.begin();
          j != jend;++j) {

      if( (*j)->proto() == NULL ) continue;
      if( (*j)->proto()->track() == NULL ) continue;
      const Track * tk = (*j)->proto()->track();
      if( !TestTrack( tk ) ) continue;
      while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }

      if( (*iVtx)->key() == tk->key() ){
        if( (*iVtx)->key() != endkey ) ++iVtx;
        // make sure it is a new pointer so that when the mother is saved on TES we only have new pointers.
        const LHCb::Particle* clonedPart =  (*j )->clone();
        tmpVtx.addToOutgoingParticles ( clonedPart );
        tmpPart.addToDaughters( clonedPart );
        mom += clonedPart->momentum();
        newParticlesPointer.push_back(clonedPart);
        continue;
      }
    }
  }

  //Fill momentum and mass estimate
  tmpPart.setMomentum( mom );
  tmpPart.setMeasuredMass( mom.M() );
  tmpPart.setMeasuredMassErr( 0 );

  if( !TestMass( tmpPart ) ){
    for (std::vector<const Particle*>::iterator ip = newParticlesPointer.begin() ; newParticlesPointer.end()!= ip ; ip++){
      // check that the particle is not in TES
      if(!DaVinci::Utils::inTES(*ip))delete((*ip));
    }
    if( msgLevel(MSG::DEBUG) )
      debug() <<"Particle did not passed the mass cut --> disguarded !"
              << endmsg;
    return false;
  }
  Gaudi::SymMatrix4x4 MomCovMatrix =
    Gaudi::SymMatrix4x4( ROOT::Math::SMatrixIdentity() );
  tmpPart.setMomCovMatrix( MomCovMatrix );
  tmpPart.setPosCovMatrix( rv->covMatrix() );
  Gaudi::Matrix4x3 PosMomCovMatrix;
  tmpPart.setPosMomCovMatrix( PosMomCovMatrix );
  //Fix end vertex
  tmpPart.setEndVertex( tmpVtx.clone() );
  tmpPart.setReferencePoint( point );
  tmpPart.addInfo(52,r );
  tmpPart.addInfo(53,rv->tracks().size() );
  //Store 51 info if found to be in detector material
  if( m_materialVeto->isInMatter(tmpVtx.position()) )
    tmpPart.addInfo(51,1.);
  this->markNewTree(tmpPart.clone());
  nbRecParts++;

  setFilterPassed(true);

  return true;
}

//=============================================================================
// Create Particle from ProtoParticle with best PID
//   Adapted from BestPIDParticleMaker
//=============================================================================
const Particle * RecVertices2Particles::MakeParticle( const ProtoParticle * pp ){

  // Test PID info consistency
  // test RICH links
  if ( pp->hasInfo( ProtoParticle::RichPIDStatus ) ){
    const RichPID * rpid = pp->richPID();
    if ( !rpid ){
      Error( "ProtoParticle has RICH information but NULL RichPID SmartRef !" ).ignore();
    }
  }
  // test MUON links
  if ( pp->hasInfo( ProtoParticle::MuonPIDStatus ) ){
    const MuonPID * mpid = pp->muonPID();
    if ( !mpid ){
      Error( "ProtoParticle has MUON information but NULL MuonPID SmartRef !" ).ignore();
    }
  }

  //Create a vector of PID and DLL pairs
  DLLPIDVector dllpid;
  dllpid.push_back( DLLPIDPair( "pi+",
                                pp->info( ProtoParticle::CombDLLpi,0) ) );
  double dll = pp->info( ProtoParticle::CombDLLmu,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "mu+", dll ) );
  dll = pp->info( ProtoParticle::CombDLLe,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "e+", dll ) );
  dll = pp->info( ProtoParticle::CombDLLk,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "K+", dll ) );
  dll = pp->info( ProtoParticle::CombDLLp,0);
  if( dll > -0.01 ) dllpid.push_back( DLLPIDPair( "p+", dll ) );

  //Sort the DLL
  sort( dllpid.begin(), dllpid.end(), DLLPIDPair::cmp );

  //Find the best PID
  DLLPIDVector::iterator i = dllpid.begin();
  ProtoMap::const_iterator imap;
  while( i != dllpid.end() ){
    string & pid = (*i).GetPid();
    //Continue if filters are not satisfied
    bool sel = false;
    for ( imap = m_protoMap.begin(); imap != m_protoMap.end(); ++imap ){
      if( (*imap).first->particle() == pid ){
        sel = (*imap).second->isSatisfied( pp );
        break;
      }
    }
    if( !sel ){ i++; continue; }
    //get out of here
    break;
  }

  // make a new Particle
  Particle p = Particle();
  const LHCb::ParticleProperty* pprop = (*imap).first;
  // Start filling particle with orgininating ProtoParticle
  p.setProto(pp);
  // ParticleID
  const int pID = pprop->particleID().pid() * (int)(pp->charge());
  p.setParticleID( ParticleID( pID ) );
  // Mass and error
  p.setMeasuredMass(pprop->mass());
  p.setMeasuredMassErr(0);

  const Track* tk = pp->track() ;
  //StatusCode sc = p2s()->state2Particle( tk->firstState(), p );
  //Remaining info at the first state...
  p.setReferencePoint( tk->position() );
  // momentum
  const Gaudi::XYZVector & mom = tk->momentum();
  const double mass = p.measuredMass();
  const double e = sqrt( tk->p()*tk->p()+mass*mass );
  p.setMomentum( Gaudi::XYZTVector(mom.X(),mom.Y(),mom.Z(),e) ) ;
  //error on position and momentum empty. If needed, use State2Particle

  // Add BremmStrahlung for electrons ?? Forget for now.

  if ( false ){
    if( msgLevel(MSG::DEBUG) ){
      debug() << "------------- Reconstructed Particle -----------" << endmsg;
      debug() << "Mass " << p.measuredMass()/GeV << "+/-"
              << p.measuredMassErr() <<" GeV, momentum "
              << p.momentum() << endmsg;
      debug() << "Pid "<< p.particleID().pid() << endmsg;
      debug() << "----------------------------" << endmsg;
    }
  }

  return p.clone();
}


//=============================================================================
// Update of the beam spot position
//=============================================================================
StatusCode RecVertices2Particles::UpdateBeamSpot()
{
  if (! exist<Condition>(detSvc(),"/dd/Conditions/Online/Velo/MotionSystem" )){
    Warning( "Unable to locate CONDITION='/dd/Conditions/Online/Velo/MotionSystem'" ) ;
    return StatusCode::FAILURE;
  }
  Condition *myCond =  get<Condition>(detSvc(),"/dd/Conditions/Online/Velo/MotionSystem" );
  //
  const double xRC = myCond -> paramAsDouble ( "ResolPosRC" ) ;
  const double xLA = myCond -> paramAsDouble ( "ResolPosLA" ) ;
  const double   Y = myCond -> paramAsDouble ( "ResolPosY"  ) ;
  //
  m_beamSpotX = ( xRC + xLA ) / 2;
  m_beamSpotY = Y ;
  return StatusCode::SUCCESS;
}
