// Include files 

// from Gaudi
#include  "CaloUtils/CaloMomentum.h"
#include "Event/RecVertex.h"
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
DECLARE_ALGORITHM_FACTORY( CaloElectronNtp )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloElectronNtp::CaloElectronNtp( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_extrapolator(NULL)
  , m_caloElectron(NULL)
  , m_toSpd(NULL)
  , m_toPrs(NULL)
  , m_calo(NULL)
  , m_odin(NULL)
{

  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty("InputContainer" , m_input =  LHCb::ProtoParticleLocation::Charged );
  declareProperty("EFilter"   , m_e = std::make_pair(0.,99999999.));
  declareProperty("EoPFilter" , m_eop = std::make_pair(0.,2.5));
  declareProperty("EtFilter"  , m_et = std::make_pair(200.,999999.));
  declareProperty("PrsFilter" , m_prs  = std::make_pair(50.,9999999.));
  declareProperty("ElectronPairing", m_pairing = true );
  declareProperty( "Tuple"    , m_tuple = true);
  declareProperty( "Histo"    , m_histo = true);
  declareProperty( "Trend"    , m_trend = false);  
  declareProperty( "VertexLoc", m_vertLoc =  LHCb::RecVertexLocation::Primary);
  declareProperty( "UsePV3D", m_usePV3D = false);
  declareProperty( "TrackTypes", m_tracks);
  declareProperty( "splitFEBs", m_splitFEBs=false);
  declareProperty( "splitE", m_splitE=false);
  
  m_tracks.push_back(LHCb::Track::Long);
  m_tracks.push_back(LHCb::Track::Downstream);  
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

  if( !m_tracks.empty() )info() << "Will only look at track type(s) = " << m_tracks << endmsg;
  else 
    info() << "Will look at any track type" << endmsg;


  m_calo = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal);

  // set vertex location
  if(m_vertLoc == "")
    m_vertLoc=(m_usePV3D) ? LHCb::RecVertexLocation::Velo3D :  LHCb::RecVertexLocation::Primary;

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
  if(m_tuple)ntp=nTuple(500, "e_tupling" ,CLID_ColumnWiseTuple);
  StatusCode scnt;

  // get input data
  if( !exist<LHCb::ProtoParticles> ( m_input ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "no protoP container found at " << m_input << endmsg;
    return StatusCode::SUCCESS ;
  };
  

  // GET ODIN INFO
  int run = 0;
  ulonglong evt = 0;
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

    // track type selection
    bool accept = m_tracks.empty() ? true : false;
    for(std::vector<int>::iterator itr = m_tracks.begin();m_tracks.end() != itr;++itr){
      if( (int)proto->track()->type() == *itr)accept=true;
    }
    if( !accept )continue;


    LHCb::CaloMomentum momentum( hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    if( !inRange(m_et , et ))continue;
    if( !inRange(m_e  , e  ))continue;
    double ePrs = proto->info(LHCb::ProtoParticle::CaloPrsE, 0.);
    //double ePrs = m_toPrs->energy ( *hypo , "Prs"  );
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
    double tX =   m_caloElectron->caloState().momentum().X();
    double tY =   m_caloElectron->caloState().momentum().Y();
    double tZ =   m_caloElectron->caloState().momentum().Z();
    double thetaX= atan2(tX,tZ);
    double thetaY= atan2(tY,tZ);
    

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


    // dielectron filter
    double mas = 999999.;
    if( m_pairing){
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
        double ePrs2 = proto2->info(LHCb::ProtoParticle::CaloPrsE, 0.);
        //double ePrs2 = m_toPrs->energy ( *hypo2 , "Prs"  );
        if( !inRange(m_prs, ePrs2))continue;
        double eOp2 = m_caloElectron->eOverP();
        if( !inRange( m_eop, eOp2))continue;        
        // compute mass
        const LHCb::Track*  t1 = proto->track();
        const LHCb::Track*  t2 = proto2->track();
        if( NULL == t1 || NULL == t2)continue;
        if( -1 != t1->charge()*t2->charge())continue;
        // track type
        bool accept2 = m_tracks.empty() ? true : false;
        for(std::vector<int>::iterator itr = m_tracks.begin();m_tracks.end() != itr;++itr){
          if( (int)t2->type() == *itr)accept2=true;
        }
        if( !accept2 )continue;

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
    }
    if( mas > 0 && mas < 100)counter("Selected (di)electron")+=1;
    


    
    // proto info
    if(m_tuple){
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
      sc=ntp->column("HypoTheta",atan2(sqrt(hR.X()*hR.X()+hR.Y()*hR.Y()),hR.Z()));
     // track info
      sc=ntp->column("TrackP", t  );
      sc=ntp->column("TrackR", tR  );
      sc=ntp->column("caloState",cs);
      sc=ntp->column("incidence",theta);
      sc=ntp->column("incidenceX",thetaX);
      sc=ntp->column("incidenceY",thetaY);
      sc=ntp->column("trackType",proto->track()->type());
      sc=ntp->column("trackProb",proto->track()->probChi2());
      // cluster info
      sc=ntp->column("id", id.index());
      sc=ntp->column("ClusterE",cluster->e());
      sc=ntp->column("ClusterR",cR); 
      // brem info
      sc=ntp->column("BremId", bid.index());
      sc=ntp->column("BremP", bP );
      
      // odin info
      sc=ntp->column("run"   , run         );
      sc=ntp->column("event" , (double) evt );
      sc=ntp->column("triggertype" , tty );
      if(m_pairing){
        sc = ntp->column("MinMee",mas);
        sc = ntp->write();
      }
    }
    
      
    // histogramming / channel    

    if(m_histo && iSpd != 0 && proto->info(LHCb::ProtoParticle::CaloTrMatch, 9999.) < 25){
      fillH(eOp,t,id);
      if( m_pairing && mas >0. && mas < 100. )fillH(eOp,t,id,"conversion/");
    }
    

    // vertices
    int nVert = 0;
    if( m_usePV3D){
      if( exist<LHCb::VertexBases>( m_vertLoc ) ){
        LHCb::VertexBases* verts= get<LHCb::VertexBases>( m_vertLoc );
        if( NULL != verts)nVert = verts->size();
          }
    }
    else{
      if( exist<LHCb::RecVertices>(m_vertLoc) ){
        LHCb::RecVertices* verts= get<LHCb::RecVertices>(m_vertLoc);
        if( NULL != verts)nVert = verts->size();
      }
    }
    counter("#PV="+ Gaudi::Utils::toString(nVert) + " ["+ m_vertLoc+"]")+=1;
    
    if( m_trend){
      std::string sNpv = "PV" + Gaudi::Utils::toString( nVert ) +"/";
      std::string sRun  =  "r" + Gaudi::Utils::toString( run   ) +"/";
      std::string base = "Trend/";
      plot1D(eOp, base+"allPV/allRun/eOp","e/p spectrum for all run & allPV", 0. , 2.5, 250);
      plot1D(eOp, base+"allPV/"+sRun+"eOp","e/p spectrum for run = "+sRun , 0. , 2.5, 250);
      plot1D(eOp, base+sNpv+sRun+"eOp","e/p spectrum for PV="+sNpv+" (run = "+sRun+")" , 0., 2.5, 250); 
      plot1D(eOp, base+sNpv+"allRun/eOp","e/p spectrum for PV="+sNpv+" (all run)" , 0., 2.5, 250); 
    } 
    
    
    
  }
  return StatusCode::SUCCESS;
}
//=============================================================================



void CaloElectronNtp::fillH(double eOp, Gaudi::LorentzVector t, LHCb::CaloCellID id,std::string hat){
  if(!m_histo)return;  
  plot1D(eOp, hat+"all/eOp","all/eOp",0.,3.,300);
  std::string zone=id.areaName();
  plot1D(eOp, hat+zone+"/eOp",zone+"/eOp",0.,3.,300);
  if( m_splitFEBs){
    std::ostringstream sid;
    int feb = m_calo->cardNumber( id );
    sid << hat << "crate" << format("%02i", m_calo->cardCrate( feb ) ) <<  "/"
        << "feb"   << format( "%02i" , m_calo->cardSlot( feb  ) )<< "/"
        <<Gaudi::Utils::toString( m_calo->cardColumn( id ) + nColCaloCard * m_calo->cardRow( id ) );
    plot1D(eOp, sid.str() , sid.str() , 0., 3., 300);
  }      
  if( m_splitE && t.P() < 200. * Gaudi::Units::GeV ){  // put a limit at 200 GeV
    int ebin=( t.P() / 5000.)*5; // 1 GeV binning
    std::ostringstream sid ;
    sid << "/E_"<< ebin<<"/eOp";
    counter(sid.str()+"[<p>]") += t.P();
    plot1D(eOp, hat+zone+sid.str(),zone+sid.str(),0.,3.,300);
    plot1D(eOp, hat+"all"+sid.str(),"all"+sid.str(),0.,3.,300);        
  }
}


