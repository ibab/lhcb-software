// $Id: CaloElectronNtp.cpp,v 1.1 2010/05/20 09:55:38 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include  "CaloUtils/CaloMomentum.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/ODIN.h" 
// local
#include "CaloElectronNtp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloElectronNtp
//
// 2009-12-11 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloElectronNtp );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloElectronNtp::CaloElectronNtp( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{

  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty("InputContainer" , m_input =  LHCb::ProtoParticleLocation::Charged );
  declareProperty("EFilter"   , m_e = std::make_pair(0.,99999999.));
  declareProperty("EoPFilter" , m_eop = std::make_pair(0.3,2.5));
  declareProperty("EtFilter"  , m_et = std::make_pair(150.,999999.));
  declareProperty("PrsFilter" , m_prs  = std::make_pair(-1.,1024));
  declareProperty("ElectronPairing", m_pairing = true );
  declareProperty( "Tupling"  , m_tupling = true);
  declareProperty( "Histo"    , m_histo = true);
  
}
//=============================================================================
// Destructor
//=============================================================================
CaloElectronNtp::~CaloElectronNtp() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloElectronNtp::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_calo = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal);

  // get tools
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this); 
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  m_toSpd = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Spd" , this );
  m_toSpd->setCalos( "Ecal" ,"Spd");
  m_toPrs = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Prs" , this );
  m_toPrs->setCalos( "Ecal" ,"Prs");
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloElectronNtp::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Tuple ntp = NULL;
  if(m_tupling)ntp=nTuple(500, "e_tupling" ,CLID_ColumnWiseTuple);
  StatusCode scnt;

  // get input data
  if( !exist<LHCb::ProtoParticles> ( m_input ) ){
    debug() << "no protoP container found at " << m_input << endmsg;
    return StatusCode::SUCCESS ;
  };
  

  // GET ODIN INFO
  int run = 0;
  long evt = 0;
  int tty = 0;
  m_odin->getTime();
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
    run = odin->runNumber();
    evt =  odin->eventNumber();
    tty =  odin->triggerType();
    
  }


  LHCb::ProtoParticles* protos = get<LHCb::ProtoParticles> ( m_input );
  
  for( LHCb::ProtoParticles::const_iterator p = protos->begin(); protos->end () != p ; ++p ){ 
    const LHCb::ProtoParticle* proto = *p;
    if( !m_caloElectron->set(proto))continue;;
    counter("proto electron")+=1;
    LHCb::CaloHypo* hypo = m_caloElectron->electron();
    if ( NULL == hypo ) continue;
    LHCb::CaloMomentum momentum( hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    if( !inRange(m_et , et ))continue;
    if( !inRange(m_e  , e  ))continue;
    double ePrs = m_toPrs->energy ( *hypo , "Prs"  );
    int iSpd = m_toSpd->multiplicity ( *hypo , "Spd");
    
    if( !inRange(m_prs, ePrs))continue;
    LHCb::CaloCellID id = LHCb::CaloCellID();
    SmartRef<LHCb::CaloCluster> cluster;
    if ( hypo->clusters().size() > 0 ){
      cluster= *(hypo->clusters().begin());
      id = (*cluster).seed();      
    }
    double eOp = m_caloElectron->eOverP();
    if( !inRange( m_eop, eOp))continue;

    counter("Selected electron")+=1;


    Gaudi::XYZPoint hR( hypo->position()->x(),hypo->position()->y(),hypo->position()->z());
    Gaudi::XYZPoint cR( cluster->position().x() , cluster->position().y() , cluster->position().z() );
    Gaudi::XYZPoint tR = m_caloElectron->caloState().position();
    Gaudi::LorentzVector cs(m_caloElectron->caloState().momentum().x(),
                            m_caloElectron->caloState().momentum().y(),
                            m_caloElectron->caloState().momentum().z(),
                            m_caloElectron->caloState().p());
    double theta = m_caloElectron->caloState().momentum().Theta();

    Gaudi::LorentzVector t(proto->track()->momentum().x(),
                           proto->track()->momentum().y(),
                           proto->track()->momentum().z (),
                           proto->track()->p());
    StatusCode sc;
    const LHCb::CaloHypo* brem = m_caloElectron->bremstrahlung();
    LHCb::CaloCellID bid = LHCb::CaloCellID();
    Gaudi::LorentzVector bP;
    if( brem != NULL ){
      if ( brem->clusters().size() > 0 ){
        SmartRef<LHCb::CaloCluster> bcluster= *(brem->clusters().begin());
        bid = (*bcluster).seed();      
      }
      LHCb::CaloMomentum bremM = m_caloElectron->bremCaloMomentum();
      bP = Gaudi::LorentzVector ( bremM.momentum().x(),
                                  bremM.momentum().y(),
                                  bremM.momentum().z(),
                                  bremM.momentum().e());  
    }    
    
    // proto info
    if(m_tupling){
      sc=ntp->column("TrackMatch", proto->info(LHCb::ProtoParticle::CaloTrMatch, 9999.));
      sc=ntp->column("ElecMatch", proto->info(LHCb::ProtoParticle::CaloElectronMatch, 9999.));
      sc=ntp->column("BremMatch", proto->info(LHCb::ProtoParticle::CaloBremMatch, 9999.));
      sc=ntp->column("Spd", iSpd );
      sc=ntp->column("Prs", ePrs );
      sc=ntp->column("TrajectoryL", proto->info(LHCb::ProtoParticle::CaloTrajectoryL, 9999.));
      sc=ntp->column("VeloCharge", proto->info(LHCb::ProtoParticle::VeloCharge, -1.));
      sc=ntp->column("DLLe", proto->info(LHCb::ProtoParticle::CombDLLe, 0.));
      sc=ntp->column("RichDLLe", proto->info(LHCb::ProtoParticle::RichDLLe, 0.));
      // hypo info
      sc=ntp->column("EoP" , eOp );
      sc=ntp->column("HypoE", e  );
      sc=ntp->column("HypoR", hR  );
      // track info
      sc=ntp->column("TrackP", t  );
      sc=ntp->column("TrackR", tR  );
      sc=ntp->column("caloState",cs);
      sc=ntp->column("incidence",theta);
      // cluster info
      sc=ntp->column("id", id.index());
      sc=ntp->column("ClusterE",cluster->e());
      sc=ntp->column("ClusterR",cR); 
      // brem info
      sc=ntp->column("BremId", bid.index());
      sc=ntp->column("BremP", bP );
      
      // odin info
      sc=ntp->column("run"   , run         );
      sc=ntp->column("event" , evt );
      sc=ntp->column("triggertype" , tty );
    }
    
      
    // histogramming / channel
    
    if(m_histo){
      std::ostringstream sid;
      int feb = m_calo->cardNumber( id );
      sid << "crate" << format("%02i", m_calo->cardCrate( feb ) ) <<  "/"
          << "feb"   << format( "%02i" , m_calo->cardSlot( feb  ) )<< "/"
        <<Gaudi::Utils::toString( m_calo->cardColumn( id ) + nColCaloCard * m_calo->cardRow( id ) );
      plot1D(eOp, sid.str() , sid.str() , 0., 2.5, 100);
    }
    

    if(m_tupling){
      // perform electron pairing
      double mas = 999999.;
      if( !m_pairing)continue;
      for( LHCb::ProtoParticles::const_iterator pp = p+1 ; protos->end () != pp ; ++pp ){
        const LHCb::ProtoParticle* proto2 = *pp;
        if( !m_caloElectron->set(proto2))continue;;
        LHCb::CaloHypo* hypo2 = m_caloElectron->electron();
        if ( NULL == hypo2 ) continue;
        if( hypo == hypo2 )continue;
        LHCb::CaloMomentum momentum2( hypo2 );

        
        // filtering proto2
        const double e2 = momentum2.e();
        const double et2 = momentum2.pt();
        if( !inRange(m_et , et2))continue;
        if( !inRange(m_e  , e2  ))continue;
        double ePrs2 = m_toPrs->energy ( *hypo2 , "Prs"  );
        if( !inRange(m_prs, ePrs2))continue;
        double eOp2 = m_caloElectron->eOverP();
        if( !inRange( m_eop, eOp2))continue;
        
        
        // compute mass
        const LHCb::Track*  t1 = proto->track();
        const LHCb::Track*  t2 = proto2->track();
        
        if( NULL == t1 || NULL == t2)continue;
        if( -1 != t1->charge()*t2->charge())continue;
        LHCb::State st1 = t1->firstState();
        LHCb::State st2 = t2->firstState();
        
        if( NULL == extrapolator() ){
          Warning("No extrapolator defined");
          continue;
        }
        StatusCode sc = extrapolator()->propagate(st1, 0.);
        if(sc.isFailure())Warning("Propagation 1 failed").ignore();
        sc = extrapolator()->propagate(st2, 0.);
        if(sc.isFailure())Warning("Propagation 2 failed").ignore();
        Gaudi::XYZVector p1 = st1.momentum();
        Gaudi::XYZVector p2 = st2.momentum();
        double m2  = p1.R()*p2.R();
        m2 -= p1.X()*p2.X();
        m2 -= p1.Y()*p2.Y();
        m2 -= p1.Z()*p2.Z();
        m2 *= 2;
        if( m2 > 0){
          double m = sqrt(m2) ;
          if( m < mas )mas = m;
        }
      }
      // add mass
      sc = ntp->column("MinMee",mas);
      sc = ntp->write();
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloElectronNtp::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
