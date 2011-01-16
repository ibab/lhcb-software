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

//for beam line
#include "LHCbMath/EigenSystem.h"   
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TF1.h"

// Local
#include "Hlt2DisplVerticesDEV.h"

using namespace Gaudi::Units ;
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2DisplVerticesDEV
//
// 2009-july-1 : Neal Gauvin
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(Hlt2DisplVerticesDEV);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2DisplVerticesDEV::Hlt2DisplVerticesDEV(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
    , m_nbevent(0)
    , m_nbpassed(0)
    , m_pt(400.)
    , m_BeamLine(0)
    , m_bin(500)
  //for now, settable only internally
    , m_minz(-25*cm)
    , m_maxz(75*cm)
    , m_maxx(5*mm)
    , m_maxy(5*mm)
    , m_maxchi(2)
    , m_mintrks(10)
{
  declareProperty("SaveHidValSelCut", m_HidValSel = false );
  declareProperty("SaveInTuple", m_Save = true );
  declareProperty("DefMom", m_DefMom = true );
  declareProperty("RCutMethod", m_RCutMethod = "FromUpstreamPV" );
  declareProperty("InputDisplacedVertices", m_InputDisplVertices = 
                  "Rec/Vertices/Hlt2RV");
  declareProperty("RMin", m_RMin = 0.3 );
  declareProperty("MinMass", m_MinMass = 2*GeV );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 1*mm  );
  declareProperty("RemVtxFromDetWithSigma", m_RemVtxFromDetSig = -1  );
  declareProperty("BeamLineInitPos", m_BLInitPos ); //no default values !
  declareProperty("BeamLineInitDir", m_BLInitDir );
  declareProperty("BeamLineCycle", m_cycle = 1000 );
  declareProperty("ForwardTracks", m_forwardTracks = "");
  declareProperty("VeloTracks", m_veloTracks = "");
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
    debug() << "==> Initialize Hlt2DisplVerticesDEV" << endmsg;
    debug() << "---------------- CUTS on RecVertex --------------"<< endmsg;
    debug() << "No backward tracks"<< endmsg;
    debug() << "the upstream RV will be disguarded"<< endmsg;
    debug() << "Min number of tracks           "<< m_MinNbtrks << endmsg;
    debug() << "Min radial displacement        "<< m_RMin <<" mm"<< endmsg;
    debug() << "Min reconstructed mass         "<< m_MinMass/GeV 
	    <<" GeV"<< endmsg;
    debug()<< "The radial displacement is ";
    if( m_RCutMethod == "LocalVeloFrame" )
      debug() << "computed with respect to (0,0,z) in the local Velo frame." 
	      << endmsg; 
    else if( m_RCutMethod == "FromUpstreamPV" ){
      debug() << "computed with respect to the upstream PV of PV3D." << endmsg;
    } else if( m_RCutMethod == "FromUpstreamPVOpt" ){
      debug() << "computed with respect to the upstream rec vertex." 
	      << endmsg;
    } else if( m_RCutMethod == "CorrFromUpstreamPV" ){
      debug() << "computed with respect to the upstream PV"
	      <<" Take the position of the associated 2D RV, if any."<< endmsg;
    } else {
      debug() << "computed with respect to (0,0,z) in the global LHCb frame" 
	      << endmsg;
      debug()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!" 
             << endmsg;
      
    }
    debug() << "------------------------------------"<< endmsg;
    if( m_HidValSel ){
      m_Save = true;
      debug() << "You asked to save the Hlt2HidValley values."<< endmsg;
      debug() << "------------------------------------"<< endmsg;
    }
  }

  if( m_RemVtxFromDet != 0 || m_RemVtxFromDetSig > 0 ){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();

    // Get Transport Service
    if( m_RemVtxFromDet > 0 || m_RemVtxFromDetSig > 0 ) 
      m_transSvc = svc<ITransportSvc>( "TransportSvc", true  );
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
    //m_toGlobalFrame = halfgeominfo->toGlobalMatrix();
  }

  //Initialize the beam line
  m_BeamLine = new Particle();
  if( m_RCutMethod == "FromBeamLine" ){

    if( !m_BLInitPos.empty() ){
      //sanity check
      if( m_BLInitPos.size()<3 ) 
        info()<<"BeamLineInitPos not set properly. Need 3 doubles. "
              <<"Beam line initial position will be disguarded."<< endmsg;
      m_BeamLine->setReferencePoint(
        Gaudi::XYZPoint( m_BLInitPos[0], m_BLInitPos[1], m_BLInitPos[2] ) );
      if(msgLevel(MSG::DEBUG))
        debug()<<"Beam line initial position : "<< m_BLInitPos << endmsg;
    }
    
    if( !m_BLInitDir.empty() ){
      //sanity check
      if( m_BLInitDir.size()<3 ) 
        info()<<"BeamLineInitDir not set properly. Need 3 doubles. "
              <<"Beam line initial direction will be disguarded."<< endmsg;
      m_BeamLine->setMomentum(
  Gaudi::LorentzVector( m_BLInitDir[0], m_BLInitDir[1], m_BLInitDir[2], 0. ) );
      if(msgLevel(MSG::DEBUG))
        debug()<<"Beam line initial direction : "<< m_BLInitDir << endmsg;
    }    

    //Book histos
    m_x = book1D("PV x position", -m_maxx, m_maxx, m_bin);
    m_y = book1D("PV y position", -m_maxy, m_maxy, m_bin);
    m_z = book1D("PV z position", m_minz, m_maxz, m_bin);
    
    int bins2 = m_bin*m_bin;
    m_xx = book1D("PVxx", 0., (m_maxx*m_maxx), bins2);
    m_yy = book1D("PVyy", 0., (m_maxy*m_maxy), bins2);
    m_zz = book1D("PVzz", 0. , (m_maxz*m_maxz), bins2);
    m_xy = book1D("PVxy", -(m_maxx*m_maxy), (m_maxx*m_maxy), bins2);
    m_xz = book1D("PVxz", -(m_maxx*m_maxz), (m_maxx*m_maxz), bins2);
    m_yz = book1D("PVyz", -(m_maxy*m_maxz), (m_maxy*m_maxz), bins2);
  }

  //Set beam line to z axis
  if( m_RCutMethod=="" ){
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( 0., 0., 0. ) );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  } 


  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode Hlt2DisplVerticesDEV::execute() {

  ++m_nbevent;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute event "
                                   << m_nbevent << endmsg;
  setFilterPassed(false);

  //A container to save all candidates
  Particle::ConstVector RecParts;

  //update beam line position and direction
  if( m_RCutMethod == "FromBeamLine" && !BeamLineCalibration() ) 
    return desktop()->saveTrees( RecParts );

  m_map.clear(); //Re-initialize the map

  //Retrieve the RecVertex from private PatPV3D
  RecVertices* RVs = get<RecVertices>(m_InputDisplVertices);
  if(msgLevel(MSG::DEBUG))
    debug()<<"Retrieved "<< RVs->size() 
	   <<" displ vertices, the one with lowest z will be disguarded" 
	   << endmsg;
  plot( RVs->size(), "NbRV", 0, 6);
  if( RVs->empty() ) return desktop()->saveTrees( RecParts );
  //sort them by ascending z position
  sort( RVs->begin(), RVs->end(), SortPVz);

  //Retrieve the RecVertex from PV official reconstruction
  RecVertex::ConstVector PVs;
  if( m_RCutMethod=="FromUpstreamPV" || m_RCutMethod=="CorrFromUpstreamPV" ){
    const RecVertex::Range PVc = this->primaryVertices();
    int size = PVc.size();
    if(msgLevel(MSG::DEBUG))
      debug()<<"Retrieved "<< size <<" primary vertices" << endmsg;
    plot( size, "NbPV", 0, 6);
    if( PVc.empty() ) return StatusCode::SUCCESS;
    for( RecVertex::Range::const_iterator i = PVc.begin(); 
         i != PVc.end(); ++i ){
      PVs.push_back( *i );
    }
    //sort them by ascending z position
    sort( PVs.begin(), PVs.end(), SortPVz);
  }

  //Some check up
  //PrintTracksType();
  //PrintParticle();

  vector<double> Xs, Ys, Zs, sumPt, RPt, Ms, Muonpts; 
  vector<int> Nbtrks; 
  vector<int> InDets;

  //Let's loop on the RecVertex
  RecVertices::const_iterator iRV = RVs->begin();

  //Set reference axis for radial measurment
  Gaudi::XYZPoint UpPV;
  if( m_RCutMethod=="FromUpstreamPV" || m_RCutMethod=="CorrFromUpstreamPV" ){
    UpPV = (*PVs.begin())->position();
    if(msgLevel(MSG::DEBUG))
      debug() <<"Upstream PV position "<< UpPV << endmsg;
    m_BeamLine->setReferencePoint( UpPV );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  } else if( m_RCutMethod=="FromBeamLine" ){
    UpPV = m_BeamLine->referencePoint();
  } else {
    UpPV = (*iRV)->position();
    m_BeamLine->setReferencePoint( UpPV );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  }

  iRV++; //Do not consider first one
  for(; RVs->end() != iRV; ++iRV) {
    const RecVertex* RV = *iRV;

    if(msgLevel(MSG::DEBUG)) 
      debug()<<"Rec Vertex position "<< RV->position() << endmsg;

    if(( m_RCutMethod == "FromUpstreamPV"  || m_RCutMethod=="CorrFromUpstreamPV" ) && RV->position().z() < UpPV.z() ){
      if(msgLevel(MSG::DEBUG))
	debug() <<"RV z position comes before the upstream 2D RV," 
		<<" not considered !"<< endmsg;
      continue;
    }

    //Check if RV has backward tracks to avoid PVs
    if( HasBackwardTracks(RV) ){ 
      if(msgLevel(MSG::DEBUG))
	debug() <<"RV has a backward track, not considered !"<< endmsg;
      continue;
    }

    Gaudi::XYZPoint Pos( RV->position() );
    if( m_RCutMethod == "LocalVeloFrame" ){
      Pos = m_toVeloFrame * Pos;
    }
    else if( m_RCutMethod == "CorrFromUpstreamPV" ){
      Pos = GetCorrPosition( RV, PVs );
    }

    double R;
    if( m_RCutMethod == "FromUpstreamPV" || 
        m_RCutMethod == "FromUpstreamPVOpt" ||
        m_RCutMethod == "CorrFromUpstreamPV" ){
      R = (RV->position() - UpPV).rho();
    } else if( m_RCutMethod == "FromBeamLine" ){
      R = RadDist(RV->position());
    } else {
      R = Pos.rho();
    }

    double sumpt, recpt, mass, muonpt;

    // Turn RecVertex into a Particle
    if( !RecVertex2Particle( RV, RecParts, mass, sumpt, recpt, muonpt ) ) 
      continue;

    //Properties of reconstructed vertices
    Ms.push_back( mass );
    Zs.push_back( Pos.z() );
    Ys.push_back( Pos.y() );
    Xs.push_back( Pos.x() );
    sumPt.push_back( sumpt );
    Nbtrks.push_back( RV->tracks().size() );
    RPt.push_back( recpt );
    Muonpts.push_back( muonpt );

    if(msgLevel(MSG::DEBUG)){
      debug()<<"RV mass "<< mass/GeV <<" GeV, R "<< R <<" mm, rec pt "<< recpt 
	     <<" GeV, sum pt "<< sumPt <<" GeV"<< endmsg;
      debug()<<"RV contains a muon with pt "<< muonpt << endmsg;
    }

    //Do not save if found to be in detector material
    bool InDet = false;
    if( ( m_RemVtxFromDet || m_RemVtxFromDetSig > 0 ) && RemVtxFromDet(RV) ){
      InDet = true;
    } 
    InDets.push_back(InDet);

    //rm candidate from Desktop if it doesn't pass the cuts
    //mass cut already been applied
    if( mass > m_MinMass && (InDet || R < m_RMin) ) RecParts.pop_back();
  }    

  //Don't save every ntuples
  if( Zs.empty() ) return desktop()->saveTrees( RecParts );

  if( m_Save ){
    const int maxSize = 25;
    Tuple tuple = nTuple("HltLine");
    tuple->farray( "X", Xs.begin(), Xs.end(), "Nb", maxSize );
    tuple->farray( "Y", Ys.begin(), Ys.end(), "Nb", maxSize );
    tuple->farray( "Z", Zs.begin(), Zs.end(), "Nb", maxSize );
    tuple->farray( "sumPt", sumPt.begin(), sumPt.end(), "Nb", maxSize );
    tuple->farray( "NbTrks", Nbtrks.begin(), Nbtrks.end(), "Nb", maxSize );
    tuple->farray( "RPt", RPt.begin(), RPt.end(), "Nb", maxSize );
    tuple->farray( "M", Ms.begin(), Ms.end(), "Nb", maxSize );
    tuple->farray( "InDet", InDets.begin(), InDets.end(), "Nb", maxSize );
    tuple->farray( "MuonPt", Muonpts.begin(), Muonpts.end(), "Nb", maxSize );
    tuple->column( "PVx", UpPV.x() );
    tuple->column( "PVy", UpPV.y() );
    if( !fillHeader( tuple ) ) return desktop()->saveTrees( RecParts );
    if( m_HidValSel && !SaveHidValSel( tuple, RVs ) ) 
      return desktop()->saveTrees( RecParts );
    tuple->write();
  }

  // Cuts 
  if( RecParts.size() > 0 ){
    if(msgLevel(MSG::DEBUG))
      debug()<<"Event satisfied HLT2DisplVertices criteria ! "
	     <<"Nb of candidates "<< RecParts.size() << endmsg;
    ++m_nbpassed;
    setFilterPassed(true);
    SaveBeamLine();
  } else { if(msgLevel(MSG::DEBUG)) debug()<<"Event rejected !"<< endmsg; } 

  //Save eventual candidates in TES
  return desktop()->saveTrees( RecParts );
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode Hlt2DisplVerticesDEV::finalize() {

  if (msgLevel(MSG::DEBUG)) {
    debug() << "==> Finalize Hlt2DisplVerticesDEV" << endmsg;
    if(m_nbevent == 0) m_nbevent++;
    double err = 10.*std::sqrt( static_cast<double>(m_nbpassed/m_nbevent) );
    debug() << "------------- Efficiency -----------"<< endmsg;
    debug() << "| Accepted event (%)     "<< 100.*m_nbpassed/m_nbevent 
	    <<"+-" << err
	    << " ( "<< m_nbpassed <<")" << endmsg;
    debug() << "| Total number of events " << m_nbevent << endmsg;
    debug() << "------------------------------------"<< endmsg;
  }

  delete m_BeamLine;

  return DVAlgorithm::finalize();
}

//=============================================================================
// Turn RecVertex into a Particle
//=============================================================================
bool Hlt2DisplVerticesDEV::RecVertex2Particle( const RecVertex* rv, 
			  Particle::ConstVector & RecParts, double & mass,
			  double & sumpt, double & recpt, double & muonpt  ){

  //Retrieve Particles corresponding to vertices
  if( m_map.empty() ) CreateMap(); //Create map if necessary

  Particle::ConstVector Parts ;
  GetPartsFromRecVtx( rv, Parts );

  //Create a particle
  Particle tmpPart = Particle( ); //don't give any PID.

  //Compute momentum
  muonpt = 0;
  Gaudi::LorentzVector mom;
  sumpt = 0.;
  Particle::ConstVector::const_iterator ip = Parts.begin();
  Particle::ConstVector::const_iterator iend = Parts.end();
  for( ; ip != iend; ++ip ){
    mom += (*ip)->momentum();
    double pt = (*ip)->pt();
    sumpt += pt;
    //save only high pt daughters, get rid of default pions
    if( pt > 1*GeV ) continue;
    tmpPart.addToDaughters(*ip);
    //check if tracks could be a muon
    if( (*ip)->proto() == NULL ) continue;
    if( (*ip)->proto()->muonPID() == NULL ) continue;
    if( pt > muonpt && (*ip)->proto()->muonPID()->IsMuonLoose() ) 
      muonpt = pt;
    
  }
  // Compute the pT of a RecVertex from all its tracks 
  recpt = mom.Pt();
  // mass
  mass = mom.M();

  //create a particle to save in TES only if candidates passes the cuts
  if( mass < m_MinMass ) return true;

  //Create an decay vertex
  const Gaudi::XYZPoint point = rv->position();
  Vertex tmpVtx = Vertex( point );
  tmpVtx.setNDoF( rv->nDoF());
  tmpVtx.setChi2( rv->chi2());
  tmpVtx.setCovMatrix( rv->covMatrix() );
  
  //Fix end vertex
  tmpPart.setEndVertex( &tmpVtx );
  tmpPart.setReferencePoint( point );
  tmpPart.setMomentum( mom );
  tmpPart.setMeasuredMass( mass );
  tmpPart.setMeasuredMassErr( 0 );

  Gaudi::SymMatrix4x4 MomCovMatrix = 
    Gaudi::SymMatrix4x4( ROOT::Math::SMatrixIdentity() );
  tmpPart.setMomCovMatrix( MomCovMatrix );
  Gaudi::SymMatrix3x3 PosCovMatrix = 
    Gaudi::SymMatrix3x3( ROOT::Math::SMatrixIdentity() );
  tmpPart.setPosCovMatrix( PosCovMatrix );
  Gaudi::Matrix4x3 PosMomCovMatrix;
  tmpPart.setPosMomCovMatrix( PosMomCovMatrix );

  //Save Rec Particle in the Desktop
  RecParts.push_back( desktop()->keep( &tmpPart ) );
  

  return true;
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
  double pz = m_pt/sqrt( sx*sx + sy*sy );
  double e = std::sqrt( m_piMass*m_piMass + m_pt*m_pt + pz*pz );
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
// If can find an associated 2D RV return the 2D RV  
//============================================================================
Gaudi::XYZPoint Hlt2DisplVerticesDEV::GetCorrPosition( const RecVertex* RV,
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

    //Check that the 2DRV is not too far
    //if( abs( RV->position().z() - (*iRV)->position().z() ) > 1. )
    //return RV->position();

    //loop on the 2D RV's tracks
    for( SmartRefVector<Track>::const_iterator it = 
	   (*iRV)->tracks().begin(); it != (*iRV)->tracks().end(); ++it ){

      //Check if track is in the 3D RV
      tRV = tbeg;
      while( tRV != tend ){

// 	std::cout<<"Checking key "<< (*tRV)->key() <<" "<< key <<" "
// 		 << (*tRV)->slopes() <<" "<< it->target()->slopes() 
// 		 << std::endl;

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
	plot( RV->position().z() - (*iRV)->position().z(), "RVdZ3Dto2D_trig",
	      -1.2, 1.2 );
	plot( (RV->position() - (*iRV)->position()).rho(), "RVdR3Dto2D_trig",
	      0, 2 );
	plot( (*iRV)->position().rho(), "RVR2D", 0, 1. );
	return (*iRV)->position();
      }
    }
  }
  
  return RV->position();
}

//============================================================================
// Save in Tuple the values from Hlt2HidValley
//============================================================================
StatusCode  Hlt2DisplVerticesDEV::SaveHidValSel( Tuple & tuple,  RecVertices* RVs ){

  //Global track cuts
  double sumPtTracks = 0.;
  double sumXYTrackfirstStates = 0.;

  //Get forward tracks
  Tracks* inputTracks = get<Tracks>(m_forwardTracks);

  for(Track::Container::const_iterator itr = inputTracks->begin(); 
      inputTracks->end() != itr; itr++) {
    const Track* trk = *itr;
    double xyfState = sqrt(trk->firstState().x() * trk->firstState().x() +
			   trk->firstState().y() * trk->firstState().y());
    sumPtTracks += trk->pt();
    sumXYTrackfirstStates += xyfState;
  }

  //Find the upstream PV
  const RecVertex::Range primVertices = this->primaryVertices();
  if((primVertices.size() == 0) && inputTracks->size() == 0)
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
  for( RecVertices::const_iterator itRV = RVs->begin();
       RVs->end() != itRV; ++itRV) {
    const RecVertex* dVtx = *itRV;
    
    double distVtcs = sqrt((realPV->position().x() - dVtx->position().x()) * 
			   (realPV->position().x() - dVtx->position().x()) +
			   (realPV->position().y() - dVtx->position().y()) * 
			   (realPV->position().y() - dVtx->position().y()) +
			   (realPV->position().z() - dVtx->position().z()) * 
			   (realPV->position().z() - dVtx->position().z()));
    double xyDist = sqrt(dVtx->position().x() * dVtx->position().x() +
			 dVtx->position().y() * dVtx->position().y());
    if(distVtcs > .001) 
      sumSVxyDist += xyDist;
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

//============================================================================
// Event and run number 
//============================================================================
StatusCode Hlt2DisplVerticesDEV::fillHeader( Tuple & tuple ){

  const LHCb::RecHeader* header = get<RecHeader>(RecHeaderLocation::Default);  
  debug() << "Filling Tuple Event " << header->evtNumber() << endmsg ;
  tuple->column("Event", (int)header->evtNumber());
  return StatusCode::SUCCESS ;
}


//=============================================================================
//  Print track and particles used in this algo
//=============================================================================

void Hlt2DisplVerticesDEV::PrintTracksType(){

  Tracks* VeloTrks = get<Tracks>(m_veloTracks);
  Tracks* ForwardTrks = get<Tracks>(m_forwardTracks);

  debug()<<"Printing Velo Track content, size "<< VeloTrks->size() <<endmsg;
  for(Track::Container::const_iterator itr = VeloTrks->begin(); 
      VeloTrks->end() != itr; itr++) {
    const Track* trk = *itr;
    string s = "False !";
    if ( trk->checkFlag( Track::Backward ) ) s = "True !";
    debug()<<"Track key "<< trk->key()<<" slope "<< trk->slopes() 
	   <<" type "<<trk->type()<<" Is backward ? "<< s <<endmsg;
//     for( SmartRefVector<Track>::const_iterator j = trk->ancestors().begin();
// 	 j != trk->ancestors().end(); ++j )
//       debug()<<"Anc. Track key "<< (*j)->key()<<" slope "<< (*j)->slopes() 
// 	     <<" type "<<(*j)->type() <<endmsg;
  }
  debug()<<"Printing Foward Track content, size "<< ForwardTrks->size() <<endmsg;
  for(Track::Container::const_iterator itr = ForwardTrks->begin(); 
      ForwardTrks->end() != itr; itr++) {
    const Track* trk = *itr;
    string s = "False !";
    if ( trk->checkFlag( Track::Backward ) ) s = "True !";
    debug()<<"Track key "<< trk->key()<<" slope "<< trk->slopes() 
	   <<" type "<<trk->type()<<" Is backward ? "<< s <<endmsg;
//     for( SmartRefVector<Track>::const_iterator j = trk->ancestors().begin();
// 	 j != trk->ancestors().end(); ++j )
//       debug()<<"Anc. Track key "<< (*j)->key()<<" slope "<< (*j)->slopes() 
// 	     <<" type "<<(*j)->type() <<endmsg;  
  }


}
void Hlt2DisplVerticesDEV::PrintParticle(){
  const Particle::ConstVector& InputParts = desktop()->particles();
  debug()<<"#########  Dumping Particle content, size "
	 << InputParts.size() <<endmsg;
  for ( Particle::ConstVector::const_iterator j = InputParts.begin();
	j != InputParts.end();++j) {
    debug()<<"Particle id "<< (*j)->particleID().pid() <<" Mass "
           << (*j)->measuredMass()/GeV << " GeV"<<" slope "
           << (*j)->slopes() << endmsg;
    if( (*j)->proto() == NULL ) continue;
    if( (*j)->proto()->muonPID() == NULL ) continue;
    debug() <<"Is muon ? "<< (*j)->proto()->muonPID()->IsMuon() 
	    <<" Loose ? "<< (*j)->proto()->muonPID()->IsMuonLoose() << endmsg;
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
// if m_RemVtxFromDetSig > 0 : remove reco vtx if rad length along 
//                             +- m_RemVtxFromDetSig * PositionCovMatrix
//
//=============================================================================
bool Hlt2DisplVerticesDEV::RemVtxFromDet( const RecVertex* RV ){

  double threshold = 1e-10;

  if( m_RemVtxFromDet < 0 && m_RemVtxFromDet < 0 ){

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
  else if( m_RemVtxFromDetSig < 0 && m_RemVtxFromDet > 0 ){

    const Gaudi::XYZPoint pos = RV->position();
    const Gaudi::XYZPoint dz = Gaudi::XYZPoint( 0., 0., m_RemVtxFromDet );


    if( m_lhcbGeo == 0 ){ 
      warning()<<"IGeometryInfo* m_lhcbGeo is broken"<< endmsg; return true; }


    IGeometryInfo* dum = 0;
    Gaudi::XYZPoint start = Minus(pos,dz);
    Gaudi::XYZPoint end = Plus(pos,dz);
    double radlength = m_transSvc->distanceInRadUnits
      ( start, end, 1e-35, dum, m_lhcbGeo );

    plot( radlength, "RVRadLength", 0, 0.01);
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
  } //end of >0 condition
  else if( m_RemVtxFromDetSig > 0 ){
 
    Gaudi::XYZPoint RVPosition = RV->position();
    Gaudi::SymMatrix3x3 RVPositionCovMatrix = RV->covMatrix();
    double sigNx = m_RemVtxFromDetSig*sqrt(RVPositionCovMatrix[0][0]);
    double sigNy = m_RemVtxFromDetSig*sqrt(RVPositionCovMatrix[1][1]);
    double sigNz = m_RemVtxFromDetSig*sqrt(RVPositionCovMatrix[2][2]);
    // Is there material within N*sigma
    double radlength = 0;
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
  }
  
  return false;
}

//============================================================================
// Update beam line position and direction 
//============================================================================
bool Hlt2DisplVerticesDEV::BeamLineCalibration(){

  //Get the reconstructed primary vertices
  const RecVertex::Range PVc = this->primaryVertices();
  int size = PVc.size();
  if(msgLevel(MSG::DEBUG))
    debug()<<"Retrieved "<< size <<" primary vertices." << endmsg;

  //Loop on the reconstructed primary vertices
  for( RecVertex::Range::const_iterator ipv = PVc.begin() ;
       ipv != PVc.end(); ++ipv ) {
    const RecVertex* pv = *ipv ;
    int nbtrks = pv->tracks().size();

    //transform the PV position to the local Velo frame
//     Gaudi::XYZPoint localPV;
//     if( msgLevel(MSG::DEBUG) || m_local ){
//       localPV = m_toVeloFrame * pv->position();
//     }

    if( msgLevel( MSG::DEBUG ) ){
      debug()<<"Primary Vertex position "<< pv->position() 
             <<" R "<< pv->position().rho()
             <<" Number of associated tracks "<< nbtrks 
             <<" Chi2/NDoF " << pv->chi2PerDoF() << endmsg;
      //debug()<<" PV in Velo local frame "<< localPV << endmsg;
    }
    Gaudi::XYZPoint PV = pv->position();
    //if( m_local ) PV = localPV;
    double x = PV.x();
    double y = PV.y();
    double z = PV.z();
    double r = PV.rho();
    double chi = pv->chi2PerDoF();
    
    //eventually cut on the PV !
    if( chi > m_maxchi || nbtrks < m_mintrks ) 
      continue;
    ++m_nbpv;
    
    //plot variables
    m_x->fill( x ); m_y->fill( y ); m_z->fill( z );
    m_xx->fill( x*x ); m_yy->fill( y*y ); m_zz->fill( z*z );
    m_xy->fill( x*y ); m_yz->fill( y*z ); m_xz->fill( x*z );
    
    plot( r, "PV radial distance to z axis", 0., m_maxx, m_bin ); 
    plot( chi, "PV chisquare per dof",0,5) ;
    plot( nbtrks, "PV number of tracks", -0.5, 120.5, 121 );
  }

  //Do not go on without any beam line position !  
  if( m_nbevent < m_cycle && m_BLInitPos.empty() ) return false;

  //Update the beam line 
  if( m_nbevent%m_cycle == 0 ){

    //Translate AIDA into ROOT
    TH1D* rx = Gaudi::Utils::Aida2ROOT::aida2root( m_x );
    TH1D* ry = Gaudi::Utils::Aida2ROOT::aida2root( m_y );
    TH1D* rz = Gaudi::Utils::Aida2ROOT::aida2root( m_z );
    TH1D* rxx = Gaudi::Utils::Aida2ROOT::aida2root( m_xx );
    TH1D* ryy = Gaudi::Utils::Aida2ROOT::aida2root( m_yy );
    TH1D* rzz = Gaudi::Utils::Aida2ROOT::aida2root( m_zz );
    TH1D* rxy = Gaudi::Utils::Aida2ROOT::aida2root( m_xy );
    TH1D* ryz = Gaudi::Utils::Aida2ROOT::aida2root( m_yz );
    TH1D* rxz = Gaudi::Utils::Aida2ROOT::aida2root( m_xz );
    
    //Get the means
    double mx, my, mz, mxx, myy, mzz, mxy, myz, mxz;
    rx->Fit("gaus","QO","goff"); ry->Fit("gaus","QO","goff");
    rz->Fit("gaus","QO","goff"); 
    mx = rx->GetFunction("gaus")->GetParameter(1);
    my = ry->GetFunction("gaus")->GetParameter(1);
    mz = rz->GetFunction("gaus")->GetParameter(1);
    mxx = rxx->GetMean();
    myy = ryy->GetMean();
    mzz = rzz->GetMean();
    mxy = rxy->GetMean();
    myz = ryz->GetMean();
    mxz = rxz->GetMean();

    //Compute the covariance matrix
    Gaudi::SymMatrix3x3 Cov;
    Cov(0,0) = mxx - mx*mx;
    Cov(0,1) = mxy - mx*my;
    Cov(0,2) = mxz - mx*mz;
    Cov(1,2) = myz - my*mz;
    Cov(1,1) = myy - my*my;
    Cov(2,2) = mzz - mz*mz;
    
    //let's diagonalize the matrix
    // create the evaluator 
    Gaudi::Math::GSL::EigenSystem Eval ;  

   // get the sorted vector of eigenvalues and eigenvectors
    Gaudi::Vector3  eigval;
    Gaudi::Matrix3x3 eigvects;
    StatusCode sc = 
      Eval.eigenVectors( Cov, eigval, eigvects );

    //fill eigenvalues and eigenvectors in beam calibration class
    if( sc.isFailure() ){
      warning()<<"Failed to get the eigenvalues and eigenvectors of " 
               <<"the IP covariance matrix, saving the estimate of "
               <<"the mean interaction point position."<< endmsg;
      return false;
    }
    
    //Transform back into LHCb frame


    //  Set X,Y,Z eigenvectors as being the vectors closest to the X,Y,Z axis, 
    //     respectively, with positive eigenvalues.

    // Eigenvectors that is most along x,y,z
    Gaudi::Matrix3x3 tmp;
    Gaudi::Vector3 tmpd;

    for(size_t ivec=0; ivec<3; ++ivec) {
      // clumsy way of finding the largest coordinate
      size_t icoord = 0;
      for(size_t jcoord = 1; jcoord<3; ++jcoord)
        if( abs(eigvects(icoord,ivec)) < abs(eigvects(jcoord,ivec)) )
          icoord = jcoord;
      tmp(0, icoord) = eigvects(0,ivec);
      tmp(1, icoord) = eigvects(1,ivec);
      tmp(2, icoord) = eigvects(2,ivec);
      tmpd.At(icoord) = eigval.At(ivec);
      // turn the vector around if the direction is wrong.
      if( tmp(icoord,icoord) < 0 ){ 
        tmp(0,icoord) *= -1; tmp(1,icoord) *= -1; tmp(2,icoord) *= -1;
        //change the sign of the eigenvalue as well.
        tmpd.At(icoord) *= -1;
      } 
    }
    eigval = tmpd;
    eigvects = tmp;

    //Eventually reset if the beams have moved
    double x = m_BeamLine->referencePoint().x() - mx;
    double y = m_BeamLine->referencePoint().y() - my;
    if( m_BeamLine->referencePoint().z() != 0. && sqrt( x*x + y*y ) > 0.05 ){
      m_x->reset();
      m_y->reset();
      m_z->reset();
      m_xx->reset();
      m_yy->reset();
      m_zz->reset();
      m_xy->reset();
      m_xz->reset();
      m_yz->reset();
    }

    //Save beam line informations
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( mx, my, mz ) );
    m_BeamLine->setMomentum(
     Gaudi::LorentzVector( eigvects(0,2), eigvects(1,2), eigvects(2,2), 0. ) );

    //Print out !
    int nbentries = m_x->allEntries();
    info()<<"Average interaction point position : "
          <<"( " << mx <<" +- "<< sqrt(abs(eigval(0))/nbentries) 
          <<", "<< my <<" +- "<< sqrt(abs(eigval(1))/nbentries) 
          <<", "<< mz <<" +- "<< sqrt(abs(eigval(2))/nbentries) 
          <<" )"<< endmsg;
    if ( msgLevel(MSG::DEBUG) ) 
      info()<<"Covariance matrix of the interaction point "
            <<"position distribution : "<< Cov << endmsg;
    info()<<"X eigenvector ( "<< eigvects(0,0) <<", "<< eigvects(1,0) <<", " 
          << eigvects(2,0) <<" ),"
          <<" Y eigenvector ( "<< eigvects(0,1) <<", "<< eigvects(1,1) <<", " 
          << eigvects(2,1) <<" )," 
          <<" Z eigenvector ("<< eigvects(0,2) <<", "<< eigvects(1,2) <<", " 
          << eigvects(2,2) <<" )" <<endmsg;
    info()<<"Eigenvalues "<< eigval << endmsg;
    info()<<"Done on "<< m_nbpv <<" primary vertices."<< endmsg;
  }
  
  //anyway...
  if( m_BeamLine->referencePoint().z() == 0. ) return false;
  
  //Save the beam line on the TES.
  SaveBeamLine();

  return true;
}

void Hlt2DisplVerticesDEV::SaveBeamLine(){

  //Save the beam line on the TES.
  Particles* vec = new Particles();
  vec->insert( m_BeamLine->clone() ); 
  put( vec, "/Event/HLT/Hlt2LineDisplVertices/BeamLine");

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

//============================================================================
// Distance of a XYZPoint to the beam interaction line
//============================================================================
double Hlt2DisplVerticesDEV::RadDist( const Gaudi::XYZPoint& p ){
  
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



