// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

//get the Header of the event
#include "Event/RecHeader.h"

//Use ODIN
#include "Event/ODIN.h"

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
    , m_pt(400.)
{
  declareProperty("SaveTuple", m_SaveTuple = false );//save prey infos in Tuple
  declareProperty("Prey", m_Prey = "~chi_10" );
  //Bc+ Mass 6.286GeV
  //declareProperty("PreyMinMass", m_PreyMinMass = 6.3*Gaudi::Units::GeV );
  declareProperty("PreyMinMass", m_PreyMinMass = 0.0 );
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*Gaudi::Units::TeV );
  declareProperty("RMin", m_RMin = 0.3*Gaudi::Units::mm );
  declareProperty("RMax", m_RMax = 50.*Gaudi::Units::m );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks
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
		  m_BLLoc = "HLT/Hlt2LineDisplVertices/BeamLine");
  declareProperty("UseMap", m_UseMap = false );
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
  if( m_UseMap ) m_Fitter = "none";
    
  

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
    info()<<"Reconstructed Mass of the RecVertex"<< endmsg;
    info()<<"Min Mass : " << m_PreyMinMass/GeV <<" GeV"<< endmsg;
    info()<<"Max Mass : " << m_PreyMaxMass/GeV <<" GeV"<< endmsg;
    info()<<"Minimum number of tracks at the RecVertex : "
          << m_nTracks <<" tracks."<< endmsg;
    info()<< "The radial displacement is ";
    if( m_RCut == "FromUpstreamPV" ){
      info()<< "computed with respect to the upstream PV of PV3D." << endmsg;
    } else if( m_RCut == "FromBeamLine" ){
      info()<< "computed with respect to the beam line given at " 
            << m_BLLoc << endmsg;
    } else {
      info()<< "computed with respect to (0,0,z) in the global LHCb frame" 
            << endmsg;
      info()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!" 
            << endmsg;
    }
    info()<<"Min R    : " << m_RMin/mm <<" mm"<< endmsg ;
    info()<<"Max R    : " << m_RMax/mm <<" mm"<< endmsg ;
    info()<<"Particles will be made with "<< m_Prey <<" id" << endmsg ;
    info()<<"Vertex fitter used : "<< m_Fitter << endmsg ;
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
  if( m_RCut !="FromBeamLine" ) m_BeamLine = new Particle();

  //Set beam line to z axis
  if( m_RCut =="" ){
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( 0., 0., 0. ) );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  } 

  if( m_RemFromRFFoil || m_RemVtxFromDet == 4){
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
  //return StatusCode::SUCCESS;

  //Set the beam line
  if( m_RCut=="FromBeamLine" ){
    if( exist<Particles>( m_BLLoc ) ){
      Particles* BL = get<Particles>( m_BLLoc );      
      m_BeamLine = *(BL->begin());
      if( msgLevel(MSG::DEBUG) )
        debug()<<"Beam line position "<< m_BeamLine->referencePoint()
               <<" direction " << m_BeamLine->momentum() << endmsg;
    } else {
      warning()<<"No Beam line found at "<< m_BLLoc << endmsg;
      return StatusCode::SUCCESS;
    }
  } else if( m_RCut=="FromUpstreamPV" ){
    const RecVertex * UpPV = GetUpstreamPV();
    if(UpPV == NULL) return StatusCode::SUCCESS;
    m_BeamLine->setReferencePoint( UpPV->position() );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
    if(msgLevel(MSG::DEBUG))
      debug() <<"Upstream PV position "<< UpPV->position() << endmsg;
  }

  if( m_UseMap ) m_map.clear(); //Re-initialize the map

  //Retrieve data Particles from Desktop.
  const Particle::ConstVector & Parts = desktop()->particles();
  int size = Parts.size() ;
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Particles in TES " << size << endmsg;
  if( context() == "Info" ) plot( size,"NbofPartsTES", 0,500 );


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
  for( ; i != iend; ++i) {
    const RecVertex* rv = *i;
    const SmartRefVector< Track > & Tracks = rv->tracks();
    size = Tracks.size();
    double r = RFromBL( rv->position() ); //R to beam line
    if( msgLevel(MSG::DEBUG) ){
      debug() << "------------- Reconstructed Vertex -----------" << endmsg;
      debug() << "Position " << rv->position() <<" R "<< r
	      << " Number of associated tracks " << size 
	      << " Chi2/NDoF " << rv->chi2PerDoF() << endmsg;
    }

    //Do not change the PV into a particle ! (if any !)
    if( !m_KeepLowestZ && i == RV.begin() ) continue;

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

    //Turn it into a Particle !
    //Eventually don't keep it if close to/in detector material
    if( !RecVertex2Particle( rv, Parts, RecParts ) ) continue;

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
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RecVertices2Particles::finalize() {

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

//=============================================================================
// Get the upstream PV
//=============================================================================
const RecVertex * RecVertices2Particles::GetUpstreamPV(){

  const RecVertex::Container * PVs = desktop()->primaryVertices();
  const RecVertex * upPV = NULL;
  double tmp = 1000;
  for ( RecVertex::Container::const_iterator i = PVs->begin(); 
	i != PVs->end() ; ++i ){
    double z = (*i)->position().z();
    if( z < tmp ){
      tmp = z;
      upPV = (*i);
    } 
  }
  return upPV;
}


//=============================================================================
// Turn RecVertices into Particles (from Parts) saved in RecParts
//=============================================================================
bool RecVertices2Particles::RecVertex2Particle( const RecVertex* rv,
                                const Particle::ConstVector & Parts,
                                Particle::ConstVector & RecParts ){  

  Gaudi::LorentzVector mom;
  SmartRefVector< Track >::const_iterator iVtx = rv->tracks().begin();
  SmartRefVector< Track >::const_iterator iVtxend = rv->tracks().end();
  int endkey = rv->tracks().back()->key();

  //Create map if necessary
  if( m_UseMap && m_map.empty() ) CreateMap( Parts );

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
 
    if( m_UseMap ){
      //Loop on RecVertex daughter tracks and save corresponding Particles
      for( ; iVtx != iVtxend; ++iVtx ){
        //debug()<<"Key "<< (*iVtx)->key() <<" type "
        //    <<(*iVtx)->type()  <<" slope "<< (*iVtx)->slopes() << endmsg;
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
    } else {

      //Find all particles that have tracks in RecVertex
      Particle::ConstVector::const_iterator jend = Parts.end();
      for ( Particle::ConstVector::const_iterator j = Parts.begin();
            j != jend;++j) {

        if( (*j)->proto() == NULL ) continue;
        if( (*j)->proto()->track() == NULL ) continue;
        const Track * tk = (*j)->proto()->track();

        while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
          ++iVtx;
        }

        if( (*iVtx)->key() == tk->key() ){ 
          //debug()<<"Track should be the same "<< (*iVtx)->momentum() <<" "
          //     << tk->momentum() << endmsg;
          //debug() <<"Track type "<< tk->type() << endmsg;
          if( (*iVtx)->key() != endkey ) ++iVtx; 
          tmpVtx.addToOutgoingParticles ( *j );
          tmpPart.addToDaughters( *j );
          mom += (*j)->momentum();
          continue;
        }
      }
    }
    //Should always be 100%
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

    //Remove if found to be in detector material
    if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) return false;

    //Save Rec Particle in the Desktop
    RecParts.push_back( desktop()->keep( &tmpPart ) );
    
  } else {

    Particle::ConstVector Daughters;
    Particle::ConstVector::const_iterator jend = Parts.end();
    for ( Particle::ConstVector::const_iterator j = Parts.begin();
	  j != jend;++j) {
      if( (*j)->proto()->track() == NULL ) continue;
      const Track * tk = (*j)->proto()->track();
      while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }
      if( (*iVtx)->key() == tk->key() ){ 
        //debug()<<"Track should be the same "<< (*iVtx)->momentum() <<" "
        //     << tk->momentum() << endmsg;
        //debug() <<"Track type "<< tk->type() << endmsg;
        if( (*iVtx)->key() != endkey ) ++iVtx; 
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

    //Remove if found to be in detector material
    if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) return false;
    
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
      debug() << "Pos of end vertex " << pos << " R "<< pos.rho() << endmsg;
      debug() << "Number of associated tracks " 
	      << RecParts.back()->daughtersVector().size() << endmsg;
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
  //plot( eff, "MapEff", 0., 105. ); //always 100%
  //plot( clone, "NbClonePart", 0., 10. );
  //debug() <<"eff "<< eff <<" nb of clone "<< clone <<" Nb of Part "
  //        << Parts.size() <<" Map size "<< m_map.size() << endmsg;
  //}
  
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
//  if = 3 : remove reco vtx if rad length along 
//                             +- DetDist * PositionCovMatrix
//  if = 4 : 3 but range+3 if in RF foil.
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
    Gaudi::SymMatrix3x3 RVPositionCovMatrix = RV->covMatrix();
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

  return false;
}

//=============================================================================
// Save Preys infos in tuple
//=============================================================================
StatusCode RecVertices2Particles::SavePreysTuple( Tuple & tuple, Particle::ConstVector & RecParts ){

  vector<int>  nboftracks;
  vector<double> chindof, px, py, pz, e, x, y, z, sumpts, indets;
  
  Particle::ConstVector::const_iterator iend = RecParts.end();
  for( Particle::ConstVector::const_iterator is = RecParts.begin();
       is < iend; ++is ){
    const Particle * p = (*is);

    int nbtrks = p->endVertex()->outgoingParticles().size();
    double chi = p->endVertex()->chi2PerDoF();
    const Gaudi::XYZPoint & pos = p->endVertex()->position();
    Gaudi::LorentzVector mom = p->momentum();
    double sumpt = GetSumPt(p);

    nboftracks.push_back( nbtrks ); chindof.push_back( chi );
    e.push_back(mom.e());
    px.push_back(mom.x()); py.push_back(mom.y()); pz.push_back(mom.z());
    x.push_back(pos.x()); y.push_back(pos.y()); z.push_back(pos.z());
    double indet = 0;
    if( IsAPointInDet( *p, 2 ) ) indet += 1;
    if( IsAPointInDet( *p, 3, 2 ) ) indet += 10;
    if( IsAPointInDet( *p, 4, 2 ) ) indet += 100;
    indets.push_back( indet );
    sumpts.push_back(sumpt);
  }
  const int NbPreyMax = 20;
  //if( !SaveCaloInfos(tuple)  ) return StatusCode::FAILURE;
  if( !fillHeader(tuple) ) return StatusCode::FAILURE;
  tuple->farray( "PreyPX", px.begin(), px.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyPY", py.begin(), py.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyPZ", pz.begin(), pz.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyPE", e.begin(), e.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyXX", x.begin(), x.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyXY", y.begin(), y.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyXZ", z.begin(), z.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreySumPt", sumpts.begin(), sumpts.end(), 
		 "NbPrey", NbPreyMax );
  tuple->farray( "InDet", indets.begin(),indets.end(), "NbPrey", NbPreyMax );
  tuple->farray( "PreyNbofTracks", nboftracks.begin(), nboftracks.end(),
		 "NbPrey", NbPreyMax );
  tuple->farray( "PreyChindof", chindof.begin(), chindof.end(),
		 "NbPrey", NbPreyMax );
  tuple->column( "BLX", m_BeamLine->referencePoint().x() );
  tuple->column( "BLY", m_BeamLine->referencePoint().y() );
  
  return tuple->write();
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
double RecVertices2Particles::RFromBL( const Gaudi::XYZPoint& p ){
  
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
// Compute the sum pT of a bunch of track (daughters of a Particle)
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
  const Particle::ConstVector& inputParts = desktop()->particles();
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
