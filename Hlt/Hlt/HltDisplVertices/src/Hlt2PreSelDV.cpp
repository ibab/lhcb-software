// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

//get the Header of the event
#include "Event/RecHeader.h"

//Use ODIN
#include "Event/ODIN.h"

// local
#include "Hlt2PreSelDV.h"

//calorimeter
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"


using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2PreSelDV
//
// 2009-april-14 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt2PreSelDV );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2PreSelDV::Hlt2PreSelDV( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_vFit(0)
  , pi(3.1415926)
  , m_MapCalled(false)
  , m_pt(400.)
{ 
  declareProperty("SaveTuple", m_SaveTuple = false );//save prey infos in Tuple
  declareProperty("Prey", m_Prey = "~chi_10" );
  //Bc+ Mass 6.286GeV
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
  declareProperty("UseMap", m_UseMap = true );
  declareProperty("PVnbtrks", m_PVnbtrks = 10 ); //corr. to 'tight' PV reco
}
//=============================================================================
// Destructor
//=============================================================================
Hlt2PreSelDV::~Hlt2PreSelDV() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2PreSelDV::initialize() {

  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize the Hlt2PreSelDV algorithm" << endmsg;

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
  if( m_UseMap ) m_Fitter = "none";
    
  

  if(msgLevel(MSG::DEBUG)){
    debug()<<"-------------------------------------------------------"<<endmsg;
    debug()<<"Hlt2PreSelDV takes every RecVertex from "
          << m_RVLocation <<" and turns them into Particles if they satisfies "
          <<"the following criterias :"<< endmsg ;
    debug()<<"RecVertex has no backward tracks"<< endmsg ;
    if(!m_KeepLowestZ) 
      debug()<<"RecVertex has not the lowest z position (PV)"<< endmsg;
    if( m_RemFromRFFoil )
      debug()<<"Remove RecVertex if in RF-Foil region"<< endmsg ;
    if( m_RemVtxFromDet == 1 )
      debug()<<"Remove RecVertex if in detector material"<< endmsg;
    if( m_RemVtxFromDet == 2 )
      debug()<<"Remove RecVertex if closer than " << m_DetDist 
            <<"mm from detector material along momentum"<< endmsg;
    if( m_RemVtxFromDet == 3 || m_RemVtxFromDet == 4 )
      debug()<<"Remove RecVertex if closer than " << m_DetDist 
            <<"*PosCovMatric from detector material"<< endmsg;
    if( m_RemVtxFromDet == 4 )
      debug()<<"("<< m_DetDist+3 <<" when in RF-Foil region)"<< endmsg;
    debug()<<"Reconstructed Mass of the RecVertex"<< endmsg;
    debug()<<"Min Mass : " << m_PreyMinMass/GeV <<" GeV"<< endmsg;
    debug()<<"Max Mass : " << m_PreyMaxMass/GeV <<" GeV"<< endmsg;
    debug()<<"Minimum number of tracks at the RecVertex : "
          << m_nTracks <<" tracks."<< endmsg;
    debug()<< "The radial displacement is ";
    if( m_RCut == "FromUpstreamPV" ){
      debug()<< "computed with respect to the upstream PV of PV3D." << endmsg;
      debug()<< "Min nb of tracks on the upPV candidate : "
            << m_PVnbtrks << endmsg; 
    } else if( m_RCut == "FromBeamLine" ){
      debug()<< "computed with respect to the beam line given at " 
            << m_BLLoc << endmsg;
    } else {
      debug()<< "computed with respect to (0,0,z) in the global LHCb frame" 
            << endmsg;
      debug()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!" 
            << endmsg;
    }
    debug()<<"Min R    : " << m_RMin/mm <<" mm"<< endmsg ;
    debug()<<"Max R    : " << m_RMax/mm <<" mm"<< endmsg ;
    debug()<<"Particles will be made with "<< m_Prey <<" id" << endmsg ;
    debug()<<"Vertex fitter used : "<< m_Fitter << endmsg ;
    debug()<<"-------------------------------------------------------"<<endmsg;
  }
  

  //Initialize Vertex Fitter
  if( m_Fitter != "none" ){
    m_vFit = vertexFitter(m_Fitter);
  } 


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
  if( m_RCut =="" ){
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( 0., 0., 0. ) );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  } 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
 StatusCode Hlt2PreSelDV::execute() {

  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ++counter("Processed evt nb");
  if( msgLevel(MSG::DEBUG) )
    debug() << "==> Execute the Hlt2PreSelDV algorithm, event "
	    << counter("Processed evt nb").flag() << endmsg;

  //Check track and Particle content
  //PrintTrackandParticles();
  //return StatusCode::SUCCESS;
  m_GeoInit = false; //be sure the goe is initialised if needed.


  const RecVertex * UpPV = GetUpstreamPV();
  if(UpPV == NULL) return StatusCode::SUCCESS;

  //Set the beam line first, it may be used by GetUpstreamPV()
  if( m_RCut=="FromBeamLine"  ){
    if( exist<Particle::Range>( m_BLLoc ) ){
      const Particle::Range BL = get<Particle::Range>( m_BLLoc );      
      const LHCb::Particle* tmp = *(BL.begin());
      m_BeamLine->setReferencePoint( tmp->referencePoint() );
      m_BeamLine->setMomentum( tmp->momentum() );
      //always()<<"Beam line position "<< m_BeamLine->referencePoint()
      //<<" direction " << m_BeamLine->momentum() << endmsg;
      if( msgLevel(MSG::DEBUG) )
        debug()<<"Beam line position "<< m_BeamLine->referencePoint()
               <<" direction " << m_BeamLine->momentum() << endmsg;
    } else { 
        //always()<<"No Beam line found at "<< m_BLLoc << " uses upstream PV" << endmsg;
      if( msgLevel(MSG::DEBUG) )
        debug()<<"No Beam line found at "<< m_BLLoc << " uses upstream PV" << endmsg;
      m_BeamLine->setReferencePoint( UpPV->position() );
      m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
    }
  } 
  else if (m_RCut=="FromUpstreamPV"){
      m_BeamLine->setReferencePoint( UpPV->position() );
      m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  }


  if( m_UseMap ){ 
    m_map.clear(); //Re-initialize the map
    m_MapCalled = false; //The map hasn't been called yet.
  }
  
      

  //Retrieve data Particles from Desktop.
  const Particle::ConstVector Parts = this->i_particles();
  int size = Parts.size() ;
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Particles in TES " << size << endmsg;
  

  //Retrieve Reconstructed Vertices
  RecVertex::ConstVector RV;
  GetRecVertices( RV );
  if( RV.size() < 1 ) return StatusCode::SUCCESS;

  //The PV is assumed to be the rec vertex with lowest z
  //sort rec vertex according to z position
  sort( RV.begin(), RV.end(), Hlt2PreSelDV::CondRVz() );


  //Print out displaced vertices and change them into Particles
  //Particle::ConstVector RecParts;
  int nbRecParts(0);
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
    if( !m_KeepLowestZ ){
      if( i == RV.begin() ) continue;
      //Do not keep if before the upPV
      if( m_RCut=="FromUpstreamPV" && 
          rv->position().z() < m_BeamLine->referencePoint().z() ) continue;
    }

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
    //InitialiseGeoInfo();
    //Eventually don't keep it if close to/in detector material
    if( !RecVertex2Particle( rv, Parts, nbRecParts, r ) ) continue;


  }
  size = nbRecParts;
  if( msgLevel(MSG::DEBUG) )
    debug()<<"# of Preys " << size << endmsg;

  //Save Preys infos in tuple
  /*if( m_SaveTuple ){
    Tuple tuple = nTuple("DisplVertices"); //defines a tuple to save infos
    if( SavePreysTuple( tuple, RecParts ).isFailure() )
      warning()<<"Impossible to fill tuple with candidate infos"<<endmsg;
      }*/

  //Save Preys from Desktop to the TES.
  //Note : both solutions work fine in any Hlt2 line sequence.
  //   Sole difference : OutputStream is not able to write objects containing 
  //       default pions as daughters saved with the cloneTrees method.
  //return desktop()->cloneTrees( RecParts );
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2PreSelDV::finalize() {

  if( m_RCut !="FromBeamLine" ) delete m_BeamLine;

  if( msgLevel( MSG::DEBUG ) ) debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize(); 
}

//=============================================================================
// Get the reconstructed vertices
//=============================================================================
void Hlt2PreSelDV::GetRecVertices( RecVertex::ConstVector & RV )
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
  
}

//=============================================================================
// Get the upstream PV
//=============================================================================
const RecVertex * Hlt2PreSelDV::GetUpstreamPV(){

  const RecVertex::Range PVs = this->primaryVertices();
  const RecVertex * upPV = NULL;
  double tmp = 1000;
  for ( RecVertex::Range::const_iterator i = PVs.begin(); 
        i != PVs.end() ; ++i ){
    const RecVertex* pv = *i;
    //Apply some cuts
    if( abs(pv->position().x()>1.5*mm) || abs(pv->position().y()>1.5*mm))
      continue;
    double z = pv->position().z();
    if( abs(z) > 150*mm ) continue;
    if( !HasBackAndForwardTracks( pv ) ) continue;
    //const Gaudi::SymMatrix3x3  & mat = pv->covMatrix();
    //double sr = sqrt( mat(0,0) + mat(1,1) );
    if( msgLevel( MSG::DEBUG ) )
      debug() <<"PV candidate : nb of tracks "<< pv->tracks().size() << endmsg;
    //<<" sigmaR "<< sr <<" sigmaZ "<< sqrt(mat(2,2)) << endmsg;
    //if( sr > m_PVsr ) continue;
    //if( sqrt(mat(2,2)) > m_PVsz ) continue;
    if( pv->tracks().size() < m_PVnbtrks ) continue;
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
bool Hlt2PreSelDV::RecVertex2Particle( const RecVertex* rv,
                                const Particle::ConstVector & Parts,
				       int& nbRecParts, double r ){  

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
    tmpPart.addInfo(52,r ); 
    //Store 51 info if found to be in detector material
    //always()<<"before checking material"<<endreq;
    //if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) tmpPart.addInfo(51,1.);
    //always()<<"after checking material"<<endreq;
 
 
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

        if( it == m_map.end() ) {
	  part = DefaultParticle(*iVtx);
	}
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
          if( (*iVtx)->key() != endkey ) ++iVtx; 
          tmpVtx.addToOutgoingParticles ( (*j)->clone() );
          tmpPart.addToDaughters( (*j)->clone() );
          mom += (*j)->momentum();
          continue;
        }
      }
    }

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

    tmpPart.setEndVertex( tmpVtx.clone() );
    tmpPart.setReferencePoint( point );

    //Remove if found to be in detector material
    if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) return false;

    this->markNewTree(tmpPart.clone());
    
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
        if( (*iVtx)->key() != endkey ) ++iVtx; 
        Daughters.push_back( (*j)->clone() );
        continue;
      }
    }
    
    //Make particle !
    Vertex* tmpVtx = new Vertex();; 
    Particle tmpPart;
    tmpPart.addInfo(52,r ); 
    //Store 51 info if found to be in detector material
    //if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) tmpPart.addInfo(51,1.);
 
    if( !( m_vFit->fit( Daughters, *tmpVtx, tmpPart ) ) ){
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
    //if( IsAPointInDet( tmpPart, m_RemVtxFromDet, m_DetDist ) ) return false;
    nbRecParts++;
    this->markNewTree(tmpPart.clone());
  }
  setFilterPassed(true);

  /*if( msgLevel(MSG::DEBUG) ){
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
    }*/
  return true;
}

//=============================================================================
// Create a map between Particles and their Velo tracks ancestors
//=============================================================================
void Hlt2PreSelDV::CreateMap( const Particle::ConstVector & Parts ){

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
  //debug() <<"eff "<< eff <<" nb of clone "<< clone <<" Nb of Part "
  //        << Parts.size() <<" Map size "<< m_map.size() << endmsg;
  //}

  m_MapCalled = true;
  
  return;
}

//=============================================================================
// Create default pions with 400 MeV pt, see p. 128
//=============================================================================
const Particle * Hlt2PreSelDV::DefaultParticle( const Track * p ){

  double sx = p->slopes().x(); double sy = p->slopes().y();
  double pz = m_pt/sqrt( sx*sx + sy*sy );
  double e = std::sqrt( m_piMass*m_piMass + m_pt*m_pt + pz*pz );
  Particle pion;
  const Gaudi::LorentzVector mom = Gaudi::LorentzVector(sx*pz, sy*pz, pz,e );
  pion.setMomentum(mom);

  //debug()<<"Creating default pion for key "<< p->key() <<" and slopes "
  //     <<sx<<" "<<sy<<" yielding momentum "<< mom << endmsg;

  return pion.clone();
  }

//=============================================================================
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool Hlt2PreSelDV::HasBackwardTracks( const RecVertex* RV ){
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
bool Hlt2PreSelDV::HasBackAndForwardTracks( const RecVertex* RV ){
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
double Hlt2PreSelDV::HasMuons( const Particle * p ){

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

bool Hlt2PreSelDV::IsIsolated( const RecVertex* rv, RecVertex::ConstVector & RV ){

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
bool Hlt2PreSelDV::IsAPointInDet( const Particle & P, int mode, 
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

bool Hlt2PreSelDV::IsInMaterialBoxLeft(const Gaudi::XYZPoint& point){
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

bool Hlt2PreSelDV::IsInMaterialBoxRight(const Gaudi::XYZPoint& point){
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
StatusCode Hlt2PreSelDV::SavePreysTuple( Tuple & tuple, 
                                         Particle::ConstVector & RecParts ){

  //Save Global Cut Event
  if( false ) SaveGEC( tuple, RecParts );

  InitialiseGeoInfo();
  vector<int>  nboftracks;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts, indets, muons, minvd, minvdchi2;

  
  const RecVertex::Range PVs = this->primaryVertices();
  //double tmp = 1000;
  std::vector<const RecVertex*> SelectedPVs; 
  for ( RecVertex::Range::const_iterator i = PVs.begin(); 
        i != PVs.end() ; ++i ){
    const RecVertex* pv = *i;
    //Apply some cuts
    if( abs(pv->position().x()>1.5*mm) || abs(pv->position().y()>1.5*mm))
      continue;
    double z = pv->position().z();
    if( abs(z) > 150*mm ) continue;
    if( !HasBackAndForwardTracks( pv ) ) continue;
    SelectedPVs.push_back(pv);
  }
  
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
    const Gaudi::SymMatrix3x3 & Cov = p->endVertex()->covMatrix();

    //get distance to closest PV
    double mindist = 1000000000.;
    for (std::vector<const RecVertex*>::const_iterator ipv = SelectedPVs.begin(); ipv != SelectedPVs.end(); ipv++){
      if((p->endVertex()->position()-(*ipv)->position()).R()<mindist)mindist=(p->endVertex()->position()-(*ipv)->position()).R();
    }
    minvd.push_back(mindist);
    nboftracks.push_back( nbtrks ); chindof.push_back( chi );
    e.push_back(mom.e()); muons.push_back(muon);
    px.push_back(mom.x()); py.push_back(mom.y()); pz.push_back(mom.z());
    x.push_back(pos.x()); y.push_back(pos.y()); z.push_back(pos.z());
    errx.push_back(sqrt(Cov(0,0))); erry.push_back(sqrt(Cov(1,1)));
    errz.push_back(sqrt(Cov(2,2)));

    double indet = 0;
    if( IsAPointInDet( *p, 2 ) ) indet += 1;
    if( IsAPointInDet( *p, 3, 2 ) ) indet += 10;
    if( IsAPointInDet( *p, 4, 2 ) ) indet += 100;
    if( IsAPointInDet( *p, 5, 2 ) ) indet += 1000;
    indets.push_back( indet );
    sumpts.push_back(sumpt);
  }
  const int NbPreyMax = 20;
  if( !fillHeader(tuple) ) return StatusCode::FAILURE;
  if( !SaveCaloInfos(tuple)  ) return StatusCode::FAILURE;
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
  tuple->farray( "MinVD", minvd.begin(), minvd.end(),
		 "NbPrey", NbPreyMax );
  tuple->farray( "PreyChindof", chindof.begin(), chindof.end(),
		 "NbPrey", NbPreyMax );
  tuple->column( "BLX", m_BeamLine->referencePoint().x() );
  tuple->column( "BLY", m_BeamLine->referencePoint().y() );
  tuple->column( "BLZ", m_BeamLine->referencePoint().z() );
  //Save number of Velo tracks...
  const Track::Range & VeloTrks = get<Track::Range>( "Hlt/Track/Velo" );
  tuple->column( "NbVelo", VeloTrks.size() );
  return tuple->write();
}

//============================================================================
//  Event number 
//============================================================================
StatusCode Hlt2PreSelDV::fillHeader( Tuple & tuple ){

  int evtnb = 0;
  int runnb = 0;
  if( exist<RecHeader>(RecHeaderLocation::Default) ){  
    const RecHeader* header = 
      get<RecHeader>(RecHeaderLocation::Default);  
    evtnb = (int)header->evtNumber();
    runnb = (int)header->runNumber();
  }
  
  //debug() << "Filling Tuple Event " << header->evtNumber() << endmsg ;
  tuple->column("Event", evtnb );
  tuple->column("Run", runnb );


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
//  Save Total Transverse Energy in Calorimeters
//============================================================================
StatusCode Hlt2PreSelDV::SaveCaloInfos( Tuple& tuple ){

  double E = 0; double Et = 0.;
  StatusCode sc = GetCaloInfos( "Ecal", E, Et ) && 
    GetCaloInfos( "Hcal", E, Et ) &&
    //GetCaloInfos( "Prs", E, Et ) && GetCaloInfos( "Spd", E, Et ) &&
    GetCaloInfos( "Muon", E, Et );
  tuple->column( "TotEt", Et );
  return sc;  
}

StatusCode Hlt2PreSelDV::GetCaloInfos( string CaloType, double & En, double & Et ){

  double EC = 0; double EtC = 0.;

  if( CaloType == "Muon" ){

    if( !exist<MuonPIDs>( MuonPIDLocation::Default ) ){
      //warning()<<"No valid data at "<< MuonPIDLocation::Default << endmsg;
      return StatusCode::SUCCESS ;
    }
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
    if( !exist<CaloDigits>( "Raw/"+CaloType+"/Digits" ) ){
      //warning()<<"No valid data at "<< "Raw/"+CaloType+"/Digits" << endmsg;
      return StatusCode::SUCCESS ;
    }
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
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Apply some cuts on the Particle or Vertex
//=============================================================================
bool Hlt2PreSelDV::TestMass( Particle & part ){
  double mass = part.measuredMass();
  if( mass > m_PreyMinMass && mass < m_PreyMaxMass ) return true;
  return false;
}
bool Hlt2PreSelDV::TestMass( const Particle * part ){
  double mass = part->measuredMass();
  if( mass > m_PreyMinMass && mass < m_PreyMaxMass ) return true;
  return false;
}

//=============================================================================
// Is the point in the RF-Foil ?
//=============================================================================
bool Hlt2PreSelDV::IsInRFFoil( const Gaudi::XYZPoint & pos){
  
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
double Hlt2PreSelDV::RFromBL( const Gaudi::XYZPoint& p ){
  
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
double Hlt2PreSelDV::GetSumPt( const Particle * p ){

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
double Hlt2PreSelDV::VertDistance( const Gaudi::XYZPoint & v1, 
					    const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}


//============================================================================
// Angle between two tracks   :   tracks are supposed to be straight lines
//============================================================================
double Hlt2PreSelDV::Angle( const Gaudi::LorentzVector & a, 
			     const Gaudi::LorentzVector & b ){
  return Angle( a.Vect(), b.Vect() );  
};

double Hlt2PreSelDV::Angle( const Gaudi::XYZVector & a, 
			     const Gaudi::XYZVector & b ){
  return acos( a.Dot(b) / sqrt( a.Mag2() * b.Mag2() ) );
}


//============================================================================
// Basic operations between two Gaudi::XYZPoint
//============================================================================
Gaudi::XYZPoint Hlt2PreSelDV::Plus( const Gaudi::XYZPoint & p1, 
				  const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint Hlt2PreSelDV::Minus( const Gaudi::XYZPoint& p1, 
				   const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}

double Hlt2PreSelDV::Mult( const Gaudi::XYZPoint & p1, 
			 const Gaudi::XYZPoint & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

double Hlt2PreSelDV::Mult(  const Gaudi::LorentzVector & p1, 
			  const Gaudi::LorentzVector & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

Gaudi::XYZPoint Hlt2PreSelDV::Normed( const Gaudi::LorentzVector & P,
                                       double range ){
  double norm = range/Norm( P );
  return Gaudi::XYZPoint( P.x()*norm, P.y()*norm, P.z()*norm );
}

double Hlt2PreSelDV::Norm( const Gaudi::LorentzVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}

double Hlt2PreSelDV::Norm( const Gaudi::XYZPoint & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}

double Hlt2PreSelDV::Norm( const Gaudi::XYZVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}


//=============================================================================
// Print out Track and Particle content
//=============================================================================
void Hlt2PreSelDV::PrintTrackandParticles(){

  //TrackLocation::Default = Rec/Track/Best 
  //(Upstream,Long,Ttrack,Downstream, Velo)
  const Track::Range & BestTrks = get<Track::Range>( TrackLocation::Default );
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
  const Particle::ConstVector& inputParts = this->i_particles();
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

//============================================================================
// Save in Tuple some Global Event Cut
//============================================================================
StatusCode  Hlt2PreSelDV::SaveGEC( Tuple & tuple,  
				    Particle::ConstVector & RVs ){

  //Global track cuts
  double sumPtTracks = 0.;
  double sumXYTrackfirstStates = 0.;

  //Get forward tracks
  const Track::Range & inputTracks = get<Track::Range>(TrackLocation::Default);

  for(Track::Range::const_iterator itr = inputTracks.begin(); 
      inputTracks.end() != itr; itr++) {
    const Track* trk = *itr;
    const double xyfState = sqrt(trk->firstState().x() * trk->firstState().x() +
			   trk->firstState().y() * trk->firstState().y());
    sumPtTracks += trk->pt();
    sumXYTrackfirstStates += xyfState;
  }

  //Find the upstream PV
  const RecVertex::Range primVertices = this->primaryVertices();
  if((primVertices.size() == 0) && inputTracks.size() == 0)
    return StatusCode::FAILURE; 
  vector<const RecVertex*> primVrtcs;
  for( RecVertex::Range::const_iterator 
        itPV = primVertices.begin(); primVertices.end() != itPV; ++itPV) {
    const RecVertex* pvtx = *itPV;
    primVrtcs.push_back(pvtx);
  }
  std::sort( primVrtcs.begin(), primVrtcs.end(), SortPVz );
  const RecVertex* realPV = *(primVrtcs.begin());

  //Global RV cut
  double sumSVxyDist = 0.;
  for( Particle::ConstVector::const_iterator itRV = RVs.begin();
       RVs.end() != itRV; ++itRV) {
    const Gaudi::XYZPoint & pos = (*itRV)->endVertex()->position();
    double distVtcs = VertDistance( realPV->position(), pos );
    if(distVtcs > .001) 
      sumSVxyDist += pos.rho();
  }
  
  //Write values in tuple
  tuple->column( "sumPtTracks", sumPtTracks );
  tuple->column( "sumXYTrackfirstStates", sumXYTrackfirstStates );
  tuple->column( "sumSVxyDist", sumSVxyDist );

  if(msgLevel(MSG::DEBUG))
    debug()<<"Global event values : sumPtTracks "<< sumPtTracks/GeV 
	   <<" GeV, sumXYTrackfirstStates "<< sumXYTrackfirstStates 
	   <<" mm, sumSVxyDist "<< sumSVxyDist <<" mm" << endmsg;

  return StatusCode::SUCCESS ;
}

void Hlt2PreSelDV::InitialiseGeoInfo(){
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
