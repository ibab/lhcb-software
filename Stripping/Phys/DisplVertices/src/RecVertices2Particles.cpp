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

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertices2Particles
//
// 2009-april-14 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecVertices2Particles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertices2Particles::RecVertices2Particles( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_vFit(0)
  , pi(3.1415926)
  , m_MapCalled(false)
  , m_FromBL(false)
  , m_FromUpPV(false)
  , m_GeoInit(false)
  , m_pt(400.)
{
  declareProperty("SaveTuple", m_SaveTuple = false );//save prey infos in Tuple
  declareProperty("UsePartFromTES", m_UsePartFromTES = false );
  declareProperty("Prey", m_Prey = "~chi_10" );
  //Bc+ Mass 6.286GeV
  //declareProperty("PreyMinMass", m_PreyMinMass = 6.3*Gaudi::Units::GeV );
  declareProperty("PreyMinMass", m_PreyMinMass = 0.0 );
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*Gaudi::Units::TeV );
  declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
  declareProperty("RMin", m_RMin = 0.3*Gaudi::Units::mm );
  declareProperty("RMax", m_RMax = 50.*Gaudi::Units::m );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks
  declareProperty("TrackMaxChi2oNDOF", m_TChi2 = 10. );
  declareProperty("RecVerticesLocation", m_RVLocation );
  //"BlindVertexFitter", "OfflineVertexFitter"
  declareProperty("VertexFitter", m_Fitter = "none"  );
  declareProperty("KeepLowestZ", m_KeepLowestZ = false  );
  declareProperty("IsolationDistance", m_Dist = 0.0*mm  );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
  declareProperty("DetDist", m_DetDist = 1*mm );
  declareProperty("RemFromRFFoil", m_RemFromRFFoil = false );
  //"", "FromUpstreamPV", "FromBeamLine"
  declareProperty("RCutMethod", m_RCut = "FromUpstreamPV" );
  declareProperty("BeamLineLocation", 
		  m_BLLoc = "/Event/BeamLine");
  declareProperty("UseMap", m_UseMap = false );
  declareProperty("PVnbtrks", m_PVnbtrks = 5 ); //corr. to 'tight' PV reco
}
//=============================================================================
// Destructor
//=============================================================================
RecVertices2Particles::~RecVertices2Particles() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecVertices2Particles::initialize() {

  StatusCode sc = DVAlgorithm::initialize(); 
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
  if( m_RemFromRFFoil && m_RemVtxFromDet == 4 ){
    info()<<"RemFromRFFoil = "<< m_RemFromRFFoil <<" and RemVtxFromDet = "
	  << m_RemVtxFromDet <<" are incompatible. RemFromRFFoil set to false"
	  << endmsg;
    m_RemFromRFFoil = false;
  }
  //Sanity checks
  if( context() == "HLT" ){
    m_UseMap = true; 
  }
  if( m_UseMap ){
    m_Fitter = "none";
    m_UsePartFromTES = true; 
  }

  //Set the value of m_FromBL and m_FromUpPV according to what is given in
  //  m_RCut
  if( !m_RCut.empty() ){
    if( m_RCut.find("FromBeamLine") != string::npos ) m_FromBL = true;    
    if( m_RCut.find("UpstreamPV") != string::npos ) m_FromUpPV = true; 
  }
  


  if( context() == "Info" ){
    info()<<"--------------------------------------------------------"<<endmsg;
    info()<<"RecVertices2Particles takes every RecVertex from "
          << m_RVLocation <<" and turns them into Particles if they satisfies "
          <<"the following criterias :"<< endmsg ;
    info()<<"RecVertex has no backward tracks"<< endmsg ;
    if(!m_KeepLowestZ) 
      info()<<"RecVertex has not the lowest z position (PV)"<< endmsg;
    if( m_RemFromRFFoil )
      info()<<"Remove RecVertex if in RF-Foil region"<< endmsg ;
    if( m_RemVtxFromDet == 1 )
      info()<<"Remove RecVertex if in detector material"<< endmsg;
    if( m_RemVtxFromDet == 2 )
      info()<<"Remove RecVertex if closer than " << m_DetDist 
            <<"mm from detector material along momentum"<< endmsg;
    if( m_RemVtxFromDet == 3 || m_RemVtxFromDet == 4 )
      info()<<"Remove RecVertex if closer than " << m_DetDist 
            <<"*PosCovMatric from detector material"<< endmsg;
    if( m_RemVtxFromDet == 4 )
      info()<<"("<< m_DetDist+3 <<" when in RF-Foil region)"<< endmsg;
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
  

  //Initialize Vertex Fitter
  if( m_Fitter != "none" ){
    m_vFit = vertexFitter(m_Fitter);
  } 

  //To recreate track measurements
  //   m_measProvider = tool<IMeasurementProvider>
  //     ( "MeasurementProvider","MeasProvider", this );

  if( m_RemVtxFromDet != 0 || m_SaveTuple){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();

    // Get Transport Service
    m_transSvc = svc<ITransportSvc>( "TransportSvc", true  );
  }

  //Initialize the beam line
  m_BeamLine = new Particle();

  //Set beam line to z axis
  if( m_RCut.empty() ){
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( 0., 0., 0. ) );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  } 

  if( m_RemFromRFFoil || m_RemVtxFromDet == 4 || m_SaveTuple ){
    //get the Velo geometry
    string velo = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo";
    const IDetectorElement* lefthalv = getDet<IDetectorElement>( velo+"Left" );
    const IDetectorElement* righthalv = 
      getDet<IDetectorElement>( velo + "Right" );
    const IGeometryInfo* halflgeominfo = lefthalv->geometry();
    const IGeometryInfo* halfrgeominfo = righthalv->geometry();
    Gaudi::XYZPoint localorigin(0,0,0);
    Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin);
    Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin);
    if( msgLevel( MSG::DEBUG ) )
      debug() <<"Velo global right half center "
	      << rightcenter <<", left half center "<< lefthalv << endmsg;
    //matrix to transform to local velo frame
    m_toVeloRFrame = halfrgeominfo->toLocalMatrix() ;
    //m_toGlobalFrame = halfgeominfo->toGlobalMatrix();
    m_toVeloLFrame = halflgeominfo->toLocalMatrix() ;
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

  //Check track and Particle content
  //PrintTrackandParticles();

  //Clear stuff
  PVs.clear();

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
  if( (m_FromBL || m_FromUpPV) && UpPV==NULL){
    if(msgLevel(MSG::DEBUG))
      debug() <<"No PV match the requirements" << endmsg;
    return StatusCode::SUCCESS;
  }

  if( m_FromUpPV ){
    m_BeamLine->setReferencePoint( UpPV->position() );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
    if(msgLevel(MSG::DEBUG))
      debug() <<"Upstream PV position "<< UpPV->position() << endmsg;
  }
  
  if( m_UseMap ){ 
    m_map.clear(); //Re-initialize the map
    m_MapCalled = false; //The map hasn't been called yet.
  }

  int size = 0;
  //Retrieve Reconstructed Vertices
  RecVertex::ConstVector RV;
  GetRecVertices( RV );
  if( RV.size() < 1 ) return StatusCode::SUCCESS;

  //The PV is assumed to be the rec vertex with lowest z
  //sort rec vertex according to z position
  sort( RV.begin(), RV.end(), RecVertices2Particles::CondRVz() );


  //Print out displaced vertices and change them into Particles
  Particle::ConstVector RecParts;
  RecVertex::ConstVector::const_iterator i = RV.begin();
  RecVertex::ConstVector::const_iterator iend = RV.end();
  m_GeoInit = false; //be sure the goe is initialised if needed.
  for( ; i != iend; ++i) {
    const RecVertex* rv = *i;
    const SmartRefVector< Track > & Tracks = rv->tracks();
    size = Tracks.size();
    double r = GetRFromBL( rv->position() ); //R to beam line
    
    if( msgLevel(MSG::DEBUG) ){
      const Gaudi::SymMatrix3x3 & err = rv->covMatrix();
      double errr = sqrt( err(0,0) + err(1,1) );
      debug() << "------------- Reconstructed Vertex -----------" << endmsg;
      debug() << "Position " << rv->position() <<" R "<< r
              << " sigma R "<< errr << " sigmaZ "<< sqrt(err(2,2))
              << " Number of associated tracks " << size 
              << " Chi2/NDoF " << rv->chi2PerDoF() << endmsg;
    }

    //Do not change the PV into a particle ! (if any !)
    if( !m_KeepLowestZ && i == RV.begin() ) continue;

    //Do not keep if upstream to the upPV
    if( (m_FromBL || m_FromUpPV) && 
        rv->position().z() < UpPV->position().z() ) continue;


    //PVs have no backward tracks
    if( HasBackwardTracks(rv) ){ 
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV has a backward track --> disguarded !"<< endmsg;
      continue;
    }

    if( !IsIsolated( rv, RV ) ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV is not isolated  --> disguarded !"<< endmsg;
      continue;
    }

    if( m_RemFromRFFoil && IsInRFFoil( rv->position() ) ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV is in the RF-Foil  --> disguarded !"<< endmsg;
      continue;
    }

    //Cuts !!
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

    if( m_RemFromRFFoil || m_RemVtxFromDet == 4 || m_RemVtxFromDet==5 || 
        m_SaveTuple ){
      InitialiseGeoInfo();
    }    
    //Turn it into a Particle !
    //Will put in info 51: if the particle is in detector 52: the distance to beamline
    if( !RecVertex2Particle( rv, RecParts , r ) ) continue;

    //Study Prey Composition
    // ***Don't forget to turn on the IMeasurementProvider tool***
    if( false && r > m_RMin && r < m_RMax ) 
      StudyPreyComposition( RecParts.back() );

  }
  size = RecParts.size();
  if( msgLevel(MSG::DEBUG) )
    debug()<<"# of Preys " << size << endmsg;
  if( context() == "Info" ) plot( size, "NbofPreys", 0, 20 );

  //Save Preys infos in tuple
  if( m_SaveTuple ){
    Tuple tuple = nTuple("DisplVertices"); //defines a tuple to save infos
    if( SavePreysTuple( tuple, RecParts ).isFailure() )
      warning()<<"Impossible to fill tuple with candidate infos"<<endmsg;
  }

  //Save Preys from Desktop to the TES.
  return desktop()->cloneTrees( RecParts );
  //return desktop()->saveDesktop();
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RecVertices2Particles::finalize() {

  delete m_BeamLine;

  debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize(); 
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
    RecVertex::Range::const_iterator i = tRV.begin();
    for( ; i != tRV.end(); ++i ) RV.push_back( *i );
  }
  unsigned int size = RV.size();
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Reconstructed Vertices " << size << endmsg;
  if( context() == "Info" ) plot( size,"NbofRecVtx", 0,50 );
  
}

//============================================================================
// Get Nb of Velo tracks in event
//============================================================================
unsigned int RecVertices2Particles::GetNbVeloTracks(){

  unsigned int nbv = 0;
  const Track::Range Trks = get<Track::Range>( TrackLocation::Default );
  for(Track::Range::const_iterator itr = Trks.begin(); 
        Trks.end() != itr; ++itr) {
      if( (*itr)->hasVelo() ) ++nbv;
  }
  return nbv;
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
         if( context() == "Info" ) plot( rho, "PVr", 0., 1.5*mm, 50 );
         if( rho > m_RMin ) continue;         
    }
    else if( abs(pv->position().x()>1.5*mm) || abs(pv->position().y()>1.5*mm)){
      continue;}
    double z = pv->position().z();
    if( abs(z) > 400*mm ) continue;
    if( !HasBackAndForwardTracks( pv ) ) continue;
    //const Gaudi::SymMatrix3x3  & mat = pv->covMatrix();
    if( msgLevel( MSG::DEBUG ) )
      debug() <<"PV candidate : nb of tracks "<< pv->tracks().size() << endmsg;
    //<<" sigmaR "<< sr <<" sigmaZ "<< sqrt(mat(2,2)) << endmsg;
    //if( sr > m_PVsr ) continue;
    //if( sqrt(mat(2,2)) > m_PVsz ) continue;
    if( pv->tracks().size() < m_PVnbtrks ) continue;
    PVs.push_back(pv);
  }
  sort( PVs.begin(), PVs.end(), RecVertices2Particles::CondRVz() );
}

//============================================================================
// Get the upstream Primary vertex
//============================================================================
const RecVertex * RecVertices2Particles::GetUpstreamPV(){
  GetPVs();
  if( PVs.empty() ){ return NULL; } else { return *(PVs.begin());}
}


//=============================================================================
// Turn RecVertices into Particles (from Parts) saved in RecParts
//=============================================================================
bool RecVertices2Particles::RecVertex2Particle( const RecVertex* rv,
						Particle::ConstVector & RecParts , double r){  

  //Retrieve data Particles from Desktop.
  Particle::ConstVector Parts;
  if( m_UsePartFromTES ) Parts = desktop()->particles();

  Gaudi::LorentzVector mom;
  SmartRefVector< Track >::const_iterator iVtx = rv->tracks().begin();
  SmartRefVector< Track >::const_iterator iVtxend = rv->tracks().end();
  int endkey = rv->tracks().back()->key();

  //Create map if necessary
  if( m_UseMap && !m_MapCalled ) CreateMap( Parts );

  if( m_Fitter == "none" ){

    //Create an decay vertex
    const Gaudi::XYZPoint point = rv->position();
    Vertex tmpVtx = Vertex( point );
    tmpVtx.setNDoF( rv->nDoF());
    tmpVtx.setChi2( rv->chi2());
    tmpVtx.setCovMatrix( rv->covMatrix() );

    //Create a particle
    Particle tmpPart = Particle( m_PreyID );
    //Fix end vertex
    tmpPart.setEndVertex( &tmpVtx );
    tmpPart.setReferencePoint( point );
    tmpPart.addInfo(52,r ); 
    //Store 51 info if found to be in detector material
    if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) 
      tmpPart.addInfo(51,1.);
 
    if( m_UseMap ){
      //Loop on RecVertex daughter tracks and save corresponding Particles
      for( ; iVtx != iVtxend; ++iVtx ){
        //debug()<<"Key "<< (*iVtx)->key() <<" type "
        //    <<(*iVtx)->type()  <<" slope "<< (*iVtx)->slopes() << endmsg;
        if( !TestTrack( *iVtx ) ) continue;
        const int key = (*iVtx)->key();
        GaudiUtils::VectorMap<int, const Particle *>::const_iterator it;

        it = m_map.find( key );
        const Particle * part = NULL; 

        //Give a default pion with pT of 400 MeV
        if( it != m_map.end() ){ 
          part = it->second; 
          //debug()<<"got Particle from map with slope "
          //     << part->slopes() <<endmsg;
        }
        
        if( it == m_map.end() ) part = DefaultParticle(*iVtx);
        if( part != NULL ){
          tmpVtx.addToOutgoingParticles( part );
          tmpPart.addToDaughters( part );
          mom += part->momentum();
        }      
      }
    } else if( !m_UsePartFromTES ){

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
          //debug()<<"Mom should be the same "<< (*iVtx)->momentum() <<" "
          //       << pp->track()->momentum() << endmsg;
          //Make a Particle with best PID 
          if( !TestTrack( *iVtx ) ) continue;
          const Particle * part = MakeParticle( pp );
          tmpVtx.addToOutgoingParticles ( part );
          tmpPart.addToDaughters( part );
          mom += part->momentum();
          continue;
        }
      }      
    } else {
      
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
          //debug()<<"Track should be the same "<< (*iVtx)->momentum() <<" "
          //       << tk->momentum() << endmsg;
          //debug() <<"Track type "<< tk->type() << endmsg;
          if( (*iVtx)->key() != endkey ) ++iVtx; 
          //if( (*iVtx)->chi2PerDoF() > m_TChi2 ) continue;
          tmpVtx.addToOutgoingParticles ( *j );
          tmpPart.addToDaughters( *j );
          mom += (*j)->momentum();
          continue;
        }
      }
    }
    //Should //always be 100%
    //double eff = 100.*(double)tmpPart.daughters().size()/
      //(double)rv->tracks().size();
//     plot( eff, "Trk2PartEff", 0., 101.);
    //debug()<<"Track to Particle matching efficiency "<< eff << endmsg;
    //debug()<<"Found "<< Daughters.size() <<" related particles."<< endmsg;
    //Do I really care about the number of tracks found ?
    //if( Daughters.size() < m_nTracks ) return;
    

    //Fill momentum and mass estimate
    tmpPart.setMomentum( mom );
    tmpPart.setMeasuredMass( mom.M() );
    tmpPart.setMeasuredMassErr( 0 );

    if( !TestMass( tmpPart ) ){
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
    

    //Save Rec Particle in the Desktop
    RecParts.push_back( desktop()->keep( &tmpPart ) );
    
  } else {

    Particle::ConstVector Daughters;
    Particle::ConstVector::const_iterator jend = Parts.end();
    for ( Particle::ConstVector::const_iterator j = Parts.begin();
	  j != jend;++j) {
      if( (*j)->proto()->track() == NULL ) continue;
      const Track * tk = (*j)->proto()->track();
      if( !TestTrack( tk) ) continue;
      while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }
      if( (*iVtx)->key() == tk->key() ){ 
        //debug()<<"Track should be the same "<< (*iVtx)->momentum() <<" "
        //     << tk->momentum() << endmsg;
        //debug() <<"Track type "<< tk->type() << endmsg;
        if( (*iVtx)->key() != endkey ) ++iVtx; 
        //if( (*iVtx)->chi2PerDoF() > m_TChi2 ) continue;
        Daughters.push_back( *j );
        continue;
      }
    }
    
    //Make particle !
    Vertex tmpVtx; 
    Particle tmpPart;
    if( !( m_vFit->fit( Daughters, tmpVtx, tmpPart ) ) ){
      Warning("Fit error."+ m_Fitter +" not able to create Particle !"); 
      return false;
    }

    if( !TestMass( tmpPart ) ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"Particle did not passed the mass cut --> disguarded !"
                << endmsg;
      return false;   
    }

    //Store 51 info if found to be in detector material
    if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) 
      tmpPart.addInfo(51,1.);
    tmpPart.addInfo(52,r );
    
    //Save Rec Particle in the Desktop
    RecParts.push_back( desktop()->keep( &tmpPart ) );
     
  }
  setFilterPassed(true);

  if ( true ){
    if( msgLevel(MSG::DEBUG) ){
      debug() << "------------- Reconstructed Particle -----------" << endmsg;
      debug() << "Mass " << RecParts.back()->measuredMass()/GeV << "+/-" 
              << RecParts.back()->measuredMassErr() <<" GeV" << endmsg;
      debug() << "Chi2 " << RecParts.back()->endVertex()->chi2() 
	      << " Chi2/NDoF " << RecParts.back()->endVertex()->chi2PerDoF()
	      <<endmsg;
      Gaudi::XYZPoint pos = RecParts.back()->endVertex()->position();
      debug() << "Pos of decay vertex " << pos << " R "<< pos.rho() << endmsg;
      debug() << "Number of associated tracks " 
	      << RecParts.back()->daughtersVector().size() << endmsg;
      double isindet = RecParts.back()->info(51,0.);
      if( isindet > 0. )
        debug() <<"Decay vertex found to be in detector material."<< endmsg;
      debug() << "----------------------------" << endmsg;
    }
  }
  return true;
}

//=============================================================================
// Create a map between Particles and their Velo tracks ancestors
//=============================================================================
void RecVertices2Particles::CreateMap( const Particle::ConstVector & Parts ){

  m_map.reserve( Parts.size() );
  int nb = 0;

  for ( Particle::ConstVector::const_iterator j = Parts.begin();
	j != Parts.end();++j) {

    if( (*j)->proto() == NULL ) continue;
    if( (*j)->proto()->track() == NULL ) continue;
    const Track * tk = (*j)->proto()->track();
    
    SmartRefVector< Track > old = tk->ancestors();

    for( SmartRefVector<Track>::const_iterator i = 
           old.begin(); i != old.end(); ++i ){
//       debug()<<"Part "<< (*j)->key() <<" type "<< tk->type() 
//              <<" slope "<< (*j)->slopes() 
//              << " Ancestor " << (*i)->key() <<" type "
//              <<(*i)->type()  <<" slope "<< (*i)->slopes() << endmsg;
      
      if( !((*i)->checkType(Track::Velo)) ) continue;
      m_map.insert( (*i)->key(), (*j) );
      nb++;
      //debug()<<"a track has been inserted !" << endmsg;
      break;
    }
  }
  //if( context() == "Info" ){
  //double eff = 100.*( (double)nb )/( (double)Parts.size() );
  //a few particles may originate from the same Velo ancestor !
  //int clone = nb - m_map.size();
  //plot( eff, "MapEff", 0., 105. ); ////always 100%
  //plot( clone, "NbClonePart", 0., 10. );
  //debug() <<"eff "<< eff <<" nb of clone "<< clone <<" Nb of Part "
  //        << Parts.size() <<" Map size "<< m_map.size() << endmsg;
  //}

  m_MapCalled = true;
  
  return;
}

//=============================================================================
// Create default pions with 400 MeV pt, see p. 128
//=============================================================================
const Particle * RecVertices2Particles::DefaultParticle( const Track * p ){

  double sx = p->slopes().x(); double sy = p->slopes().y();
  double pz = m_pt/sqrt( sx*sx + sy*sy );
  double e = std::sqrt( m_piMass*m_piMass + m_pt*m_pt + pz*pz );
  Particle pion;
  const Gaudi::LorentzVector mom = Gaudi::LorentzVector(sx*pz, sy*pz, pz,e );
  pion.setMomentum(mom);

  //debug()<<"Creating default pion for key "<< p->key() <<" and slopes "
  //     <<sx<<" "<<sy<<" yielding momentum "<< mom << endmsg;

  return desktop()->keep(&pion);
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
//   debug()<<"ProtoParticle DLL ";
//   for( DLLPIDVector::iterator j = dllpid.begin();j != dllpid.end();++j){
//     debug() << (*j).GetPid()<<" : "<< (*j).GetDll() <<" ";}
//   debug() << endmsg;

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
    //debug() << " -> Particle type " << pid << " selected=" << sel << endmsg;
    if( !sel ){ i++; continue; }      
      //get out of here
    break;
  }
  //if( i==dllpid.end() ) return XX; //That should not happen !
  
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

  return desktop()->keep(&p);
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

//============================================================================
// if particule has a daughter muon, return highest pt
//============================================================================
double RecVertices2Particles::HasMuons( const Particle * p ){

  double muonpt = 0;
  //loop on daughters
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i = 
	 p->daughters().begin(); i != iend; ++i ){
    //check if tracks could be a muon
    if( (*i)->proto() == NULL ) continue;
    if( (*i)->proto()->muonPID() == NULL ) continue;
    if( !( (*i)->proto()->muonPID()->IsMuonLoose() ) ) continue;
    double pt = p->pt();    
    if( pt > muonpt ) muonpt = pt;
  }

  return muonpt;
}

//=============================================================================
//  Check if RV is isolated from other RVs
//=============================================================================

bool RecVertices2Particles::IsIsolated( const RecVertex* rv, RecVertex::ConstVector & RV ){

  if( m_Dist < 0.01 ) return true;

  //Retrieve Displaced Vertices
  RecVertex::ConstVector::const_iterator i = RV.begin();
  RecVertex::ConstVector::const_iterator iend = RV.end();
  for( ; i != iend; ++i) {
    if( (*i) == rv ) continue;
    if( VertDistance(rv->position(),(*i)->position()) < m_Dist ) return false;
  }
  return true;
}

//=============================================================================
//  Remove RV if found to be in detector material
//
//  if = 0  : disabled
//  if = 1  : remove reco vtx if in detector material
//  if = 2  : remove reco vtx if rad length from decay pos - DetDist 
//            to decay pos + DetDist along momentum is > threshold
//  if = 3  : remove reco vtx if rad length along 
//                             +- DetDist * PositionCovMatrix
//  if = 4  : 3 but range+3 if in RF foil.
//  if = 5  : use home-made parametrisation of the detector material.

//=============================================================================
bool RecVertices2Particles::IsAPointInDet( const Particle & P, int mode, 
                                           double range ){

  if( mode < 1 ) return false;

  double threshold = 1e-10;
  const Vertex * RV = P.endVertex();

  if( mode == 1 ){

    IGeometryInfo* start = 0;
    ILVolume::PVolumePath path ;
    
    //Get the physical volume, if any.
    StatusCode sc = m_lhcbGeo->fullGeoInfoForPoint
      ( RV->position(), 1000, start, path ) ;
    if ( sc.isFailure() ) { 
      warning()<<"Impossible to get any physical volume related to point "
               << RV->position() <<endmsg;
      return false; 
    } 
    int size = path.size();
    if( context() == "Info" ) plot( size, "NbofDetV", 0, 5 );
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Found "<< size <<" physical volumes related to point "
             << RV->position() <<endmsg;
    
    const IPVolume* pvlast = 0;
    if ( !path.empty() ) { pvlast = path.back(); }
    
    const ILVolume * lvlast = 0;
    if ( 0 != pvlast ) { lvlast = pvlast->lvolume(); }
    
    const Material* matlast = 0 ;
    if ( 0 != lvlast ) { matlast = lvlast->material(); }
    
    if ( 0 != matlast )  { 
      if( msgLevel(MSG::DEBUG) ){
        debug()<<"Physical volume related to point "<< RV->position() <<endmsg;
        debug()<< matlast << endl;
      }
      //if( matlast->name() == "Vacuum" ) return false;
      return true;
    } 
  } //end of <0 condition
  else if( mode == 2 ){

    const Gaudi::XYZPoint pos = RV->position();
    const Gaudi::XYZPoint nvec = Normed( P.momentum(), range );
    const Gaudi::XYZPoint start = Minus( pos, nvec );
    const Gaudi::XYZPoint end = Plus( pos, nvec );

    //Compute the radiation length
    if( m_lhcbGeo == 0 ){ 
      warning()<<"IGeometryInfo* m_lhcbGeo is broken"<< endmsg; return true; }

    IGeometryInfo* dum = 0;
    double radlength = m_transSvc->distanceInRadUnits
      ( start, end, 1e-35, dum, m_lhcbGeo );

    if( context() == "Info" ) plot( radlength, "RVRadLength", 0, 0.01);
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Radiation length from "<< start <<" to "
	     << end <<" : "<< radlength 
	     <<" [mm]" << endmsg;


    if( radlength > threshold ){
      if( msgLevel(MSG::DEBUG) )
        debug()<<"RV is too closed to a detector material --> disguarded !"
               << endmsg;
      return true;
    }
    

  } //end of 2 condition
  else if( mode == 3 || mode == 4 ){

    const Gaudi::XYZPoint  RVPosition = RV->position();
    const Gaudi::SymMatrix3x3 & RVPositionCovMatrix = RV->covMatrix();
    double sigNx = range*sqrt(RVPositionCovMatrix[0][0]);
    double sigNy = range*sqrt(RVPositionCovMatrix[1][1]);
    double sigNz = range*sqrt(RVPositionCovMatrix[2][2]);
    // Is there material within N*sigma
    double radlength(0);
    if( mode == 4 && IsInRFFoil( RVPosition ) ) range += 3;
    for (int ix = -1 ; ix<2; ix += 2 ){
      for (int iy = -1 ; iy<2; iy += 2 ){
        Gaudi::XYZPoint start( RVPosition.x()+ix*sigNx,
				    RVPosition.y()+iy*sigNy,
				    RVPosition.z()+sigNz );
        Gaudi::XYZPoint end( RVPosition.x()-ix*sigNx,
			     RVPosition.y()-iy*sigNy,
			     RVPosition.z()-sigNz );
	radlength = m_transSvc->distanceInRadUnits( start, end );
	if(msgLevel(MSG::DEBUG))
	  debug()<<"Radiation length from "<< start <<" to "
		 << end <<" : "<< radlength 
		 <<" [mm]" << endmsg;
        if( radlength > threshold ){
	  if(msgLevel(MSG::DEBUG))
	    debug()<<"RV is too closed to a detector material --> disguarded !"
		   << endmsg;
          return true;
        }
      }
    }
  } // end of 3 and 4 cond
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
    if( point.x() > -2. ){ //right half
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
// Save Preys infos in tuple
//=============================================================================
StatusCode RecVertices2Particles::SavePreysTuple( Tuple & tuple, Particle::ConstVector & RecParts ){

  vector<int>  nboftracks;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts, indets, muons;
  
  Particle::ConstVector::const_iterator iend = RecParts.end();
  for( Particle::ConstVector::const_iterator is = RecParts.begin();
       is < iend; ++is ){
    const Particle * p = (*is);

    int nbtrks = p->endVertex()->outgoingParticles().size();
    double chi = p->endVertex()->chi2PerDoF();
    const Gaudi::XYZPoint & pos = p->endVertex()->position();
    Gaudi::LorentzVector mom = p->momentum();
    double sumpt = GetSumPt(p);
    double muon = HasMuons(p);
    const Gaudi::SymMatrix3x3 & err = p->endVertex()->covMatrix();

    nboftracks.push_back( nbtrks ); chindof.push_back( chi );
    e.push_back(mom.e()); muons.push_back(muon);
    px.push_back(mom.x()); py.push_back(mom.y()); pz.push_back(mom.z());
    x.push_back(pos.x()); y.push_back(pos.y()); z.push_back(pos.z());
    errx.push_back(sqrt(err(0,0))); erry.push_back(sqrt(err(1,1)));
    errz.push_back(sqrt(err(2,2)));

    double indet = 0;
    if( IsAPointInDet( *p, 2 ) ) indet += 1;
    if( IsAPointInDet( *p, 3, 2 ) ) indet += 10;
    if( IsAPointInDet( *p, 4, 2 ) ) indet += 100;
    if( IsAPointInDet( *p, 5, 2 ) ) indet += 1000;
    indets.push_back( indet );
    sumpts.push_back(sumpt);
  }
  const int NbPreyMax = 20;
  if( !SaveCaloInfos(tuple)  ) return StatusCode::FAILURE;
  if( !fillHeader(tuple) ) return StatusCode::FAILURE;
  tuple->farray( "PreyPX", px.begin(), px.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyPY", py.begin(), py.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyPZ", pz.begin(), pz.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyPE", e.begin(), e.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyXX", x.begin(), x.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyXY", y.begin(), y.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyXZ", z.begin(), z.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyerrX", errx.begin(), errx.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyerrY", erry.begin(), erry.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyerrZ", errz.begin(), errz.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreySumPt", sumpts.begin(), sumpts.end(), 
		 "NbPrey", NbPreyMax );
  tuple->farray( "Muon", muons.begin(), muons.end(), "NbPrey", NbPreyMax );
  tuple->farray( "InDet", indets.begin(),indets.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyNbofTracks", nboftracks.begin(), nboftracks.end(),
		 "NbPrey", NbPreyMax );
  tuple->farray( "PreyChindof", chindof.begin(), chindof.end(),
		 "NbPrey", NbPreyMax );
  tuple->column( "BLX", m_BeamLine->referencePoint().x() );
  tuple->column( "BLY", m_BeamLine->referencePoint().y() );
  tuple->column( "BLZ", m_BeamLine->referencePoint().z() );
  if( !SavePVs( tuple )  ) return StatusCode::FAILURE;
  tuple->column( "NbVelo", GetNbVeloTracks() );
  return tuple->write();
}

//============================================================================
// Save in Tuple the PV candidates
//============================================================================
StatusCode  RecVertices2Particles::SavePVs( Tuple & tuple ){
  
  GetPVs();
  vector<double> x,y,z;
  for ( vector<const RecVertex*>::const_iterator i = PVs.begin(); 
        i != PVs.end() ; ++i ){
    const RecVertex* pv = *i;
    x.push_back( pv->position().x() );
    y.push_back( pv->position().y() );
    z.push_back( pv->position().z() );
  }
  const int NbPVMax = 20;
  tuple->farray( "PVX", x.begin(), x.end(), "NbPVMax", NbPVMax );
  tuple->farray( "PVY", y.begin(), y.end(), "NbPVMax", NbPVMax );
  tuple->farray( "PVZ", z.begin(), z.end(), "NbPVMax", NbPVMax );

  return StatusCode::SUCCESS ;
}

//============================================================================
//  Event number 
//============================================================================
StatusCode RecVertices2Particles::fillHeader( Tuple & tuple ){
  const RecHeader* header = 
    get<RecHeader>(RecHeaderLocation::Default);  
  //debug() << "Filling Tuple Event " << header->evtNumber() << endmsg ;
  tuple->column("Event", (int)header->evtNumber());
  tuple->column("Run", (int)header->runNumber());

  LHCb::ODIN * odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );
  if( odin ){
    //NoBeam = 0, Beam1 = 1, Beam2 = 2, BeamCrossing = 3
    tuple->column("BXType", 
                  static_cast<unsigned int>( odin->bunchCrossingType()  ) );
//     //tuple->column("Event", odin->eventNumber()); //ulonglong !
//     tuple->column("Run", odin->runNumber());
//     tuple->column("BunchID", odin->bunchId());
//     tuple->column("BunchCurrent", odin->bunchCurrent());
//     //tuple->column("GpsTime", (double)odin->gpsTime()); //ulonglong !
//     //tuple->column("EventTime", odin->eventTime() );
//     tuple->column("OrbitNumber", odin->orbitNumber());
  
  if( msgLevel( MSG::DEBUG ) && false )
    debug() <<"Reading of ODIN banks : Event nb "<< odin->eventNumber() 
            <<" Run nb "<< odin->runNumber() <<" BunchID "<< odin->bunchId() 
            <<" BunchCurrent "<< odin->bunchCurrent() <<" GpsTime "
            << odin->gpsTime() <<" EventTime " << odin->eventTime() 
            <<" OrbitNumber "<< odin->orbitNumber() <<" Bunch Crossing Type " 
            << odin->bunchCrossingType() << endmsg;
  
  
  } else {
    Warning("Can't get LHCb::ODINLocation::Default");
  }


  return StatusCode::SUCCESS ;
}

//============================================================================
//  Save Calorimeter info for a Prey
//============================================================================
StatusCode RecVertices2Particles::SaveCaloInfos( Tuple& tuple ){
  double E = 0; double Et = 0.;
  StatusCode sc = GetCaloInfos( "Ecal", E, Et ) && 
    GetCaloInfos( "Hcal", E, Et ) &&
    //GetCaloInfos( "Prs", E, Et ) && GetCaloInfos( "Spd", E, Et ) &&
    GetCaloInfos( "Muon", E, Et );
  tuple->column( "TotEt", Et );
  return sc;  
}

StatusCode RecVertices2Particles::GetCaloInfos( string CaloType, double& En, double& Et ){
  double EC = 0; double EtC = 0.;

  if( CaloType == "Muon" ){

    const MuonPIDs* pMU = get<MuonPIDs>( MuonPIDLocation::Default );
    for(  MuonPIDs::const_iterator imu = pMU->begin() ; 
	  imu !=  pMU->end() ; ++imu ){
      const MuonPID* myMu = *imu;
      const LHCb::Track* myTrk = myMu->idTrack();
      double Q = myTrk->charge();
      double CloneDist = myTrk->info(LHCb::Track::CloneDist,9999.); 
      if (Q==0.) { continue; }
      if (CloneDist!=9999.) { continue; }
      
      double myP = myTrk->p();
      double mE = sqrt((myP*myP) + 105.66*105.66)/ GeV;
      double mET = mE*sqrt(myTrk->position().Perp2()/myTrk->position().Mag2());
//       debug() << "P (GeV) : " << myP / Gaudi::Units::GeV  
// 	      << " is muon=" << (*imu)->IsMuon() << endmsg;
      EC += mE;
      EtC += mET;

    }

  } else {
    double x=0,y=0,z=0;
    //CaloDigitLocation::Spd
    const CaloDigits*  digitsCalo = get<CaloDigits>("Raw/"+CaloType+"/Digits");
    //Nothing in here...
    //const CaloDigits*  digitsCalo = get<CaloDigits>("Raw/"+CaloType+"/AllDigits");
    //Nothing in here...
    //const CaloDigits*  digitsCalo = get<CaloDigits>("Raw/"+CaloType+"/Hlt1Digits");

    //DeCalorimeterLocation::Spd
    const DeCalorimeter*  Dcalo = getDet<DeCalorimeter>
      ( "/dd/Structure/LHCb/DownstreamRegion/"+CaloType );

    for ( CaloDigits::const_iterator idigit=digitsCalo->begin(); 
	  digitsCalo->end()!=idigit ; ++idigit ){  
      const CaloDigit* digit = *idigit ;
      if ( 0 == digit ) { continue ; }
      // get unique calorimeter cell identifier
      const CaloCellID& cellID = digit->cellID() ;
      // get the energy of the digit
      const double e = digit->e()  / Gaudi::Units::GeV ;
      // get the position of the cell (center)
      const Gaudi::XYZPoint& xcenter = Dcalo->cellCenter( cellID ) ;
      //Compute transverse energy !
      x = xcenter.x();
      y = xcenter.y();
      z = xcenter.z();
      EC += e;
      EtC+= e*sqrt( (x*x + y*y)/(x*x + y*y + z*z) );
    }
  }

  if( msgLevel( MSG::DEBUG ) )
    debug() << CaloType <<" : Total Energy "<< EC <<" GeV, total Et "<< EtC 
            << endmsg;
  
  En += EC;
  Et += EtC;
  return StatusCode::SUCCESS;
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
// Is the point in the RF-Foil ?
//=============================================================================
bool RecVertices2Particles::IsInRFFoil( const Gaudi::XYZPoint & pos){
  
  //debug()<<"Probing pos "<< pos;

  Gaudi::XYZPoint posloc;
  //move to local Velo half frame
  if( pos.x() < 0 ){ //right half
    posloc = m_toVeloRFrame * pos;
    //debug()<<", position in local R velo frame "<< pos << endmsg;

    //remove cylinder
    double r = posloc.rho();
    if( r > 5.5*mm && r < 12*mm ) return true;
 
    //then remove the boxes
    if( abs(posloc.y()) > 5.5*mm && posloc.x() < -5*mm && posloc.x() > 4*mm ) 
      return true;
  } else { //left part
    posloc = m_toVeloLFrame * pos;
    //debug()<<", position in local L velo frame "<< pos << endmsg;

    //remove cylinder
    double r = posloc.rho();
    if( r > 5.5*mm && r < 12*mm ) return true;
    
    //then remove the boxes
    if( abs(posloc.y()) > 5.5*mm && posloc.x() < 5*mm && posloc.x() > -4*mm ) 
      return true;
  }
  return false;
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
// Compute the sum pT of daughter tracks
//=============================================================================
double RecVertices2Particles::GetSumPt( const Particle * p ){

  double sumpt = 0;
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i = 
         p->daughters().begin(); i != iend; ++i ){
    sumpt += i->target()->pt();
  }
  return sumpt;
}

double RecVertices2Particles::GetSumPt( const LHCb::RecVertex * rv ){
  double sumpt = 0;
  SmartRefVector< Track >::const_iterator iend = rv->tracks().end();
  for( SmartRefVector< Track >::const_iterator i = rv->tracks().begin();
       i < iend; ++i ){
    //cout<<"Track "<< i->target()->type() <<" momentum "
    //  << i->target()->momentum() <<" pt "<< i->target()->pt() << endl;
    if( i->target()->chi2PerDoF() > m_TChi2 ) continue;
    sumpt += i->target()->pt();
  }
  return sumpt;
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
    //cout<<"Track "<< i->target()->type() <<" momentum "
    //  << i->target()->momentum() <<" pt "<< i->target()->pt() << endl;
    if( i->target()->chi2PerDoF() > m_TChi2 ) continue;
    pttot += i->target()->pt();
    nb++;
  }
};




//=============================================================================
// Distance between two vertices
//=============================================================================
double RecVertices2Particles::VertDistance( const Gaudi::XYZPoint & v1, 
					    const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}


//============================================================================
// Angle between two tracks   :   tracks are supposed to be straight lines
//============================================================================
double RecVertices2Particles::Angle( const Gaudi::LorentzVector & a, 
			     const Gaudi::LorentzVector & b ){
  return Angle( a.Vect(), b.Vect() );  
};

double RecVertices2Particles::Angle( const Gaudi::XYZVector & a, 
			     const Gaudi::XYZVector & b ){
  return acos( a.Dot(b) / sqrt( a.Mag2() * b.Mag2() ) );
}


//============================================================================
// Basic operations between two Gaudi::XYZPoint
//============================================================================
Gaudi::XYZPoint RecVertices2Particles::Plus( const Gaudi::XYZPoint & p1, 
				  const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint RecVertices2Particles::Minus( const Gaudi::XYZPoint& p1, 
				   const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}

double RecVertices2Particles::Mult( const Gaudi::XYZPoint & p1, 
			 const Gaudi::XYZPoint & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

double RecVertices2Particles::Mult(  const Gaudi::LorentzVector & p1, 
			  const Gaudi::LorentzVector & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

Gaudi::XYZPoint RecVertices2Particles::Normed( const Gaudi::LorentzVector & P,
                                       double range ){
  double norm = range/Norm( P );
  return Gaudi::XYZPoint( P.x()*norm, P.y()*norm, P.z()*norm );
}

double RecVertices2Particles::Norm( const Gaudi::LorentzVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}

double RecVertices2Particles::Norm( const Gaudi::XYZPoint & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}

double RecVertices2Particles::Norm( const Gaudi::XYZVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}


//=============================================================================
// Print out Track and Particle content
//=============================================================================
void RecVertices2Particles::PrintTrackandParticles(){

  //TrackLocation::Default = Rec/Track/Best 
  //(Upstream,Long,Ttrack,Downstream, Velo)
  const Track::Range BestTrks = get<Track::Range>( TrackLocation::Default );
  debug()<<"Dumping "<< TrackLocation::Default <<" Track content, size "
	 << BestTrks.size() <<endmsg;
  for(Track::Range::const_iterator itr = BestTrks.begin(); 
      BestTrks.end() != itr; ++itr) {
    const Track* trk = *itr;
    string s = "False !";
    if ( trk->checkFlag( Track::Backward ) ) s = "True !";
    debug()<<"Track key "<< trk->key() //<<" slope "<< trk->slopes() 
	   <<" type "<<trk->type()<<" Is backward ? "<< s 
	   <<" momentum "<< trk->momentum() <<" pt "<< trk->pt() <<endmsg;
  }

  debug()<<"---------------------------------------------------------"<<endmsg;
  const Particle::ConstVector inputParts = desktop()->particles();
  debug()<<"Dumping Particle content, size "
	 << inputParts.size() <<endmsg;
  for ( Particle::ConstVector::const_iterator j = inputParts.begin();
	j != inputParts.end();++j) {
    if( (*j)->proto()->track() == NULL ) continue;
    const Track * trk = (*j)->proto()->track();
    debug()<<"Particle id "<< (*j)->particleID().pid() <<" key "
	   << (*j)->key() <<" Mass "<< (*j)->measuredMass()/1000. << " GeV" 
      //<<" slope "<< (*j)->slopes()
      //<<" Mass "<< (*j)->momentum().M()
	   <<" type "<< trk->type() 
	   <<" momentum "<< (*j)->momentum() <<" pt "<< (*j)->pt() << endmsg;
  }
}


//=============================================================================
// Study in details the composition of the Prey's daughters (id..)
//=============================================================================
void RecVertices2Particles::StudyPreyComposition( const Particle * p ){

  if( m_measProvider == 0){
    warning()<<"Turn on the IMeasurementProvider tool"<< endmsg;
    return;
  } 

  Gaudi::XYZPoint pos = p->endVertex()->position();
  if( msgLevel(MSG::DEBUG) ){
    debug()<<"----------------------------------------" << endmsg;
    debug() <<"Particle Mass " << p->measuredMass() << ", Pos of end vertex " 
	    << pos << ", R "<< pos.rho() << ", Number of associated tracks " 
	    << p->daughtersVector().size() << endmsg;
    debug()<<"--------------- Composition -----------" << endmsg;
  }

  int nbe = 0; //count number of electrons
  int nbz = 0; //nb of tracks with first z measurement after RV position
  int nbm = 0; //nb of 2 by 2 null rec masses 
  int nbpairs = 0;      //Nb of daughter pairs
  double meanangle = 0; //mean angle between pair of particles
  double maxangle = 0;  //max angle between pair of particles

  //Let's loop on the daughters
  SmartRefVector< Particle >::const_iterator iend = p->daughters().end();
  SmartRefVector< Particle >::const_iterator i = p->daughters().begin();
  for( ; i != iend; ++i ){
    const Particle * d = i->target() ;
    int pid = d->particleID().pid();
    if( abs(pid) == 11 ) ++nbe;
    
    //Let's find the position of the first hit
    if( d->proto()->track() == NULL ) continue;
    const Track * tk = d->proto()->track();
    StatusCode sc = m_measProvider -> load( const_cast<Track &>( *tk ) );
    //if(!sc) debug()<<"Unable to recreate track measurements"<< endmsg;
    double z = tk->measurements().front()->z();
    debug()<<"ID "<< pid <<", z first meas. "
	   << z << endmsg;
    if( z < pos.z() ) ++nbz;
    
    //let's take the tracks 2 by 2 and look if there is some null rec mass
    SmartRefVector< Particle >::const_iterator j = i; ++j;
    while( j != iend ){
      //Compute angle between the 2 tracks
      ++nbpairs;
      double angle = Angle( j->target()->momentum(), d->momentum() );
      if( angle > maxangle  ) maxangle = angle;
      meanangle += angle;
      const Gaudi::LorentzVector l = d->momentum() + j->target()->momentum();
      plot( l.M()/1000., "PreyDgts2b2Mass", 0, 6 );
      plot( l.M()/1000., "PreyDgts2b2Mass2", 0, 0.5 );
      if( l.M() < 100 ) ++nbm;

      //debug()<<"Rec mass "<< l.M() << endmsg;
      ++j;
    }
    
  }
  meanangle = double( meanangle/nbpairs );
  //if( maxangle > pi ) maxangle -= pi;

  plot( nbe, "PreyDaughtersNbEl", 0, 10 );
  plot( nbz, "PreyZDgtsBefRV", 0, 10 );
  plot( nbm, "PreyDgts2b2MassNb", 0, 10);
  plot( maxangle, "PreyDgtsMaxAngle", 0., pi/2);
  plot( meanangle, "PreyDgtsMeanAngle", 0., pi/4 );
  //Study Boost
  plot( p->momentum().Gamma(), "PreyGamma", 0, 30. );
  plot( p->momentum().BoostToCM().z(), "PreyGammaZ", -1., -0.93 );

  debug() <<"Mean Prey Gamma "<< p->momentum().Gamma() <<", z boost "
	  << p->momentum().BoostToCM().z() << endmsg;

}

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
	    << rightcenter <<", left half center "<< lefthalv << endmsg;
  //matrix to transform to local velo frame
  m_toVeloRFrame = halfrgeominfo->toLocalMatrix() ;
  //m_toGlobalFrame = halfgeominfo->toGlobalMatrix();
  m_toVeloLFrame = halflgeominfo->toLocalMatrix() ;
  if(m_RemVtxFromDet==5 || m_SaveTuple){
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
