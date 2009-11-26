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

//get the Header of the event
#include "Event/RecHeader.h"

// Local
#include "Hlt2DisplVertices.h"

using namespace Gaudi::Units ;
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2DisplVertices
//
// 2009-july-16 : Neal Gauvin
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(Hlt2DisplVertices);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2DisplVertices::Hlt2DisplVertices(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
    , m_pt(400.)
{
  declareProperty("InputDisplacedVertices", m_InputDisplVertices = 
                  "Hlt/Vertices/Hlt2RV");
  declareProperty("RCutMethod", m_RCutMethod = "FromUpstreamPV" );
  declareProperty("MinNbTracks", m_MinNbtrks = 0 );
  declareProperty("RMin", m_RMin = 0.4 );
  declareProperty("MinMass1", m_MinMass1 = 9*GeV );
  declareProperty("MinMass2", m_MinMass2 = 4*GeV );
  declareProperty("MinSumpt1", m_MinSumpt1 = 9*GeV );
  declareProperty("MinSumpt2", m_MinSumpt2 = 4*GeV );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 1*mm  );
}

//=============================================================================
// Destructor
//=============================================================================
Hlt2DisplVertices::~Hlt2DisplVertices() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode Hlt2DisplVertices::initialize() {

  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;

  if(msgLevel(MSG::DEBUG)){
    debug() << "==> Initialize" << endmsg;
    debug() << "---------------- CUTS on RecVertex --------------"<< endmsg;
    debug() << "No backward tracks"<< endmsg;
    debug() << "the upstream RV will be disguarded"<< endmsg;
    debug() << "Min number of tracks           "<< m_MinNbtrks << endmsg;
    debug() << "Min radial displacement        "<< m_RMin <<" mm";
    if( m_RCutMethod == "LocalVeloFrame" )
      debug() << ", computed with respect to (0,0,z) in the local Velo frame" 
	      << endmsg; 
    else if( m_RCutMethod == "FromUpstreamPV" ){
      debug() << ", computed with respect to the upstream PV" << endmsg;
    } else if( m_RCutMethod == "FromUpstreamPV3D" ){
      debug() << ", computed with respect to the upstream PV from PatPV3D" 
	      << endmsg;
    } else if( m_RCutMethod == "CorrFromUpstreamPV" ){
      debug() << ", computed with respect to the upstream 2D PV." 
	      <<" Take the position of the associated 2D RV, if any."<< endmsg;
    } else {
      debug() << ", computed with respect to (0,0,z) in the global LHCb frame" 
	      << endmsg;
    }
    debug() << "For single prey hunting :"<< endmsg;
    debug() << "Min reconstructed mass         "<< m_MinMass1/GeV 
	    <<" GeV"<< endmsg;
    debug() << "Min sum of all daughter tracks "<< m_MinSumpt1/GeV 
	    <<" GeV"<< endmsg;
    debug() << "For double prey hunting :"<< endmsg;
    debug() << "Min reconstructed mass         "<< m_MinMass2/GeV 
	    <<" GeV"<< endmsg;
    debug() << "Min sum of all daughter tracks "<< m_MinSumpt2/GeV
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

  //Sanity checks
  if( m_MinMass2 > m_MinMass1 ){
    warning()<<"MinMass2 set to a value smaller than MinMass1 : "	
             << m_MinMass2 <<"<"<< m_MinMass1 
             <<"This is non-sense !"<< endmsg;
    return StatusCode::FAILURE;
  }

  //Get the pion mass
  const ParticleProperty* Ppion = ppSvc()->find( "pi+" );
  m_piMass = Ppion->mass();

  //get the Velo geometry
  if( m_RCutMethod == "LocalVeloFrame" ){
    string velo = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo";
    const IDetectorElement* lefthalv = getDet<IDetectorElement>( velo+"Left" );
    const IDetectorElement* righthalv = 
      getDet<IDetectorElement>( velo + "Right" );
    const IGeometryInfo* halfgeominfo = lefthalv->geometry();
    //check that Velo is closed
    Gaudi::XYZPoint localorigin(0,0,0);
    Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin);
    Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin);
    if( abs(leftcenter.x() - rightcenter.x())> 0.1 *Gaudi::Units::mm ) {
      info() << "Velo not closed, work in global frame" << endmsg;
     m_RCutMethod == "";
    }
    //matrix to transform to local velo frame
    m_toVeloFrame = halfgeominfo->toLocalMatrix() ;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode Hlt2DisplVertices::execute() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);

  //save a dummy Particle in the TES
  Particle::ConstVector Pions ;
  const Particle::ConstVector & InputParts = desktop()->particles();
  if( InputParts.empty() ) return StatusCode::SUCCESS;
  const LHCb::Particle* Pion = *(InputParts.begin()); //a pion

  m_map.clear(); //Re-initialize the map

  //Retrieve the RecVertex
  RecVertices* RVs = get<RecVertices>(m_InputDisplVertices);
  if(msgLevel(MSG::DEBUG)) 
    debug()<<"Retrieved "<< RVs->size() 
           <<" displ vertices, the one with lowest z will be disguarded" 
           << endmsg;
  if( RVs->size()<2 ) return StatusCode::SUCCESS;
  //sort them by ascending z position
  sort( RVs->begin(), RVs->end(), SortPVz);

  //Retrieve the RecVertex from PV official reconstruction
  RecVertex::ConstVector PVs;
  if( m_RCutMethod=="FromUpstreamPV" || m_RCutMethod=="CorrFromUpstreamPV" ){
    const RecVertex::Container * PVc = desktop()-> primaryVertices();
    int size = PVc->size();
    if(msgLevel(MSG::DEBUG))
      debug()<<"Retrieved "<< size <<" primary vertices" << endmsg;
    plot( size, "NbPV", 0, 6);
    if( PVc->empty() ) return StatusCode::SUCCESS;
    for( RecVertex::Container::const_iterator i = PVc->begin(); 
         i != PVc->end(); ++i ){
      PVs.push_back( *i );
    }
    //sort them by ascending z position
    sort( PVs.begin(), PVs.end(), SortPVz);
  }


  //Selections
  int Sel1 = 0; //for single dv hunting
  int Sel2 = 0;//for double dv hunting

  //Let's loop on the RecVertex
  RecVertices::const_iterator iRV = RVs->begin(); 
  Gaudi::XYZPoint UpPV;
  if( m_RCutMethod=="FromUpstreamPV" || m_RCutMethod=="CorrFromUpstreamPV" ){
    UpPV = (*PVs.begin())->position();
    if(msgLevel(MSG::DEBUG))
      debug() <<"Upstream 2D RV position "<< UpPV << endmsg;
  } else {
    UpPV = (*iRV)->position();
  }

  iRV++; //Do not consider first one
  for(; RVs->end() != iRV; iRV++) {
    const RecVertex* RV = *iRV;
 
    if(msgLevel(MSG::DEBUG)) 
      debug()<<"Rec Vertex position "<< RV->position() << endmsg;

    if( ( m_RCutMethod == "FromUpstreamPV"  || m_RCutMethod=="CorrFromUpstreamPV" ) && RV->position().z() < UpPV.z() ){
      if(msgLevel(MSG::DEBUG))
	debug() <<"RV z position comes before the upstream 2D RV," 
		<<" not considered !"<< endmsg;
      continue;
    }

    //Check if RV is radially displaced
    double R;
    if( m_RCutMethod == "CorrFromUpstreamPV3D" || 
        m_RCutMethod == "FromUpstreamPV3D" ||
        m_RCutMethod == "FromUpstreamPV" ){
      R = (RV->position() - UpPV).rho();
    } else if( m_RCutMethod == "LocalVeloFrame" ){
      Gaudi::XYZPoint localPV = m_toVeloFrame * RV->position();
      R = localPV.rho();
    } else { R = RV->position().rho(); }
    
    if( R < m_RMin ){
      if(msgLevel(MSG::DEBUG))
        debug() <<"RV has an insufficent radial displacement !"<< endmsg;
      continue;
    }

    //Check if RV has backward tracks to avoid PVs
    if( HasBackwardTracks(RV) ){ 
      if(msgLevel(MSG::DEBUG))
        debug() <<"RV has a backward track, not considered !"<< endmsg;
      continue;
    }

    //Eventually correct the position of 3D RV with associated 2D ones.
    if( m_RCutMethod == "CorrFromUpstreamPV" && 
        ( GetCorrPosition( RV, PVs ) - UpPV).rho() < m_RMin ){
      if(msgLevel(MSG::DEBUG))
        debug() <<"Corr RV has an insufficent radial displacement !"<< endmsg;
      continue;

    }

    //Retrieve Particles corresponding to vertices
    if( m_map.empty() ) CreateMap(); //Create map if necessary
    Particle::ConstVector RecParts ;
    GetPartsFromRecVtx( RV, RecParts );

    //compute the kinematics
    double sumpt, mass;
    Kinematics( RecParts, mass, sumpt ); 

    //Properties of reconstructed vertices
    if(msgLevel(MSG::DEBUG)) 
      debug()<<"RV mass "<< mass <<" R "<< R <<" sum pt "<< sumpt << endmsg;

    //Remove low mass and sumpt RV 
    if( mass < m_MinMass2 || sumpt < m_MinSumpt2 ) continue;

    //Remove if found to be in detector material
    bool InDet = false;
    if( m_RemVtxFromDet && RemVtxFromDet(RV) ){ InDet = true; } 

    //Criterias
    if( !InDet ){
      if( mass >= m_MinMass1 && sumpt >= m_MinSumpt1 ) Sel1++;
      Sel2++;
    }
  }    


  // Cuts 
  if( Sel1 > 0 || Sel2 >1 ){

    if(msgLevel(MSG::DEBUG)) 
      debug()<<"Event satisfied HLT2DisplVertices criteria !"<< endmsg;
    setFilterPassed(true);
    Pions.push_back(Pion);
    return desktop()->cloneTrees(Pions);

  } else { if(msgLevel(MSG::DEBUG)){ debug()<<"Event rejected !"<< endmsg;} } 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode Hlt2DisplVertices::finalize() {

  return DVAlgorithm::finalize();
}

//=============================================================================
// Get Particles related to a RecVertex
//=============================================================================
void Hlt2DisplVertices::GetPartsFromRecVtx(const RecVertex* RV, 
					 Particle::ConstVector & RecParts){


  SmartRefVector< Track >::const_iterator iVtx = RV->tracks().begin();
    
  //Loop on RecVertex daughter tracks and save corresponding Particles
  for( ; iVtx != RV->tracks().end(); iVtx++ ){
      
    const int key = (*iVtx)->key();
    GaudiUtils::VectorMap<int, const Particle *>::const_iterator it;
    it = m_map.find( key );
    const Particle * part = NULL; 
 
    //Give a default pion with pT of 400 MeV
    if( it != m_map.end() ) part = it->second;
    if( it == m_map.end() ) part = DefaultParticle(*iVtx);
    if( part != NULL ) RecParts.push_back( part );
  }
}

//=============================================================================
// Create a map between Particles and their Velo tracks ancestors
//=============================================================================
void Hlt2DisplVertices::CreateMap(){

  const Particle::ConstVector & InputParts = desktop()->particles();

  m_map.reserve( InputParts.size() );
  for ( Particle::ConstVector::const_iterator j = InputParts.begin();
	j != InputParts.end();++j) {
    
    if( (*j)->proto()->track() == NULL ) continue;
    const Track * tk = (*j)->proto()->track();
    
    SmartRefVector< Track > old = tk->ancestors();
    for( SmartRefVector<Track>::const_iterator i = 
	   old.begin(); i != old.end(); i++ ){
      
      if( !((*i)->checkType(Track::Velo)) ) continue;
      m_map.insert( (*i)->key(), (*j) );
      break;
    }
  }

  return;
}

//=============================================================================
// Create default pions with 400 MeV pt, see p. 128
//=============================================================================
const Particle * Hlt2DisplVertices::DefaultParticle( const Track * p ){

  double sx = p->slopes().x(); double sy = p->slopes().y();
  double pz = m_pt/std::sqrt( sx*sx + sy*sy );
  double e = std::sqrt( m_piMass*m_piMass + m_pt*m_pt + pz*pz );
  Particle pion;
  const Gaudi::LorentzVector mom = Gaudi::LorentzVector(sx*pz, sy*pz, pz,e );
  pion.setMomentum(mom);

  return desktop()->keep(&pion);
}


//=============================================================================
// Compute some kinematical variables
//=============================================================================
void Hlt2DisplVertices::Kinematics( Particle::ConstVector & Parts, 
				       double & mass, 
				       double & sumpt ){

  sumpt = 0.; 
  Gaudi::LorentzVector  mom;

  for( Particle::ConstVector::const_iterator i = 
	 Parts.begin(); i != Parts.end(); i++ ){
    const Particle * p = (*i);

    // Sum the pT of all tracks of a RecVertex
    sumpt += p->pt();

    mom += p->momentum();
  }

  // mass
  mass = mom.M();
}

//============================================================================
// If can find an associated 2D RV return the 2D RV  
//============================================================================
Gaudi::XYZPoint Hlt2DisplVertices::GetCorrPosition( const RecVertex* RV, 
						        RecVertex::ConstVector & PVs ){

  /// sort the 2D with ascending dZ : begin with 2DRV closest in z.
  SortPVdz.refz = RV->position().z();
  sort( PVs.begin(), PVs.end(), SortPVdz);

  //look if I can find an associated 2D RV to this one
  int com = 0;

  //Let's put in a vector all VeloR ancestors of our 3D RV
  vector<const Track*> RVolds;
  for( SmartRefVector<Track>::const_iterator it = RV->tracks().begin(); 
       it != RV->tracks().end(); ++it ){
    if( (*it)->ancestors().empty() ) continue;
    SmartRefVector<Track>::const_iterator ioldtk=(*it)->ancestors().begin();
    RVolds.push_back( ioldtk->target() );
  }
  vector<const Track*>::const_iterator tbeg = RVolds.begin();
  vector<const Track*>::const_iterator tend = RVolds.end();
  vector<const Track*>::const_iterator tRV;


  //loop on PVs
  for( RecVertex::ConstVector::const_iterator iRV = PVs.begin(); 
       iRV != PVs.end(); ++iRV ){
    com = 0;

    //loop on the 2D RV's tracks
    for( SmartRefVector<Track>::const_iterator it = 
	   (*iRV)->tracks().begin(); it != (*iRV)->tracks().end(); ++it ){

      //Check if track is in the 3D RV
      tRV = tbeg;
      while( tRV != tend ){
	if( (*tRV) == it->target() ){
	  ++com;
	  break;
	}
	++tRV;
      }
      if( com > 3 ){
	if(msgLevel(MSG::DEBUG)) 
	  debug()<<"3D RV position "<< RV->position() 
		 <<" has been associated with 2D RV position "
		 << (*iRV)->position() << endmsg;
	return (*iRV)->position();
      }
    }
  }
  
  return RV->position();
}

//=============================================================================
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool Hlt2DisplVertices::HasBackwardTracks( const RecVertex* RV ){
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

bool Hlt2DisplVertices::RemVtxFromDet( const RecVertex* RV ){

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
    if(msgLevel(MSG::DEBUG))
      debug()<<"Found "<< size <<" physical volumes related to point "
             << RV->position() <<endmsg;

    const IPVolume* pvlast = 0;
    if ( !path.empty() ) { pvlast = path.back(); }

    const ILVolume * lvlast = 0;
    if ( 0 != pvlast ) { lvlast = pvlast->lvolume(); }
 
    const Material* matlast = 0 ;
    if ( 0 != lvlast ) { matlast = lvlast->material (); }

    if ( 0 != matlast )  { 
      if(msgLevel(MSG::DEBUG)){
        debug()<<"Physical volume related to point "<< RV->position() <<endmsg;
        debug()<< matlast << endl;
      }
      //if( matlast->name() == "Vacuum" ) return false;
      return true;
    } 
  } //end of <0 condition
  else if( m_RemVtxFromDet > 0 ){

    if( m_lhcbGeo == 0 ){ 
      warning()<<"IGeometryInfo* m_lhcbGeo is broken"<< endmsg; return true; }

    IGeometryInfo* dum = 0;

    const Gaudi::XYZPoint pos = RV->position();
    Gaudi::XYZPoint start = Gaudi::XYZPoint( pos.x(), pos.y(), 
					     pos.z()-m_RemVtxFromDet );
    Gaudi::XYZPoint end = Gaudi::XYZPoint( pos.x(), pos.y(), 
					   pos.z()+m_RemVtxFromDet );
    double radlength = m_transSvc->distanceInRadUnits
      ( start, end, 1e-35, dum, m_lhcbGeo );

    plot( radlength, "RVRadLength", 0, 0.01);
    if(msgLevel(MSG::DEBUG)){
      debug()<<"Radiation length from "<< start <<" to "
             << end <<" : "<< radlength 
             <<" [mm]" << endmsg;
    }

    if( radlength > threshold ){
      if(msgLevel(MSG::DEBUG))
        debug()<<"RV is too closed to a detector material --> disguarded !"
               << endmsg;
      return true;
    }
  } //end of >0 condition


  return false;
}
