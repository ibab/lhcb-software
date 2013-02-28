// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

//Use ODIN
#include "Event/ODIN.h"

//Trigger decisions
//#include "Event/L0DUReport.h"
//#include "Event/HltDecReports.h"

//get the Header of the event
#include "Event/RecHeader.h"

//calorimeter
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"

//to retrieve routing bits
#include "Kernel/ReadRoutingBits.h"

#include <functional>
#include <vector>

// local
#include "DisplVertices.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : DisplVertices
//
// 2008-april-2 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DisplVertices )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  DisplVertices::DisplVertices( const std::string& name,
                                ISvcLocator* pSvcLocator)
    : DaVinciTupleAlgorithm ( name , pSvcLocator )
    , pi(3.1415926)
    , PV(0)
    , m_PreyID(0)
    , m_MotherPreyID(0){
    declareProperty("SaveTuple", m_SaveTuple = false );//save prey infos in Tuple
    //declareProperty("SaveTrigInfos", m_SaveTrigInfos = false );
    declareProperty("Prey", m_Prey = "~chi_10" );
    declareProperty("MotherPrey", m_MotherPrey = "H_10" );
    //>6.286GeV=Bc+ Mass
    declareProperty("MinNBCands", m_NbCands = 1 );
    declareProperty("PreyMinMass", m_PreyMinMass = 6.3*GeV );
    declareProperty("PreyMinMass2", m_PreyMinMass2 = 6.3*GeV );
    //Unlimited
    declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*TeV );
    declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
    declareProperty("PreyMinSumpt", m_PreyMaxSumPt = 14.*TeV );
    declareProperty("RMin", m_RMin = 0.3*mm );//0.06 in K
    declareProperty("RMax", m_RMax = 10.*m );
    declareProperty("DistMax", m_DistMax = 10.* m );//Check value.
    declareProperty("MaxChi2OvNDoF", m_MaxChi2OvNDoF = 1000. );
    declareProperty("MuonpT", m_MuonpT = -1*GeV );
    declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks 5
    declareProperty("SigmaZ", m_SigmaZ = 1000. );
    declareProperty("SigmaR", m_SigmaR = 1000. );
    declareProperty("RCutMethod", m_RCut = "FromPreyInfo" );
    declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
    declareProperty("DetDist", m_DetDist = 1*mm );
    declareProperty("RemFromRFFoil", m_RemFromRFFoil = false );
    declareProperty("MinX", m_MinX = -10.*m );
    declareProperty("MaxX", m_MaxX = 10.*m );
    declareProperty("MinY", m_MinY = -10.*m );
    declareProperty("MaxY", m_MaxY = 10.*m );
    declareProperty("MinZ", m_MinZ = -10.*m );
    declareProperty("MaxZ", m_MaxZ = 100*m );
    declareProperty("PVnbtrks", m_PVnbtrks = 5 ); //corr. to 'tight' PV reco
    declareProperty("BeamLineLocation",
                    m_BLLoc = "HLT/Hlt2LineDisplVertices/BeamLine");
    declareProperty("Backtoback", m_Backtoback = -1 );
  }

//=============================================================================
// Destructor
//=============================================================================
DisplVertices::~DisplVertices() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DisplVertices::initialize() {
  StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize the DisplVertices algorithm" << endmsg;

  /*if( m_SaveTrigInfos ){
    m_tisTos = tool<ITriggerTisTos>("TriggerTisTos",this);
    m_l0BankDecoder = tool<IL0DUFromRawTool>("L0DUFromRawTool");
    }*/

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

  const ParticleProperty* MotherPrey = ppSvc()->find( m_MotherPrey );
  if ( !MotherPrey ) { //
    err() << "Cannot find particle property for " << m_MotherPrey << endmsg ;
    return StatusCode::FAILURE;
  }
  m_MotherPreyID = MotherPrey->particleID();

  //Sanity checks
  if( m_RemFromRFFoil && m_RemVtxFromDet == 4 ){
    info()<<"RemFromRFFoil = "<< m_RemFromRFFoil <<" and RemVtxFromDet = "
          << m_RemVtxFromDet <<" are incompatible. RemFromRFFoil set to false"
          << endmsg;
    m_RemFromRFFoil = false;
  }

  if( context() == "HLT" ){
    m_SaveTuple = false;
  }

  if( m_Backtoback > 0 && m_NbCands < 2 ) m_NbCands = 2;


  if( context() == "Info" ){
    info()<<"--------------------------------------------------------"<<endmsg;
    info() << "DisplVertices will select " << m_Prey
           << " candidates (ID=" << m_PreyID.pid() <<") ";
    info() << ". Selection Cuts are : " << endmsg;
    info() << "Min nb of desired candidates : "<< m_NbCands<< endmsg;
    info() << m_Prey <<" minimum mass "
           << m_PreyMinMass/Gaudi::Units::GeV <<" GeV" << endmsg;
    info() << m_Prey <<" maximum mass "
           << m_PreyMaxMass/Gaudi::Units::GeV <<" GeV" << endmsg;
    info() << "Minimum number of tracks at the reconstructed vertex of "
           << m_nTracks <<" tracks."<< endmsg ;
    info() << "Max distance of the vertices "<< m_DistMax <<" mm" << endmsg;
    info() << "Max chi2/ndof of a vertex "<< m_MaxChi2OvNDoF << endmsg;
    info() << "Min sum of daughters's pT "<< m_SumPt/GeV <<" GeV"<< endmsg;
    info() << m_MinX/mm <<" mm < pos X < "<< m_MaxX/mm <<" mm"<< endmsg;
    info() << m_MinY/mm <<" mm < pos Y < "<< m_MaxY/mm <<" mm"<< endmsg;
    info() << m_MinZ/mm <<" mm < pos Z < "<< m_MaxZ/mm <<" mm"<< endmsg;
    info() << "Max sigma R : "<< m_SigmaR <<" mm"<< endmsg;
    info() << "Max sigma Z : "<< m_SigmaZ <<" mm"<< endmsg;
    if( m_MuonpT )
      info()<<"At least one muon with pT > "<< m_MuonpT << endmsg;
    if( m_RemFromRFFoil )
      info()<<"Not in RF-Foil region"<< endmsg ;
    if( m_RemVtxFromDet == 1 )
      info()<<"Not in detector material"<< endmsg;
    if( m_RemVtxFromDet == 2 )
      info()<<"Not closer than " << m_DetDist
            <<"mm from detector material along momentum"<< endmsg;
    if( m_RemVtxFromDet == 3 || m_RemVtxFromDet == 4 )
      info()<<"Not closer than " << m_DetDist
            <<"*PosCovMatric from detector material"<< endmsg;
    if( m_RemVtxFromDet == 4 )
      info()<<"("<< m_DetDist+3 <<" when in RF-Foil region)"<< endmsg;
    info()<< "The radial displacement is ";
    if( m_RCut == "FromUpstreamPV" ){
      info() << "computed with respect to the upstream PV of PV3D." << endmsg;
      info()<< "Min nb of tracks on the upPV candidate : "
            << m_PVnbtrks << endmsg;
    } else if( m_RCut == "FromBeamLine" ){
      info() << "computed with respect to the beam line given at "
             << m_BLLoc << endmsg;
    }else if( m_RCut == "FromPreyInfo" ){
      info() << "obtained from previous calculation"  << endmsg;
    } else {
      info() << "computed with respect to (0,0,z) in the global LHCb frame"
             << endmsg;
      info()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!"
            << endmsg;
    }
    info() <<"Min R    : " << m_RMin/mm <<" mm"<< endmsg ;
    info() <<"Max R    : " << m_RMax/mm <<" mm"<< endmsg ;
    if( m_Backtoback > 0 ){
      info() << "MCDisplVertices will also try to reconstruct "<< m_MotherPrey
             <<" from two "<< m_Prey <<" decaying back to back with |dphi| "
             << m_Backtoback <<" angle wrst the upstream PVs"<< endmsg;
    }
    info()<<"--------------------------------------------------------"<<endmsg;
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
              << rightcenter <<", left half center "<< leftcenter << endmsg;
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
StatusCode DisplVertices::execute(){

  ++counter("Processed evt nb");
  if( msgLevel( MSG::DEBUG ) )
    debug() << "==> Execute the DisplVertices algorithm, event "
            << counter("Processed evt nb").flag() << endmsg;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.

  //Clear stuff
  PVs.clear();
  //always()<<"Entering"<<endreq;
  //---------------------------------------------
  /*if( m_SaveTrigInfos && !m_SaveTuple ){
    Tuple tuple = nTuple("Trigger");
    if( fillHeader( tuple ).isFailure() ||
    SaveTrigInfinTuple( tuple ).isFailure() )
    Warning("Not being able to save trigger infos in tuple !");
    if( !(tuple->write()) ) return StatusCode::FAILURE;
    }*/
  double minMass2 = 0.;

  //always()<<"tag1"<<endreq;
  if( m_SaveTuple || m_RCut != "FromPreyInfo"){

    //------------------Set the beam line------------------
    if( m_RCut=="FromBeamLine" || (m_SaveTuple&&m_RCut=="FromPreyInfo") ){
      if( exist<Particle::Range>( m_BLLoc ) ){
        const Particle::Range BL = get<Particle::Range>( m_BLLoc );
        const Particle* tmp = *(BL.begin());
        m_BeamLine->setReferencePoint( tmp->referencePoint() );
        m_BeamLine->setMomentum( tmp->momentum() );
        if( msgLevel(MSG::DEBUG) )
          debug()<<"Beam line position "<< m_BeamLine->referencePoint()
                 <<" direction " << m_BeamLine->momentum() << endmsg;
      } else {
        debug()<<"No Beam line found at "<< m_BLLoc << endmsg;
        return StatusCode::SUCCESS;
      }
    } else if( m_RCut=="FromUpstreamPV" ){
      //------------------Get the upstream Primary Vertex-----
      GetUpstreamPV();
      if(PV == NULL) return StatusCode::SUCCESS;
      m_BeamLine->setReferencePoint( PV->position() );
      m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
      if( msgLevel( MSG::DEBUG ) )
        debug() << "Upstream PV Position : " << PV->position() << endmsg ;
      //StudyDgtsIP( PV ); //Study the IP to PV of the daughters
      //Study resolution of the reconstructed PV
      //if(false) Resolution();
    }
  }

  //------------------The Code---------------------------
  const Particle::ConstVector preys = this->i_particles();
  if( msgLevel( MSG::DEBUG ) )
    debug() << "There are " << preys.size() <<" particles in TES !" << endmsg;
  if( preys.size() < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of particles in TES !" << endmsg;
    return StatusCode::SUCCESS;
  }
  bool oneisOut = false;
  vector<int>  nboftracks, nboftracksl;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts, muons, indets, recqs, massls,ma,rhos;
  if( msgLevel( MSG::DEBUG ) )
    debug()<<"--------Reconstructed Displ. Vertices --------------"<< endmsg;
  Particle::ConstVector Cands;
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
    if( mass > minMass2 ) minMass2 = mass ;
    int nbtrks =  p->info(53,-1000.);//p->endVertex()->outgoingParticles().size();
    double chi = p->endVertex()->chi2PerDoF();
    const Gaudi::XYZPoint & pos = p->endVertex()->position();
    double rho = p->info(52,-1000.);//(m_RCut=="FromPreyInfo")?p->info(52,-1000.):GetRFromBL(pos);
    //always()<<"R "<<rho<<" "<<endreq;

    double zpos = pos.z();
    Gaudi::LorentzVector mom = p->momentum();
    double muon = HasMuons(p);
    const Gaudi::SymMatrix3x3 & err = p->endVertex()->covMatrix();
    double errr = sqrt( err(0,0) + err(1,1) );
    double sumpt = 0.;
    double recq = 0.;
    GetQPt( p->endVertex(), sumpt, recq );

    //Let's go for Prey hunting
    if( msgLevel( MSG::DEBUG ) ){
      debug()<< m_Prey <<" candidate with mass "<< mass/Gaudi::Units::GeV
             <<" GeV, nb of tracks " << nbtrks << ", Chi2/ndof "
             << chi <<", R "<< rho <<", pos of end vtx "<< pos <<", sigmaX "
             << sqrt(err(0,0))<<", sigmaY "<< sqrt(err(1,1)) <<", sigmaZ "
             << sqrt(err(2,2)) <<", sigmaR "<< errr <<", rec charge "<< recq ;
      if(muon){
        debug()<<", has muon with pt "<< muon <<" GeV" << endmsg;
      } else { debug()<< endmsg; }
    }

    //Is the particle close to the detector material ?
    if( m_RemVtxFromDet!= 5 && IsAPointInDet( p, m_RemVtxFromDet, m_DetDist ) ) continue;
    if( m_RemVtxFromDet== 5 && p->info(51,-1000.)>-900) continue;
    if( m_RemVtxFromDet== -5 && p->info(51,-1000.)<-900) oneisOut = true;


    //Is the particle decay vertex in the RF-foil ?
    if( m_RemFromRFFoil && IsInRFFoil( pos ) ){
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Decay vertex in the RF-foil, particle disguarded"<< endmsg;
      continue;
    }

    if( mass < m_PreyMinMass || ( mass > m_PreyMaxMass ) ||
        nbtrks < m_nTracks || rho <  m_RMin || rho > m_RMax ||
        sumpt < m_SumPt || chi > m_MaxChi2OvNDoF || muon < m_MuonpT ||
        pos.x() < m_MinX || pos.x() > m_MaxX || pos.y() < m_MinY ||
        pos.y() > m_MaxY ||( pos.z() < m_MinZ )|| pos.z() > m_MaxZ ||
        errr > m_SigmaR || sqrt(err(2,2)) > m_SigmaZ ){
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the cuts"<< endmsg;
      continue;
    }

    //Save infos in tuple !
    if( m_SaveTuple ){
      nboftracks.push_back( nbtrks ); chindof.push_back( chi );
      e.push_back(mom.e());
      ma.push_back(mass);
      rhos.push_back(rho);
      px.push_back(mom.x()); py.push_back(mom.y()); pz.push_back(mom.z());
      x.push_back(pos.x()); y.push_back(pos.y()); z.push_back(zpos);
      errx.push_back(sqrt(err(0,0))); erry.push_back(sqrt(err(1,1)));
      errz.push_back(sqrt(err(2,2)));
      sumpts.push_back(sumpt); muons.push_back(muon);
      recqs.push_back(recq);
      double indet = 0;
      if( IsAPointInDet( p, 2 ) ) indet += 1;
      if( IsAPointInDet( p, 3, 2 ) ) indet += 10;
      if( IsAPointInDet( p, 4, 2 ) ) indet += 100;
      if( p->info(51,-1000.)>-900 ) indet += 1000;
      indets.push_back( indet );
      if(true){
        double mlong = 0.; int nbtlong = 0;
        GetMassFromLongTracks( p, mlong, nbtlong );
        nboftracksl.push_back( nbtlong );
        massls.push_back( mlong );
      }
    }
    Particle clone = Particle( *p );
    clone.setParticleID( m_PreyID );
    Cands.push_back( this->markTree(&clone) );
  }//  <--- end of Prey loop

  if( Cands.size() < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of candidates !"<< endmsg;
    return StatusCode::SUCCESS;
  }
  if( m_RemVtxFromDet== -5 && oneisOut == false ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Not even one outside detector !"<< endmsg;
    return StatusCode::SUCCESS;
  }
  if( m_RemVtxFromDet== -5 && minMass2 < m_PreyMinMass2 ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Highest mass too low !"<< endmsg;
    return StatusCode::SUCCESS;
  }

  //always()<<"There should be candidate"<<endreq;
  setFilterPassed(true);
  counter("Nb of candidates") += Cands.size();

  if( msgLevel( MSG::DEBUG ) )
    debug() << "Nb of " << m_Prey <<" candidates "<< Cands.size() << endmsg;


  //Save nTuples
  if( m_SaveTuple ){
    Tuple tuple = nTuple("DisplVertices");
    const int NbPreyMax = 20;
    if( !SaveCaloInfos(tuple)  ) return StatusCode::FAILURE;
    if( !fillHeader(tuple) ) return StatusCode::FAILURE;
    tuple->farray( "PreyM", ma.begin(), ma.end(), "NbPrey", NbPreyMax );
    tuple->farray( "PreyR", rhos.begin(), rhos.end(), "NbPrey", NbPreyMax );
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
    tuple->farray( "PreyQ", recqs.begin(), recqs.end(), "NbPrey", NbPreyMax );
    tuple->farray( "InDet", indets.begin(),indets.end(), "NbPrey", NbPreyMax );
    tuple->farray( "Muon", muons.begin(), muons.end(), "NbPrey", NbPreyMax );
    tuple->farray( "PreyNbofTracks", nboftracks.begin(), nboftracks.end(),
                   "NbPrey", NbPreyMax );
    tuple->farray( "PreyChindof", chindof.begin(), chindof.end(),
                   "NbPrey", NbPreyMax );
    if( massls.size() != 0 ){
      tuple->farray( "PreyMLongs", massls.begin(), massls.end(),
                     "NbPrey", NbPreyMax );
      tuple->farray( "PreyNbofLongTracks", nboftracksl.begin(),
                     nboftracksl.end(), "NbPrey", NbPreyMax );
    }
    tuple->column( "BLX", m_BeamLine->referencePoint().x() );
    tuple->column( "BLY", m_BeamLine->referencePoint().y() );
    tuple->column( "BLZ", m_BeamLine->referencePoint().z() );
    if( !SavePVs( tuple )  ) return StatusCode::FAILURE;
    tuple->column( "NbVelo", GetNbVeloTracks() );
    //if( m_SaveTrigInfos && !SaveTrigInfinTuple( tuple ) )
    // return StatusCode::FAILURE;
    if( !(tuple->write()) ) return StatusCode::FAILURE;
  }

  //Save Preys from Desktop to the TES.
  //if( m_SaveonTES ) desktop()->saveDesktop();


  //--------------Mother Reconstruction------------------
  if( m_Backtoback > 0 && Cands.size() >= 2 ){
    if( BackToBack( Cands ).isFailure() )
      Warning("Reconstruction process for mother"+ m_MotherPrey +" failed !");
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DisplVertices::finalize() {
  debug() << "==> Finalize" << endmsg;

  if( m_RCut !="FromBeamLine" ) delete m_BeamLine;

  if ( context() != "HLT" )
  {
    info()<<"-------------------------------------------------------"<< endreq;
    info()<<"              DisplVertices Statistics                 "<< endreq;
    info()<<"-------------------------------------------------------"<< endreq;
    info()<<"Number of reconstructed "<< m_Prey <<"               : "
          << counter("Nb of candidates").flag() << endreq;
    info()<<"Number of reconstructed "<<  m_MotherPrey <<"                 : "
          << counter("Nb of mothers").flag() << endreq;
    info()<<"-------------------------------------------------------"<< endreq;
  }

  return DaVinciTupleAlgorithm::finalize();
}

//============================================================================
// Get Nb of Velo tracks inevent
//============================================================================
unsigned int DisplVertices::GetNbVeloTracks(){

  unsigned int nbv = 0;
  const Track::Range Trks = get<Track::Range>( TrackLocation::Default );
  for(Track::Range::const_iterator itr = Trks.begin();
      Trks.end() != itr; ++itr) {
    if( (*itr)->hasVelo() ) ++nbv;
  }
  return nbv;
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
bool DisplVertices::IsAPointInDet( const Gaudi::XYZPoint & pos ){

  IGeometryInfo* start = 0;
  ILVolume::PVolumePath path ;

  //Get the physical volume, if any.
  StatusCode sc = m_lhcbGeo->fullGeoInfoForPoint
    ( pos, 1000, start, path ) ;
  if ( sc.isFailure() ) {
    warning()<<"Impossible to get any physical volume related to point "
             << pos <<endmsg;
    return false;
  }
  int size = path.size();
  if( context() == "Info" ) plot( size, "NbofDetV", 0, 5 );
  if( msgLevel(MSG::DEBUG) )
    debug()<<"Found "<< size <<" physical volumes related to point "
           << pos <<endmsg;

  const IPVolume* pvlast = 0;
  if ( !path.empty() ) { pvlast = path.back(); }

  const ILVolume * lvlast = 0;
  if ( 0 != pvlast ) { lvlast = pvlast->lvolume(); }

  const Material* matlast = 0 ;
  if ( 0 != lvlast ) { matlast = lvlast->material(); }

  if ( 0 != matlast )  {
    if( msgLevel(MSG::DEBUG) ){
      debug()<<"Physical volume related to point "<< pos <<endmsg;
      debug()<< matlast << endl;
    }
    return true;
  }
  if( msgLevel(MSG::DEBUG) )
    debug()<<"There is no physical volume related to point "<< pos <<endmsg;
  return false;

}


bool DisplVertices::IsAPointInDet( const Particle* P, int mode, double range ){

  if( mode < 1 ) return false;

  const Vertex* RV = P->endVertex();
  double threshold = 1e-10;

  if( mode == 1 ){
    return IsAPointInDet( RV->position() );
  } //end of 1 condition
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

    if( context() == "Info" ) plot( radlength, "RVRadLength", 0, 0.01);
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
bool DisplVertices::IsInRFFoil( const Gaudi::XYZPoint & pos){

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
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool DisplVertices::HasBackwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = RV->tracks().end();
  for( ; i != iend; ++i ){
    if ( (*i)->checkFlag( Track::Backward ) ) return true;
  }
  return false;
}

bool DisplVertices::HasBackwardTracks( const Particle * p ){

  Particle::ConstVector dgts = p->daughtersVector();
  Particle::ConstVector::const_iterator iend = dgts.end();
  for( Particle::ConstVector::const_iterator i = dgts.begin(); i != iend; ++i ){
    const Track * trk = (*i)->proto()->track();
    if ( trk->checkFlag( Track::Backward ) ) return true;
  }
  return false;
}

//=============================================================================
//  Loop on the daughter track to see if there is at least one backward track
//  and one forward tracks
//=============================================================================
bool DisplVertices::HasBackAndForwardTracks( const RecVertex* RV ){
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
double DisplVertices::HasMuons( const Particle * p ){

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

//============================================================================
// Basic operations between two Gaudi::XYZPoint
//============================================================================
Gaudi::XYZPoint DisplVertices::Normed( const Gaudi::LorentzVector & P,
                                       double range ){
  double norm = range/Norm( P );
  return Gaudi::XYZPoint( P.x()*norm, P.y()*norm, P.z()*norm );
}


double DisplVertices::Norm( const Gaudi::LorentzVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) +
               pow(P.z(),2) );
}

double DisplVertices::Norm( const Gaudi::XYZPoint & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) +
               pow(P.z(),2) );
}

double DisplVertices::Norm( const Gaudi::XYZVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) +
               pow(P.z(),2) );
}

double DisplVertices::Mult( const Gaudi::XYZPoint & p1,
                            const Gaudi::XYZPoint & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

double DisplVertices::Mult(  const Gaudi::LorentzVector & p1,
                             const Gaudi::LorentzVector & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

Gaudi::XYZPoint DisplVertices::Plus( const Gaudi::XYZPoint & p1,
                                     const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint DisplVertices::Minus( const Gaudi::XYZPoint& p1,
                                      const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}

//============================================================================
// Compute distance between two vertices
//============================================================================
double DisplVertices::VertDistance( const Gaudi::XYZPoint & v1,
                                    const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}

//============================================================================
// Angle between two tracks   :   tracks are supposed to be straight lines
//============================================================================
double DisplVertices::Angle( const Gaudi::LorentzVector & a,
                             const Gaudi::LorentzVector & b ){
  return Angle( a.Vect(), b.Vect() );
}

double DisplVertices::Angle( const Gaudi::XYZVector & a,
                             const Gaudi::XYZVector & b ){
  return acos( a.Dot(b) / sqrt( a.Mag2() * b.Mag2() ) );
}

//============================================================================
// Compute the impact parameter of a Particle to a Vertex
//============================================================================
double DisplVertices::ImpactParam( const Gaudi::XYZPoint & V,
                                   const Gaudi::LorentzVector & P,
                                   const Gaudi::XYZPoint & V0 ){
  return ImpactParam( V, Gaudi::XYZVector( P.x(),P.y(),P.z() ), V0 );
}

double DisplVertices::ImpactParam( const Gaudi::XYZPoint & V,
                                   const Gaudi::XYZVector & P,
                                   const Gaudi::XYZPoint & V0 ){

  //A line is v + lambda*Pn, with v a point on it.
  //double norm = Norm( static_cast<const Gaudi::XYZVector>(P) );
  double norm = Norm( P );
  const Gaudi::XYZPoint Pn = Gaudi::XYZPoint( P.x()/norm, P.y()/norm,
                                              P.z()/norm );
  double lambda = -( Mult(Pn,V) - Mult(Pn,V0) );

  Gaudi::XYZPoint dist = Minus( Plus(lambda*Pn, V), V0);
  //debug() <<"Impact parameter " << dist.R() << endmsg;

  return dist.R();

}

//============================================================================
// Compute mass and nb of tracks from Long tracks only
//============================================================================
void DisplVertices::GetMassFromLongTracks( const Particle * p,
                                           double & mlong, int & nbtlong ){

  Gaudi::LorentzVector mom;

  //Loop on the daughters.
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin(); i != iend; ++i ){
    const Particle * d = (*i);
    if( d->proto() == NULL ) continue;
    if( d->proto()->track() == NULL ) continue;
    const Track * tk = d->proto()->track();
    if( !tk->checkType(Track::Long) ) continue;
    ++nbtlong;
    mom += d->momentum();
  }
  mlong = mom.M();
}

//=============================================================================
// Return a mass estimate from a vector of Particles
//=============================================================================
double DisplVertices::GetRecMass( const Particle::ConstVector & RecParts ){

  Gaudi::LorentzVector mom;

  Particle::ConstVector::const_iterator iend = RecParts.end();
  for( Particle::ConstVector::const_iterator i = RecParts.begin();
       i != iend; ++i ){
    mom += (*i)->momentum();
  }
  return mom.M();
}

double DisplVertices::GetRecMass( const Particle::ConstVector & RecParts,
                                  double & recpt ){

  Gaudi::LorentzVector mom;
  Particle::ConstVector::const_iterator iend = RecParts.end();
  for( Particle::ConstVector::const_iterator i = RecParts.begin();
       i != iend; ++i ){
    mom += (*i)->momentum();
  }
  recpt = mom.Pt();
  return mom.M();
}

//=============================================================================
// Compute azimuthal angle between the 2 candidates
//=============================================================================
double DisplVertices::GetDeltaPhi( const Gaudi::XYZPoint & v1,
                                   const Gaudi::XYZPoint & v2 ){

  double dphi = v1.Phi() - v2.Phi();
  if( dphi >= pi ) dphi -= 2*pi;
  if( dphi < -pi ) dphi += 2*pi;
  return dphi;
}

//=============================================================================
// Compute charge of a vertex, possibly weighted by the momentum, and sum pt.
//=============================================================================
void DisplVertices::GetQPt( const Vertex * v, double & sumpt, double & sumq,
                            bool weight ){

  double sumqpt = 0.;
  SmartRefVector<Particle>::const_iterator ip = v->outgoingParticles().begin();
  for( ; ip < v->outgoingParticles().end(); ++ip ){
    const Particle * d = ip->target() ;
    if( d->proto() == NULL ) continue;
    if( d->proto()->track() == NULL ) continue;
    const Track * t = d->proto()->track();
    sumq += t->charge();
    sumpt += t->pt();
    sumqpt += t->pt()*t->charge();
  }

  if( weight ){
    if( sumpt == 0. ) ++sumpt;
    sumq = sumqpt/sumpt;
  }
}


//=============================================================================
// Compute charge of a vertex, possibly weighted by the momentum
//=============================================================================
double DisplVertices::GetCharge( const Vertex * v, bool weight ){

  double sumqpt = 0.;
  double sumq = 0.;
  double sumpt = 0.;
  SmartRefVector<Particle>::const_iterator ip = v->outgoingParticles().begin();
  for( ; ip < v->outgoingParticles().end(); ++ip ){
    const Particle * d = ip->target() ;
    if( d->proto() == NULL ) continue;
    if( d->proto()->track() == NULL ) continue;
    const Track * t = d->proto()->track();
    sumq += t->charge();
    sumpt += t->pt();
    sumqpt += t->pt()*t->charge();
  }
  if( weight ){
    return (sumpt!=0. ? sumqpt/sumpt:0. );
  } else {
    return sumq;
  }
}

//=============================================================================
// Compute the sum pT of a bunch of track (daughters of a Particle)
//=============================================================================
double DisplVertices::GetSumPt( const Particle * p ){

  double sumpt = 0;
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin(); i != iend; ++i ){
    sumpt += i->target()->pt();
  }
  return sumpt;
}

//=============================================================================
// Compute pT from a bunch of track (daughters of a RecVertex)
//=============================================================================
double DisplVertices::GetPt( const RecVertex* RV ){

  Gaudi::XYZVector mom;
  SmartRefVector<Track>::const_iterator iend = RV->tracks().end();
  for( SmartRefVector<Track>::const_iterator i =
         RV->tracks().begin(); i != iend; ++i ){
    mom += i->target()->momentum();
  }
  return mom.rho();
}

//=============================================================================
// Compute pT of a daughter particle relativ to its mother particle
//=============================================================================
double DisplVertices::GetPt( const Gaudi::LorentzVector & d,
                             const Gaudi::LorentzVector & m ){
  double norm = Norm( m );
  const Gaudi::LorentzVector mn = m/norm;
  norm = Norm( d );
  const Gaudi::LorentzVector dn = d/norm;

  return norm*sin( acos( Mult(dn,mn) ) );

}

//============================================================================
// get the delta R between two particles
//============================================================================
double DisplVertices::GetDeltaR( const LHCb::Particle * p1,
                                 const LHCb::Particle * p2 ){

  double Deta = fabs( p1->momentum().eta() - p2->momentum().eta() );
  double Dphi = fabs( p1->momentum().phi() - p2->momentum().phi() );
  if (Dphi>pi) { Dphi = 2*pi - Dphi; };

  //Compute delta R = sqrt ( deltaeta**2  + deltaphi**2  )
  return  sqrt( Deta*Deta + Dphi*Dphi );
}

double DisplVertices::GetR( const Gaudi::XYZPoint & p){

  return sqrt( p.x()*p.x() + p.y()*p.y() );

}

//=============================================================================
// Get radial distance to Beam Line
//=============================================================================
double DisplVertices::GetRFromBL( const Gaudi::XYZPoint& p ){

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
// Get candidate PVs
//============================================================================
void DisplVertices::GetPVs(){

  //The PVs container is emptied at the begining of the execution
  if( !PVs.empty() ) return;
  const RecVertex::Range & PVCs = this->primaryVertices();
  if( PVCs.empty() ) return;

  for ( RecVertex::Range::const_iterator i = PVCs.begin();
        i != PVCs.end() ; ++i ){
    const RecVertex* pv = *i;
    //Apply some cuts
    if(m_RCut=="FromBeamLine" || (m_RCut=="FromPreyInfo" && m_SaveTuple)){
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
  sort( PVs.begin(), PVs.end(), SortPVz );
}

//============================================================================
// Get the upstream Primary vertex
//============================================================================
void DisplVertices::GetUpstreamPV(){
  GetPVs();
  if( PVs.empty() ){ PV = NULL; } else { PV = *(PVs.begin()); }
}

//=============================================================================
// Get Particles related to a RecVertex
//=============================================================================
void DisplVertices::GetPartsFromRecVtx(const RecVertex* dVtx,
                                       const Particle::ConstVector & inputParts,
                                       Particle::ConstVector & RecParts){


  SmartRefVector< Track >::const_iterator iVtx = dVtx->tracks().begin();

  //if( (*iVtx)->checkType(Track::Long) ){
  int endkey = dVtx->tracks().back()->key();

  Particle::ConstVector::const_iterator jend = inputParts.end();
  for ( Particle::ConstVector::const_iterator j = inputParts.begin();
        j != jend;++j) {

    if( (*j)->proto()->track() == NULL ) continue;
    const Track * tk = (*j)->proto()->track();
    while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
      ++iVtx;
    }
    if( (*iVtx)->key() == tk->key() ){
      if( (*iVtx)->key() != endkey ) ++iVtx;
      RecParts.push_back( *j );
      continue;
    }
  }
}

//============================================================================
//  Save Calorimeter info for a Prey
//============================================================================
StatusCode DisplVertices::SaveCaloInfos( Tuple& tuple ){
  double E = 0; double Et = 0.;
  StatusCode sc = GetCaloInfos( "Ecal", E, Et ) &&
    GetCaloInfos( "Hcal", E, Et ) &&
    //GetCaloInfos( "Prs", E, Et ) && GetCaloInfos( "Spd", E, Et ) &&
    GetCaloInfos( "Muon", E, Et );
  tuple->column( "TotEt", Et );
  return sc;
}

StatusCode DisplVertices::GetCaloInfos( string CaloType, double& En, double& Et ){
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
      //        << " is muon=" << (*imu)->IsMuon() << endmsg;
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



StatusCode DisplVertices::GetCaloClusterInfos( string CaloType, Tuple & tuple,
                                               double& En, double& Et ){

  double x=0,y=0,z=0;
  std::vector<double> X,Y,Z,E;
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
      debug() << "P (GeV) : " << myP / Gaudi::Units::GeV
              << " is muon=" << (*imu)->IsMuon() << endmsg;
      En += mE;
      Et += mET;
    }

  } else {
    //CaloDigitLocation::Spd
    const CaloDigits*  digitsCalo = get<CaloDigits>("Raw/"+CaloType+"/Digits");
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
      En += e;
      Et+= e*sqrt( (x*x + y*y)/(x*x + y*y + z*z) );
      X.push_back( x );
      Y.push_back( y );
      Z.push_back( z );
      E.push_back( e );
    }
  }

  tuple->column( "ETtot"+CaloType, Et );

  //Save position and Energy of the calorimeters in Tuple
  const int maxSize = 50000;
  tuple->farray( "X"+CaloType, X.begin(),X.end(), CaloType+"_n" , maxSize );
  tuple->farray( "Y"+CaloType, Y.begin(),Y.end(), CaloType+"_n" , maxSize );
  tuple->farray( "Z"+CaloType, Z.begin(),Z.end(), CaloType+"_n" , maxSize );
  tuple->farray( "E"+CaloType, E.begin(),E.end(), CaloType+"_n" , maxSize );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Save trigger and event infos in a tuple
//  To find out the alleys, TriggerMonitor.py with hlt2 = True
//=============================================================================
// StatusCode DisplVertices::SaveTrigInfinTuple( Tuple & tuple ){

//   if( m_tisTos == NULL ) return Error("m_tisTos not initialized");

//   //Get L0 info and save decision in tuple
//   if( !exist<L0DUReport>( L0DUReportLocation::Default )){
//     err()<<"You requested the trigger, make sure you run it."<<endmsg;
//     err()<<"No LODU report at " << L0DUReportLocation::Default <<endmsg;
//     return StatusCode::FAILURE;
//   }
//   L0DUReport * l0 = get<L0DUReport>( L0DUReportLocation::Default );
//   int L0dec = l0->decision();
//   if( msgLevel(MSG::DEBUG) )
//     debug()<<"L0 decision                  : " << L0dec << endreq;
//   tuple->column( "L0", L0dec );

//   //fill the HLT global : Hlt2 : 77,Hlt1 :  46
//   vector<unsigned int> m_routingBits;
//   m_routingBits.push_back( 46 );
//   m_routingBits.push_back( 77 );
//   //   for ( unsigned int i = 32 ; i < 96 ; i++){m_routingBits.push_back(i);}
//   bool Hlt1Globdec = false;
//   bool Hlt2Globdec = false;
//   if(exist<LHCb::RawEvent>(RawEventLocation::Default)){
//     RawEvent* rawEvent = get<RawEvent>(RawEventLocation::Default);
//     vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,m_routingBits);
//     unsigned int d1 = 46;
//     unsigned int d2 = 77;
//     if( find( yes.begin(), yes.end(), d1 ) != yes.end() ) Hlt1Globdec = true;
//     if( find( yes.begin(), yes.end(), d2 ) != yes.end() )  Hlt2Globdec = true;
//     if( msgLevel(MSG::DEBUG) ){
//       debug()<<"Firing routing bits : "<< yes << endmsg;
//       debug()<<"Hlt1 Global decision         : " << Hlt1Globdec << endmsg;
//       debug()<<"Hlt2 Global decision         : " << Hlt2Globdec << endmsg;
//     }
//   }
//   tuple->column( "Hlt1", Hlt1Globdec );
//   tuple->column( "Hlt2", Hlt2Globdec);

//   /***************************
//    * Beware : it seems that HltDecRepor writes only on TES algos that fired
//    *****************************/
//   if (!exist<HltDecReports>( HltDecReportsLocation::Default ) ){
//     return true;
//     warning()<<"No HltDecReports at "
//       << HltDecReportsLocation::Default << endmsg;
//   }
//   const HltDecReports* decReports = get<HltDecReports>
//     ( HltDecReportsLocation::Default );

//   int DVdec = 0;
//   const LHCb::HltDecReport * decrep =
//     decReports->decReport("Hlt2DisplVerticesSingleDecision");
//   if( decrep != NULL && decrep->decision() ) DVdec += 1;
//   decrep = decReports->decReport("Hlt2DisplVerticesDoubleDecision");
//   if( decrep != NULL && decrep->decision() ) DVdec += 10;
//   if( msgLevel(MSG::DEBUG) )
//     debug()<<"Hlt2DisplVerticesDecision    : " << DVdec << endmsg;
//   tuple->column( "HltDV", DVdec );

//   //Get topological trigger decision with TisTosTools
//   m_tisTos->setOfflineInput();
//   m_tisTos->setTriggerInput(); // reset trigger names
//   m_tisTos->setTriggerInput("Hlt2Topo.*Decision");//addToTriggerInput
//   bool TopoDec = m_tisTos->triggerTisTos().decision();
//   if( msgLevel(MSG::DEBUG) )
//     debug()<<"Hlt2Topo{2,3,4}Decision      : "<< TopoDec << endmsg;
//   tuple->column( "Hlt2Topo", TopoDec );

//   //////////////////////////////////////////////////////////////////
//   //Do not use HltDecReport for Hlt1Global and Hlt2Global decisions...
//   // Offline, all the rate limited stuff, including the Lumi/MinBias,
//   // doesn't work properly as you don't have the "counter" to limit it,
//   // so the Global //always goes to 1...
//   //   bool Hlt1Globdec = false;
//   //   const LHCb::HltDecReport * decrep = decReports->decReport("Hlt1Global");
//   //   if( decrep != NULL ) Hlt1Globdec = decrep->decision();
//   //   if( msgLevel(MSG::DEBUG) )
//   //     debug()<<"Hlt1 Global decision         : " << Hlt1Globdec << endmsg;
//   //   tuple->column( "Hlt1", Hlt1Globdec );

//   //   decrep = decReports->decReport("Hlt2Global");
//   //   bool Hlt2Globdec = false;
//   //   if( decrep != NULL ) Hlt2Globdec = decrep->decision();
//   //   if( msgLevel(MSG::DEBUG) )
//   //     debug()<<"Hlt2 Global decision         : " << Hlt2Globdec << endreq;
//   //   tuple->column( "Hlt2", Hlt2Globdec);

//   //To print entire report container :
//   //vector<string> allConfiguredTrgLines = decReports->decisionNames();
//   //debug() << *decReports << endmsg;

//   return SaveL0RawInfos(tuple);
// }

//============================================================================
//  Save the SPD multiplicity applied in the hadron and electron triggers.
//============================================================================
/*StatusCode DisplVertices::SaveL0RawInfos( Tuple& tuple ){
//TupleToolRecoStats.cpp

int nSpd = -1;
if(!m_l0BankDecoder){
Error("unable to get the L0DUFromRawTool");
} else {
m_l0BankDecoder->fillDataMap();
bool l0BankDecoderOK = m_l0BankDecoder->decodeBank();
if(!l0BankDecoderOK ){
Error("Readout error : unable to monitor the L0DU rawBank",
StatusCode::SUCCESS,StatusCode::SUCCESS).ignore();
} else {
nSpd = m_l0BankDecoder->data("Spd(Mult)");
}
}
tuple->column( "SPDMult", nSpd );

return StatusCode::SUCCESS ;
}*/

//============================================================================
// Save in Tuple the PV candidates
//============================================================================
StatusCode  DisplVertices::SavePVs( Tuple & tuple ){

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
StatusCode DisplVertices::fillHeader( Tuple& tuple ){
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
// Mother Reconstruction
//============================================================================
StatusCode DisplVertices::BackToBack( Particle::ConstVector & Cands ){

  setFilterPassed(false);
  GetPVs();
  if( PVs.empty() ) return StatusCode::SUCCESS ;

  //Loop on candidates
  Particle::ConstVector::const_iterator iend = Cands.end();
  for( Particle::ConstVector::const_iterator i1 = Cands.begin();
       i1 < iend; ++i1 ){
    const Particle * p1 = (*i1);
    Particle::ConstVector::const_iterator i2 = i1; ++i2;
    for( ; i2 < iend; ++i2 ){
      const Particle * p2 = (*i2);

      //Loop on PVs
      for ( RecVertex::Range::const_iterator i = PVs.begin();
            i != PVs.end() ; ++i ){
        const RecVertex* pv = *i;
        //check that no candidate comes before the pv.
        const Gaudi::XYZPoint & pospv = pv->position();
        const Gaudi::XYZPoint & pos1 = p1->endVertex()->position();
        const Gaudi::XYZPoint & pos2 = p2->endVertex()->position();
        if( pos1.z() < pospv.z() || pos2.z() < pospv.z() ) break;

        Gaudi::XYZPoint v1 = Minus( pos1, pospv);
        Gaudi::XYZPoint v2 = Minus( pos2, pospv);
        //compute azimuthal angle between the 2 candidates
        double dphi = GetDeltaPhi( v1, v2 );
        if( dphi > m_Backtoback ){
          ++counter("Nb of mothers");
          setFilterPassed(true);
          return StatusCode::SUCCESS;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
