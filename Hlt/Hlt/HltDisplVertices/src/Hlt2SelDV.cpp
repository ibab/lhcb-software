// Include files 

// from Gaudi
//#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"

//Use ODIN
#include "Event/ODIN.h"

//get the Header of the event
#include "Event/RecHeader.h"

// local
#include "Hlt2SelDV.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/OdinCuts.h"

//calorimeter
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"


using namespace Gaudi::Units ;
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2SelDV
//
// 2010-01-22 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt2SelDV );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2SelDV::Hlt2SelDV( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_nbevent(0)
    , m_nbpassed(0)
    , PV(0)
    , m_PreyID(0)
{
  declareProperty("SaveOnTES", m_SaveonTES = true );
  declareProperty("SaveTuple", m_SaveTuple = false );//save prey infos in Tuple
  declareProperty("Prey", m_Prey = "~chi_10" );
  //>6.286GeV=Bc+ Mass
  declareProperty("MinNBCands", m_NbCands = 1 );
  declareProperty("MinNBCandsExclusive", m_NbCandsExclusive = false );
  declareProperty("PreyMinMass", m_PreyMinMass = 6.3*GeV );
  //Unlimited
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*TeV );
  declareProperty("PreyMaxSumpt", m_PreyMaxSumPt = 14.*TeV );
  declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
  declareProperty("PreyMinAngle", m_PreyMinAngle = 0. );
  declareProperty("PreyMinFD", m_PreyMinFD = 0. );
  declareProperty("PreyMinHighestMass", m_PreyMinHighMass = 0*GeV );
  declareProperty("AllOutDet", m_allOutDet = true );

  declareProperty("MinRecpt", m_MinRecpt = 0*GeV );
  declareProperty("RMin", m_RMin = 0.3*mm );//0.06 in K
  declareProperty("RMax", m_RMax = 10.*m );
  declareProperty("MaxChi2OvNDoF", m_MaxChi2OvNDoF = 1000. );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks 5
  declareProperty("RCutMethod", m_RCut = "FromBeamSpot" );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
  declareProperty("RemFromRFFoil", m_RemFromRFFoil = false );
  declareProperty("BeamLineLocation", 
                  m_BLLoc = "HLT/Hlt2LineDisplVertices/BeamLine");
  declareProperty("SigmaZ", m_SigmaZ = 1000. );
  declareProperty("SigmaR", m_SigmaR = 1000. );
  declareProperty("MinZ", m_MinZ = -10.*m );
  declareProperty("MaxZ", m_MaxZ = 100*m );
  declareProperty("PVnbtrks", m_PVnbtrks = 5 ); //corr. to 'tight' PV reco
  declareProperty("PhaseSpacePS", m_phaseSpacePS = false ); //phase space prescaler


}
//=============================================================================
// Destructor
//=============================================================================
Hlt2SelDV::~Hlt2SelDV() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2SelDV::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if( m_RemVtxFromDet != 0 || m_SaveTuple ){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();

    // Get Transport Service
    m_transSvc = svc<ITransportSvc>( "TransportSvc", true  );
  }

  const ParticleProperty* Prey = ppSvc()->find( m_Prey );
  if ( !Prey ) { //
    err() << "Cannot find particle property for " << m_Prey << endmsg ;
    return StatusCode::FAILURE;
  }
  m_PreyID = Prey->particleID();

  if( m_RemFromRFFoil && m_RemVtxFromDet == 4 ){
    info()<<"RemFromRFFoil = "<< m_RemFromRFFoil <<" and RemVtxFromDet = "
	  << m_RemVtxFromDet <<" are incompatible. RemFromRFFoil set to false"
	  << endmsg;
    m_RemFromRFFoil = false;
  }

  if(msgLevel(MSG::DEBUG)){
    debug() <<"==> Initialize Hlt2SelDV"<< endmsg;
    debug()<<"-------------------------------------------------------"<<endmsg;
    debug() << "Hlt2SelDV will select " << m_Prey 
            << " candidates (ID=" << m_PreyID.pid() <<") ";
    debug() << ". Selection Cuts are : " << endmsg;
    debug() << "Min nb of desired candidates : "<< m_NbCands<< endmsg;
    debug() << m_Prey <<" minimum mass " 
            << m_PreyMinMass/Gaudi::Units::GeV <<" GeV" << endmsg;
    debug() << m_Prey <<" maximum mass " 
            << m_PreyMaxMass/Gaudi::Units::GeV <<" GeV" << endmsg;
    debug() << "Minimum number of tracks at the reconstructed vertex of "
            << m_nTracks <<" tracks."<< endmsg ;
    debug() << "Max chi2/ndof of a vertex "<< m_MaxChi2OvNDoF << endmsg;
    debug() <<"Min measured pT : "<< m_MinRecpt/GeV <<" GeV"<< endmsg;
    debug() << "Min sum of daughters's pT "<< m_SumPt << endmsg;
    debug() << "Max sigma R : "<< m_SigmaR <<" mm"<< endmsg;
    debug() << "Max sigma Z : "<< m_SigmaZ <<" mm"<< endmsg;
    if( m_RemFromRFFoil )
      debug()<<"Not in RF-Foil region"<< endmsg ;
    debug()<< "The radial displacement is ";
    if( m_RCut == "FromUpstreamPV" ){
      debug() << "computed with respect to the upstream PV of PV3D." << endmsg;
      debug()<< "Min nb of tracks on the upPV candidate : "
            << m_PVnbtrks << endmsg;      
    } else if( m_RCut == "FromBeamLine" ){
      debug() << "computed with respect to the beam line given at " 
              << m_BLLoc << endmsg;
    } else {
      debug() << "computed with respect to (0,0,z) in the global LHCb frame" 
              << endmsg;
      debug()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!" 
             << endmsg;
    }
    debug() <<"Min R             : " << m_RMin/mm <<" mm"<< endmsg ;
    debug() <<"Max R             : " << m_RMax/mm <<" mm"<< endmsg ;
    debug() <<"BEWARE : not all daughters may be saved !"<< endmsg;
    debug()<<"-------------------------------------------------------"<<endmsg;
    
  }

  //Initialize the beam line
  m_BeamLine = new Particle();

  //Set beam line to z axis
  if( m_RCut=="" ){
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
    //    Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin);
    //Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin);
    //if( msgLevel( MSG::DEBUG ) )
      //debug() <<"Velo global right half center "
	    //  << rightcenter <<", left half center "<< lefthalv << endmsg;
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
StatusCode Hlt2SelDV::execute() {

  ++m_nbevent;
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute event "
				      << m_nbevent  << endmsg;
  setFilterPassed(false);
  m_GeoInit = false; //be sure the goe is initialised if needed.

  //------------------Get the (upstream) Primary Vertex------------------
  GetUpstreamPV();
  if( msgLevel( MSG::DEBUG ) && PV != NULL )
    debug() << "Upstream PV Position : " << PV->position() << endmsg ;

  //------------------The Code---------------------------  
  Particle::Range preys = particles();
  if( msgLevel( MSG::DEBUG ) )
    debug() << "There are " << preys.size() <<" particles in TES !" << endmsg;
  if( preys.size() < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of particles in TES !" << endmsg;
    return StatusCode::SUCCESS;
  }

  vector<int>  nboftracks;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts,  indets,masses,minvd,rs,massCorrs,massCorrsIP,thetas;
  //int nPV(0);
  //int nPVSel(0);
  const RecVertex::Range PVs = this->primaryVertices();
  //nPV =PVs.size();
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
  //nPVSel =SelectedPVs.size();
 
  if( msgLevel( MSG::DEBUG ) )
    debug()<<"--------Reconstructed Displ. Vertices --------------"<< endmsg;
  //Particle::ConstVector Cands;
  int nbCands(0);
  double highestMass(0.);
  bool oneOutMat = false;

  Particle::Range::const_iterator iend = preys.end();
  for( Particle::Range::const_iterator is = preys.begin(); 
       is < iend; ++is ){
    const Particle * p = (*is);
    
    //Get rid of non-reconstructed particles, i.e. with no daughters.
    if( p->isBasicParticle() ){ 
      debug()<<"Basic particle !" << endmsg; 
      continue;
    }
    double mass = p->measuredMass();
    int nbtrks = p->endVertex()->outgoingParticles().size();
    double chi = p->endVertex()->chi2PerDoF();
    const Gaudi::XYZPoint & pos = p->endVertex()->position();
    double rho = p->info(52,-1000.);
    //double zpos= pos.z();
    Gaudi::LorentzVector mom = p->momentum();
    double sumpt = GetSumPt(p);
    //double muon = HasMuons(p);
    const Gaudi::SymMatrix3x3 & err = p->endVertex()->covMatrix();
    double errr = sqrt( err(0,0) + err(1,1) );

    //Let's go for Prey hunting
    if( msgLevel( MSG::DEBUG ) ){
      debug()<< m_Prey <<" candidate with mass "<< mass/Gaudi::Units::GeV 
             <<" GeV, nb of tracks " << nbtrks << ", Chi2/ndof " 
             << chi <<", R "<< rho <<", pos of end vtx "<< pos <<", sigmaX "
             << sqrt(err(0,0))<<", sigmaY "<< sqrt(err(1,1)) <<", sigmaZ "
             << sqrt(err(2,2)) <<", sigmaR "<< errr ;
      //if(muon){
      //  debug()<<", has muon with pt "<< muon <<" GeV" << endmsg;
      //} else { debug()<< endmsg; }
    }
    
    InitialiseGeoInfo();
    bool indet = IsAPointInDet( p, m_RemVtxFromDet );
    
    //Is the particle close to the detector material ?
    if( m_allOutDet && indet ) continue;    
    if( !m_allOutDet && !indet) oneOutMat=true;

    if( mass < m_PreyMinMass || 
        nbtrks < m_nTracks || rho <  m_RMin || rho > m_RMax || 
        sumpt < m_SumPt || chi > m_MaxChi2OvNDoF || 
        pos.z() < m_MinZ || pos.z() > m_MaxZ ||
        errr > m_SigmaR || sqrt(err(2,2)) > m_SigmaZ || ( mass > m_PreyMaxMass && sumpt > m_PreyMaxSumPt) ){  
      //muon < m_MuonpT || 
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the cuts"<< endmsg; 
      continue; 
    }
    if(mass > highestMass )highestMass = mass;

    double theta = 0.;
    double mindist = 1000000000.;
    double minip = 1000000000.;
    for (std::vector<const RecVertex*>::const_iterator ipv = SelectedPVs.begin(); ipv != SelectedPVs.end(); ipv++){
      //Check that we are forward
      if ((p->endVertex()->position().z()-(*ipv)->position().z())<0.)continue;
      // Get BestIP...
      Gaudi::XYZPoint vertex = (*ipv)->position();
      //Gaudi::XYZPoint point = pos;
      Gaudi::XYZVector direction (mom.Px(),mom.Py(),mom.Pz()); 
      Gaudi::XYZVector d = direction.unit();
      Gaudi::XYZVector ipV = d.Cross((pos-vertex).Cross(d));
      double ip = (ipV.z() < 0) ? -ipV.R() : ipV.R();
      if (ip< minip ){
        theta = fabs(atan(sqrt(pow(pos.x()-(*ipv)->position().x(),2)+
                               pow(pos.y()-(*ipv)->position().y(),2))/(pos.z()-(*ipv)->position().z())));
        minip = ip;
      }
      if((p->endVertex()->position()-(*ipv)->position()).R()<mindist){
	mindist=(p->endVertex()->position()-(*ipv)->position()).R();
      }
    }
    
    if( theta< m_PreyMinAngle || mindist < m_PreyMinFD ){
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the min angle/ FD cut"<< endmsg; 
      continue; 
    }

    //Particle * clone = new Particle( *p );
    Particle clone = Particle( *p );
    //clone->setParticleID( m_PreyID );
    clone.addInfo(80,mindist ); 
    clone.addInfo(81, mass ); 
    clone.addInfo(82, sumpt ); 
    clone.addInfo(51,double(indet) ); 
    clone.setParticleID( m_PreyID );
    this->markTree( &clone );
    nbCands++;

  }//  <--- end of Prey loop
//   if( (unsigned int)nbCands < m_NbCands || (m_NbCandsExclusive && (unsigned int)nbCands!=m_NbCands)){
  if( (unsigned int)nbCands < m_NbCands || 
      (m_NbCandsExclusive && (unsigned int)nbCands!=m_NbCands )|| 
      (m_NbCands>1 && ( highestMass < m_PreyMinHighMass || (!m_allOutDet && !oneOutMat ))))
  {
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of candidates !"<< endmsg;
    return StatusCode::SUCCESS;
  }
  setFilterPassed(true); 
  ++m_nbpassed;

  if( msgLevel( MSG::DEBUG ) )
    debug() << "Nb of " << m_Prey <<" candidates "<< nbCands << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2SelDV::finalize() {

  if ( msgLevel(MSG::DEBUG) ){ debug() << "==> Finalize Hlt2SelDV" 
				       << endmsg;
    if(m_nbevent == 0) m_nbevent++;
    double err = 10.*std::sqrt( static_cast<double>(m_nbpassed/m_nbevent) );
    debug() << "------------- Efficiency -----------"<< endmsg;
    debug() << "| Accepted event (%)     "<< 100.*m_nbpassed/m_nbevent 
	    <<"+-" << err
	    << " ( "<< m_nbpassed <<" )" << endmsg;
    debug() << "| Total number of events " << m_nbevent << endmsg;
    debug() << "------------------------------------"<< endmsg;
  }

  if( m_RCut !="FromBeamLine" )  delete m_BeamLine;

  return DVAlgorithm::finalize();
}

//=============================================================================
//  Remove RV if found to be in detector material
//=============================================================================

bool Hlt2SelDV::IsAPointInDet( const Particle* P, int mode){
  /* for other mode please get the code in old versions */
  if( mode < 1 ) return false;

  const Vertex* RV = P->endVertex();
  if( mode == 5 ){
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

bool Hlt2SelDV::IsInMaterialBoxLeft(const Gaudi::XYZPoint& point){
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

bool Hlt2SelDV::IsInMaterialBoxRight(const Gaudi::XYZPoint& point){
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
//  Loop on the daughter track to see if there is at least one backward track
//  and one forward tracks
//=============================================================================
bool Hlt2SelDV::HasBackAndForwardTracks( const RecVertex* RV ){
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
double Hlt2SelDV::GetSumPt( const Particle * p ){

  double sumpt = 0;
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i = 
	 p->daughters().begin(); i != iend; ++i ){
    sumpt += i->target()->pt();
  }
  return sumpt;
}

//=============================================================================
// Get radial distance to Beam Line
//=============================================================================
double Hlt2SelDV::GetRFromBL( const Gaudi::XYZPoint& p ){
  
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

//============================================================================
// Get the upstream Primary vertex
//============================================================================
void Hlt2SelDV::GetUpstreamPV(){

  const RecVertex::Range PVs = this->primaryVertices();
  PV = NULL;
  if( PVs.empty() ) return;
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
    if( msgLevel( MSG::DEBUG ) )
      debug() <<"PV candidate : nb of tracks "<< pv->tracks().size() << endmsg;
    //<<" sigmaR "<< sr <<" sigmaZ "<< sqrt(mat(2,2)) << endmsg;
    //if( sr > m_PVsr ) continue;
    //if( sqrt(mat(2,2)) > m_PVsz ) continue;
    if( pv->tracks().size() < m_PVnbtrks ) continue;
    if( z < tmp ){
      tmp = z;
      PV = (*i);
    } 
  }
}

// //============================================================================
// // Prescale as function of mass and r
// //============================================================================
// bool Hlt2SelDV::parametricPrescaler( double mass , double r , int cand ){
//   // the function
//   double acceptFrac = 1. ;
//   std::ostringstream seed;
//   seed << "DisplVertPrescale" << cand ;
//   // Prescale or not?
//   const LoKi::Cuts::ODIN_PRESCALE scale  = LoKi::Cuts::ODIN_PRESCALE ( acceptFrac , seed.str() ) ;
//   const LHCb::ODIN* odin = get<LHCb::ODIN*>( LHCb::ODINLocation::Default ) ;
//   return scale ( odin ) ;
// }


void Hlt2SelDV::InitialiseGeoInfo(){
  if( m_GeoInit ) return; //no need to do it more than once per event.
  
  //get the Velo geometry
  string velo = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo";
  const IDetectorElement* lefthalv = getDet<IDetectorElement>( velo+"Left" );
  const IDetectorElement* righthalv =  getDet<IDetectorElement>( velo + "Right" );
  const IGeometryInfo* halflgeominfo = lefthalv->geometry();
  const IGeometryInfo* halfrgeominfo = righthalv->geometry();
  Gaudi::XYZPoint localorigin(0,0,0);
  //Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin);
  //Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin);
  if( msgLevel( MSG::DEBUG ) )
    //debug() <<"Velo global right half center "
	  //  << rightcenter <<", left half center "<< lefthalv << endmsg;
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
  m_GeoInit = true;
}
