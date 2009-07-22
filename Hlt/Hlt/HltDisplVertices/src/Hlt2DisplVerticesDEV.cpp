/**********************************************************
 * THIS IS THE DEVELOPMENT VERSION
 * Hlt2DisplVertices is the REFERENCE VERSION
 *********************************************************/

// Include files:
//from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IAlgTool.h"

//from Kernel
#include "Kernel/DVAlgorithm.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"

//get the Header of the event
#include "Event/RecHeader.h"

// Local
#include "Hlt2DisplVerticesDEV.h"

using namespace Gaudi::Units ;
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2DisplVerticesDEV
//
// 2009-july-1 : Neal Gueissaz
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(Hlt2DisplVerticesDEV);

//=============================================================================
// Sort containers
//=============================================================================
bool Hlt2DisplVerticesDEV::sortPVz(const RecVertex* first, const RecVertex* second) {
  return first->position().z() < second->position().z();
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2DisplVerticesDEV::Hlt2DisplVerticesDEV(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
    , m_nbevent(0)
    , m_nbpassed(0)
{
  declareProperty("SaveInTuple", m_Save = false );
  declareProperty("DefMom", m_DefMom = true );
  declareProperty("InputDisplacedVertices", m_InputDisplVertices = 
                  "Rec/Vertices/Hlt2RV");
  declareProperty("MinNbTracks", m_MinNbtrks = 0 );
  declareProperty("RMin", m_RMin = 0.3 );
  declareProperty("MinMass1", m_MinMass1 = 9*GeV );
  declareProperty("MinMass2", m_MinMass2 = 4*GeV );
  declareProperty("MinSumpt", m_MinSumpt = 10.*GeV );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 1*mm  );
}

//=============================================================================
// Destructor
//=============================================================================
Hlt2DisplVerticesDEV::~Hlt2DisplVerticesDEV() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode Hlt2DisplVerticesDEV::initialize() {

  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;

  if(msgLevel(MSG::DEBUG)){
    debug() << "==> Initialize" << endmsg;
    debug() << "---------------- CUTS on RecVertex --------------"<< endmsg;
    debug() << "No backward tracks"<< endmsg;
    debug() << "the RV with lowest z will be disguarded"<< endmsg;
    debug() << "Min number of tracks           "<< m_MinNbtrks << endmsg;
    debug() << "Min radial displacement        "<< m_RMin <<" mm"<< endmsg;
    debug() << "For single prey hunting :"<< endmsg;
    debug() << "Min reconstructed mass         "<< m_MinMass1/GeV 
	    <<" GeV"<< endmsg;
    debug() << "Min sum of all daughter tracks "<< m_MinSumpt 
	    <<" GeV"<< endmsg;
    debug() << "For double prey hunting :"<< endmsg;
    debug() << "Min reconstructed mass         "<< m_MinMass2/GeV 
	    <<" GeV"<< endmsg;
    debug() << "Min sum of all daughter tracks "<< m_MinMass2/GeV
	    <<" GeV"<< endmsg;
    debug() << "------------------------------------"<< endmsg;
  }

  if( m_RemVtxFromDet != 0 ){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();

    // Get Transport Service
    if( m_RemVtxFromDet > 0 ) 
      m_transSvc = svc<ITransportSvc>( "TransportSvc", true  );
  }


  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode Hlt2DisplVerticesDEV::execute() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  setFilterPassed(false);
  m_nbevent++;

  //save a dummy Particle in the TES
  Particle::ConstVector Pions ;
  const Particle::ConstVector & InputParts = desktop()->particles();
  if( InputParts.empty() ) return desktop()->cloneTrees(Pions);
  Pion = *(InputParts.begin());

  m_map.clear(); //Re-initialize the map

  //Retrieve the RecVertex
  RecVertices* RVs = get<RecVertices>(m_InputDisplVertices);
  //sort them by ascending z position
  sort( RVs->begin(), RVs->end(), sortPVz);
  debug()<<"Retrived "<< RVs->size() 
	 <<" displ vertices, the one with lowest z will be disguarded" 
	 << endmsg;
  plot( RVs->size(), "NbRV", 0, 6);
  if( RVs->empty() ) return desktop()->cloneTrees(Pions);


  //Some check up
  //PrintTracksType();
  //PrintParticle();

  vector<double> Zs, Rs, sumPt, RPt, Ms; 
  vector<int> Nbtrks; 
  vector<int> InDets;

  //Selections
  int Sel1 = 0; //for single dv hunting
  int Sel2 = 0;//for double dv hunting

  //Let's loop on the RecVertex
  RecVertices::const_iterator iRV = RVs->begin(); iRV++;
  for(; RVs->end() != iRV; iRV++) {
    const RecVertex* RV = *iRV;
 
    debug()<<"Rec Vertex position "<< RV->position() << endmsg;

    //Check if RV has backward tracks to avoid PVs
    if( HasBackwardTracks(RV) ){ 
      debug() <<"RV has a backward track, not considered !"<< endmsg;
      continue;
    }



    double R = RV->position().rho();
    double sumpt, recpt, mass;

    //Retrieve Particles corresponding to vertices
    if( m_map.empty() ) CreateMap(); //Create map if necessary
    Particle::ConstVector RecParts ;
    GetPartsFromRecVtx( RV, RecParts );

    //compute the kinematics
    Kinematics( RecParts, mass, sumpt, recpt ); 

    //Properties of reconstructed vertices
    Ms.push_back( mass );
    Zs.push_back( RV->position().z() );
    Rs.push_back( R );
    sumPt.push_back( sumpt );
    Nbtrks.push_back( RV->tracks().size() );
    RPt.push_back( recpt );

    debug()<<"RV mass "<< mass <<" R "<< R <<" rec pt "<< recpt 
	   <<" sum pt "<< sumPt << endmsg;

    //Remove if found to be in detector material
    bool InDet = false;
    if( m_RemVtxFromDet && RemVtxFromDet(RV) ){
      InDet = true;
    } 
    InDets.push_back(InDet);


    //Criterias
    if( !InDet && R >= m_RMin ){
      if( Ms.back() >= m_MinMass1 && sumpt >= m_MinSumpt ) Sel1++;
      if( Ms.back() >= m_MinMass2 && sumpt >= m_MinMass2 ) Sel2++;
    }
  }    

  //Don't save every ntuples
  if( Zs.empty() ) return desktop()->cloneTrees(Pions);

  if( m_Save ){
    const int maxSize = 25;
    Tuple tuple = nTuple("HltLine");
    tuple->farray( "Z", Zs.begin(), Zs.end(), "Nb", maxSize );
    tuple->farray( "R", Rs.begin(), Rs.end(), "Nb", maxSize );
    tuple->farray( "sumPt", sumPt.begin(), sumPt.end(), "Nb", maxSize );
    tuple->farray( "NbTrks", Nbtrks.begin(), Nbtrks.end(), "Nb", maxSize );
    tuple->farray( "RPt", RPt.begin(), RPt.end(), "Nb", maxSize );
    tuple->farray( "M", Ms.begin(), Ms.end(), "Nb", maxSize );
    tuple->farray( "InDet", InDets.begin(), InDets.end(), "Nb", maxSize );
    if( !fillHeader( tuple ) ) return desktop()->cloneTrees(Pions);
    tuple->write();
  }


  // Cuts 
  if( Sel1 > 0 || Sel2 >1 ){
    return Done(Pions);
  } else { debug()<<"Event rejected !"<< endmsg; } 

  return desktop()->cloneTrees(Pions);
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode Hlt2DisplVerticesDEV::finalize() {

  if (msgLevel(MSG::DEBUG)) {
    debug() << "==> Finalize" << endmsg;
    if(m_nbevent == 0) m_nbevent++;
    double err = 10.*sqrt( m_nbpassed/m_nbevent );
    debug() << "------------- Efficiency -----------"<< endmsg;
    debug() << "| Accepted event         "<< 100.*m_nbpassed/m_nbevent 
	    <<"+-" << err
	    << " ( "<< m_nbpassed <<")" << endmsg;
    debug() << "| Total number of events " << m_nbevent << endmsg;
    debug() << "------------------------------------"<< endmsg;
  }

  return DVAlgorithm::finalize();
}

//=============================================================================
// Event is accepted
//=============================================================================
StatusCode Hlt2DisplVerticesDEV::Done( Particle::ConstVector & Pions){
    debug()<<"Event satisfied HLT2DisplVertices criteria !"<< endmsg;
    m_nbpassed++;
    setFilterPassed(true);
    Pions.push_back(Pion);
    LHCb::Tracks* inputTracks = get<Tracks>(TrackLocation::HltForward);
    plot( inputTracks->size(), "NbTracksSelEvts", 0, 150 );
    return desktop()->cloneTrees(Pions);
}

//=============================================================================
// Get Particles related to a RecVertex
//=============================================================================
void Hlt2DisplVerticesDEV::GetPartsFromRecVtx(const RecVertex* RV, 
					 Particle::ConstVector & RecParts){


  SmartRefVector< Track >::const_iterator iVtx = RV->tracks().begin();
    
  //Loop on RecVertex daughter tracks and save corresponding Particles
  float nblong = 0.;
  for( ; iVtx != RV->tracks().end(); iVtx++ ){
      
//       debug()<<"Key "<< (*iVtx)->key() <<" type "
// 	     <<(*iVtx)->type()  <<" slope "<< (*iVtx)->slopes() << endmsg;
    const int key = (*iVtx)->key();
    GaudiUtils::VectorMap<int, const Particle *>::const_iterator it;
    it = m_map.find( key );
    const Particle * part = NULL; 
 
    //Give a default pion with pT of 400 MeV
    if( it != m_map.end() ) part = it->second; nblong++;
    //debug()<<"got Particle from map with slope "<< part->slopes() <<endmsg;

    if( m_DefMom && it == m_map.end() ) part = DefaultParticle(*iVtx);
    if( part != NULL ) RecParts.push_back( part );
  }
  double eff = 100.*( nblong )/( RV->tracks().size() );
  plot( eff, "Velo2PartEff", 0., 105.);
}

//=============================================================================
// Create a map between Particles and their Velo tracks ancestors
//=============================================================================
void Hlt2DisplVerticesDEV::CreateMap(){

  const Particle::ConstVector & InputParts = desktop()->particles();

  m_map.reserve( InputParts.size() );
  int nb = 0;
  for ( Particle::ConstVector::const_iterator j = InputParts.begin();
	j != InputParts.end();++j) {
    
    if( (*j)->proto()->track() == NULL ) continue;
    const Track * tk = (*j)->proto()->track();
    
    SmartRefVector< Track > old = tk->ancestors();
    for( SmartRefVector<Track>::const_iterator i = 
	   old.begin(); i != old.end(); i++ ){
//       debug()<<"Part "<< (*j)->key() <<" type "<< tk->type() 
// 	     <<" slope "<< (*j)->slopes() 
// 	     << " Ancestor " << (*i)->key() <<" type "
//           <<(*i)->type()  <<" slope "<< (*i)->slopes() << endmsg;
      
      if( !((*i)->checkType(Track::Velo)) ) continue;
      m_map.insert( (*i)->key(), (*j) );
      nb++;
      //debug()<<"a track has been inserted !" << endmsg;
      break;
    }
  }
  //double eff = 100.*( (double)nb )/( (double)InputParts.size() );
  //plot( eff, "MapEff", 0., 105. ); //always 100%
  int clone = nb - m_map.size();
  plot( clone, "NbClonePart", 0., 10. );
  //debug() <<"eff "<< eff <<" nb of clone "<< clone <<" Nb of Part "
  //<< InputParts.size() <<" Map size "<< m_map.size() << endmsg;

  return;
}

//=============================================================================
// Create default pions with 400 MeV pt, see p. 128
//=============================================================================
const Particle * Hlt2DisplVerticesDEV::DefaultParticle( const Track * p ){

  double sx = p->slopes().x(); double sy = p->slopes().y();
  double pz = 400/sqrt( sx*sx + sy*sy );
  double e = sqrt( pow(139.57,2) + pow(400.,2) + pz*pz );
  Particle pion;
  const Gaudi::LorentzVector mom = Gaudi::LorentzVector(sx*pz, sy*pz, pz,e );
  pion.setMomentum(mom);

  //debug()<<"Creating default pion for key "<< p->key() <<" and slopes "
  // <<sx<<" "<<sy<<" yielding momentum "<< mom << endmsg;

  return desktop()->keep(&pion);
}


//=============================================================================
// Compute some kinematical variables
//=============================================================================
void Hlt2DisplVerticesDEV::Kinematics( Particle::ConstVector & Parts, 
				       double & mass, 
				       double & sumpt, double & recpt ){

  sumpt = 0.; 
  Gaudi::LorentzVector  mom;

  for( Particle::ConstVector::const_iterator i = 
	 Parts.begin(); i != Parts.end(); i++ ){
    const Particle * p = (*i);

    // Sum the pT of all tracks of a RecVertex
    sumpt += p->pt();

    mom += p->momentum();
  }

  // Compute the pT of a RecVertex from all its tracks 
  recpt = mom.Pt();

  // mass
  mass = mom.M();

}


//============================================================================
//  Event and run number 
//============================================================================
StatusCode Hlt2DisplVerticesDEV::fillHeader( Tuple & tuple ){

  const LHCb::RecHeader* header = get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);  
  debug() << "Filling Tuple Event " << header->evtNumber() << endmsg ;
  tuple->column("Event", (int)header->evtNumber());
  return StatusCode::SUCCESS ;
}


//=============================================================================
//  Print track and particles used in this algo
//=============================================================================

void Hlt2DisplVerticesDEV::PrintTracksType(){

  Tracks* VeloTrks = get<Tracks>(TrackLocation::HltVelo);
  Tracks* ForwardTrks = get<Tracks>(TrackLocation::HltForward);

  debug()<<"Printing Velo Track content, size "<< VeloTrks->size() <<endmsg;
  for(Track::Container::const_iterator itr = VeloTrks->begin(); 
      VeloTrks->end() != itr; itr++) {
    const Track* trk = *itr;
    string s = "False !";
    if ( trk->checkFlag( Track::Backward ) ) s = "True !";
    debug()<<"Track key "<< trk->key()<<" slope "<< trk->slopes() 
	   <<" type "<<trk->type()<<" Is backward ? "<< s <<endmsg;
  }
  debug()<<"Printing Foward Track content, size "<< ForwardTrks->size() <<endmsg;
  for(Track::Container::const_iterator itr = ForwardTrks->begin(); 
      ForwardTrks->end() != itr; itr++) {
    const Track* trk = *itr;
    string s = "False !";
    if ( trk->checkFlag( Track::Backward ) ) s = "True !";
    debug()<<"Track key "<< trk->key()<<" slope "<< trk->slopes() 
	   <<" type "<<trk->type()<<" Is backward ? "<< s <<endmsg;
  }
}
void Hlt2DisplVerticesDEV::PrintParticle(){
  const Particle::ConstVector& InputParts = desktop()->particles();
  debug()<<"#########  Dumping Particle content, size "
	 << InputParts.size() <<endmsg;
  for ( Particle::ConstVector::const_iterator j = InputParts.begin();
	j != InputParts.end();++j) {
    debug()<<"Particle id "<< (*j)->particleID().pid() <<" Mass "<< (*j)->measuredMass()/1000. << " GeV"<<" slope "<< (*j)->slopes() << endmsg;
  }

}

//=============================================================================
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool Hlt2DisplVerticesDEV::HasBackwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  for( ; i != RV->tracks().end(); i++ ){
    if ( (*i)->checkFlag( Track::Backward ) ) return true;
  }
  return false;
}

//=============================================================================
//  Remove RV if found to be in detector material
//
// if m_RemVtxFromDet = 0  : disabled
// if m_RemVtxFromDet < 0  : remove reco vtx if in detector material
// if m_RemVtxFromDet > 0  : remove reco vtx if rad length along z 
//                           from (decay pos - m_RemVtxFromDet) to 
//                           (decay pos + m_RemVtxFromDet)  is > threshold
//
//=============================================================================

bool Hlt2DisplVerticesDEV::RemVtxFromDet( const RecVertex* RV ){

  double threshold = 1e-10;

  if( m_RemVtxFromDet < 0 ){

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
    plot( size, "NbofDetV", 0, 5 );
    debug()<<"Found "<< size <<" physical volumes related to point "
	   << RV->position() <<endmsg;

    const IPVolume* pvlast = 0;
    if ( !path.empty() ) { pvlast = path.back(); }

    const ILVolume * lvlast = 0;
    if ( 0 != pvlast ) { lvlast = pvlast->lvolume(); }
 
    const Material* matlast = 0 ;
    if ( 0 != lvlast ) { matlast = lvlast->material (); }

    if ( 0 != matlast )  { 
      debug()<<"Physical volume related to point "<< RV->position() <<endmsg;
      debug()<< matlast << endl;
      //if( matlast->name() == "Vacuum" ) return false;
      return true;
    } 
  } //end of <0 condition
  else if( m_RemVtxFromDet > 0 ){

    const Gaudi::XYZPoint pos = RV->position();
    const Gaudi::XYZPoint dz = Gaudi::XYZPoint( 0., 0., m_RemVtxFromDet );
    //const Gaudi::XYZPoint dx = Gaudi::XYZPoint( m_RemVtxFromDet, 0., 0. );
    //const Gaudi::XYZPoint dy = Gaudi::XYZPoint( 0., m_RemVtxFromDet, 0. );

    //Compute the radiation length
    //double radlength = m_transSvc->distanceInRadUnits( start, end );

    if( m_lhcbGeo == 0 ){ 
      warning()<<"IGeometryInfo* m_lhcbGeo is broken"<< endmsg; return true; }


    IGeometryInfo* dum = 0;
    Gaudi::XYZPoint start = Minus(pos,dz);
    Gaudi::XYZPoint end = Plus(pos,dz);
    double radlength = m_transSvc->distanceInRadUnits
      ( start, end, 1e-35, dum, m_lhcbGeo );

//     start = Minus(pos,dx); end = Plus(pos,dx);
//     radlength += m_transSvc->distanceInRadUnits
//       ( start, end, 1e-35, dum, m_lhcbGeo );
//     start = Minus(pos,dy); end = Plus(pos,dy);
//     radlength += m_transSvc->distanceInRadUnits
//       ( start, end, 1e-35, dum, m_lhcbGeo );

    plot( radlength, "RVRadLength", 0, 0.01);
    debug()<<"Radiation length from "<< start <<" to "
	   << end <<" : "<< radlength 
	   <<" [mm]" << endmsg;

    if( radlength > threshold ){ 
      debug()<<"RV is too closed to a detector material --> disguarded !"
	     << endmsg;
      return true;
    }
  } //end of >0 condition


  return false;
}

//============================================================================
// Basic operations between two Gaudi::XYZPoint
//============================================================================
Gaudi::XYZPoint Hlt2DisplVerticesDEV::Plus( const Gaudi::XYZPoint & p1, 
				  const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint Hlt2DisplVerticesDEV::Minus( const Gaudi::XYZPoint& p1, 
				   const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}
