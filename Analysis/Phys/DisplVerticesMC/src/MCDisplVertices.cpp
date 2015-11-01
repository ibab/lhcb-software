// $Id: $
// Include files

// from Gaudi
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

//Use ODIN
#include "Event/ODIN.h"

//Trigger decisions
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

//get the Header of the event
#include "Event/RecHeader.h"

//MC Check
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Event/HepMCEvent.h"
#include "Event/MCTrackInfo.h"

//calorimeter
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"

//to retrieve routing bits
#include "Kernel/ReadRoutingBits.h"

#include <functional>
#include <vector>

// local
#include "MCDisplVertices.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : MCDisplVertices
//
// 2008-april-2 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCDisplVertices )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCDisplVertices::MCDisplVertices( const std::string& name,
                                    ISvcLocator* pSvcLocator)
    : DaVinciTupleAlgorithm ( name , pSvcLocator )
    , m_pLinker()
    , m_vFit(0)
    , m_tisTos(0)
    , pi(3.1415926)
    , m_maxunpure(0.1)
    , m_minmixed(0.7)
    , m_maxmixed(0.95)
    , PV(0)
    , MCPV(0)
    , m_PreyID(0)
    , m_MotherPreyID(0)
    , m_IsPreyFromMother(false){
    declareProperty("MC", m_MC = false );//if work in a MC gen sample
    declareProperty("HepMC", m_HepMC = "" );//if work in a HepMC gen sample
    declareProperty("SaveOnTES", m_SaveonTES = true );
    declareProperty("SaveTuple", m_SaveTuple = false );//save prey infos in Tuple
    declareProperty("SaveTrigInfos", m_SaveTrigInfos = false );
    declareProperty("RemVtxFromAir", m_RemVtxFromAir = false );
    declareProperty("Prey", m_Prey = "~chi_10" );
    declareProperty("MotherPrey", m_MotherPrey = "H_10" );
    //>6.286GeV=Bc+ Mass
    declareProperty("MinNBCands", m_NbCands = 1 );
    declareProperty("PreyMinMass", m_PreyMinMass = 6.3*GeV );
    //Unlimited
    declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*TeV );
    declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
    declareProperty("RMin", m_RMin = 0.3*mm );//0.06 in K
    declareProperty("RMax", m_RMax = 10.*m );
    declareProperty("DistMax", m_DistMax = 10.* m );//Check value.
    declareProperty("MaxChi2OvNDoF", m_MaxChi2OvNDoF = 1000. );
    declareProperty("MuonpT", m_MuonpT = -1*GeV );
    declareProperty("PurityMin", m_PurityMin = 100 );
    declareProperty("DocaMax", m_DocaMax = 0.1 * mm);//Simulate resolution
    declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks 5
    declareProperty("SigmaZ", m_SigmaZ = 1000. );
    declareProperty("SigmaR", m_SigmaR = 1000. );
    declareProperty("RCutMethod", m_RCut = "FromUpstreamPV" );
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
MCDisplVertices::~MCDisplVertices() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCDisplVertices::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if( msgLevel( MSG::DEBUG ) )
    debug() << "==> Initialize the MCDisplVertices algorithm" << endmsg;


  if( m_SaveTrigInfos ){
    m_tisTos = tool<ITriggerTisTos>("TriggerTisTos",this);
    m_l0BankDecoder = tool<IL0DUFromRawTool>("L0DUFromRawTool");
  }


  //Initialize Vertex Fitter
  //NOTE : BlindVertex fitter is temp. not available in v19r11 !
  //m_vFit = vertexFitter("BlindVertexFitter");
  //debug() << "BlindVertexFitter retrieved "<< endmsg ;
  m_vFit = vertexFitter("OfflineVertexFitter");
  debug() << "OfflineVertexFitter retrieved "<< endmsg ;
  m_tFit = tool<ITrackVertexer>
    ( "TrackVertexer","TrackTools", this );

  //Retrieve MC linker tools
  m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Links,
                                    std::vector<std::string>(1,"") );
  //m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Links,
  //"Phys/StdNoPIDsPions" );
  //m_pLinker = new Object2FromMC<>(this,Particle2MCMethod::Links,
  //"Phys/StdNoPIDsPions" );
  //Methods are WithChi2, Chi2, Links, Composite, ChargedPP, NeutralPP, Max

  //To recreate track measurements
  m_measProvider = tool<IMeasurementProvider>
    ( "MeasurementProvider","MeasProvider", this );

  if( m_RemVtxFromDet != 0 || m_SaveTuple || m_RemVtxFromAir){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();

    // Get Transport Service
    m_transSvc = svc<ITransportSvc>( "TransportSvc", true  );
  }


  //Get the track extrapolator
  //m_extra = tool<ITrackExtrapolator>("TrackMasterExtrapolator");

  //To fit tracks
  //m_pToState = tool<IParticle2State>("Particle2State");
  //m_trackPreFit = tool<ITrackFitter>("TrackMasterFitter"," preFit", this);
  //m_trackFit = tool<ITrackFitter>("TrackMasterFitter", "fit", this);



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
  if( m_PurityMin < 1.1 ) m_MC = true;

  if( context() == "HLT" ){
    m_SaveTuple = false; m_MC = false;
  }

  if( m_Backtoback > 0 && m_NbCands < 2 ) m_NbCands = 2;

  if( context() == "Info" ){
    info()<<"--------------------------------------------------------"<<endmsg;
    info() << "MCDisplVertices will select " << m_Prey
           << " candidates (ID=" << m_PreyID.pid() <<") ";
    if(m_MC){
      info() << "with theoretical mass of " << Prey->mass()/GeV
             <<" GeV"; }
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
    } else if( m_RCut == "FromPreyInfo" ){
      info() << "taken from p->info(52)." << endmsg;
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
StatusCode MCDisplVertices::execute(){

  ++counter("Processed evt nb");
  if( msgLevel( MSG::DEBUG ) )
    debug() << "==> Execute the MCDisplVertices algorithm, event "
            << counter("Processed evt nb").flag() << endmsg;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.

  //Clear stuff
  PVs.clear();

  //------------------Some Studies------------------
  //StudyPV(); return StatusCode::SUCCESS;  //PV Properties
  //Study Activity : propriety of vtx wr to the position
  //StudyActivity(); return StatusCode::SUCCESS;
  //StudyDistanceBet2Interactions(); //return StatusCode::SUCCESS;
  //Study decay length discrepancy between MC and Gen container
  //StudyDiffGenMC();return StatusCode::SUCCESS;
  //StudyEoverNbTrk();return StatusCode::SUCCESS;

  //---------------------------------------------
  if( m_SaveTrigInfos && !m_SaveTuple ){
    Tuple tuple = nTuple("Trigger");
    if( fillHeader( tuple ).isFailure() ||
        SaveTrigInfinTuple( tuple ).isFailure() )
      Warning("Not being able to save trigger infos in tuple !");
    if( !(tuple->write()) ) return StatusCode::FAILURE;
  }

  //------------------Retrieve HepMC Infos------------------
  if( m_HepMC !="" ) {
    StatusCode sc = GetHepMCInfos();
    if( sc.isFailure() ) {
      fatal() << "Could not retrieve Hep Monte-Carlo infos." << endmsg;
      return StatusCode::FAILURE;
    }
    //return StatusCode::SUCCESS;
  }

  //------------------Retrieve MC Infos------------------
  m_IsPreyFromMother = false;
  if( m_MC && false ) {
    StatusCode sc = GetMCInfos();
    if( sc.isFailure() ) {
      fatal() << "Could not retrieve Monte-Carlo infos." << endmsg;
      return StatusCode::FAILURE;
    }
    //return StatusCode::SUCCESS;
  }

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

  vector<int>  nboftracks, nboftracksl;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts, muons, indets, recqs, massls;

  if( msgLevel( MSG::DEBUG ) )
    debug()<<"--------Reconstructed Displ. Vertices --------------"<< endmsg;
  Particle::ConstVector Cands;
  m_purities.clear();
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
    double rho = (m_RCut=="FromPreyInfo")?p->info(52,-1000.):GetRFromBL(pos);
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
             << sqrt(err(2,2)) <<", sigmaR "<< errr <<", rec charge "<< recq;
      if(muon){
        debug()<<", has muon with pt "<< muon <<" GeV" << endmsg;
      } else { debug()<< endmsg; }
    }

    //Has the (MC) vertex been generated by collisin of flying particles
    // with air of beam pipe ?
    if( m_RemVtxFromAir && IsFromAir(p) ) continue;

    //Is the particle close to the detector material ?
    if( m_RemVtxFromDet!= 5 && IsAPointInDet( p, m_RemVtxFromDet, m_DetDist ) )
      continue;
    if( m_RemVtxFromDet== 5 && p->info(51,-1000.)>-900) continue;

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
    if( m_MC && !GetMCPrey( p ) ) continue;

    //Save infos in tuple !
    if( m_SaveTuple ){
      nboftracks.push_back( nbtrks ); chindof.push_back( chi );
      e.push_back(mom.e());
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
      if( !m_MC ) m_purities.push_back( 0. );
      if(true){
        double mlong = 0.; int nbtlong = 0;
        GetMassFromLongTracks( p, mlong, nbtlong );
        nboftracksl.push_back( nbtlong );
        massls.push_back( mlong );
      }
    }

    Particle clone = Particle( *p );
    clone.setParticleID( m_PreyID );
    this->markTree( &clone );
    Cands.push_back( clone.clone() );

  }//  <--- end of Prey loop

  if( Cands.size() < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of candidates !"<< endmsg;
    return StatusCode::SUCCESS;
  }
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
    tuple->farray( "PreyPurity", m_purities.begin(), m_purities.end(),
                   "NbPrey", NbPreyMax );
    if( massls.size() != 0 ){
      tuple->farray( "PreyMLongs", massls.begin(), massls.end(),
                     "NbPrey", NbPreyMax );
      tuple->farray( "PreyNbofLongTracks", nboftracksl.begin(),
                     nboftracksl.end(), "NbPrey", NbPreyMax );
    }
    if( !m_SaveTrigInfos ) tuple->column( "FromMother", m_IsPreyFromMother );
    tuple->column( "BLX", m_BeamLine->referencePoint().x() );
    tuple->column( "BLY", m_BeamLine->referencePoint().y() );
    tuple->column( "BLZ", m_BeamLine->referencePoint().z() );
    if( !SavePVs( tuple )  ) return StatusCode::FAILURE;
    tuple->column( "NbVelo", GetNbVeloTracks() );
    //if( !SaveGEC( tuple, Cands ) ) return StatusCode::FAILURE;
    if( m_SaveTrigInfos && !SaveTrigInfinTuple( tuple ) )
      return StatusCode::FAILURE;
    if( !(tuple->write()) ) return StatusCode::FAILURE;
  }

  //Save Preys from Desktop to the TES.
  //if( m_SaveonTES ) desktop()->saveDesktop();
  //The following just saves the Particles in the desktop !!
  //if( m_SaveonTES ) desktop()->saveTrees( m_outputParticles ) ;


  //--------------Mother Reconstruction------------------
  if( m_Backtoback > 0 && Cands.size() >= 2 ){
    if( BackToBack( Cands ).isFailure() )
      Warning("Reconstruction process for mother"+ m_MotherPrey +" failed !");
  }

  //---------------------------------------------
  //StudyParticleGun(Cands);


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCDisplVertices::finalize() {
  debug() << "==> Finalize" << endmsg;

  if (NULL!=m_pLinker) delete m_pLinker ;

  if( m_RCut !="FromBeamLine" ) delete m_BeamLine;

  if( context() == "HLT" ) return DaVinciTupleAlgorithm::finalize();


  info()<<"-------------------------------------------------------"<< endmsg;
  info()<<"              DisplVertices Statistics                 "<< endmsg;
  info()<<"-------------------------------------------------------"<< endmsg;
  info()<<"Number of reconstructed "<< m_Prey <<"               : "
        << counter("Nb of candidates").flag() << endmsg;
  info()<<"Number of reconstructed "<<  m_MotherPrey <<"                 : "
        << counter("Nb of mothers").flag() << endmsg;
  if( m_MC ){
    info()<<"Percentage of reconstructed "<< m_Prey <<"           : "
          << 100.*counter("RecbleMCPrey").flagMean() << endmsg;
    info()<<"Nb of ass. MC tracks of "<< m_Prey <<"               : "
          << counter("NbassfromaPrey").flagMean() <<" +- "
          << counter("NbassfromaPrey").flagMeanErr() << endmsg;
    info()<<"Average purity of reconstructed "<< m_Prey <<"       : "
          << counter("purity").flagMean() <<" +- "
          << counter("purity").flagMeanErr() <<endmsg;
    info()<<"Pure "<< m_Prey <<"    (purity > "<< m_maxmixed
          << ")               :" << 100.*counter("p1").flagMean() <<"%"<< endmsg;
    info()<<"Mixed "<< m_Prey <<"   ( "<< m_minmixed <<" < purity < "
          << m_maxmixed << ")        :" << 100.*counter("p2").flagMean()
          <<"%"<< endmsg;
    info()<<"Unpure "<< m_Prey <<"  (purity < "
          << m_maxunpure << ")                :"
          << 100.*counter("p3").flagMean() <<"%"<< endmsg;
  }
  info()<<"-------------------------------------------------------"<< endmsg;


  return DaVinciTupleAlgorithm::finalize();
}

//============================================================================
//  Study impact parameter of Prey to the PV
//============================================================================
void MCDisplVertices::StudyPreyIP( const LHCb::Particle * p, const RecVertex * PV ){

  //Histogramm IPs of Prey to the PV
  double imp,chi2;
  distanceCalculator()->distance( p, PV, imp, chi2);
  //debug() << "Impact parameter " << imp << " " << impe << endmsg;
  plot( imp, "IPtoPV", 0., 10.);
  plot( chi2, "IPchi2toPV", 0., 200.);

  //Study sum of pT of daughter tracks !
  double pt = 0.;
  SmartRefVector<LHCb::Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<LHCb::Particle>::const_iterator i =
         p->daughters().begin(); i!= iend; ++i ){
    pt += (*i)->pt();
  }
  plot( pt/1000., "PreypTTot", 0., 100. );

}

//============================================================================
// Study Activity
//============================================================================
void MCDisplVertices::StudyActivity(){

  Tuple tuple = nTuple("Activity");

  vector<int> nbtrk;
  vector<double> r,m;
  vector<double> dist, disttoPV; //<--- mais quel PV ???
  vector<Gaudi::XYZPoint> pos; //  <--- essaye de mettre des pointeurs !
  vector<Gaudi::LorentzVector> mom;

  // select Monte Carlo Vertices:
  const MCVertex::Container* mcvertices = get<MCVertex::Container>
    ( MCVertexLocation::Default ) ;
  int size = mcvertices->size();
  debug()<<"There are "<< size << " MC vertices." << endmsg;

  int nb;
  Gaudi::LorentzVector mass;
  //Loop on vertices
  MCVertex::Container::const_iterator ivend = mcvertices->end();
  for ( MCVertex::Container::const_iterator i = mcvertices->begin();
        ivend != i ; ++i ){
    nb = 0;
    mass.SetCoordinates( 0., 0., 0., 0. );
    GetMCStable( *i, nb, mass );    //look for stable daughters

    //Check if Vertex already in the list.
    size = pos.size();
    for( int j = 0; j != size; ++j ){

      if( VertDistance( pos.at(j), (*i)->position() ) < m_DocaMax ){
        //Vertices are considered the same
        nbtrk.at(j) += nb;
        mom.at(j) += mass;
        goto exit;
      }
    } // end of check list
    if( nb == 0 ) continue;
    pos.push_back( (*i)->position() );
    nbtrk.push_back( nb );
    mom.push_back( mass );
    r.push_back( pos.back().Rho() );
    dist.push_back( pos.back().z() );
    disttoPV.push_back( VertDistance( (*i)->primaryVertex()->position(),
                                      (*i)->position() ) );
  exit:;

  }// end of vertex

  //Save vertex masses
  vector<Gaudi::LorentzVector>::iterator iend = mom.end();
  for( vector<Gaudi::LorentzVector>::iterator i = mom.begin();
       i != iend; ++i ){
    m.push_back( (*i).mass() );
  }

  //Print out the vertices
  if( msgLevel(MSG::DEBUG) ){
    size = pos.size();
    for( int j = 0; j != size; ++j ){
      debug()<<"Vertex pos "<< pos.at(j) << ", nb of trks "<< nbtrk.at(j)
             <<", r "<< r.at(j) << ", mass "<< m.at(j) <<", distance to PV "
             << disttoPV.at(j) << endmsg;

    }
  }

  const int maxSize = 5000;
  tuple->farray( "NbTrk", nbtrk.begin(), nbtrk.end(), "Nb", maxSize );
  tuple->farray( "R", r.begin(), r.end(), "Nb", maxSize );
  tuple->farray( "M", m.begin(), m.end(), "Nb", maxSize );
  tuple->farray( "Dist", dist.begin(), dist.end(), "Nb", maxSize );
  tuple->farray( "DisttoPV", disttoPV.begin(), disttoPV.end(), "Nb", maxSize );

  if( !fillHeader( tuple ) ) always()<<"Could not retrieve header !"<< endmsg;

  tuple->write();
}

//============================================================================
//  Study the distance between 2 interactions
//============================================================================
void MCDisplVertices::StudyDistanceBet2Interactions(){

  // select Monte Carlo Vertices:
  const MCVertex::Container* mcvertices = get<MCVertex::Container>
    ( MCVertexLocation::Default ) ;

  //Find PV's
  vector< MCVertex * > MCPV;
  MCVertex::Container::const_iterator iend = mcvertices->end();
  for ( MCVertex::Container::const_iterator i = mcvertices->begin();
        iend != i ; ++i ){
    if( !(*i)->isPrimary() )continue;
    MCPV.push_back( *i );
  }
  if( MCPV.size() >= 2 ){
    plot( VertDistance( MCPV.at(0)->position(), MCPV.at(1)->position() ),
          "MCDistBet2Int", 0., 200.);
  }
}

//============================================================================
//  Study decay length discrepancy between MC and Gen containers
//============================================================================
void MCDisplVertices::StudyDiffGenMC(){

  Tuple tuple = nTuple("GenMC");
  if( !fillHeader( tuple ) ) return;
  vector<double> mcneut, genneut, mcch, gench;

  //Gen container
  HepMCEvents* events = get<HepMCEvents>( HepMCEventLocation::Default ) ;

  HepMCEvents::const_iterator ie=events->begin();
  const HepMCEvent* event = *ie ;  //get first MC event in a collision
  const HepMC::GenEvent* theEvent = event->pGenEvt();

  //Find Gen PV
  HepMC::GenVertex* PV = 0;
  HepMC::GenEvent::particle_const_iterator p= theEvent->particles_begin();
  PV = (*p)->end_vertex();
  debug()<<"--------------------------------------------------" << endmsg;
  debug()<<"Primary Vertex " << Print(PV->position()) << endmsg;

  //Find the interesting particles
  HepMC::GenEvent::particle_const_iterator pend = theEvent->particles_end();
  for( p= theEvent->particles_begin(); p!= pend; ++p){
    //debug()<<"Particle " << (*p)->pdg_id() <<" "<< (*p)->status()<< endmsg;
    //the prey
    if( (*p)->pdg_id() == 1000022 && (*p)->status() == 2 ){
      HepMC::GenVertex* Vert = (*p)->end_vertex();
      debug()<<"Position of neut decay "<< Print(Vert->position()) << endmsg;
      double dist = VertDistance( PV,Vert );
      debug() << "Dist of flight " << dist << endmsg;
      //plot( dist, "GenPreyFlight", 0., 100. );
      genneut.push_back( dist );
    }
    //the chargino
    if( abs((*p)->pdg_id()) == 1000024 && (*p)->status() == 2 ){
      HepMC::GenVertex* Vert = (*p)->end_vertex();
      debug()<<"Position of charg decay "<< Print(Vert->position()) << endmsg;
      double dist = VertDistance( PV,Vert );
      debug() << "Dist of flight " << dist << endmsg;
      //plot( dist, "GenChargFlight", 0., 100. );
      gench.push_back( dist );
    }
  }

  //MC container
  MCParticles* mcparts = get<MCParticles>(MCParticleLocation::Default );
  if( !mcparts ){
    warning() << "Unable to find MC particles at '"
              << MCParticleLocation::Default << "'" << endmsg;
    return;
  }
  //Find MC PV
  MCVertex * MCPV = NULL;
  const MCVertex::Container* mcvertices = get<MCVertex::Container>
    ( MCVertexLocation::Default ) ;
  MCVertex::Container::const_iterator imcend = mcvertices->end();
  for ( MCVertex::Container::const_iterator imc = mcvertices->begin() ;
        imcend != imc ; ++imc ){
    if (  (*imc)->isPrimary() ) {
      if( MCPV == NULL ) MCPV = (*imc);
      debug() << "MC Primary Vertex : " << (*imc)->position() << endmsg;
      break;
    }
  }
  //Find the interesting particles
  MCParticle::Vector::iterator iend = mcparts->end();
  for( MCParticle::Vector::iterator i = mcparts->begin(); i != iend; ++i ){

    //the neutralino
    if( (*i)->particleID().abspid() == 1000022 ){
      SmartRefVector< LHCb::MCVertex >::const_iterator vtx =
        (*i)->endVertices().begin();
      Gaudi::XYZPoint pos = (*vtx)->position();
      debug()<<"Position of neut decay "<< pos << endmsg;
      double dist = VertDistance( MCPV->position(), pos );
      debug() << "Dist of flight " << dist << endmsg;
      //plot( dist, "MCPreyFlight", 0., 100. );
      mcneut.push_back( dist );
    }
    //the chargino
    if( (*i)->particleID().abspid() == 1000024 ){
      SmartRefVector< LHCb::MCVertex >::const_iterator vtx =
        (*i)->endVertices().begin();
      Gaudi::XYZPoint pos = (*vtx)->position();
      debug()<<"Position of charg decay "<< pos << endmsg;
      double dist = VertDistance( MCPV->position(), pos );
      debug() << "Dist of flight " << dist << endmsg;
      //plot( dist, "MCChargFlight", 0., 100. );
      mcch.push_back( dist );
    }
  }

  //Save ntuple
  const int max = 5;
  tuple->farray( "GenKDist", genneut.begin(),genneut.end(), "NK", max );
  tuple->farray( "MCKDist", mcneut.begin(),mcneut.end(), "NK", max );
  tuple->farray( "GenCHDist", gench.begin(),gench.end(), "NCH", max );
  tuple->farray( "MCCHDist", mcch.begin(),mcch.end(), "NCH", max );
  tuple->write();
}

//============================================================================
//  Study the Hlt efficiency for reconstructing Prey DV
//============================================================================
void MCDisplVertices::StudyHltEfficiency( const Gaudi::XYZPoint & pos ){

  //******** Menu
  double diff = 1.;

  //Retrieve HLT reconstructed vertices if Hlt fired
  if( !exist<RecVertices>( "Rec/Vertices/Hlt2DisplVerticesV3D" ) ) return;
  RecVertices* DV = get<RecVertices>( "Rec/Vertices/Hlt2DisplVerticesV3D" );
  int size = DV->size();
  if( msgLevel( MSG::DEBUG ) ) debug()<<"Hlt nb of RV "<< size << endmsg;
  //if( size == 0 ) return;

  bool HltOk = false;
  RecVertices::const_iterator iend = DV->end();
  for( RecVertices::const_iterator i = DV->begin(); i != iend; ++i ){
    double dist = VertDistance( (*i)->position(), pos );
    //debug()<<"dist  " << dist <<endmsg;
    plot( dist, "HltDVReso", 0., diff*5 );
    if( dist < diff ){
      HltOk = true;
      break;
    }
  }
  plot( HltOk, "HltRecVtx", 0, 2 );

}

//============================================================================
//  Study the properties of the PVs
//============================================================================
void MCDisplVertices::StudyPV(){

  /******************************************************
   * Here for archiving purpose
   * Please prefer P2PV.py
   * Has proven not to work on RecVertices->work on Particles
   *****************************************************/

  //**** menu***
  double diff = 0.5;

  int nbmax = 0;
  double massmax = 0.;
  const Particle * PrV = NULL ;

  //Get MC Infos (PV and preys)
  GetMCInfos();

  //Get all reconstructed 3D vertices
  //RecVertices* DV = get<RecVertices>( "Rec/Vertices/RV" );
  const Particle::ConstVector DV = this->i_particles();
  unsigned int size = DV.size() ;
  debug()<< "Number of Displaced Vertices " << size << endmsg;
  plot( size,"NbofDisplVtx", 0,12 );
  if( size < 1 ) return;

  //Loop on all rec vertices
  double mass = 0.; int nb = 0;
  Particle::ConstVector::const_iterator iend = DV.end();
  for( Particle::ConstVector::const_iterator i = DV.begin(); i != iend; ++i ){

    //Remove low tracks vertices
    //int nb = (*i)->tracks().size();
    const Vertex * v = (*i)->endVertex();
    nb = v->outgoingParticles().size();
    if( nb < m_nTracks ) continue;
    mass = (*i)->measuredMass()/1000.;
    debug()<<"RV nb of tracks "<< nb <<" mass "<< mass <<endmsg;
    //If it is too close to the MC PV, continue
    double dist = VertDistance(MCPV->position(),v->position());
    //debug()<<"DIST " << dist <<endmsg;
    if( dist < diff ){
      PrV=(*i);
      debug()<<"PV position "<< v->position() << endmsg;
      plot( nb, "PVNboftracks", 0., 80.);
      plot( v->position().z(), "PVz", -300., 300.);
      plot( v->position().Rho(), "PVr", 0., 1.);
      double recpt = PrV->pt() ;// sumpt;
      plot( mass , "PVMass", 0., 100.);
      plot( recpt/1000. , "PVrecpt", 0., 50.);
      plot( dist , "PVReso", 0.0, 1.05 );
      bool bk = HasBackwardTracks( PrV );
      plot( bk, "HasPVbckwd", 0, 2 );
      continue;
    }
    if( nb > nbmax ) nbmax = nb;
    if( mass > massmax ) massmax = mass;

    //if it is too close to a prey, continue
    bool quit = false;
    vector<Gaudi::XYZPoint>::iterator jend = m_MCPos.end();
    for( vector<Gaudi::XYZPoint>::iterator j =  m_MCPos.begin(); j!=jend;++j ){
      dist = VertDistance( (*j),v->position());
      if( dist < diff ){
        quit = true;
        debug()<<"Prey position "<< v->position() << endmsg;
        bool bk = HasBackwardTracks( (*i) );
        plot( bk, "HasPreybckwd", 0, 2 );
        break;
      }
    }
    if( quit ) continue;


    //The DV is a SV ?
    quit = false;
    jend = m_MCSVPos.end();
    for( vector<Gaudi::XYZPoint>::iterator j =  m_MCSVPos.begin();
         j!=jend; ++j ){
      dist = VertDistance( (*j),v->position());
      if( dist < diff ){
        quit = true;
        debug()<<"SV position "<< v->position() << endmsg;
        plot( nb, "SVNboftracks", 0., 50.);
        plot( v->position().z(), "SVz", -300., 300.);
        plot( v->position().Rho(), "SVr", 0., 1.);
        double recpt = (*i)->pt(); //sumpt;
        plot( mass , "SVMass", 0., 100.);
        plot( recpt/1000. , "SVrecpt", 0., 50.);
        plot( dist, "SVRes", 0., 1. );
        bool bk = HasBackwardTracks( (*i) );
        plot( bk, "HasSVbckwd", 0, 2 );
        break;}
    }
    if( quit ) continue;
  }


  if( PrV != NULL ){
    bool trk = false; bool m = false;
    //Is the PV the RecVertex with the most tracks ?
    nb = PrV->endVertex()->outgoingParticles().size();
    if( nbmax < nb ) trk = true;
    // Is the PV the RecVertex with the higher rec mass ?
    if( massmax < (PrV->measuredMass()/1000.) ) m = true;
    debug()<<"Has the PV the biggest mass ? "<< m <<" the gratest nb of trks ?"
           << trk << endmsg ;
    plot( trk, "IsPVTrk", 0, 2);
    plot( m, "IsPVMass", 0, 2);
  }

}

//============================================================================
// Study the impact parameter of the Daughters to the PV
//============================================================================
void MCDisplVertices::StudyDgtsIP( const Particle * p ){
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin();
       i!= iend; ++i ){
    double imp, chi2;
    distanceCalculator()->distance( static_cast<const Particle*>(*i),
                                    PV, imp, chi2);
    //debug() << "Impact parameter " << imp << " " << impe << endmsg;
    plot( imp, "PreyDgtIPtoPV", 0., 15.);
    plot( chi2, "PreyDgtIPchi2toPV", 0., 300.);
  }
}

void MCDisplVertices::StudyDgtsIP( const LHCb::RecVertex * v ){
  SmartRefVector<Track>::const_iterator iend = v->tracks().end();
  for( SmartRefVector<Track>::const_iterator i = v->tracks().begin();
       i != iend; ++i ){
    //Beware : computed from first track state !
    double imp = ImpactParam( (*i)->position(), (*i)->momentum(),
                              PV->position() );
    //debug() << "Impact parameter " << imp << endmsg;
    plot( imp, "PVDgtIPtoPV", 0., 2.);
  }
}

//============================================================================
// Study angular aperture of prey daughters and delta R (dispersion)
//============================================================================
void MCDisplVertices::StudyDispersion( const LHCb::Particle * p ){

  //loop on daughters
  double dthetamax = 0;
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin();
       i!= p->daughters().end(); ++i ){
    //compute angular aperture, i.e. angle between p and its daughters
    double dtheta = fabs( (*i)->momentum().theta() - p->momentum().theta() );
    //debug() << "" << dtheta << endmsg;
    plot( dtheta, "PreyDgtsTheta", 0., pi/4 );
    plot( GetDeltaR( (*i), p ), "PreyDgtsDR", 0., 10.);
    if( abs(dtheta) > dthetamax ) dthetamax = dtheta;
  }
  plot( dthetamax, "PreyDgtsThetaMax", 0., pi/4 );
}

void MCDisplVertices::StudyDispersion( const LHCb::Particle * p1,
                                       const LHCb::Particle * p2 ){

  plot( GetDeltaR( p1, p2 ), "PreyNeutDR", 0., 10.);

}

//============================================================================
//  Compute Energy over number of reconstructed tracks
//============================================================================
void MCDisplVertices::StudyEoverNbTrk(){

  double PtMin = 1*GeV;
  double E = 0.; double ET = 0.;
  int nblong = 0; int nbhighpt = 0;
  int nbdown = 0;

  //Get tracks
  const Track::Range gettracks = get<Track::Range>( TrackLocation::Default );
  debug() << "There are " << gettracks.size() << " tracks" << endmsg ;
  Track::ConstVector tracks(gettracks.begin(), gettracks.end());

  // Retrieve Primary vertices and remove tracks that contributed
  if( false ){
    const RecVertex::Range & PVCs = this->primaryVertices();
    for( RecVertex::Range::const_iterator i = PVCs.begin();
         i != PVCs.end() ; ++i ){
      //RemoveTracks( tracks, (*i) ); see v4r7 for this function
    }
  }

  //Compute nb of long tracks
  for( Track::ConstVector::const_iterator i = tracks.begin();
       i != tracks.end(); ++i ){
    if( (*i)->checkType( Track::Downstream) ) nbdown++ ;
    if( !(*i)->checkType( Track::Long) ) continue;
    ++nblong;
    if( (*i)->pt() > PtMin ) ++nbhighpt;
  }


  //Get Calo infos
  GetCaloInfos( "Ecal", E, ET );
  GetCaloInfos( "Hcal", E, ET );
  GetCaloInfos( "Muon", E, ET );

  debug()<<"Total Energy "<< E <<", Total transverse energy "<< ET
         <<", Nb of long tracks "<< nblong <<", nb of high pt long tracks "
         << nbhighpt << endmsg;

  if( nblong == 0 ) nblong = 1;
  if( nbhighpt == 0 ) nbhighpt = 1;
  if( nbdown == 0 ) nbdown = 1;

  double Eonblong = E/nblong;
  double ETonblong = ET/nblong;
  double Eonbhighpt = E/nbhighpt;
  double ETonbhighpt = ET/nbhighpt;
  double Edownolong = E*nbdown/nblong;
  double ETdownolong = ET*nbdown/nblong;

  debug()<<"Eonblong "<< Eonblong <<", ETonblong "<< ETonblong
         <<", Eonbhighpt "<< Eonbhighpt <<", ETonbhighpt "<< ETonbhighpt
         << endmsg;

  Tuple tuple = nTuple("EoverTrk");
  tuple->column( "NbLong", nblong );
  tuple->column( "NbDown", nbdown );
  //tuple->column( "NbHighpt", nbhighpt );
  tuple->column( "Eonblong", Eonblong );
  tuple->column( "ETonblong", ETonblong );
  //tuple->column( "Eonbhighpt", Eonbhighpt );
  //tuple->column( "ETonbhighpt", ETonbhighpt );
  tuple->column( "Edownolong", Edownolong );
  tuple->column( "ETdownolong", ETdownolong );

  tuple->write();

  plot( Edownolong, "Edownolong", 0., 1300 );
  plot( ETdownolong, "ETdownolong", 0., 100. );
  return;
}

//=============================================================================
// Study in details the composition of the Prey's daughters (id..)
//=============================================================================
void MCDisplVertices::StudyPreyComposition( const Particle * p, string s ){

  Gaudi::XYZPoint pos = p->endVertex()->position();
  if ( msgLevel( MSG::DEBUG )  ){
    debug()<<"-------------------"<< s <<"-----------------" << endmsg;
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
    //if(!sc) info()<<"Unable to recreate track measurements"<< endmsg;
    double z = tk->measurements().front()->z();
    debug()<<"ID "<< pid <<", z first meas. "
           << z << endmsg;
    if( z < pos.z() ) ++nbz;

    //let's take the tracks 2 by 2 and look if there is some null rec mass
    //if( d->pt() < 1*GeV ) continue;
    SmartRefVector< Particle >::const_iterator j = i; ++j;
    SmartRefVector< Particle >::const_iterator jend = p->daughters().end();
    while( j != jend ){
      //if( j->target()->pt() < 1*GeV ) continue;
      //debug()<<"Caught in infinite loop !"<< endmsg;

      //Compute angle between the 2 tracks
      ++nbpairs;
      double angle = Angle( j->target()->momentum(), d->momentum() );
      if( angle > maxangle  ) maxangle = angle;
      meanangle += angle;
      const Gaudi::LorentzVector l = d->momentum() + j->target()->momentum();
      plot( l.M()/1000., s+"PreyDgts2b2Mass", 0, 6 );
      plot( l.M()/1000., s+"PreyDgts2b2Mass2", 0, 0.5 );
      if( l.M() < 100 ) ++nbm;
      //debug()<<"Rec mass "<< l.M() << endmsg;
      ++j;
    }

  }
  meanangle = double( meanangle/nbpairs );
  //if( maxangle > pi ) maxangle -= pi;

  plot( nbe, s+"PreyDaughtersNbEl", 0, 10 );
  plot( nbz, s+"PreyZDgtsBefRV", 0, 10 );
  plot( nbm, s+"PreyDgts2b2MassNb", 0, 10);
  plot( maxangle, s+"PreyDgtsMaxAngle", 0., pi/2);
  plot( meanangle, s+"PreyDgtsMeanAngle", 0., pi/4 );
  //Study Boost
  plot( p->momentum().Gamma(), s+"PreyGamma", 0, 30. );
  plot( p->momentum().BoostToCM().z(), s+"PreyGammaZ", -1., -0.93 );

  debug() <<"Mean Prey Gamma "<< p->momentum().Gamma() <<", z boost "
          << p->momentum().BoostToCM().z() << endmsg;


  //later : put id info in a tuple !

}

//=============================================================================
// Study the Particles from Particle Gun
//=============================================================================
void MCDisplVertices::StudyParticleGun( Particle::ConstVector & preys ){

  //a ntuple for infos specific to particle gun
  Tuple tuple = nTuple("PartGun");

  vector<int> pids;

  Particle::ConstVector::const_iterator iend = preys.end();
  for( Particle::ConstVector::const_iterator is = preys.begin();
       is < iend; ++is ){
    const Particle * p = (*is);

    //retrieve particle causing the RecVertex
    const MCParticle * mcmother = WhichMother( p );
    //int pid = -1;
    //if( mcmother != NULL ) pid = mcmother->particleID().abspid();
    int pid = (mcmother != NULL) ? int(mcmother->particleID().abspid()) : -1;

    //Save PID in tuple
    pids.push_back( pid );

    //Save other infos in tuple

    //print out
    if ( msgLevel( MSG::DEBUG )  ){
      debug()<<"RecVertex from ParticleGun with mass "
             << p->measuredMass()/Gaudi::Units::GeV
             <<" GeV, nb of tracks "
             << p->endVertex()->outgoingParticles().size() << ", Chi2/ndof "
             << p->endVertex()->chi2PerDoF() <<", pos of end vtx "
             << p->endVertex()->position() << endmsg;
      debug()<<"Mother ID "<< pid << endmsg;
    }

  }

  //Save nTuples
  if( m_SaveTuple ){
    const int NbPreyMax = 20;
    if( !fillHeader(tuple) ) return;
    tuple->farray( "MotherID", pids.begin(), pids.end(), "NbPrey", NbPreyMax );
    tuple->write();
  }

}

//============================================================================
//  HepMC Informations
//  Warning : need to be rechecked
//============================================================================
StatusCode MCDisplVertices::GetHepMCInfos()
{
  Tuple tuple = nTuple("MCInfos");
  if( !fillHeader( tuple ) ) return StatusCode::FAILURE;


  //--------------- Generator Infos
  HepMCEvents* events = get<HepMCEvents>( HepMCEventLocation::Default ) ;

  HepMCEvents::const_iterator ie=events->begin();
  const HepMCEvent* event = *ie ;  //get first MC event in a collision
  const HepMC::GenEvent* theEvent = event->pGenEvt();

  //Save Prey calo informations in a Tuple
  //SaveGenPartinTuple( theEvent );

  if(true) theEvent->print( std::cout );

  //HepMC::GenParticle* h = 0;
  HepMC::GenVertex* PV = 0;
  vector< HepMC::GenParticle* > neut;
  vector< HepMC::GenParticle* > charg;

  //Find PV
  HepMC::GenEvent::particle_const_iterator p= theEvent->particles_begin();
  PV = (*p)->end_vertex();
  debug()<<"--------------------------------------------------" << endmsg;
  debug()<<"Primary Vertex " << Print(PV->position()) << endmsg;


  if ( m_HepMC=="mSUGRA" || m_HepMC=="AMSB" ){  //Find the LSP's

    //Find the interesting particles
    HepMC::GenEvent::particle_const_iterator pend = theEvent->particles_end();
    for( p= theEvent->particles_begin(); p!= pend;++p){
      //debug()<<"Particle " << (*p)->pdg_id() <<" "<< (*p)->status() << endmsg;
      // if( (*p)->pdg_id() == 25 && (*p)->status() == 2 ){ h = *p; }
      if( (*p)->pdg_id() == 1000022 && (*p)->status() == 2 ){
        neut.push_back( (*p) );
        //debug()<<"Found a neut !"<< endmsg;
      }
      if( abs((*p)->pdg_id()) == 1000024 && (*p)->status() == 2 ){
        charg.push_back( (*p) );
        //debug()<<"Found a charg !"<< endmsg;
      }
    }

    //loop on neutralinos found
    vector<double> energie;
    vector<double> pt;
    vector<double> eta;
    vector<double> kz;
    vector<double> kr;
    vector<double> flight;
    vector< HepMC::GenParticle* >::const_iterator iend = neut.end();
    for( vector< HepMC::GenParticle* >::const_iterator i = neut.begin();
         i != iend; ++i ){
      HepMC::GenParticle* p = *i;
      HepMC::GenVertex* VertNeut = p->end_vertex();

      //Vertk1qqq->print( std::cout );
      //Vertk2qqq->print( std::cout );
      debug()<<"Position of neut decay "<< Print( VertNeut->position() )
             << endmsg;

      energie.push_back( p->momentum().e()/GeV );
      pt.push_back( sqrt( pow(p->momentum().px(),2)+
                          pow(p->momentum().py(),2) )/GeV );
      eta.push_back( p->momentum().eta() );
      kz.push_back( VertNeut->point3d().z() );
      kr.push_back( VertNeut->point3d().perp() );
      flight.push_back( VertDistance( PV,VertNeut ) );
      debug() << "Dist of flight " << flight.back() << endmsg;

      //Find and study stable daughters  <--- work only for Kaplan
      int dgt1=0; dumV1 = HepMC::FourVector();
      for( HepMC::GenVertex::particles_out_const_iterator po =
             VertNeut->particles_out_const_begin();
           po !=  VertNeut->particles_out_const_end(); ++po){

        if( (*po)->pdg_id()==22 ){ continue;}
        if( ((*po)->pdg_id())== 92 ) { GetHepMCStable( p,dgt1, VertNeut );break; }
        HepMC::GenVertex::particles_out_const_iterator p1;

        for( p1 = (*po)->end_vertex()->particles_out_const_begin();
             p1 !=  (*po)->end_vertex()->particles_out_const_end(); ++p1){
          if( ((*p1)->pdg_id())== 92 ) {
            GetHepMCStable( (*p1),dgt1, VertNeut ); goto s; }
        }
      }
    s:
      debug() << "Neutralino mass "<< dumV1.m() <<" Nb of daughters : "<< dgt1;
      if ( dgt1 > m_nTracks ) { debug() <<" Selected";}
      debug() <<" "<< endmsg;
      plot( dgt1, "GenNeutNbofDgts", 0, 30 );
      plot( dumV1.m()/1000., "GenNeutPerfectMass", 0, 70 );
    }//end of loop on found neutralinos

    tuple->farray("GenNeutE",energie.begin(), energie.end(), "NGenNeut", 10 );
    tuple->farray("GenNeutPt",pt.begin(),pt.end(), "NGenNeut", 10 );
    tuple->farray("GenNeuteta", eta.begin(), eta.end(),"NGenNeut", 10 );
    tuple->farray("GenNeutz", kz.begin(), kz.end(),"NGenNeut", 10 );
    tuple->farray("GenNeutr", kr.begin(), kr.end(),"NGenNeut", 10 );
    //debug() << " R distance " << kr.back() << endmsg;
    //if ( kr.back < 42 ) m_kok1++;   //study if decays in VELO
    tuple->farray("GenNeutDistFlight", flight.begin(),
                  flight.end(),"NGenNeut", 10  );

    //loop on the found charginos
    if( m_HepMC=="AMSB" ){
      energie.clear();
      pt.clear();
      eta.clear();
      kz.clear();
      kr.clear();
      flight.clear();

      for( vector< HepMC::GenParticle* >::const_iterator i = charg.begin();
           i != charg.end(); ++i ){
        HepMC::GenParticle* p = *i;
        HepMC::GenVertex* VertCharg = p->end_vertex();
        debug()<<"Position of chargino decay "<< Print( VertCharg->position() )
               << endmsg;

        energie.push_back( p->momentum().e()/Gaudi::Units::GeV );
        pt.push_back( sqrt( pow(p->momentum().px(),2)+
                            pow(p->momentum().py(),2) )/Gaudi::Units::GeV );
        eta.push_back( p->momentum().eta() );
        kz.push_back( VertCharg->point3d().z() );
        kr.push_back( VertCharg->point3d().perp() );
        flight.push_back( VertDistance( PV,VertCharg ) );
        debug() << "Dist of flight " << flight.back() << endmsg;

        //Find and study stable daughters <--- work only for Kaplan
        //  int dgt1=0; dumV1 = HepMC::FourVector();
        //  for( HepMC::GenVertex::particles_out_const_iterator po =
        //         VertCharg->particles_out_const_begin();
        //       po !=  VertCharg->particles_out_const_end(); ++po){
        //    if( (*po)->pdg_id()==22 ){ continue;}
        //    if( ((*po)->pdg_id())== 92 ) { GetHepMCStable( p,dgt1, VertCharg );break; }
        //    HepMC::GenVertex::particles_out_const_iterator p1;
        //    for( p1 = (*po)->end_vertex()->particles_out_const_begin();
        //         p1 !=  (*po)->end_vertex()->particles_out_const_end(); ++p1){
        //      if( ((*p1)->pdg_id())== 92 ) {GetHepMCStable( (*p1),dgt1, VertCharg );goto t; }
        //    }
        //  }
        //       t:
        //  debug() << "Chargino mass "<< dumV1.m() <<" Nb of daughters : "<< dgt1;
        //  if ( dgt1 > m_nTracks ) { debug() <<" Selected";}
        //  debug() <<" "<< endmsg;
        //  plot( dgt1, "GenChargNbofDgts", 0, 30 );
        //  plot
        //   ( dumV1.m()/1000., "GenChargPerfectMass", 0, 70 );
      }//end of loop on found charginos
      tuple->farray("GenChargE",energie.begin(), energie.end(), "NGenCharg", 10 );
      tuple->farray("GenChargPt",pt.begin(),pt.end(), "NGenCharg", 10 );
      tuple->farray("GenChargeta", eta.begin(), eta.end(),"NGenCharg", 10 );
      tuple->farray("GenChargz", kz.begin(), kz.end(),"NGenCharg", 10 );
      tuple->farray("GenChargr", kr.begin(), kr.end(),"NGenCharg", 10 );
      //debug() << " R distance " << kr.back() << endmsg;
      tuple->farray("GenChargDistFlight", flight.begin(), flight.end(),
                    "NGenCharg", 10  );
    }//end of AMSB
  }// end of mSUGRA and AMSB

  int dgt=0;
  if (m_HepMC=="B"){  //Find a B meson
    dumV1 = HepMC::FourVector();
    HepMC::GenParticle* B = 0;
    HepMC::GenVertex* Bdec = 0;
    HepMC::GenVertex* Bcrea = 0;
    for( HepMC::GenEvent::particle_const_iterator p =
           theEvent->particles_begin();
         p!= theEvent->particles_end();++p){
      //debug() << "Particle ID "  << (*p)->pdg_id() << endmsg;
      if((*p)->pdg_id() >= 500 && (*p)->pdg_id() < 600 ){
        //debug() << "Found a B meson !"  << (*p)->pdg_id() << endmsg;
        B = (*p);
        if ( B->end_vertex() != NULL ){ Bdec = B->end_vertex();}else{continue;}
        if ( B->production_vertex() != NULL ) {Bcrea = B->production_vertex();}
        else { continue; }
        double dist = VertDistance( Bdec, Bcrea );
        //debug() << "Distance of flight " << dist << endmsg;
        if ( dist < m_DocaMax ) continue;
        plot( dist, "GenBDistFlight", 0.0, 500.);
        plot ( Bdec->point3d().z(), "GenBz", 0.0, 1000.);
        plot ( Bdec->point3d().perp(), "GenBr", 0.0, 10.);
        //Find and study stable daughter
        GetHepMCStable( B,dgt , Bdec );
        goto endB;
      }
    }
  endB:
    plot( dumV1.m()/1000, "GenBPerfectMass", 0, 70 );
    plot( dgt, "GenBNbofDgts", 0, 30 );
    debug() << "Nb of B daughters : " << dgt << endmsg;
  }

  if (m_HepMC=="D"){     //Find a D meson
    dumV1 = HepMC::FourVector();
    HepMC::GenParticle* D = 0;
    HepMC::GenVertex* Ddec = 0;
    HepMC::GenVertex* Dcrea = 0;
    dgt=0;
    for( HepMC::GenEvent::particle_const_iterator p= theEvent->particles_begin();
         p!= theEvent->particles_end();++p){
      //debug() << "Particle ID "  << (*p)->pdg_id() << endmsg;
      if((*p)->pdg_id() >= 400 && (*p)->pdg_id() < 500 ){
        //debug() << "Found a D meson !"  << (*p)->pdg_id() << endmsg;
        D = (*p);
        if ( D->end_vertex() != NULL ){ Ddec = D->end_vertex();}else{continue;}
        if ( D->production_vertex() != NULL ) {Dcrea = D->production_vertex();}
        else { continue; }
        double dist = VertDistance( Ddec, Dcrea );
        //debug() << "Distance of flight " << dist << endmsg;
        if ( dist <m_DocaMax ) continue;
        plot( dist, "GenDDistFlight", 0.0, 500.);
        plot ( Ddec->point3d().z(), "GenDz", 0.0, 1000.);
        plot ( Ddec->point3d().perp(), "GenDr", 0.0, 10.);
        //Find and study stable daughter
        GetHepMCStable( D,dgt , Ddec );
        goto endD;
      }
    }
  endD:
    plot( dumV1.m()/1000., "GenDPerfectMass", 0, 70 );
    plot( dgt, "GenDNbofDgts", 0, 30 );
    debug() << "Nb of D daughters : " << dgt << endmsg;
  }

  if (m_HepMC=="K"){     //Find a K meson
    dumV1 = HepMC::FourVector();
    HepMC::GenParticle* K = 0;
    HepMC::GenVertex* Kdec = 0;
    HepMC::GenVertex* Kcrea = 0;
    dgt=0;
    for( HepMC::GenEvent::particle_const_iterator p= theEvent->particles_begin();
         p!= theEvent->particles_end();++p){
      //debug() << "Particle ID "  << (*p)->pdg_id() << endmsg;
      if((*p)->pdg_id() >= 300 && (*p)->pdg_id() < 400 ){
        //debug() << "Found a K meson !"  << (*p)->pdg_id() << endmsg;
        K = (*p);
        if ( K->end_vertex() != NULL ){ Kdec = K->end_vertex();}else{continue;}
        if ( K->production_vertex() != NULL ) {Kcrea = K->production_vertex();}
        else { continue; }
        double dist = VertDistance( Kdec, Kcrea );
        //debug() << "Distance of flight " << dist << endmsg;
        if ( dist <m_DocaMax ) continue;
        plot( dist, "GenKDistFlight", 0.0, 500.);
        plot ( Kdec->point3d().z(), "GenKz", 0.0, 1000.);
        plot ( Kdec->point3d().perp(), "GenKr", 0.0, 10.);
        //Find and study stable daughter
        GetHepMCStable( K,dgt , Kdec );
        goto endK;
      }
    }
  endK:
    plot( dumV1.m()/1000., "GenKPerfectMass", 0, 70 );
    plot( dgt, "GenKNbofDgts", 0, 30 );
    debug() << "Nb of K daughters : " << dgt << endmsg;
  }

  if (m_HepMC=="HD"){
    HepMC::GenParticle* K = 0;
    HepMC::GenVertex* Kdec = 0;
    HepMC::GenVertex* Kcrea = 0;
    for( HepMC::GenEvent::particle_const_iterator p= theEvent->particles_begin();
         p!= theEvent->particles_end();++p){
      if((*p)->pdg_id() == 36 && (*p)->status() == 2 ){
        K = (*p);
        if ( K->end_vertex() != NULL ){ Kdec = K->end_vertex();}else{continue;}
        if ( K->production_vertex() != NULL ) {Kcrea = K->production_vertex();}
        else { continue; }
        double dist = VertDistance( Kdec, Kcrea );
        debug() << "V-Pions position of decay "<< Print( Kdec->position() )
                << "Distance of flight " << dist << endmsg;
      }
    }
  }

  tuple->write();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Find all charged stable daughters of a particle
//=============================================================================

void MCDisplVertices::GetHepMCStable(HepMC::GenParticle* p, int & dgt, HepMC::ThreeVector* dec ) {

  //int stable = p->status();
  ParticleID id = ParticleID( p->pdg_id() );
  //ParticleProperty* test = ppSvc()->findByStdHepID( id );
  //double charge = test->charge();
  double charge = id.threeCharge();
  //debug() << "ID " << p->pdg_id() << " Charge " << charge << endmsg;

  if ( (p->end_vertex()) != NULL ){
    HepMC::GenVertex* vertex = p->end_vertex();
    //loop on the daughters
    HepMC::GenVertex::particles_out_const_iterator it;
    for ( it = vertex->particles_out_const_begin();
          it != vertex->particles_out_const_end(); ++it ){
      GetHepMCStable( (*it), dgt, dec );
    }
  } else if ( (p->production_vertex()) != NULL ) {
    if ( charge == 0 ) goto t;
    if ( p->momentum().eta() < 1.8 || p->momentum().eta() > 4.9 ) goto t;
    double dist = VertDistance( dec, p->production_vertex() );
    //debug() << "Distance " << dist << endmsg;
    plot( dist, "GenDgtsSepfromKhi", 0.0, 1000. );
    if ( dist < m_DocaMax ) {
      dumV1.set( dumV1.x()+(p->momentum()).x(), dumV1.y()+(p->momentum()).y(),
                 dumV1.z()+(p->momentum()).z(), dumV1.e()+(p->momentum()).e());
      double pT = sqrt( pow(p->momentum().x(),2) + pow(p->momentum().y(),2) );
      if (m_HepMC=="Khi") plot( pT,"GenKhiDaughpT",0.0,20.*Gaudi::Units::GeV);
      if (m_HepMC=="B") plot( pT,"GenBDaughpT",0.0, 20.*Gaudi::Units::GeV);
      if (m_HepMC=="D") plot( pT,"GenDDaughpT",0.0, 20.*Gaudi::Units::GeV);
      if (m_HepMC=="K") plot( pT,"GenKDaughpT",0.0, 20.*Gaudi::Units::GeV);
      ++dgt;
      //       debug() << "Nb of daughters yet " << dgt
      //        << " momentum " <<  p->momentum()
      //        << " pT " << pT << endmsg;
    }
  }
 t:
  return;
}

void MCDisplVertices::GetHepMCStable(HepMC::GenParticle* p, int & dgt,
                                     HepMC::GenVertex* dec ) {

  //int stable = p->status();
  ParticleID id = ParticleID( p->pdg_id() );
  double charge = id.threeCharge();
  //debug() << "ID " << p->pdg_id() << " Charge " << charge << endmsg;

  if ( (p->end_vertex()) != NULL ){
    HepMC::GenVertex* vertex = p->end_vertex();
    //loop on the daughters
    HepMC::GenVertex::particles_out_const_iterator it;
    for ( it = vertex->particles_out_const_begin();
          it != vertex->particles_out_const_end(); ++it ){
      GetHepMCStable( (*it), dgt, dec );
    }
  } else if ( (p->production_vertex()) != NULL ) {
    if ( charge == 0 ) goto t;
    if ( p->momentum().eta() < 1.8 || p->momentum().eta() > 4.9 ) goto t;
    double dist = VertDistance( p->production_vertex(), dec );
    //debug() << "Distance " << dist << endmsg;
    plot( dist, "GenDgtsSepfromKhi", 0.0, 1000. );
    if ( dist < m_DocaMax ) {
      dumV1.set( dumV1.x()+(p->momentum()).x(), dumV1.y()+(p->momentum()).y(),
                 dumV1.z()+(p->momentum()).z(), dumV1.e()+(p->momentum()).e());
      double pT = sqrt( pow(p->momentum().x(),2) + pow(p->momentum().y(),2) );
      if (m_HepMC=="Khi") plot( pT,"GenKhiDaughpT",0.0,20.*Gaudi::Units::GeV);
      if (m_HepMC=="B") plot( pT,"GenBDaughpT",0.0, 20.*Gaudi::Units::GeV);
      if (m_HepMC=="D") plot( pT,"GenDDaughpT",0.0, 20.*Gaudi::Units::GeV);
      if (m_HepMC=="K") plot( pT,"GenKDaughpT",0.0, 20.*Gaudi::Units::GeV);
      ++dgt;
      //       debug() << "Nb of daughters yet " << dgt
      //        << " momentum " <<  p->momentum()
      //        << " pT " << pT << endmsg;
    }
  }
 t:
  return;
}

//=============================================================================
// Return decay vertices as a string
//=============================================================================
string MCDisplVertices::Print( HepMC::ThreeVector v ){
  stringstream kss;
  kss<<" ( "<< v.x() <<", "<< v.y() <<", "<< v.z() <<" ) ";
  return kss.str();
}

string MCDisplVertices::Print( HepMC::FourVector v ){
  stringstream kss;
  kss<<" ( "<< v.x() <<", "<< v.y() <<", "<< v.z() <<", "<< v.t() <<" ) ";
  return kss.str();
}

//=============================================================================
//  MC Infos
//=============================================================================
StatusCode MCDisplVertices::GetMCInfos() {

  //Get the MC Particles
  MCParticles* mcparts = get<MCParticles>(MCParticleLocation::Default );
  if( !mcparts ){
    warning() << "Unable to find MC particles at '"
              << MCParticleLocation::Default << "'" << endmsg;
    return StatusCode::FAILURE;
  }
  //debug() << "There are " <<  mcparts->size() << " MC particles" << endmsg;
  //Save all stable MC Part in Tuple for Calo Analysis
  //SaveMCPartinTuple( mcparts );

  //Find the preys and save infos in tuple
  MCPV = NULL; m_MCPos.clear();
  MCParticle::Vector::iterator iend = mcparts->end();
  for( MCParticle::Vector::iterator i = mcparts->begin(); i != iend; ++i ){
    if( (*i)->particleID().abspid() == m_PreyID.abspid() ){
      if( (*i)->endVertices().size() < 1 ) continue;
      if( (*i)->mother() != NULL ){
        if( (*i)->mother()->particleID().abspid() == m_MotherPreyID.abspid() )
          m_IsPreyFromMother = true;
      }
      //PV is set to be the Prey parent.
      if( MCPV == NULL ) MCPV = (*i)->primaryVertex ();
      SmartRefVector< LHCb::MCVertex >::const_iterator vtx =
        (*i)->endVertices().begin();
      Gaudi::XYZPoint pos = (*vtx)->position();
      m_MCPos.push_back( pos );
      if( msgLevel( MSG::DEBUG ) ){
        debug()<< "MC "<< m_Prey <<" mass "
               << (*i)->virtualMass()/GeV
               <<" GeV, pos of end vtx " << pos << endmsg;
      }
      if(true) GetMCStable( static_cast<const MCVertex*>(*vtx), "MCPrey" );
      if(false) GetMCStable( static_cast<const MCVertex*>(*vtx) );
    }
  }
  //Find the Kshort and save infos in tuple
  if(false){
    for( MCParticle::Vector::iterator i = mcparts->begin();
         i != iend; ++i ){
      if( (*i)->particleID().abspid() == 310 ){
        double eta = (*i)->momentum().eta();
        if( eta < 1.8 || eta > 4.9 ) continue;
        if( (*i)->endVertices().size() < 1 ) continue;
        SmartRefVector< LHCb::MCVertex >::const_iterator vtx =
          (*i)->endVertices().begin();
        Gaudi::XYZPoint pos = (*vtx)->position();
        m_MCPos.push_back( pos );
        debug()<< "MC K_S mass " << (*i)->virtualMass()/GeV
               <<" GeV, pos of end vtx " << pos << endmsg;
        if(true) GetMCStable( static_cast<const MCVertex*>(*vtx) );
      }
    }
  }

  //Study nboftracks,pt, mass and IP to PV of PV tracks
  if( msgLevel(MSG::DEBUG) )
    debug()<<"MC Primary Vertex : " << MCPV->position() << endmsg;
  if(false) GetMCStable( MCPV, "MCPV" );

  // get all Monte Carlo vertices from TES and save SV:
  const MCVertex::Container* mcvertices = get<MCVertex::Container>
    ( MCVertexLocation::Default ) ;
  int size = 0;//= mcvertices->size();
  //debug()<<"There are "<< size << " MC vertices." << endmsg;
  // select all Monte Carlo  Primary Vertices:

  m_MCSVPos.clear();
  MCVertex::Container::const_iterator imcend =  mcvertices->end();
  for ( MCVertex::Container::const_iterator imc = mcvertices->begin() ;
        imcend != imc ; ++imc )
  {
    if (  (*imc)->isPrimary() && (*imc) != MCPV ) {
      debug() << "MC Secondary interaction  Vertex : "
              << (*imc)->position() << endmsg;
      m_MCSVPos.push_back( (*imc)->position() );
      ++size;
    } else {
      //debug() << "Non-PV         : " << (*imc)->position() << endmsg;
    }
  }
  if( msgLevel( MSG::DEBUG ) )
    debug()<<"There are "<< size + 1<< "MC PV vertices." << endmsg;


  return StatusCode::SUCCESS;
}
//============================================================================
// Get purity and number of associated tracks of prey
//============================================================================
bool MCDisplVertices::GetMCPrey( const Particle * p ){

  //Compute "purity" of the prey. Purity is defined as the number of
  //associated tracks from a prey over the total number of
  //associated tracks.

  //loop on the daughters to look for MC associated particle.
  float nbassfromaPrey = 0.;
  float nbasstot = 0.;
  float purity = 200.;
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin(); i!= iend; ++i ){
    const MCParticle *MCpart = m_pLinker->firstMCP( (*i) );
    if(MCpart != NULL ){
      //debug() << "Particle " << (*i)->key() << " "
      //<< (*i)->particleID().pid()
      //<< " associated to MC part " << MCpart->key() << " "
      //<< MCpart->particleID().pid() << endmsg;
      ++nbasstot;
      if( IsitFromaPrey( MCpart ) ) ++nbassfromaPrey;
    }
  }

  if( nbasstot == 0 ) ++nbasstot;

  //Compute purity
  purity =  nbassfromaPrey/nbasstot;

  //if only interessted in the prey, uncomment foll. lines
  //if( purity < m_minmixed) return StatusCode::SUCCESS;
  counter("purity") += purity;
  counter("NbassfromaPrey") += nbassfromaPrey;
  if( purity < m_maxunpure ){ ++counter("p3"); } else { counter("p3") += 0; }
  if( purity > m_minmixed && purity < m_maxmixed ){ ++counter("p2"); }
  else { counter("p2") += 0; }
  if( purity > m_maxmixed ){ ++counter("p1"); } else { counter("p1") += 0; }

  m_purities.push_back( purity );
  if( msgLevel( MSG::DEBUG ) )
    debug()<< m_Prey <<" purity " << purity
           <<" Nb of associated trks "<< nbassfromaPrey <<endmsg;

  //if( purity < m_maxunpure ) {
  //Study the composition
  //StudyPreyComposition( p, "Unpure" );
  //}

  //if( purity > m_maxmixed ){
  //plot( nbassfromaPrey, "PreyNbofTrueMCTrk", 0, 20 );
  //plot( p->measuredMass()/1000., "PreyMass", 0., 80. );

  //Study the composition
  //StudyPreyComposition( p, "Pure" );

  //Resolution of the prey reconstructed vertex
  //if( p->endVertex()->position().z() > 200. )
  //Resolution( p->endVertex()->position() );

  //Study of Hlt efficiency
  //StudyHltEfficiency( p->endVertex()->position() );

  //Study impact param to PV and some properties
  //StudyPreyIP( p, PV );

  //Study daughters IP to PV :
  //StudyDgtsIP( p );

  //Study dispersion of Prey tracks :
  //StudyDispersion( p );
  //}

  if( m_PurityMin < 1.01 && purity < m_PurityMin ){
    return false;
  } else return true;
  return true;
}

//============================================================================
// Find all stable charged daughters in acceptance
//============================================================================
void MCDisplVertices::GetMCStable( const MCParticle * p, unsigned int & dgt ){

  if( p->endVertices().size() < 1 ) return;

  SmartRefVector< MCVertex >::const_iterator vtx =
    p->endVertices().begin();

  const Gaudi::XYZPoint pos = vtx->target()->position();

  SmartRefVector<LHCb::MCParticle>::const_iterator iend =
    (*vtx)->products().end();
  for( SmartRefVector<LHCb::MCParticle>::const_iterator i =
         (*vtx)->products().begin(); i != iend; ++i ){
    GetMCStable( i->target(), pos, dgt );
  }
}

void MCDisplVertices::GetMCStable( const MCParticle * p,
                                   const Gaudi::XYZPoint & pos,
                                   unsigned int & dgt ){

  //Compare production vtx with pos
  double dist = VertDistance( pos, p->originVertex()->position() );
  //debug()<< "Distance " << dist << endmsg;
  if( dist > m_DocaMax ) return;

  //I found a MCParticle with no end MCVertex...
  if( p->endVertices().size() < 1 ) return;

  SmartRefVector<LHCb::MCVertex>::const_iterator v = p->endVertices().begin();
  //Only need to take the first decay vertex.
  //Compute time of flight

  dist = VertDistance( p->originVertex()->position(), (*v)->position() );

  if( dist < m_DocaMax ){ //part considered unstable
    //loop on the daughters
    SmartRefVector<LHCb::MCParticle>::const_iterator iend =
      (*v)->products().end();
    for( SmartRefVector<LHCb::MCParticle>::const_iterator i =
           (*v)->products().begin(); i != iend; ++i ){
      GetMCStable( i->target(), pos, dgt );
    }
  }

  if( dist > 1000. ) { //Particle is considered stable.
    //Continue if neutral or out of acceptance
    if( p->particleID().threeCharge() == 0 ) return;
    if( p->momentum().eta() < 1.8 || p->momentum().eta() > 4.9 ) return;
    //dumv = dumv + (p->momentum());
    ++dgt;
  }

  return;
}

void MCDisplVertices::GetMCStable( const LHCb::MCVertex * vtx , int & nb,
                                   Gaudi::LorentzVector & mass ){

  info()<<"This function is not to be used"<< endmsg;

  for( SmartRefVector<LHCb::MCParticle>::const_iterator p =
         vtx->products().begin(); p != vtx->products().end(); ++p ){
    if( (*p)->momentum().eta() < 1.8 ||
        (*p)->momentum().eta() > 4.9 ) continue;
    if( (*p)->particleID().threeCharge() == 0 ) continue;
    SmartRefVector<LHCb::MCVertex>::const_iterator v =
      (*p)->endVertices().begin();
    double dist = VertDistance( (*p)->originVertex()->position(),
                                (*v)->position() );
    if( dist < 100. ) continue;
    //Particle considered stable
    mass =+ (*p)->momentum();
    ++nb;
  }
}

void  MCDisplVertices::GetMCStable( const MCVertex* V, string type ){

  MCParticle::ConstVector daughters;
  SmartRefVector<MCParticle>::const_iterator iend = V->products().end();
  for( SmartRefVector<MCParticle>::const_iterator i =
         V->products().begin(); i != iend; ++i ){
    GetMCStable( static_cast<const MCParticle*>(*i), V->position(),daughters );
  }

  int dgt = daughters.size();
  double mass = GetMCMass( daughters );
  bool recoble = false;
  if( dgt >= m_nTracks && mass > m_PreyMinMass &&
      GetR( V->position() ) > m_RMin ) recoble = true ;

  if( msgLevel( MSG::DEBUG ) )
    debug() << type+" mass " << mass <<" GeV, nb of dgts "
            << dgt <<" reconstructible ? "<< recoble << endmsg;

  if(type=="MCPrey"){
    if( recoble ){ ++counter("RecbleMCPrey"); }
    else { counter("RecbleMCPrey") += 0; }


    plot( dgt, type+"NbofTracks", 0, 30);
    plot( mass, type+"Mass", 0, 70);
  }
  if(type=="MCPV"){
    plot( dgt, type+"NbofTracks", 0, 70);
    plot( mass, type+"Mass", 0, 120);
  }

  //Loop on daughters and do some analysis
  if( true ) return;

  for( MCParticle::ConstVector::const_iterator p = daughters.begin();
       p != daughters.end(); ++p ){
    plot( ((*p)->pt())/1000., type+"DgtpT", 0., 20 );
    if(type == "MCPrey"){
      double imp = ImpactParam( *p, NULL );
      //double imp = ImpactParam( p, V );
      debug() << "Impact parameter " << imp << endmsg;
      plot( imp, type+"DgtIPto0", 0., 100.);
      //plot( imp, type+"DgtIPtoPV", 0., 15.);
    }
    if(type == "MCPV"){
      double imp = ImpactParam( *p, NULL );
      debug() << "Impact parameter " << imp << endmsg;
      plot( imp, type+"DgtIPto0", 0., 100.);
    }
    //       debug() << "Found a Daughter " << p->particleID().pid()
    //        <<" Prod vtx "<< p->originVertex()->position()
    //        <<" Dec vtx "<< (*v)->position() << endmsg;
  }
  return;
}

void  MCDisplVertices::GetMCStable( const MCVertex* V ){

  //Loop on daughters and find stable daughters
  MCParticle::ConstVector daughters;
  Tuple tuple = nTuple("MCPrey");
  SmartRefVector<MCParticle>::const_iterator iend = V->products().end();
  for( SmartRefVector<MCParticle>::const_iterator i =
         V->products().begin(); i != iend; ++i ){
    GetMCStable( static_cast<const MCParticle*>(*i), V->position(),daughters );
  }

  //*********Study if track can be reconstructed in Velo****************
  //   MCTrackInfo info = MCTrackInfo( evtSvc(), msgSvc() );
  //   //Loop on stable daughter particles
  //   int nbrecpart = 0;
  //   bool IsRec;
  //   for( MCParticle::ConstVector::const_iterator i = daughters.begin();
  //        i != daughters.end(); ++i ){
  //     IsRec = info.hasVelo( *i );
  //     if( IsRec ) nbrecpart++; // 3 r and phi
  //     unsigned int nVeloR =  info.nbVeloR( *i );
  //     unsigned int nVeloPhi =  info.nbVeloPhi( *i );
  //     plot( IsRec, "IsRcnstrctble", 0, 2);
  //     plot( nVeloR, "NbVeloHitR", 0., 15 );
  //     plot( nVeloPhi, "NbVeloHitPhi", 0., 15 );
  //   }
  //   //Is the particle reconstructible ?
  //   IsRec = false;
  //   if( nbrecpart >= m_nTracks ) IsRec = true;
  //   plot( IsRec, "IsPreyRcnstrctble", 0, 2);
  //   debug()<<" Is Particle " << V->mother()->particleID().abspid()
  //   << " reconstructible in VELO ? " << IsRec << endmsg;

  //*********Study of data linked daughters*******
  //Loop on stable daughter particles
  MCTrackInfo info = MCTrackInfo( evtSvc(), msgSvc() );
  const Particle::ConstVector preys = this->i_particles();

  //Create direct linking
  Particle::ConstVector::const_iterator pend = preys.end();
  for( Particle::ConstVector::const_iterator i = preys.begin();
       i!= pend; ++i ){

    const LHCb::MCParticle *MCpart = m_pLinker->firstMCP( *i );
    if( NULL!=MCpart && false ) debug() << "Particle " << (*i)->key()
                                        << " associated to MC part "
                                        << MCpart->key() << endmsg;
  }

  Vertex Zero = Vertex( Gaudi::XYZPoint(0., 0., 0.) );
  vector<double> IP0, ZMin, Pt, IPtoZ;
  vector<int> TkType;
  Particle::ConstVector Down;
  Particle::ConstVector Long;
  int nVeloRecAss = 0, nVeloRecTot = 0, nDownAss = 0, nVeloRecNonAss = 0;
  MCParticle::ConstVector::const_iterator dend = daughters.end();
  for( MCParticle::ConstVector::const_iterator i = daughters.begin();
       i != dend; ++i ){

    //Uncomment if wanna work only with pions
    //if( (*i)->particleID().abspid() != 211 ) continue;

    //Is potentially reconstructed in Velo ?
    bool IsRec = info.hasVelo( *i );
    if( IsRec ) ++nVeloRecTot;

    //Get pt related to the Prey
    //double pt = GetPt( (*i)->momentum(), V->mother()->momentum() )/1000;
    //debug()<<"Pt of daughter rel. to mother "<<  pt << endmsg;
    //plot( pt, "MCPreyDgtpT", 0., 10 );

    //Find related Track
    const Particle *p = m_pLinker->firstP( *i );
    if( p == NULL ) {
      if( IsRec ) ++nVeloRecNonAss;
      continue;
    }
    //debug()<<"MCParticle "<< (*i)->key() << " associated to Particle "
    //   << p->key() << endmsg;

    //Save track with descending pt
    //     pt = (p->pt())/1000.;
    //     for( vector<double>::iterator j = Pt.begin(); j != Pt.end(); j++ ){
    //       if( pt > *j ){
    //  Pt.insert( j, pt );
    //  goto end;
    //       }
    //     }
    //     Pt.push_back( pt );
    //  end:;

    //Save first z measurement
    if( p->proto()->track() == NULL ) continue;
    const Track * tk = p->proto()->track();
    //     StatusCode sc = m_measProvider -> load( const_cast<Track &>( *tk ) );
    //     //if(!sc) info()<<"Unable to recreate track measurements"<< endmsg;
    //     double z = tk->measurements().front()->z();
    //     ZMin.push_back( z );

    //     //Save impact parameter to the zero point
    //     double imp, chi2;
    //     distanceCalculator()->distance( p, &Zero, imp, chi2);
    //     IP0.push_back( imp );

    //Save downstream tracks ( Particle ) for further studies
    if( tk->checkType(Track::Downstream) ){
      Down.push_back( p );
      TkType.push_back( 2 );
      if( !IsRec ) ++nDownAss;
    }
    if( tk->checkType(Track::Long) ) {
      Long.push_back( p );
      TkType.push_back( 1 );
      if( IsRec ) ++nVeloRecAss;
    }
  }
  //debug()<<"Nb of reconstructed tracks "<< Pt.size() << endmsg;
  if( false ){
    fillHeader( tuple );
    const int max = 200;
    tuple->column( "Z", V->position().z() );
    tuple->column( "R", GetR(V->position()) );
    tuple->farray( "ZMin", ZMin.begin(), ZMin.end(), "NTk", max );
    tuple->farray( "IP0", IP0.begin(), IP0.end(), "NTk", max );
    tuple->farray( "Pt", Pt.begin(), Pt.end(), "NTk", max );
    tuple->farray( "TkType", TkType.begin(), TkType.end(), "NTk", max );
    tuple->column( "NMCDgts", daughters.size() );
    tuple->column( "NVRTot", nVeloRecTot );
    tuple->column( "NVRAss", nVeloRecAss );
    tuple->column( "NDownAss", nDownAss );
    tuple->column( "NVRNonAss", nVeloRecNonAss );
    tuple->write();
  }

  //Study the downstream tracks
  debug()<<" ------------ Downstream tracks --------------"<< endmsg;
  double doca, chi2;

  debug()<<"Track study-------------------------"<< endmsg;
  debug()<<"Nb of downstream tracks "<< Down.size()<< endmsg;
  for( Particle::ConstVector::const_iterator i = Down.begin(); i != Down.end();
       ++i ){
    Particle::ConstVector::const_iterator j = i; ++j;
    //double ip = ImpactParam( (*i)->referencePoint (), (*i)->momentum(),
    //       V->position() );
    //debug()<<"IP "<< ip << endmsg;
    //plot( ip, "DownIP", 0., 15. );

    for( ; j != Down.end(); ++j ){
      debug()<<", Trying seed tracks "<< (*i)->key() <<" "
             << (*j)->key() << endmsg;
      distanceCalculator()->distance( *i, *j, doca, chi2);
      debug()<<"Doca "<< doca;
      plot( doca, "DownDoca", 0., 10. );
      //Fit tracks to see if possible to create the prey
      Vertex vtx1;
      vertexFitter()->fit( vtx1, *(*i), *(*j) );
      debug()<<" Chi2 "<< vtx1.chi2PerDoF() << endmsg;
      //std::vector< const LHCb::Track * > temp;
      //temp.push_back((*i)->proto()->track());
      //temp.push_back((*j)->proto()->track());
      //RecVertex * vtx2 = m_tFit->fit( temp );
      //double dist1 = VertDistance( vtx1.position(), V->position() );
      //double dist2 = VertDistance( vtx2->position(), V->position() );
      //debug() <<", (Default) Position "<< vtx1.position()
      //      <<" and Resolution "<< dist1 << endmsg;
      //<<", (Wouter) Position "<< vtx2->position()
      //<<" and Resolution "<< dist2 << endmsg;
      //plot( dist1, "DownDistD", 0., 50. );
      //plot( dist2, "DownDistW", 0., 50. );

    }
  }
  //Update downstream particles to have a state at the MC decay vtx
  //   VecPart UpdatedDown;
  //   for( Particle::ConstVector::const_iterator i = Down.begin(); i != Down.end();
  //        ++i ){
  //     //Extrapolate the track
  //     ParticleID & PID = const_cast<ParticleID &>( (*i)->particleID() );
  //     Track * tki = const_cast<Track *>( (*i)->proto()->track() );
  //     double z = V->position().z();
  //     State & ClosestState = const_cast<State &>( tki->closestState( z ) );
  //     debug()<<"MC neut decay vertex  " << z << endmsg;
  //     debug()<<"Closest state position " << ClosestState.z() << endmsg;
  //     if( !(m_extra->propagate( ClosestState, z, PID ))){
  //       warning()<<"Propagation failed !"<<endmsg; continue;}
  //     debug()<<"Extrapolated state position " << ClosestState.z() << endmsg;

  //     //Compute impact parameters to MC neut decay vtx
  //     double ip = ImpactParam( ClosestState.position(), ClosestState.momentum(),
  //         V->position() );
  //     debug()<<"IP extra "<< ip << endmsg;
  //     plot( ip, "DownIPExtra", 0., 15. );

  //     //Refit track
  //     m_trackPreFit->fit( *tki, PID );
  //     m_trackFit->fit( *tki, PID );
  //     debug()<<"After refit track z "<< tki->firstState( ).z() << endmsg;

  //     //Update the particle
  //     const LHCb::State& state = tki->stateAt(State::ClosestToBeam);
  //     Particle part = **i;
  //     m_pToState->state2Particle(state, part );

  //     const Track * dum = part.proto()->track();
  //     z = dum->firstState().z();

  //     debug()<<"After Particle update z "<< z << endmsg;
  //     UpdatedDown.push_back( part );
  //   }

  //   debug()<<"Updated track study-------------------------"<< endmsg;
  //   for( VecPart::iterator i = UpdatedDown.begin(); i != UpdatedDown.end();
  //        ++i ){
  //     VecPart::iterator j = i; j++;
  //     for( ; j != UpdatedDown.end(); j++ ){
  //       debug()<<", Trying seed tracks "<< (*i).key() <<" "
  //       << (*j).key() << endmsg;
  //       distanceCalculator()->distance( &(*i), &(*j), doca, chi2);
  //       debug()<<"UpDoca "<< doca;
  //       plot( doca, "UpDownDoca", 0., 10. );
  //       //Fit tracks to see if possible to create the prey
  //       Vertex vtx1;
  //       vertexFitter()->fit( (*i), (*j), vtx1);
  //       std::vector< const LHCb::Track * > temp;
  //       temp.push_back((*i).proto()->track());
  //       temp.push_back((*j).proto()->track());
  //       RecVertex * vtx2 = m_tFit->fit( temp );
  //       double dist1 = VertDistance( vtx1.position(), V->position() );
  //       double dist2 = VertDistance( vtx2->position(), V->position() );
  //       debug() <<", (Default) Position "<< vtx1.position()
  //        <<" and Resolution "<< dist1 //<< endmsg;
  //        <<", (Wouter) Position "<< vtx2->position()
  //        <<" and Resolution "<< dist2 << endmsg;
  //       plot( dist1, "UpDownDistD", 0., 50. );
  //       plot( dist2, "UpDownDistW", 0., 50. );

  //     }
  //   }

  //   debug()<<" ------------ Long tracks --------------"<< Long.size()<< endmsg;
  //   for( Particle::ConstVector::const_iterator i = Long.begin(); i != Long.end();
  //        ++i ){
  //     Particle::ConstVector::const_iterator j = i; j++;
  //     double ip = ImpactParam( (*i)->referencePoint (), (*i)->momentum(),
  //         V->position() );
  //     debug()<<"IP "<< ip << endmsg;
  //     plot( ip, "LongIP", 0., 1. );
  //     for( ; j != Long.end();
  //        j++ ){
  //       geomDispCalculator()->calcCloseAppr(*(*i), *(*j), doca, docae);
  //       debug()<<"Doca "<< doca;
  //       plot( doca, "Longdoca", 0., 1. );
  //       //Fit tracks to see if possible to create the prey
  //       Vertex vtx;
  //       vertexFitter()->fit(*(*i),*(*j), vtx);
  //       double dist = VertDistance( vtx.position(), V->position() );
  //       debug() <<", Resolution "<< dist << endmsg;
  //       plot( dist, "LongDist", 0., 10. );
  //     }
  //   }

  //   int dum = 0;
  //   if( Down.size() == 0 && Long.size() == 0 ) dum = 1;
  //   double ratio = Long.size()/(Long.size()+Down.size()+dum);
  //   debug() <<"% of long in long + Down : "<< ratio << endmsg;
  //   plot( ratio, "Ratio", 0., 1.2 );

  return;
}


void  MCDisplVertices::GetMCStable( const MCParticle * p,
                                    const Gaudi::XYZPoint & pos,
                                    MCParticle::ConstVector & daughters ){

  //Compare production vtx with pos
  double dist = VertDistance( pos, p->originVertex()->position() );
  if( dist > m_DocaMax ) return;

  if( p->endVertices().size() < 1 ){
    //Stable MC Particle may have no end vertex
    //Continue if neutral or out of acceptance
    if( p->particleID().threeCharge() == 0 ) return;
    if( p->momentum().eta() < 1.8 || p->momentum().eta() > 4.9 ) return;
    daughters.push_back( p );
    //    debug() << "Found a Daughter " << p->particleID().pid()
    //        <<" Prod vtx "<< p->originVertex()->position()
    //        <<" Dec vtx "<< (*v)->position() << endmsg;
    return;
  }

  SmartRefVector<LHCb::MCVertex>::const_iterator v = p->endVertices().begin();
  //Only need to take the first decay vertex.
  //Compute time of flight
  dist = VertDistance( p->originVertex()->position(), (*v)->position() );

  if( dist < m_DocaMax ){ //part considered unstable
    //loop on the daughters
    SmartRefVector<LHCb::MCParticle>::const_iterator iend =
      (*v)->products().end();
    for( SmartRefVector<LHCb::MCParticle>::const_iterator i =
           (*v)->products().begin(); i != iend; ++i ){
      GetMCStable( (*i), pos, daughters );
    }
  }

  if( dist > 1000. ) { //Particle is considered stable.
    //Continue if neutral or out of acceptance
    if( p->particleID().threeCharge() == 0 ) return;
    if( p->momentum().eta() < 1.8 || p->momentum().eta() > 4.9 ) return;
    daughters.push_back( p );
    //    debug() << "Found a Daughter " << p->particleID().pid()
    //        <<" Prod vtx "<< p->originVertex()->position()
    //        <<" Dec vtx "<< (*v)->position() << endmsg;
  }

  return;
}

//============================================================================
// Save infos of MCParticles in a Tuple
//============================================================================
void MCDisplVertices::SaveMCPartinTuple( LHCb::MCParticles * mcparts){

  Tuple tuple = nTuple("MCParticles");
  vector<double> E, eta, phi, keta, kphi;
  vector<int> fromaneut;
  bool tosave = false;
  if( !fillHeader(tuple) )
    info() << "Could not write Header in MCParticles !" << endmsg;

  for( MCParticle::Vector::iterator p = mcparts->begin(); p != mcparts->end();
       ++p ){
    tosave = false;

    //Save infos about the prey
    if( (*p)->particleID().abspid() == m_PreyID.abspid() ){
      keta.push_back( (*p)->momentum().eta() );
      kphi.push_back( (*p)->momentum().phi() );
      continue;
    }

    //Forget the neutrino
    if( IsNeutrino( *p ) ) continue;
    double peta = (*p)->momentum().eta();
    if( peta < 1.8 || peta > 4.9 )continue;

    if( (*p)->endVertices().size() != 1 ) continue;
    for ( SmartRefVector<LHCb::MCVertex>::const_iterator v =
            (*p)->endVertices().begin(); v != (*p)->endVertices().end(); ++v ){
      if( (*v)->products().size() == 0 ) tosave = true;
      if( (*v)->position().z() < 1000. ) tosave = false;
    }
    if(!tosave) continue;
    //Particle is considered stable :
    E.push_back( (*p)->momentum().e() );
    eta.push_back( peta );
    phi.push_back( (*p)->momentum().phi() );
    fromaneut.push_back( IsitFromaPrey( (*p) ) );

  }
  const int maxSize = 5000;
  tuple->farray( "FromaPrey", fromaneut.begin(),fromaneut.end(),"Nb",maxSize );
  tuple->farray( "Eta", eta.begin(), eta.end(), "Nb", maxSize  );
  tuple->farray( "Phi", phi.begin(), phi.end(), "Nb", maxSize );
  tuple->farray( "E", E.begin(), E.end(), "Nb", maxSize );
  tuple->farray( "PreyEta", keta.begin(), keta.end(), "PreyNb", maxSize  );
  tuple->farray( "PreyPhi", kphi.begin(), kphi.end(), "PreyNb", maxSize );

  if( !(tuple->write()) ) info() << "Could not save MCParticles !" << endmsg;

}

//============================================================================
// Save infos of GenParticles in a Tuple
//============================================================================
void MCDisplVertices::SaveGenPartinTuple( const HepMC::GenEvent* evt ){

  Tuple tuple = nTuple("MCParticles");
  vector<double> E, eta, phi, keta, kphi, qeta, qphi;
  vector<int> fromaneut;
  if( !fillHeader(tuple) )
    info() << "Could not write Header in MCParticles !" << endmsg;

  for( HepMC::GenEvent::particle_const_iterator p= evt->particles_begin();
       p!= evt->particles_end();++p){

    //Save infos about the Prey
    unsigned int pid = abs( (*p)->pdg_id() );
    if( pid == m_PreyID.abspid() ){
      keta.push_back( (*p)->momentum().eta() );
      kphi.push_back( (*p)->momentum().phi() );

      //Save daughter quarks
      HepMC::GenVertex* vtx = (*p)->end_vertex();
      //vtx->print( std::cout );
      HepMC::GenVertex::particles_out_const_iterator po;
      for( po = vtx->particles_out_const_begin();
           po != vtx->particles_out_const_end(); ++po){
        if( !IsQuark( *po ) ) continue;
        qeta.push_back( (*po)->momentum().eta() );
        qphi.push_back( (*po)->momentum().phi() );
      }

      continue;
    }

    //Forget the neutrino
    if( IsNeutrino( *p ) ) continue;
    double peta = (*p)->momentum().eta();
    if( peta < 1.8 || peta > 4.9 )continue;

    //Keep only stable particles
    if( (*p)->status() != 1 ) if( (*p)->status() != 999 ) continue;

    E.push_back( (*p)->momentum().e() );
    eta.push_back( peta );
    phi.push_back( (*p)->momentum().phi() );
    fromaneut.push_back( IsitFromaPrey( (*p) ) );

  }
  const int maxSize = 5000;
  tuple->farray( "FromaNeut", fromaneut.begin(),fromaneut.end(),"Nb",maxSize );
  tuple->farray( "Eta", eta.begin(), eta.end(), "Nb", maxSize  );
  tuple->farray( "Phi", phi.begin(), phi.end(), "Nb", maxSize );
  tuple->farray( "E", E.begin(), E.end(), "Nb", maxSize );
  tuple->farray( "NeutEta", keta.begin(), keta.end(), "NeutNb", maxSize  );
  tuple->farray( "NeutPhi", kphi.begin(), kphi.end(), "NeutNb", maxSize );
  tuple->farray( "QEta", qeta.begin(), qeta.end(), "QNb", maxSize  );
  tuple->farray( "QPhi", qphi.begin(), qphi.end(), "QNb", maxSize );

  if( !(tuple->write()) ) info() << "Could not save GenParticles !" << endmsg;

}

//============================================================================
// Get Nb of Velo tracks inevent
//============================================================================
unsigned int MCDisplVertices::GetNbVeloTracks(){

  unsigned int nbv = 0;
  const Track::Range Trks = get<Track::Range>( TrackLocation::Default );
  for(Track::Range::const_iterator itr = Trks.begin();
      Trks.end() != itr; ++itr) {
    if( (*itr)->hasVelo() ) ++nbv;
  }
  return nbv;
}

//============================================================================
// Save in Tuple the PV candidates
//============================================================================
StatusCode  MCDisplVertices::SavePVs( Tuple & tuple ){

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
// Save in Tuple some Global Event Cut
//============================================================================
StatusCode  MCDisplVertices::SaveGEC( Tuple & tuple,
                                      Particle::ConstVector & RVs ){


  //Global track cuts
  double sumPtTracks = 0.;
  double sumXYTrackfirstStates = 0.;
  double sumSVxyDist = 0.;

  //Get forward tracks
  const Track::Range inputTracks = get<Track::Range>(TrackLocation::Default);
  //Get the PV
  const RecVertex::Range & primVertices = this->primaryVertices();

  if( (primVertices.size() != 0) && inputTracks.size() != 0 &&
      RVs.size() != 0 ){

    for(Track::Range::const_iterator itr = inputTracks.begin();
        inputTracks.end() != itr; itr++) {
      const Track* trk = *itr;
      const double xyfState = sqrt(trk->firstState().x()*trk->firstState().x()+
                                   trk->firstState().y()*trk->firstState().y());
      sumPtTracks += trk->pt();
      sumXYTrackfirstStates += xyfState;
    }


    //     //Find the upstream PV
    //     vector<const RecVertex*> primVrtcs;
    //     for( RecVertex::Range::const_iterator
    //            itPV = primVertices.begin(); primVertices.end() != itPV; ++itPV) {
    //       const RecVertex* pvtx = *itPV;
    //       primVrtcs.push_back(pvtx);
    //     }
    //     std::sort( primVrtcs.begin(), primVrtcs.end(), SortPVz );
    //     const RecVertex* realPV = *(primVrtcs.begin());

    //     //Global RV cut
    //     for( Particle::ConstVector::const_iterator itRV = RVs.begin();
    //          RVs.end() != itRV; ++itRV) {
    //       const Gaudi::XYZPoint & pos = (*itRV)->endVertex()->position();
    //       double distVtcs = VertDistance( realPV->position(), pos );
    //       if(distVtcs > .001)
    //         sumSVxyDist += pos.rho();
    //     }
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
// Does Particle come from a Prey, and from which one ?
//============================================================================
double MCDisplVertices::WhichMCPrey( const Particle * p ){

  //This function does return :
  //  zero if not from a prey
  //  z position of the prey it comes from
  //  -400 from a prey, but not produced at the prey decay vtx
  //  -500 if no associated MCPArticle found

  //Look for associated MCParticle
  const MCParticle *mcp = m_pLinker->firstMCP( p );
  if( NULL==mcp ) return -500.;
  if( false )  debug() << "Particle " << p->key()
                       << " associated to MC part "
                       << mcp->key() << endmsg;

  //Check if from a prey. If yes return its z decay position
  return IsitFromaPrey( mcp, mcp->originVertex() );
}

//=============================================================================
//  Is a MC Particle an descendant of a prey ?
//=============================================================================
bool MCDisplVertices::IsitFromaPrey( const MCParticle * p) {

  if( p->mother() ){
    const MCParticle * mother = p->mother();
    if( mother->particleID().abspid() == m_PreyID.abspid() ) {
      return true;
    } else {
      return IsitFromaPrey( mother );
    }
  }
  return false;
}

double MCDisplVertices::IsitFromaPrey( const MCParticle * p,
                                       const MCVertex * prod ) {

  if( p->mother() ){
    const MCParticle * mother = p->mother();
    if( mother->particleID().abspid() == m_PreyID.abspid() ) {
      SmartRefVector< LHCb::MCVertex >::const_iterator vtx =
        mother->endVertices().begin();
      if( VertDistance( prod->position(),(*vtx)->position() ) < 0.08 )
        return (*vtx)->position().z();
      return -400.;
    } else {
      return IsitFromaPrey( mother, prod );
    }
  }
  return 0.;
}

bool MCDisplVertices::IsitFromaPrey( HepMC::GenParticle * p ) {

  //Find the mother
  if( p->production_vertex() ){
    HepMC::GenVertex * vtx = p->production_vertex();
    //Loop on incoming particle
    HepMC::GenVertex::particles_in_const_iterator pin =
      vtx->particles_in_const_begin();
    HepMC::GenVertex::particles_in_const_iterator pend =
      vtx->particles_in_const_end();
    for( ; pin !=  pend; ++pin){
      unsigned int pid = abs( (*pin)->pdg_id() );
      if( pid == m_PreyID.abspid() ){
        return true;
      }
    }
  }
  return false;
}

//============================================================================
// return the mother MCParticle of a RecVertex
//============================================================================
const MCParticle * MCDisplVertices::WhichMother( const Particle * p ){

  //Loop on daughters
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin(); i!= iend; ++i ){
    const MCParticle * MCpart = m_pLinker->firstMCP( (*i) );
    if(MCpart != NULL ){
      //       debug() << "Particle " << (*i)->key() << " "
      //       << (*i)->particleID().pid()
      //       << " associated to MC part " << MCpart->key() << " "
      //       << MCpart->particleID().pid() << endmsg;

      const MCParticle * mcmother = WhichMother( MCpart );
      if( mcmother != NULL ) return mcmother;
    }
  }

  return NULL;
}

const MCParticle * MCDisplVertices::WhichMother( const MCParticle * p ){

  const MCParticle * mother = p;
  while( mother->mother() ){
    mother = mother->mother();
  }
  return mother;
}

//============================================================================
// return true if particle is a neutrino
//============================================================================
bool MCDisplVertices::IsNeutrino( const MCParticle * p ){

  int id = p->particleID().pid();
  if( id == 12 || id == 14 || id == 16 ) {
    //debug() << "Found a Neutrino !" << endmsg;
    return true ;
  } else {
    return false;
  }
}

bool MCDisplVertices::IsNeutrino( HepMC::GenParticle * p ){

  int id = p->pdg_id();
  if( id == 12 || id == 14 || id == 16 ) {
    return true ;
    //debug() << "Found a Neutrino !" << endmsg;
  } else {
    return false;
  }
}

//============================================================================
// return true if particle is a quark
//============================================================================
bool MCDisplVertices::IsQuark( HepMC::GenParticle * p ){

  int id = abs( p->pdg_id() );
  if( id == 1 || id == 2 || id == 3 || id == 4 || id == 5 || id == 6 ) {
    return true ;
    //debug() << "Found a Quark !" << endmsg;
  } else {
    return false;
  }
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
bool MCDisplVertices::IsAPointInDet( const Gaudi::XYZPoint & pos ){

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


bool MCDisplVertices::IsAPointInDet( const Particle* P, int mode, double range ){

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
    double sigNx = 5*range*sqrt(RVPositionCovMatrix[0][0]);
    double sigNy = 5*range*sqrt(RVPositionCovMatrix[1][1]);
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
bool MCDisplVertices::IsInRFFoil( const Gaudi::XYZPoint & pos){

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
//  Has the given Particle a related MCVertex from int with matter ?
//=============================================================================
const MCVertex * MCDisplVertices::HasIntInAncestors( const Particle * p ){

  //Loop in daughters to find associate MCParticle
  int it = 0;
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i =
         p->daughters().begin(); i!= iend; ++i ){
    if( it > 2 ) continue;
    const MCParticle * mcp = m_pLinker->firstMCP( (*i) );
    if( mcp == NULL )continue;
    const MCVertex * orvtx = mcp->originVertex();
    if( orvtx->type() > 99 && orvtx->type() < 200 ) return orvtx;
    it += 1;
    const Gaudi::XYZPoint & ref = orvtx->position();
    while( orvtx->mother() != NULL ){
      if( orvtx->mother()->originVertex() == NULL ) break;
      orvtx = orvtx->mother()->originVertex();
      if( VertDistance(ref, orvtx->position())>0.1 ) break;
      const MCVertex::MCVertexType & type = orvtx->type();
      if( type > 99 && type < 200 ) return orvtx;
    }
  }

  return NULL;
}

//=============================================================================
// Has the (MC) vertex been generated by collisin of flying particles
//    with air of beam pipe ?
// ATTENTION : this function has not been assessed ! Status : unchecked
//=============================================================================
bool MCDisplVertices::IsFromAir( const Particle * p ){

  //the vertex which has the tag >100 may not be the origin vertex of the first long-lived daughter...
  const MCVertex * mcv = HasIntInAncestors( p );
  if( mcv == NULL ) return false;

  // Vtx is from interaction with matter
  if( !IsAPointInDet( mcv->position() ) ) return true;

  return false;
}


//=============================================================================
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool MCDisplVertices::HasBackwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = RV->tracks().end();
  for( ; i != iend; ++i ){
    if ( (*i)->checkFlag( Track::Backward ) ) return true;
  }
  return false;
}

bool MCDisplVertices::HasBackwardTracks( const Particle * p ){

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
bool MCDisplVertices::HasBackAndForwardTracks( const RecVertex* RV ){
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
double MCDisplVertices::HasMuons( const Particle * p ){

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
Gaudi::XYZPoint MCDisplVertices::Normed( const Gaudi::LorentzVector & P,
                                         double range ){
  double norm = range/Norm( P );
  return Gaudi::XYZPoint( P.x()*norm, P.y()*norm, P.z()*norm );
}


double MCDisplVertices::Norm( const Gaudi::LorentzVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) +
               pow(P.z(),2) );
}

double MCDisplVertices::Norm( const Gaudi::XYZPoint & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) +
               pow(P.z(),2) );
}

double MCDisplVertices::Norm( const Gaudi::XYZVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) +
               pow(P.z(),2) );
}

double MCDisplVertices::Mult( const Gaudi::XYZPoint & p1,
                              const Gaudi::XYZPoint & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

double MCDisplVertices::Mult(  const Gaudi::LorentzVector & p1,
                               const Gaudi::LorentzVector & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

Gaudi::XYZPoint MCDisplVertices::Plus( const Gaudi::XYZPoint & p1,
                                       const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint MCDisplVertices::Minus( const Gaudi::XYZPoint& p1,
                                        const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}

//============================================================================
// Compute distance between two vertices
//============================================================================
double MCDisplVertices::VertDistance( const Gaudi::XYZPoint & v1,
                                      const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}
double MCDisplVertices::VertDistance( HepMC::ThreeVector & v1,
                                      HepMC::ThreeVector & v2 ){
  double dx = v1.x() - v2.x();
  double dy = v1.y() - v2.y();
  double dz = v1.z() - v2.z();
  return sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));

}

double MCDisplVertices::VertDistance(HepMC::GenVertex* v1,HepMC::GenVertex* v2 ){
  HepMC::ThreeVector point =
    HepMC::ThreeVector( v1->point3d().x() - v2->point3d().x(),
                        v1->point3d().y() - v2->point3d().y(),
                        v1->point3d().z() - v2->point3d().z());
  return VertDistance( point );
}

double MCDisplVertices::VertDistance(HepMC::ThreeVector * v1, Vertex * v2 ) {

  HepMC::ThreeVector point =
    HepMC::ThreeVector( v2->position().x(), v2->position().y(),
                        v2->position().z() );
  return VertDistance( *v1, point );
}

double MCDisplVertices::VertDistance(HepMC::ThreeVector * v1,
                                     HepMC::GenVertex* v2  ) {
  HepMC::ThreeVector point =
    HepMC::ThreeVector( v2->point3d().x(),v2->point3d().y(),v2->point3d().z());
  return VertDistance( *v1, point );
}

double MCDisplVertices::VertDistance(HepMC::ThreeVector * v1,
                                     const RecVertex * v2 ) const {
  HepMC::ThreeVector point =
    HepMC::ThreeVector( v2->position().x(), v2->position().y(),
                        v2->position().z() );
  double dx = v1->x() - point.x();
  double dy = v1->y() - point.y();
  double dz = v1->z() - point.z();
  return sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
}

double MCDisplVertices::VertDistance(HepMC::ThreeVector * v1,
                                     const RecVertex & v2 ) const {
  HepMC::ThreeVector point =
    HepMC::ThreeVector( v2.position().x(), v2.position().y(),
                        v2.position().z() );
  double dx = v1->x() - point.x();
  double dy = v1->y() - point.y();
  double dz = v1->z() - point.z();
  return sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
}

double MCDisplVertices::VertDistance(HepMC::ThreeVector & v1, RecVertex & v2 ) {
  HepMC::ThreeVector point =
    HepMC::ThreeVector( v2.position().x(), v2.position().y(),
                        v2.position().z() );
  return VertDistance( v1, point );
}

double MCDisplVertices::VertDistance( HepMC::ThreeVector * v1,
                                      HepMC::ThreeVector & v2 ) {
  return VertDistance( *v1, v2 );
}

double MCDisplVertices::VertDistance(HepMC::ThreeVector & point){
  return sqrt(pow( point.x(),2)+pow(point.y(),2)+pow(point.z(),2));
}

//============================================================================
// Angle between two tracks   :   tracks are supposed to be straight lines
//============================================================================
double MCDisplVertices::Angle( const Gaudi::LorentzVector & a,
                               const Gaudi::LorentzVector & b ){
  return Angle( a.Vect(), b.Vect() );
}

double MCDisplVertices::Angle( const Gaudi::XYZVector & a,
                               const Gaudi::XYZVector & b ){
  return acos( a.Dot(b) / sqrt( a.Mag2() * b.Mag2() ) );
}

//============================================================================
// Compute the impact parameter of a Particle to a Vertex
//============================================================================
double MCDisplVertices::ImpactParam( const MCParticle * p, const MCVertex * v0 ){

  if( v0 == NULL ){
    return ImpactParam( p->originVertex()->position(), p->momentum(),
                        MCVertex().position() );
  } else {
    return ImpactParam( p->originVertex()->position(), p->momentum(),
                        v0->position() );
  }
}

double MCDisplVertices::ImpactParam( const Gaudi::XYZPoint & V,
                                     const Gaudi::LorentzVector & P,
                                     const Gaudi::XYZPoint & V0 ){
  return ImpactParam( V, Gaudi::XYZVector( P.x(),P.y(),P.z() ), V0 );
}

double MCDisplVertices::ImpactParam( const Gaudi::XYZPoint & V,
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

//=============================================================================
//  Save resolution on the decay vertex of a reconstructed prey
//  Save resolution of the PV
//=============================================================================
void MCDisplVertices::Resolution( const Gaudi::XYZPoint & pos ){

  double rho = 10000.; double z = 10000.; double diff = 10000.;

  //Loop on MC data
  for( vector<Gaudi::XYZPoint>::iterator i =  m_MCPos.begin();
       i!=m_MCPos.end(); ++i ){
    Gaudi::XYZPoint tmp = Gaudi::XYZPoint( pos.X() - (*i).X(),
                                           pos.Y() - (*i).Y(),
                                           pos.Z() - (*i).Z() );
    if( sqrt(tmp.Mag2()) < diff ){
      diff = sqrt(tmp.Mag2());
      rho = tmp.Rho();
      z = tmp.Z();
    }
  }
  plot( rho,"PreyResR", 0.,0.3);
  plot( z,"PreyResZ", -2.,2.);

}

void MCDisplVertices::Resolution( ){

  if( PV == NULL || MCPV == NULL ) {
    always()<<"PV or MCPV not set !"<<endmsg;
    return;
  }
  double rho = PV->position().Rho() - MCPV->position().Rho();
  double z = PV->position().Z() - MCPV->position().Z();

  plot( rho,"PVResR", 0.,0.1);
  plot( z,"PVResZ", -1.,1.5);

}

//============================================================================
// Compute mass and nb of tracks from Long tracks only
//============================================================================
void MCDisplVertices::GetMassFromLongTracks( const Particle * p,
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
double MCDisplVertices::GetRecMass( const Particle::ConstVector & RecParts ){

  Gaudi::LorentzVector mom;

  Particle::ConstVector::const_iterator iend = RecParts.end();
  for( Particle::ConstVector::const_iterator i = RecParts.begin();
       i != iend; ++i ){
    mom += (*i)->momentum();
  }
  return mom.M();
}

double MCDisplVertices::GetRecMass( const Particle::ConstVector & RecParts,
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
// Compute the sum pT of a bunch of track (daughters of a Particle)
//=============================================================================
double MCDisplVertices::GetSumPt( const Particle * p ){

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
double MCDisplVertices::GetPt( const RecVertex* RV ){

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
double MCDisplVertices::GetPt( const Gaudi::LorentzVector & d,
                               const Gaudi::LorentzVector & m ){
  double norm = Norm( m );
  const Gaudi::LorentzVector mn = m/norm;
  norm = Norm( d );
  const Gaudi::LorentzVector dn = d/norm;

  return norm*sin( acos( Mult(dn,mn) ) );

}

//=============================================================================
// Compute charge of a vertex, possibly weighted by the momentum, and sum pt.
//=============================================================================
void MCDisplVertices::GetQPt( const Vertex * v, double & sumpt, double & sumq,
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
double MCDisplVertices::GetCharge( const Vertex * v, bool weight ){

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



//============================================================================
// get the delta R between two particles
//============================================================================
double MCDisplVertices::GetDeltaR( const Particle * p1,
                                   const LHCb::Particle * p2 ){

  double Deta = fabs( p1->momentum().eta() - p2->momentum().eta() );
  double Dphi = fabs( p1->momentum().phi() - p2->momentum().phi() );
  if (Dphi>pi) { Dphi = 2*pi - Dphi; };

  //Compute delta R = sqrt ( deltaeta**2  + deltaphi**2  )
  return  sqrt( Deta*Deta + Dphi*Dphi );
}

double MCDisplVertices::GetR( const Gaudi::XYZPoint & p){

  return sqrt( p.x()*p.x() + p.y()*p.y() );

}

//=============================================================================
// Compute azimuthal angle between the 2 candidates
//=============================================================================
double MCDisplVertices::GetDeltaPhi( const Gaudi::XYZPoint & v1,
                                     const Gaudi::XYZPoint & v2 ){

  double dphi = v1.Phi() - v2.Phi();
  if( dphi >= pi ) dphi -= 2*pi;
  if( dphi < -pi ) dphi += 2*pi;
  return dphi;
}

//=============================================================================
// Return a mass estimate from a vector of MCParticles
//=============================================================================
double MCDisplVertices::GetMCMass( MCParticle::ConstVector & daughters ){

  Gaudi::LorentzVector dumv = Gaudi::LorentzVector();
  MCParticle::ConstVector::const_iterator pend = daughters.end();
  for( MCParticle::ConstVector::const_iterator p = daughters.begin();
       p != pend; ++p ){
    dumv = dumv + ((*p)->momentum());
  }
  //GeV !!
  return dumv.M()/GeV ;
}

//=============================================================================
// Get radial distance to Beam Line
//=============================================================================
double MCDisplVertices::GetRFromBL( const Gaudi::XYZPoint& p ){

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
void MCDisplVertices::GetPVs(){

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
void MCDisplVertices::GetUpstreamPV(){
  GetPVs();
  if( PVs.empty() ){ PV = NULL; } else { PV = *(PVs.begin()); }
}



//=============================================================================
// Get Particles related to a RecVertex
//=============================================================================
void MCDisplVertices::GetPartsFromRecVtx(const RecVertex* dVtx,
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
StatusCode MCDisplVertices::SaveCaloInfos( Tuple& tuple ){
  double E = 0; double Et = 0.;
  StatusCode sc = GetCaloInfos( "Ecal", E, Et ) &&
    GetCaloInfos( "Hcal", E, Et ) &&
    //GetCaloInfos( "Prs", E, Et ) && GetCaloInfos( "Spd", E, Et ) &&
    GetCaloInfos( "Muon", E, Et );
  tuple->column( "TotEt", Et );
  return sc;
}

StatusCode MCDisplVertices::GetCaloInfos( string CaloType, double& En, double& Et ){
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



StatusCode MCDisplVertices::GetCaloClusterInfos( string CaloType, Tuple & tuple,
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
StatusCode MCDisplVertices::SaveTrigInfinTuple( Tuple & tuple ){

  if( m_tisTos == NULL ) return Error("m_tisTos not initialized");

  //Save some MC info, be sure you're not running on real data !
  if( exist<HepMCEvents>( HepMCEventLocation::Default ) ){

    //Save the number of interactions
    HepMCEvents* events = get<HepMCEvents>( HepMCEventLocation::Default ) ;
    tuple->column( "NbInt", events->size() );

    //Is the prey the daughter of a mother ?
    HepMCEvents::const_iterator ie=events->begin();
    const HepMCEvent* event = *ie ;  //get first MC event in a collision
    const HepMC::GenEvent* theEvent = event->pGenEvt();
    HepMC::GenEvent::particle_const_iterator p;
    bool mother = false;
    for( p= theEvent->particles_begin(); p!= theEvent->particles_end(); ++p ){
      unsigned int pid = abs((*p)->pdg_id());
      if( pid == m_MotherPreyID.abspid() ){
        mother = true; break;
      }
    }
    tuple->column( "FromMother", mother );
  } else {
    tuple->column( "NbInt", -1 );
    tuple->column( "FromMother", false );
  }

  //Was a prey reconstructed ? Look in the preselection container !
  const Particle::ConstVector cands = this->i_particles();
  bool cand = false;
  if( cands.size() > 0 ) cand = true;
  tuple->column( "Reco", cand );
  if( msgLevel(MSG::DEBUG) && cand )
    debug()<<"Event has a reconstructed prey !"<< endmsg;

  //Get L0 info and save decision in tuple
  if( !exist<L0DUReport>( L0DUReportLocation::Default )){
    err()<<"You requested the trigger, make sure you run it."<<endmsg;
    err()<<"No LODU report at " << L0DUReportLocation::Default <<endmsg;
    return StatusCode::FAILURE;
  }
  L0DUReport * l0 = get<L0DUReport>( L0DUReportLocation::Default );
  int L0dec = l0->decision();
  if( msgLevel(MSG::DEBUG) )
    debug()<<"L0 decision                  : " << L0dec << endmsg;
  tuple->column( "L0", L0dec );

  //fill the HLT global : Hlt2 : 77,Hlt1 :  46
  vector<unsigned int> m_routingBits;
  m_routingBits.push_back( 46 );
  m_routingBits.push_back( 77 );
  //   for ( unsigned int i = 32 ; i < 96 ; i++){m_routingBits.push_back(i);}
  bool Hlt1Globdec = false;
  bool Hlt2Globdec = false;
  if(exist<LHCb::RawEvent>(RawEventLocation::Default)){
    RawEvent* rawEvent = get<RawEvent>(RawEventLocation::Default);
    vector<unsigned int> yes = Hlt::firedRoutingBits(rawEvent,m_routingBits);
    unsigned int d1 = 46;
    unsigned int d2 = 77;
    if( find( yes.begin(), yes.end(), d1 ) != yes.end() ) Hlt1Globdec = true;
    if( find( yes.begin(), yes.end(), d2 ) != yes.end() )  Hlt2Globdec = true;
    if( msgLevel(MSG::DEBUG) ){
      debug()<<"Firing routing bits : "<< yes << endmsg;
      debug()<<"Hlt1 Global decision         : " << Hlt1Globdec << endmsg;
      debug()<<"Hlt2 Global decision         : " << Hlt2Globdec << endmsg;
    }
  }
  tuple->column( "Hlt1", Hlt1Globdec );
  tuple->column( "Hlt2", Hlt2Globdec);

  /***************************
   * Beware : it seems that HltDecReport writes only on TES algos that fired
   *****************************/
  if (!exist<HltDecReports>( HltDecReportsLocation::Default ) ){
    //     warning()<<"No HltDecReports at "
    //       << HltDecReportsLocation::Default << endmsg;
    return true;
  }
  const HltDecReports* decReports = get<HltDecReports>
    ( HltDecReportsLocation::Default );

  int DVdec = 0;
  const LHCb::HltDecReport * decrep =
    decReports->decReport("Hlt2DisplVerticesSingleDecision");
  if( decrep != NULL && decrep->decision() ) DVdec += 1;
  decrep = decReports->decReport("Hlt2DisplVerticesDoubleDecision");
  if( decrep != NULL && decrep->decision() ) DVdec += 10;
  if( msgLevel(MSG::DEBUG) )
    debug()<<"Hlt2DisplVerticesDecision    : " << DVdec << endmsg;
  tuple->column( "HltDV", DVdec );

  //Get topological trigger decision with TisTosTools
  m_tisTos->setOfflineInput();
  m_tisTos->setTriggerInput(); // reset trigger names
  m_tisTos->setTriggerInput("Hlt2Topo.*Decision");//addToTriggerInput
  bool TopoDec = m_tisTos->triggerTisTos().decision();
  if( msgLevel(MSG::DEBUG) )
    debug()<<"Hlt2Topo{2,3,4}Decision      : "<< TopoDec << endmsg;
  tuple->column( "Hlt2Topo", TopoDec );

  //////////////////////////////////////////////////////////////////
  //Do not use HltDecReport for Hlt1Global and Hlt2Global decisions...
  // Offline, all the rate limited stuff, including the Lumi/MinBias,
  // doesn't work properly as you don't have the "counter" to limit it,
  // so the Global always goes to 1...
  //   bool Hlt1Globdec = false;
  //   const LHCb::HltDecReport * decrep = decReports->decReport("Hlt1Global");
  //   if( decrep != NULL ) Hlt1Globdec = decrep->decision();
  //   if( msgLevel(MSG::DEBUG) )
  //     debug()<<"Hlt1 Global decision         : " << Hlt1Globdec << endmsg;
  //   tuple->column( "Hlt1", Hlt1Globdec );

  //   decrep = decReports->decReport("Hlt2Global");
  //   bool Hlt2Globdec = false;
  //   if( decrep != NULL ) Hlt2Globdec = decrep->decision();
  //   if( msgLevel(MSG::DEBUG) )
  //     debug()<<"Hlt2 Global decision         : " << Hlt2Globdec << endmsg;
  //   tuple->column( "Hlt2", Hlt2Globdec);

  //To print entire report container :
  //vector<string> allConfiguredTrgLines = decReports->decisionNames();
  //debug() << *decReports << endmsg;

  return SaveL0RawInfos(tuple);
}

//============================================================================
//  Save the SPD multiplicity applied in the hadron and electron triggers.
//============================================================================
StatusCode MCDisplVertices::SaveL0RawInfos( Tuple& tuple ){
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
}


//============================================================================
//  Event number
//============================================================================
StatusCode MCDisplVertices::fillHeader( Tuple& tuple ){
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
StatusCode MCDisplVertices::BackToBack( Particle::ConstVector & Cands ){

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

