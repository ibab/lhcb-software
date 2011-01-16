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
  declareProperty("PreyMinMass", m_PreyMinMass = 6.3*GeV );
  //Unlimited
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*TeV );
  declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
  declareProperty("MinRecpt", m_MinRecpt = 0*GeV );
  declareProperty("RMin", m_RMin = 0.3*mm );//0.06 in K
  declareProperty("RMax", m_RMax = 10.*m );
  declareProperty("MaxChi2OvNDoF", m_MaxChi2OvNDoF = 1000. );
  declareProperty("MuonpT", m_MuonpT = -1*GeV );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks 5
  declareProperty("RCutMethod", m_RCut = "FromUpstreamPV" );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
  declareProperty("DetDist", m_DetDist = 1*mm );
  declareProperty("RemFromRFFoil", m_RemFromRFFoil = false );
  declareProperty("BeamLineLocation", 
                  m_BLLoc = "HLT/Hlt2LineDisplVertices/BeamLine");
  declareProperty("SigmaZ", m_SigmaZ = 1000. );
  declareProperty("SigmaR", m_SigmaR = 1000. );
  declareProperty("MinX", m_MinX = -10.*m );
  declareProperty("MaxX", m_MaxX = 10.*m );
  declareProperty("MinY", m_MinY = -10.*m );
  declareProperty("MaxY", m_MaxY = 10.*m );
  declareProperty("MinZ", m_MinZ = -10.*m );
  declareProperty("MaxZ", m_MaxZ = 100*m );
  declareProperty("PVnbtrks", m_PVnbtrks = 5 ); //corr. to 'tight' PV reco
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
    debug() << m_MinX/mm <<" mm < pos X < "<< m_MaxX/mm <<" mm"<< endmsg;
    debug() << m_MinY/mm <<" mm < pos Y < "<< m_MaxY/mm <<" mm"<< endmsg;
    debug() << m_MinZ/mm <<" mm < pos Z < "<< m_MaxZ/mm <<" mm"<< endmsg;
    debug() << "Max sigma R : "<< m_SigmaR <<" mm"<< endmsg;
    debug() << "Max sigma Z : "<< m_SigmaZ <<" mm"<< endmsg;
    if( m_MuonpT )
      debug()<<"At least one muon with pT > "<< m_MuonpT << endmsg;
    if( m_RemFromRFFoil )
      debug()<<"Not in RF-Foil region"<< endmsg ;
    if( m_RemVtxFromDet == 1 )
      debug()<<"Not in detector material"<< endmsg;
    if( m_RemVtxFromDet == 2 )
      debug()<<"Not closer than " << m_DetDist 
             <<"mm from detector material along momentum"<< endmsg;
    if( m_RemVtxFromDet == 3 || m_RemVtxFromDet == 4 )
      debug()<<"Not closer than " << m_DetDist 
             <<"*PosCovMatric from detector material"<< endmsg;
    if( m_RemVtxFromDet == 4 )
      debug()<<"("<< m_DetDist+3 <<" when in RF-Foil region)"<< endmsg;
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
StatusCode Hlt2SelDV::execute() {

  ++m_nbevent;
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute event "
				      << m_nbevent  << endmsg;
  setFilterPassed(false);

  //------------------Get the (upstream) Primary Vertex------------------
  GetUpstreamPV();
  if( msgLevel( MSG::DEBUG ) && PV != NULL )
    debug() << "Upstream PV Position : " << PV->position() << endmsg ;

  //------------------Set the beam line------------------
  if( m_RCut=="FromBeamLine" ){
    if( exist<Particle::Range>( m_BLLoc ) ){
      const Particle::Range BL = get<Particle::Range>( m_BLLoc );      
      const LHCb::Particle* tmp = *(BL.begin());
      m_BeamLine->setReferencePoint( tmp->referencePoint() );
      m_BeamLine->setMomentum( tmp->momentum() );
      if( msgLevel(MSG::DEBUG) )
        debug()<<"Beam line position "<< m_BeamLine->referencePoint()
	       <<" direction " << m_BeamLine->momentum() << endmsg;
    } else {
      warning()<<"No Beam line found at "<< m_BLLoc << endmsg;
      return StatusCode::SUCCESS;
    }
  } else if( m_RCut=="FromUpstreamPV" ){
    if(PV == NULL) return StatusCode::SUCCESS;
    m_BeamLine->setReferencePoint( PV->position() );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  }

  //------------------The Code---------------------------  
  Particle::ConstVector preys = desktop()->particles();
  if( msgLevel( MSG::DEBUG ) )
    debug() << "There are " << preys.size() <<" particles in TES !" << endmsg;
  if( preys.size() < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of particles in TES !" << endmsg;
    return StatusCode::SUCCESS;
  }

  vector<int>  nboftracks;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts, muons, indets;

  if( msgLevel( MSG::DEBUG ) )
    debug()<<"--------Reconstructed Displ. Vertices --------------"<< endmsg;
  //Particle::ConstVector Cands;
  int nbCands(0);
  Particle::ConstVector::const_iterator iend = preys.end();
  for( Particle::ConstVector::const_iterator is = preys.begin(); 
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
    double rho = GetRFromBL( pos );
    double zpos = pos.z();
    Gaudi::LorentzVector mom = p->momentum();
    double sumpt = GetSumPt(p);
    double muon = HasMuons(p);
    const Gaudi::SymMatrix3x3 & err = p->endVertex()->covMatrix();
    double errr = sqrt( err(0,0) + err(1,1) );

    //Let's go for Prey hunting
    if( msgLevel( MSG::DEBUG ) ){
      debug()<< m_Prey <<" candidate with mass "<< mass/Gaudi::Units::GeV 
             <<" GeV, nb of tracks " << nbtrks << ", Chi2/ndof " 
             << chi <<", R "<< rho <<", pos of end vtx "<< pos <<", sigmaX "
             << sqrt(err(0,0))<<", sigmaY "<< sqrt(err(1,1)) <<", sigmaZ "
             << sqrt(err(2,2)) <<", sigmaR "<< errr ;
      if(muon){
        debug()<<", has muon with pt "<< muon <<" GeV" << endmsg;
      } else { debug()<< endmsg; }
    }
    //Is the particle close to the detector material ?
    if( IsAPointInDet( p, m_RemVtxFromDet, m_DetDist ) ) continue;

    //Is the particle decay vertex in the RF-foil ?
    if( m_RemFromRFFoil && IsInRFFoil( pos ) ){
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Decay vertex in the RF-foil, particle disguarded"<< endmsg; 
      continue; 
    }

    if( mass < m_PreyMinMass || mass > m_PreyMaxMass || 
        nbtrks < m_nTracks || rho <  m_RMin || rho > m_RMax || 
        sumpt < m_SumPt || chi > m_MaxChi2OvNDoF || muon < m_MuonpT || 
        pos.x() < m_MinX || pos.x() > m_MaxX || pos.y() < m_MinY || 
        pos.y() > m_MaxY || pos.z() < m_MinZ || pos.z() > m_MaxZ ||
        errr > m_SigmaR || sqrt(err(2,2)) > m_SigmaZ ){  
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the cuts"<< endmsg; 
      continue; 
    }

    //Save infos in tuple !
    if( m_SaveTuple ){
      nboftracks.push_back( nbtrks ); chindof.push_back( chi );
      e.push_back(mom.e());
      px.push_back(mom.x()); py.push_back(mom.y()); pz.push_back(mom.z());
      x.push_back(pos.x()); y.push_back(pos.y()); z.push_back(zpos);
      errx.push_back(sqrt(err(0,0))); erry.push_back(sqrt(err(1,1)));
      errz.push_back(sqrt(err(2,2)));
      sumpts.push_back(sumpt); muons.push_back(muon);
      double indet = 0;
      if( IsAPointInDet( p, 2 ) ) indet += 1;
      if( IsAPointInDet( p, 3, 2 ) ) indet += 10;
      if( IsAPointInDet( p, 4, 2 ) ) indet += 100;
      indets.push_back( indet ); 
    }

    //The only way to have the candidates saved in the Stripping is to have 
    //  the latest algo in the sequence put them in the TES.
    //As they are already saved by a preselection algorithm, 
    //  they need to be cloned to be saved again on the TES.

    //Particle * clone = new Particle( *p );
    Particle clone = Particle( *p );
    //clone->setParticleID( m_PreyID );
    clone.setParticleID( m_PreyID );
    this->markTree( &clone );
    nbCands++;
    //Cands.push_back( desktop()->keep( clone ) );
    //Cands.push_back( desktop()->keep( &clone ) );

  }//  <--- end of Prey loop
  if( (unsigned int)nbCands < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of candidates !"<< endmsg;
    return StatusCode::SUCCESS;
  }
  setFilterPassed(true); 
  ++m_nbpassed;

  if( msgLevel( MSG::DEBUG ) )
    debug() << "Nb of " << m_Prey <<" candidates "<< nbCands << endmsg;

  //Save nTuples
  if( m_SaveTuple ){
    Tuple tuple = nTuple("DisplVertices");
    const int NbPreyMax = 20;
    if( !SaveCaloInfos(tuple)  ) return StatusCode::FAILURE;
    if( !fillHeader(tuple) ) return StatusCode::FAILURE;
    //if( !SaveGEC( tuple, Cands ) ) return StatusCode::FAILURE;
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
    tuple->farray( "InDet", indets.begin(),indets.end(), "NbPrey", NbPreyMax );
    tuple->farray( "Muon", muons.begin(), muons.end(), "NbPrey", NbPreyMax );
    tuple->farray( "PreyNbofTracks", nboftracks.begin(), nboftracks.end(),
		   "NbPrey", NbPreyMax );
    tuple->farray( "PreyChindof", chindof.begin(), chindof.end(),
		   "NbPrey", NbPreyMax );
    tuple->column( "BLX", m_BeamLine->referencePoint().x() );
    tuple->column( "BLY", m_BeamLine->referencePoint().y() );
    tuple->column( "BLZ", m_BeamLine->referencePoint().z() );
    //Save number of Velo tracks...
    const Track::Range & VeloTrks = get<Track::Range>( "Hlt/Track/Velo" );
    tuple->column( "NbVelo", VeloTrks.size() );
    if( !(tuple->write()) ) return StatusCode::FAILURE;
  }

  //Save Preys from Desktop to the TES.
  //if( m_SaveonTES ) desktop()->saveDesktop();

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
//
// mode = 0  : disabled
// mode = 1  : remove reco vtx if in detector material
// mode = 2  : remove reco vtx if rad length along momentum 
//                           from (decay pos - range) to 
//                           (decay pos + range)  is > threshold
// mode = 3 : remove reco vtx if rad length along 
//                             +- range * PositionCovMatrix
// mode = 4 : 3 but range+3 if in RF foil.
//
//=============================================================================

bool Hlt2SelDV::IsAPointInDet( const Particle* P, int mode, double range ){

  if( mode < 1 ) return false;

  const Vertex* RV = P->endVertex();
  double threshold = 1e-10;

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
    const Gaudi::XYZPoint nvec =  Normed( P->momentum(), range );
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
        debug()<<"RV is closed to a detector material !"
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
  } // end of 3 cond
  
  return false;
}

//=============================================================================
// Is the point in the RF-Foil ?
//=============================================================================
bool Hlt2SelDV::IsInRFFoil( const Gaudi::XYZPoint & pos){
  
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

//============================================================================
// if particule has a daughter muon, return highest pt
//============================================================================
double Hlt2SelDV::HasMuons( const Particle * p ){

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

//============================================================================
//  Event number 
//============================================================================
StatusCode Hlt2SelDV::fillHeader( Tuple& tuple ){
  const LHCb::RecHeader* header = 
    get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);  
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
// Save in Tuple some Global Event Cut
//============================================================================
StatusCode  Hlt2SelDV::SaveGEC( Tuple & tuple,  
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

//============================================================================
//  Save Total Transverse Energy in Calorimeters
//============================================================================
StatusCode Hlt2SelDV::SaveCaloInfos( Tuple& tuple ){

  double E = 0; double Et = 0.;
  StatusCode sc = GetCaloInfos( "Ecal", E, Et ) && 
    GetCaloInfos( "Hcal", E, Et ) &&
    //GetCaloInfos( "Prs", E, Et ) && GetCaloInfos( "Spd", E, Et ) &&
    GetCaloInfos( "Muon", E, Et );
  tuple->column( "TotEt", Et );
  return sc;  
}

StatusCode Hlt2SelDV::GetCaloInfos( string CaloType, double& En, double& Et ){

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
  return StatusCode::SUCCESS ;
}

//============================================================================
// Basic operations between two Gaudi::XYZPoint
//============================================================================
Gaudi::XYZPoint Hlt2SelDV::Normed( const Gaudi::LorentzVector & P,
                                       double range ){
  double norm = range/Norm( P );
  return Gaudi::XYZPoint( P.x()*norm, P.y()*norm, P.z()*norm );
}


double Hlt2SelDV::Norm( const Gaudi::LorentzVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + 
		      pow(P.z(),2) );
}

double Hlt2SelDV::Norm( const Gaudi::XYZPoint & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + 
		      pow(P.z(),2) );
}

double Hlt2SelDV::Norm( const Gaudi::XYZVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + 
		      pow(P.z(),2) );
}

double Hlt2SelDV::Mult( const Gaudi::XYZPoint & p1, 
			 const Gaudi::XYZPoint & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

double Hlt2SelDV::Mult(  const Gaudi::LorentzVector & p1, 
			  const Gaudi::LorentzVector & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

Gaudi::XYZPoint Hlt2SelDV::Plus( const Gaudi::XYZPoint & p1, 
				  const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint Hlt2SelDV::Minus( const Gaudi::XYZPoint& p1, 
				   const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}

//============================================================================
// Compute distance between two vertices
//============================================================================ 
double Hlt2SelDV::VertDistance( const Gaudi::XYZPoint & v1, 
				 const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}
//=============================================================================
