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

#include "LoKi/BeamLineFunctions.h"
#include "LoKi/LoKiPhys.h"
#include "LoKi/LoKiTrigger.h"
#include "LoKi/ParticleContextCuts.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices.h"
#include "LoKi/Algo.h"



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
  : LoKi::Algo ( name , pSvcLocator )
  , pi(3.1415926)
  , m_MapCalled(false)
  , m_pt(400.)
{ 
  declareProperty("Prey", m_Prey = "~chi_10" );
  declareProperty("PreyMinMass", m_PreyMinMass = 0.0 );
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*Gaudi::Units::TeV );
  declareProperty("RMin", m_RMin = 0.3*Gaudi::Units::mm );
  declareProperty("RMax", m_RMax = 50.*Gaudi::Units::m );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks
  declareProperty("RecVerticesLocation", m_RVLocation );
  declareProperty("KeepLowestZ", m_KeepLowestZ = false  );
  declareProperty("ComputeMatterVeto", m_computeMV = false  );
  declareProperty("UseMap", m_UseMap = true );
  //declareProperty("GatherParticles", m_GatherParticles = true );
  declareProperty("PVnbtrks", m_PVnbtrks = 10 ); //corr. to 'tight' PV reco
  declareProperty("UseVeloTracks",   m_useVeloTrack = false );
  declareProperty("FractionEFrom1Track",   m_fractE = 0.85 );
  declareProperty("FractionTrackUpstreamV",   m_fractTrUpVertex = 0.49 );
  declareProperty("MaxChi2Long", m_chi2Long = 5. );
  declareProperty("MaxChi2Down", m_chi2Down = 10. );
}
//=============================================================================
// Destructor
//=============================================================================
Hlt2PreSelDV::~Hlt2PreSelDV() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2PreSelDV::initialize() {

  StatusCode sc = LoKi::Algo::initialize(); 
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


  if( m_computeMV != 0 ){
    //Initialise the matter Veto
    m_materialVeto = tool< IMatterVeto >( "MatterVetoTool" , "MatterVetoTool" );
  }
  
  m_veloProtoPartLocation = "Hlt2/"+this->name()+"/VeloProtoP";

  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
 StatusCode Hlt2PreSelDV::execute() {
  
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts ;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ++counter("Processed evt nb");
  if( msgLevel(MSG::DEBUG) )
    debug() << "==> Execute the Hlt2PreSelDV algorithm, event "
	    << counter("Processed evt nb").flag() << endmsg;

  // Get the upstream PV
  const RecVertex * UpPV = GetUpstreamPV();
  if(UpPV == NULL) return StatusCode::SUCCESS;

  if( m_UseMap ){ 
    m_map.clear(); //Re-initialize the map
    m_MapCalled = false; //The map hasn't been called yet.
  }   
  /// m_mapLongTrack.clear();

  //Retrieve data Particles from Desktop.
  const Particle::ConstVector Parts = this->i_particles();
  int size = Parts.size() ;

  //Retrieve Reconstructed Vertices
  RecVertex::ConstVector RV;
  GetRecVertices( RV );
  if( RV.size() < 1 ) return StatusCode::SUCCESS;

  //The PV is assumed to be the rec vertex with lowest z
  //sort rec vertex according to z position
  sort( RV.begin(), RV.end(), Hlt2PreSelDV::CondRVz() );

  LHCb::ProtoParticles* veloTrProtoP = new LHCb::ProtoParticles();
  put( veloTrProtoP , m_veloProtoPartLocation );

  // Define some cuts
  LoKi::Types::VFun beamSpotCut = LoKi::Cuts::VX_BEAMSPOTRHO( 1. ) ;

  RecVertex::ConstVector::const_iterator i = RV.begin();
  RecVertex::ConstVector::const_iterator iend = RV.end();
  for( ; i != iend; ++i) {
    const RecVertex* rv = *i;
    const SmartRefVector< Track > & Tracks = rv->tracks();
    size = Tracks.size();

    // Put here the cut on the number of tracks....

    double r  = beamSpotCut( rv ); //R to beam line

    if ( r < m_RMin || r > m_RMax ||  size < m_nTracks  ) continue;


    //Do not change the PV into a particle ! (if any !)
    //if( !m_KeepLowestZ ){
    //  if( i == RV.begin() ) continue;
      //Do not keep if before the upPV
    if( rv->position().z() < UpPV->position().z() ) continue;

    //PVs have no backward tracks
    if( HasBackwardTracks(rv) ){ 
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV has a backward track --> disguarded !"<< endmsg;
      continue;
    }
    if( !RecVertex2Particle( rv, Parts, r , *veloTrProtoP ) ) continue;
    
  }
  
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2PreSelDV::finalize() {

  if( msgLevel( MSG::DEBUG ) ) debug() << "==> Finalize" << endmsg;
  return LoKi::Algo::finalize(); 
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
    if( msgLevel( MSG::DEBUG ) )
      debug() <<"PV candidate : nb of tracks "<< pv->tracks().size() << endmsg;
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
				       const Particle::ConstVector & Parts, double r , 
                                       LHCb::ProtoParticles& veloProtos ){  

  Gaudi::LorentzVector mom;
  SmartRefVector< Track >::const_iterator iVtx = rv->tracks().begin();
  SmartRefVector< Track >::const_iterator iVtxend = rv->tracks().end();
  int endkey = rv->tracks().back()->key();

  std::vector< const Particle* > veloPartPointers;
  

  //Create map if necessary
  if( m_UseMap && !m_MapCalled ) CreateMap( Parts );

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

  double fracHitBeforeVertex(0.);
  double maxE(-1.);
  double ndaug(0.);
  
  if( m_UseMap ){
    //Loop on RecVertex daughter tracks and save corresponding Particles
    for( ; iVtx != iVtxend; ++iVtx ){
      ndaug+=1.;

      const int key = (*iVtx)->key();
      GaudiUtils::VectorMap<int, const Particle *>::const_iterator it;
      
      it = m_map.find( key );

      const Particle * part = NULL; 
      // If the vertex velo track is mapped with a particle track
      // then assign the particle as it is
      if( it != m_map.end() ){  
        double minZ = 10000.;
        for (std::vector< LHCb::LHCbID >::const_iterator ilhcbID = (*iVtx)->lhcbIDs().begin();
             (*iVtx)->lhcbIDs().end()!=ilhcbID; ilhcbID++){
          if (!(*ilhcbID).isVelo())continue;
          double z = m_velo->sensor((*ilhcbID).veloID())->z();
          if (z < minZ)minZ=z;
        }
        // if first hit is at 5 sigma before the vertex count it
        if ((minZ -  rv->position().z())/rv->covMatrix ()(2,2) < -5. ){
          fracHitBeforeVertex+=1.;
        } 
        part = it->second; 
      }
      // if the vertex velo track don't match anyone, make it velo only
      else{
        const Track* p = (*iVtx);
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
        const Gaudi::LorentzVector mom = Gaudi::LorentzVector(sx*pz, sy*pz, pz,e );
        pion->setMomentum(mom);
        part = pion->clone();
        veloPartPointers.push_back(part);
        delete(pion);
        
      }
      if( part != NULL ){
        // apply chi2 cut if the track is not velo
        if (part->proto()->track()->type()!= LHCb::Track::Velo && part->proto()->track()->chi2PerDoF () > m_chi2Long )continue;
        tmpVtx.addToOutgoingParticles( part );
        tmpPart.addToDaughters( part );
        // is it the highest energy track
        if (part->proto()->track()->type()!= LHCb::Track::Velo && part->momentum().e()>maxE)maxE=part->momentum().e();
        // if track is velo while we don't want to use velo, do not add the momenta.
        if( ! m_useVeloTrack && part->proto()->track()->type()== LHCb::Track::Velo ) continue;
        mom += part->momentum();
      }      
    }
  }
  else{
    //Find all particles that have tracks in RecVertex
    Particle::ConstVector::const_iterator jend = Parts.end();
    for ( Particle::ConstVector::const_iterator j = Parts.begin();
          j != jend;++j) {
      
      if( (*j)->proto() == NULL ) continue;
      if( (*j)->proto()->track() == NULL ) continue;
      const Track * tk = (*j)->proto()->track();
      //if( !TestTrack( tk ) ) continue;
      while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }
      
      if( (*iVtx)->key() == tk->key() ){ 
        if( (*iVtx)->key() != endkey ) ++iVtx; 
        // make sure it is a new pointer so that when the mother is saved on TES we only have new pointers.
        if( (*j)->proto()->track()->type()!= LHCb::Track::Velo && (*j)->proto()->track()->chi2PerDoF () > m_chi2Down )continue;

        tmpVtx.addToOutgoingParticles ( (*j ) );
        tmpPart.addToDaughters( (*j ) );
        mom += (*j )->momentum();
        //const LHCb::Particle* clonedPart =  (*j )->clone();
        //tmpVtx.addToOutgoingParticles ( clonedPart );
        //tmpPart.addToDaughters( clonedPart );
        //mom += clonedPart->momentum();
        continue;
      }
    }
    ndaug=1.;
  }
  //Fill momentum and mass estimate
  tmpPart.setMomentum( mom );
  tmpPart.setMeasuredMass( mom.M() );
  tmpPart.setMeasuredMassErr( 0 );
  fracHitBeforeVertex = fracHitBeforeVertex/ndaug;
  double mass = tmpPart.measuredMass();
  if( mass < m_PreyMinMass || mass >  m_PreyMaxMass ||
      fracHitBeforeVertex > m_fractTrUpVertex || maxE/tmpPart.momentum().e()>m_fractE ){
    for (std::vector<const Particle*>::iterator ip = veloPartPointers.begin() ; veloPartPointers.end()!= ip ; ip++){
      delete((*ip));
    }
    return false; 
  }
  Gaudi::LorentzVector momJet;
  
  Gaudi::SymMatrix4x4 MomCovMatrix = 
    Gaudi::SymMatrix4x4( ROOT::Math::SMatrixIdentity() );
  tmpPart.setMomCovMatrix( MomCovMatrix );
  tmpPart.setPosCovMatrix( rv->covMatrix() );
  Gaudi::Matrix4x3 PosMomCovMatrix;
  tmpPart.setPosMomCovMatrix( PosMomCovMatrix );
  tmpPart.setEndVertex( tmpVtx.clone() );
  tmpPart.setReferencePoint( point );

  tmpPart.addInfo(55,fracHitBeforeVertex);
  tmpPart.addInfo(56,maxE/tmpPart.momentum().e());
  //Remove if found to be in detector material
  if (m_computeMV) tmpPart.addInfo(51,m_materialVeto->isInMatter(tmpVtx.position()));

  this->markNewTree(tmpPart.clone());

  setFilterPassed(true);

  return true;
}

//=============================================================================
// Create a map between Particles and their Velo tracks ancestors
//=============================================================================
void Hlt2PreSelDV::CreateMap( const Particle::ConstVector & Parts ){
  m_map.reserve( Parts.size() );
  // Loop over the particles
  for ( Particle::ConstVector::const_iterator j = Parts.begin();
        j != Parts.end();++j) {
    // Check they have protoP and a track
    if( (*j)->proto() == NULL ) continue;
    if( (*j)->proto()->track() == NULL ) continue;
    const Track * tk = (*j)->proto()->track();
    SmartRefVector< Track > old = tk->ancestors();
    // Loop over the track ancestors
    for( SmartRefVector<Track>::const_iterator i = 
           old.begin(); i != old.end(); ++i ){ 
      // If the track ancestor is a velo track map the key of the ancestor with the particle
      if( !((*i)->checkType(Track::Velo)) ) continue;
      m_map.insert( (*i)->key(), (*j) );
      break;
    }
  }
  // Set a tag to make sure the map have been called.
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
  ProtoParticle protoPion;
  protoPion.setTrack(p);
  pion.setProto(protoPion.clone());
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

