// $Id: CaloAlignmentNtp.cpp,v 1.1 2010/05/20 09:55:38 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include  "CaloUtils/CaloMomentum.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/ODIN.h" 
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"
// local
#include "CaloAlignmentNtp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloAlignmentNtp
//
// 2009-12-11 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloAlignmentNtp );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloAlignmentNtp::CaloAlignmentNtp( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{

  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty("InputContainer" , m_input =  LHCb::ProtoParticleLocation::Charged );
  declareProperty("EFilter"   , m_e = std::make_pair(0.,99999999.));
  declareProperty("EoPFilter" , m_eop = std::make_pair(0.7,1.3));
  declareProperty("EtFilter"  , m_et = std::make_pair(150.,999999.));
  declareProperty("PrsFilter" , m_prs  = std::make_pair(10.,1024));
  declareProperty("SpdFilter" , m_spd  = std::make_pair(0.5,99999));
  declareProperty("DLLeFilter" , m_dlle  = std::make_pair(-99999.,99999.));
  declareProperty("RichDLLeFilter" , m_rdlle  = std::make_pair(0.,99999.));
  declareProperty("BremMatchFilter"    , m_bMatch  = std::make_pair(-999999.,999999.));
  declareProperty("ElectronMatchFilter", m_eMatch  = std::make_pair(-999999.,999999.));
  declareProperty("ClusterMatchFilter" , m_cMatch  = std::make_pair(-999999.,999999.));
  declareProperty("BremEleDistFilter" , m_dist  = std::make_pair(5.,99999.));
  declareProperty("EmlectronPairing", m_pairing = true );
  declareProperty("MassFilter", m_mas=std::make_pair(-99999.,99999.));
  declareProperty("Tuple"     , m_tuple = true);
  declareProperty("Histo"     , m_histo = true);
  declareProperty("Profile"    , m_profil = true);
  declareProperty("DeltaBin" , m_bin   = 150 );
  declareProperty("DeltaMin" , m_min   = -150. );
  declareProperty("DeltaMax" , m_max   = +150. );
  declareProperty("ThetaBin" , m_thBin = 14 );
  
}
//=============================================================================
// Destructor
//=============================================================================
CaloAlignmentNtp::~CaloAlignmentNtp() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloAlignmentNtp::initialize() {
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

  // 
  if(m_mas.first != -99999 || m_mas.second != +99999)m_pairing=true; // activate pairing when mass filter is set up

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloAlignmentNtp::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Tuple ntp = NULL;
  if(m_tuple)ntp=nTuple(500, "CaloAlignment" ,CLID_ColumnWiseTuple);
  StatusCode scnt;

  // get input data
  if( !exist<LHCb::ProtoParticles> ( m_input ) ){
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
    LHCb::CaloMomentum momentum( hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    if( !inRange(m_et , et ))continue;
    if( !inRange(m_e  , e  ))continue;
    double ePrs = m_toPrs->energy ( *hypo , "Prs"  );
    int iSpd = m_toSpd->multiplicity ( *hypo , "Spd");    
    if( !inRange(m_prs, ePrs))continue;
    if( !inRange(m_spd, (double) iSpd))continue;
    LHCb::CaloCellID id = LHCb::CaloCellID();
    SmartRef<LHCb::CaloCluster> cluster;
    if ( hypo->clusters().size() > 0 ){
      cluster= *(hypo->clusters().begin());
      id = (*cluster).seed();      
    }
    double eOp = m_caloElectron->eOverP();
    if( !inRange( m_eop, eOp))continue;
    if( !inRange( m_dlle, proto->info(LHCb::ProtoParticle::CombDLLe , 9999.)))continue;
    if( !inRange( m_rdlle, proto->info(LHCb::ProtoParticle::RichDLLe, 9999.)))continue;
    if( !inRange( m_bMatch, proto->info(LHCb::ProtoParticle::CaloBremMatch, 9999.)))continue;
    if( !inRange( m_eMatch, proto->info(LHCb::ProtoParticle::CaloElectronMatch, 9999.)))continue;
    if( !inRange( m_cMatch, proto->info(LHCb::ProtoParticle::CaloTrMatch, 9999.)))continue;

    counter("Selected electron")+=1;

    // track extrapolation
    Gaudi::XYZPoint hypoR( hypo->position()->x(),hypo->position()->y(),hypo->position()->z());// hypo position
    Gaudi::XYZPoint clusR( cluster->position().x() , cluster->position().y() , cluster->position().z() );// cluster position          
    Gaudi::XYZPoint statR = m_caloElectron->caloState().position();// caloState position
    Gaudi::XYZPoint cellR( m_calo->cellX(id) , m_calo->cellY(id), m_calo->cellZ(id) );// seed position
    Gaudi::XYZPoint closR = m_caloElectron->closestState().position();// closestState position

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


    // bremStrahlung extrapolation
    const LHCb::CaloHypo* brem = m_caloElectron->bremstrahlung();
    if( brem == NULL )continue; // brem is requested

    
    LHCb::CaloCellID bid = LHCb::CaloCellID();
    Gaudi::LorentzVector bP;
    Gaudi::XYZPoint bclusR;
    Gaudi::XYZPoint bcellR; 
    Gaudi::XYZPoint bremR;
    LHCb::CaloMomentum bremM = m_caloElectron->bremCaloMomentum();
    bP = Gaudi::LorentzVector ( bremM.momentum().x(),
                                bremM.momentum().y(),
                                bremM.momentum().z(),
                                bremM.momentum().e());  
    
    // brem position(s)
    bremR =Gaudi::XYZPoint( brem->position()->x(),brem->position()->y(),brem->position()->z());             // hypo position
    SmartRef<LHCb::CaloCluster> bcluster;
    if ( brem->clusters().size() > 0 ){
      bcluster= *(brem->clusters().begin());
      bid = (*bcluster).seed();      
    }
    bclusR=Gaudi::XYZPoint( bcluster->position().x() , bcluster->position().y() , bcluster->position().z() );  //cluster 
    bcellR=Gaudi::XYZPoint( m_calo->cellX(bid) , m_calo->cellY(bid), m_calo->cellZ(bid) );                     // seed 
        
    double dist = sqrt(fabs(double(bid.row() - id.row())) * fabs(double(bid.row() - id.row()))+
                       fabs(double(bid.col() - id.col())) * fabs(double(bid.col() - id.col())));

    if( bid.area() == id.area() &&  !inRange( m_dist, dist) )continue; // brem-electron separation
    
    counter("Selected electron+Brem")+=1;

    // perform electron pairing
    double mas = 9999.;
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
        double ePrs2 = m_toPrs->energy ( *hypo2 , "Prs"  );
        if( !inRange(m_prs, ePrs2))continue;
        double eOp2 = m_caloElectron->eOverP();
        if( !inRange( m_eop, eOp2))continue;
        if( !inRange( m_dlle, proto2->info(LHCb::ProtoParticle::CombDLLe, 0.)))continue;
        if( !inRange( m_rdlle, proto2->info(LHCb::ProtoParticle::RichDLLe, 0.)))continue;
        
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
      if( !inRange( m_mas, mas ))continue;
    }


    if(m_tuple){
      sc=ntp->column("TrackMatch", proto->info(LHCb::ProtoParticle::CaloTrMatch, 9999.));
      sc=ntp->column("ElecMatch", proto->info(LHCb::ProtoParticle::CaloElectronMatch, 9999.));
      sc=ntp->column("BremMatch", proto->info(LHCb::ProtoParticle::CaloBremMatch, 9999.));
      sc=ntp->column("BremEleDist", dist);
      sc=ntp->column("Spd", iSpd );
      sc=ntp->column("Prs", ePrs );
      sc=ntp->column("TrajectoryL", proto->info(LHCb::ProtoParticle::CaloTrajectoryL, 9999.));
      sc=ntp->column("VeloCharge", proto->info(LHCb::ProtoParticle::VeloCharge, -1.));
      sc=ntp->column("DLLe", proto->info(LHCb::ProtoParticle::CombDLLe, 0.));
      sc=ntp->column("RichDLLe", proto->info(LHCb::ProtoParticle::RichDLLe, 0.));
      // hypo info
      sc=ntp->column("EoP" , eOp );
      sc=ntp->column("hypoE", e  );
      sc=ntp->column("hypoR", hypoR  );
      // cluster info
      sc=ntp->column("id", id.index());
      sc=ntp->column("ClusterE",cluster->e());
      sc=ntp->column("ClusterR",clusR); 
      sc=ntp->column("SeedR"   ,cellR); 


      // track info
      sc=ntp->column("Charge", proto->track()->charge()  );
      sc=ntp->column("TrackP", t  );
      sc=ntp->column("TrackR"  , statR  );
      sc=ntp->column("ClosestR", closR  );
      sc=ntp->column("caloState",cs);
      sc=ntp->column("incidence",theta);

      // brem info
      sc=ntp->column("BremId", bid.index());
      sc=ntp->column("BremP", bP );
      sc=ntp->column("BremR", bremR  );
      sc=ntp->column("BremClusterR", bclusR  );
      sc=ntp->column("BremSeedR"   , bcellR  );
      
      // odin info
      sc=ntp->column("run"   , run         );
      sc=ntp->column("event" , (double) evt );
      sc=ntp->column("triggertype" , tty );
      if(m_pairing)sc = ntp->column("MinMee",mas);      
      sc = ntp->write();
      counter("Events in ntuple")+=1;
    }
    
  
    // Histogramming
    if( !m_histo && !m_profil)return StatusCode::SUCCESS;

    std::string prof ="";
    

    counter("Events in histos")+=1;
    
    std::string area   =  id.areaName() ;
    std::string side   = (id.col() < 32) ? "C" : "A";
    std::string level  = (id.row() < 32) ? "Bottom" : "Top";
    std::string barea  =  bid.areaName() ;
    std::string bside  = (bid.col() < 32) ? "C" : "A";
    std::string blevel = (bid.row() < 32) ? "Bottom" : "Top";
    
    std::string charge = (proto->track()->charge() > 0 ) ? "electron" : "positron";
    std::vector<std::string> q;
    q.push_back("all");
    q.push_back(charge);


    std::vector<std::string> split;
    split.push_back("all/");
    split.push_back( barea+"/all/");
    split.push_back( barea+"/"+charge+"/all/");
    split.push_back( barea+"/"+bside+"/all/");
    split.push_back( barea+"/"+bside+"/"+charge+"/all/");
    split.push_back( barea+"/"+bside+"/"+ blevel +"/all/");
    split.push_back( barea+"/"+bside+"/"+ blevel +"/"+charge+"/all/");
    split.push_back( barea+"/"+blevel+"/all/");
    split.push_back( barea+"/"+blevel+"/"+charge+"/all/");
    split.push_back( barea+"/"+blevel+"/"+ bside +"/all/");
    split.push_back( barea+"/"+blevel+"/"+ bside +"/"+charge+"/all/");
    
    // ------ brem aligment (DeltaXX versus ThetaYY)
    double bthX= t.X()/t.Z();
    double bthY= t.Y()/t.Z();
    double bdceX = (bcellR.X()- bthX*bcellR.Z()); // relative to cell 
    double bdceY = (bcellR.Y()- bthY*bcellR.Z());
    double bdbX = (bremR.X()  - bthX*bremR.Z());  // relative to hypo 
    double bdbY = (bremR.Y()  - bthY*bremR.Z());  
    double bdclX = (bclusR.X()- bthX*bclusR.Z()); // relative to cluster
    double bdclY = (bclusR.Y()- bthY*bclusR.Z()); 

    // extrapolate at Z = ShowerMax plane !!
    typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;
    Gaudi::XYZPoint origin(0,0,0);
    Gaudi::XYZVector vector(bthX,bthY,1.);
    Line line( origin ,vector );
    Gaudi::XYZPoint point;
    double mu;
    const Gaudi::Plane3D   plane = m_calo->plane(CaloPlane::ShowerMax);
    Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,point,mu);
    double bdpX= (bremR.X() - bthX*point.Z());
    double bdpY= (bremR.Y() - bthY*point.Z());    

    double thMin = -0.30;
    double thMax = +0.30;
    double step  = (thMax-thMin)/m_thBin;
    std::string bbinX = "bin"+Gaudi::Utils::toString( int((bthX-thMin)/step) );
    std::string bbinY = "bin"+Gaudi::Utils::toString( int((bthY-thMin)/step) );
    std::vector<std::string> bhat;
    bhat.push_back("");
    bhat.push_back(bbinX);
    bhat.push_back(bbinY);

    for(std::vector<std::string>::iterator iq=q.begin();iq< q.end();++iq){
      std::string  base = "BremAlign/Delta/" + *iq + "/";
      if(m_histo){
        for(std::vector<std::string>::iterator i=bhat.begin();i< bhat.end();++i){
          plot1D( bdceX , base+"Cell/"+*i+"/dX"  , "dX  : Cell-Brem(CellZ)"    , m_min, m_max, m_bin);
          plot1D( bdceY , base+"Cell/"+*i+"/dY"  , "dY  : Cell-Brem(CellZ)"    , m_min, m_max, m_bin);
          plot1D( bdclX , base+"Cell/"+*i+"/dX"  , "dX  : Cluster-Brem(ClusterZ)"    , m_min, m_max, m_bin);
          plot1D( bdclY , base+"Cell/"+*i+"/dY"  , "dY  : Cluster-Brem(ClusterZ)"    , m_min, m_max, m_bin);
          plot1D( bdbX ,  base+"Hypo/"+*i+"/dX", "dX  : Hypo-Brem(HypoZ)"      , m_min, m_max, m_bin);
          plot1D( bdbY ,  base+"Hypo/"+*i+"/dY", "dY  : Hypo-Brem(HypoZ)"      , m_min, m_max, m_bin);
          plot1D( bdpX ,  base+"HypoSM/"+*i+"/dX"   , "dX  : Hypo-Brem(ShowerMax)"  , m_min, m_max, m_bin);
          plot1D( bdpY ,  base+"HypoSM//"+*i+"/dY"  , "dY  : Hypo-Brem(ShowerMax)"  , m_min, m_max, m_bin);
        }
      }
      if(m_profil){
        profile1D( bthX , bdceX ,base+"Cell/dX.thX"  ,"dX.vs.thX: Cell-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdceX ,base+"Cell/dX.thY"  ,"dX.vs.thY: Cell-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , bdclX ,base+"Cluster/dX.thX","dX.vs.thX: Cluster-Brem(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdclX ,base+"Cluster/dX.thY","dX.vs.thY: Cluster-Brem(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , bdbX  ,base+"Hypo/dX.thX","dX.vs.thX: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdbX  ,base+"Hypo/dX.thY","dX.vs.thY: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , bdpX  ,base+"HypoSM/dX.thX"  ,"dX.vs.thX: Hypo-Brem(ShoweMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdpX  ,base+"HypoSM/dX.thY"  ,"dX.vs.thY: Hypo-Brem(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , bdceY ,base+"Cell/dY.thX"  ,"dY.vs.thX: Hypo-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdceY ,base+"Cell/dY.thY"  ,"dY.vs.thY: Hypo-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , bdbY  ,base+"Hypo/dY.thX"  ,"dY.vs.thX: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdbY  ,base+"Hypo/dY.thY"  ,"dY.vs.thY: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , bdpY  ,base+"HypoSM/dY.thX","dY.vs.thX: Hypo-Brem(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , bdpY  ,base+"HypoSM/dY.thY","dY.vs.thY: Hypo-Brem(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
      }
    }
    
    // S-shape
    double bsize = m_calo->cellSize(bid);
    double batx =  (bthX*bremR.Z()  - bcellR.X())/bsize; // brem provides the 'true' position @ HypoZ
    double baty =  (bthY*bremR.Z()  - bcellR.Y())/bsize;
    double batxSM =(bthX*point.Z()  - bcellR.X())/bsize; // brem provides the 'true' position @ ShowerMax
    double batySM =(bthY*point.Z()  - bcellR.Y())/bsize;

    double babx =  (bclusR.X()      - bcellR.X())/bsize; // e-weighted barycenter (cluster)
    double baby =  (bclusR.Y()      - bcellR.Y())/bsize; 

    double bacx =  (bremR.X()       - bcellR.X())/bsize; // S-shape corrected barycenter (hypo)
    double bacy =  (bremR.Y()       - bcellR.Y())/bsize; 



    for(std::vector<std::string>::iterator is = split.begin();is <split.end() ; ++is){
      std::string base = "BremAlign/Sshape/" + *is;
      if(m_histo){
        plot2D( babx , batx , base + "Cluster/X"  , "bremX(HypoZ) .vs. clusterX " , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( baby , baty , base + "Cluster/Y"  , "bremY(HypoZ) .vs. clusterY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( bacx , batx , base + "Hypo/X"  , "bremX(HypoZ) .vs. hypoX" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( bacy , baty , base + "Hypo/Y"  , "bremY(HypoZ) .vs. hypoY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);    
        plot2D( babx , batxSM , base + "ClusterSM/X"  , "bremX(ShowerMax) .vs. clusterX" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( baby , batySM , base + "ClusterSM/Y"  , "bremY(ShowerMax) .vs. clusterY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( bacx , batxSM , base + "HypoSM/X"  , "bremX(ShowerMax) .vs. hypoX" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( bacy , batySM , base + "HypoSM/Y"  , "bremY(ShowerMax) .vs. hypoY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
      }
      if(m_profil){
        profile1D( babx , batx , base + "Cluster/profX"  , "bremX(HypoZ) .vs. clusterX " , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);     
        profile1D( baby , baty , base + "Cluster/profY"  , "bremY(HypoZ) .vs. clusterY" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
        profile1D( bacx , batx , base + "Hypo/profX"  , "bremX(HypoZ) .vs. hypoX" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
        profile1D( bacy , baty , base + "Hypo/profY"  , "bremY(HypoZ) .vs. hypoY" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);    
        profile1D( babx , batxSM , base + "ClusterSM/profX"  , "bremX(ShowerMax) .vs. clusterX" , -0.6,+0.6 , 50 ,prof,-0.6,0.6);
        profile1D( baby , batySM , base + "ClusterSM/profY"  , "bremY(ShowerMax) .vs. clusterY" , -0.6,+0.6 , 50 ,prof,-0.6,0.6);
        profile1D( bacx , batxSM , base + "HypoSM/profX"  , "bremX(ShowerMax) .vs. hypoX" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
        profile1D( bacy , batySM , base + "HypoSM/profY"  , "bremY(ShowerMax) .vs. hypoY" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
      }
    }
    
    
    // ----- track extrapolator alignment
    double thX= hypoR.X()/hypoR.Z();
    double thY= hypoR.Y()/hypoR.Z();
    double dsX = (hypoR.X() -  statR.X()); // extrapolate @ ShowerMax plane
    double dsY = (hypoR.Y() -  statR.Y());
    double dcX = (hypoR.X() -  closR.X()); // extrapolate @ closest state
    double dcY = (hypoR.Y() -  closR.Y());
    double dceX = (cellR.X()- statR.X()); // relative to cell 
    double dceY = (cellR.Y()- statR.Y());
    double dclX = (clusR.X()- statR.X()); // relative to cluster
    double dclY = (clusR.Y()- statR.Y()); 

    // plot delta's
    std::string binX = "bin"+Gaudi::Utils::toString( int((thX-thMin)/step) );
    std::string binY = "bin"+Gaudi::Utils::toString( int((thY-thMin)/step) );
    std::vector<std::string> hat;
    hat.push_back("all");
    hat.push_back(binX);
    hat.push_back(binY);

    for(std::vector<std::string>::iterator iq=q.begin();iq< q.end();++iq){
      std::string base = "ElectronAlign/Delta/" + *iq + "/";
      if(m_histo){
        for(std::vector<std::string>::iterator i=hat.begin();i< hat.end();++i){
          plot1D( dceX , base+"Cell/"+*i+"/dX"   , "dX  : Cell-Track(CellZ)" , m_min, m_max, m_bin);
          plot1D( dceY , base+"Cell/"+*i+"/dY"   , "dY  : Cell-Track(CellZ)" , m_min, m_max, m_bin);
          plot1D( dclX , base+"Cluster/"+*i+"/dX"   , "dX  : Cluster-Track(ClusterZ)" , m_min, m_max, m_bin);
          plot1D( dclY , base+"Cluster/"+*i+"/dY"   , "dY  : Cluster-Track(ClusterZ)" , m_min, m_max, m_bin);
          plot1D( dsX ,  base+"HypoSM/"+*i+"/dX"   ,"dX  : Hypo-Track(ShowerMax)"  , m_min, m_max, m_bin);
          plot1D( dsY ,  base+"HypoSM/"+*i+"/dY"   ,"dY  : Hypo-Track(ShowerMax)"  , m_min, m_max, m_bin);
          plot1D( dcX ,  base+"Hypo/"+*i+"/dX" ,"dX  : Hypo-Track(HypoZ)"      , m_min, m_max, m_bin);
          plot1D( dcY ,  base+"Hypo/"+*i+"/dY" ,"dY  : Hypo-Track(HypoZ)"      , m_min, m_max, m_bin);
        }
      }
      if(m_profil){
        profile1D( bthX , dceX ,base+"Cell/dX.thX"  ,"dX.vs.thX: Cell-Track(CellZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dceX ,base+"Cell/dX.thY"  ,"dX.vs.thY: Cell-Track(CellZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dclX ,base+"Cluster/dX.thX","dX.vs.thX: Cluster-Track(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dclX ,base+"Cluster/dX.thY","dX.vs.thY: Cluster-Track(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dcX  ,base+"Hypo/dX.thX","dX.vs.thX: Hypo-Track(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dcX  ,base+"Hypo/dX.thY","dX.vs.thY: Hypo-Track(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dsX  ,base+"HypoSM/dX.thX"  ,"dX.vs.thX: Hypo-Track(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dsX  ,base+"HypoSM/dX.thY"  ,"dX.vs.thY: Hypo-Track(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dceY ,base+"Cell/dY.thX"  ,"dY.vs.thX: Hypo-Track(CellZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dceY ,base+"Cell/dY.thY"  ,"dY.vs.thY: Hypo-Track(CellZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dclY ,base+"Cluster/dY.thX"  ,"dY.vs.thX: Hypo-Track(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dclY ,base+"Cluster/dY.thY"  ,"dY.vs.thY: Hypo-Track(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dcY  ,base+"Hypo/dY.thX","dY.vs.thX: Hypo-Track(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dcY  ,base+"Hypo/dY.thY","dY.vs.thY: Hypo-Track(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthX , dsY  ,base+"HypoSM/dY.thX"  ,"dY.vs.thX: Hypo-Track(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        profile1D( bthY , dsY  ,base+"HypoSM/dY.thY"  ,"dY.vs.thY: Hypo-Track(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
      }
    }
    
    // S-shape
    double size = m_calo->cellSize(id);
    double atx =(  statR.X()       - cellR.X())/size; // track propagation provides the 'true' position @ HypoZ
    double aty =(  statR.Y()       - cellR.Y())/size; 
    double atxSM=(  statR.X()       - cellR.X())/size; // track propagation provides the 'true' position @ ShowerMax
    double atySM =(  statR.Y()       - cellR.Y())/size;
    double abx =(  clusR.X()       - cellR.X())/size; // e-weighted barycenter (cluster)
    double aby =(  clusR.Y()       - cellR.Y())/size;
    double acx =(  hypoR.X()       - cellR.X())/size; // S-shape corrected barycenter (hypo)
    double acy =(  hypoR.Y()       - cellR.Y())/size; 


    for(std::vector<std::string>::iterator is = split.begin();is <split.end() ; ++is){
      std::string base = "ElectronAlign/Sshape/" + *is;
      if(m_histo){
        plot2D( abx , atx , base + "Cluster/X"  , "trackX(HypoZ) .vs. clusterX " , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( aby , aty , base + "Cluster/Y"  , "trackY(HypoZ) .vs. clusterY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( acx , atx , base + "Hypo/X"  , "trackX(HypoZ) .vs. hypoX" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( acy , aty , base + "Hypo/Y"  , "trackY(HypoZ) .vs. hypoY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);    
        plot2D( abx , atxSM , base + "ClusterSM/X"  , "trackX(ShowerMax) .vs. clusterX" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( aby , atySM , base + "ClusterSM/Y"  , "trackY(ShowerMax) .vs. clusterY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( acx , atxSM , base + "HypoSM/X"  , "trackX(ShowerMax) .vs. hypoX" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
        plot2D( acy , atySM , base + "HypoSM/Y"  , "trackY(ShowerMax) .vs. hypoY" , -0.6 , +0.6 , -0.6 , +0.6 , 50, 50);
      }
      if(m_profil){
        profile1D( abx , atx , base + "Cluster/profX"  , "trackX(HypoZ) .vs. clusterX " , -0.6 , +0.6 , 50 , prof, -0.6, 0.6); 
        profile1D( aby , aty , base + "Cluster/profY"  , "trackY(HypoZ) .vs. clusterY" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
        profile1D( acx , atx , base + "Hypo/profX"  , "trackX(HypoZ) .vs. hypoX" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
        profile1D( acy , aty , base + "Hypo/profY"  , "trackY(HypoZ) .vs. hypoY" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);    
        profile1D( abx , atxSM , base + "ClusterSM/profX","trackX(ShowerMax) .vs. clusterX" , -0.6 , +0.6,50 ,prof, -0.6, 0.6);
        profile1D( aby , atySM , base + "ClusterSM/profY","trackY(ShowerMax) .vs. clusterY" , -0.6 , +0.6,50 ,prof, -0.6, 0.6);
        profile1D( acx , atxSM , base + "HypoSM/profX"  , "trackX(ShowerMax) .vs. hypoX" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
        profile1D( acy , atySM , base + "HypoSM/profY"  , "trackY(ShowerMax) .vs. hypoY" , -0.6 , +0.6 , 50 , prof, -0.6, 0.6);
      }
    }
    
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloAlignmentNtp::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
