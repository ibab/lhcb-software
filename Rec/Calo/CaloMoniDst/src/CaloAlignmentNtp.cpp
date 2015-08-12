// Include files 

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
DECLARE_ALGORITHM_FACTORY( CaloAlignmentNtp )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloAlignmentNtp::CaloAlignmentNtp( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : CaloNtpBase ( name , pSvcLocator )
  , m_estimator(NULL)
{

  // base properties default
  m_input =  LHCb::ProtoParticleLocation::Charged;
  m_prs  = std::make_pair(10.,1024);
  m_spd  = std::make_pair(.5,9999.);
  m_tracks.clear();
  m_tracks.push_back(LHCb::Track::Long);
  m_nSpd=std::make_pair(0.,250.);
  m_nTrk=std::make_pair(0.,350.);
  m_inArea = true;

  // specific properties
  declareProperty("EoPFilter" , m_eop = std::make_pair(0.7,1.3));
  declareProperty("DLLeFilter" , m_dlle  = std::make_pair(-9999999.,9999999.));
  declareProperty("RichDLLeFilter" , m_rdlle  = std::make_pair(0.,99999.));
  declareProperty("BremMatchFilter"    , m_bMatch  = std::make_pair(-99999999.,99999999.));
  declareProperty("ElectronMatchFilter", m_eMatch  = std::make_pair(-999999.,999999.));
  declareProperty("ClusterMatchFilter" , m_cMatch  = std::make_pair(-999999.,999999.));
  declareProperty("EmlectronPairing", m_pairing = true );
  declareProperty("MassFilter", m_mas=std::make_pair(-99999.,99999.));
  declareProperty("DeltaBin" , m_bin   = 150 );
  declareProperty("DeltaMin" , m_min   = -150. );
  declareProperty("DeltaMax" , m_max   = +150. );
  declareProperty("ThetaBin" , m_thBin = 14 );
  declareProperty("ElectronWithBremOnly"  , m_brem=false);
  declareProperty("BremEleDistFilter" , m_dist  = std::make_pair(4.,99999.));
  declareProperty("SshapeRange" , m_r = 0.7);
  declareProperty("SshapeBin" , m_b = 50);
}
//=============================================================================
// Destructor
//=============================================================================
CaloAlignmentNtp::~CaloAlignmentNtp() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloAlignmentNtp::initialize() {
  StatusCode sc = CaloNtpBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if(m_mas.first != -99999 || m_mas.second != +99999)m_pairing=true; //activate pairing when mass filter is on

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


  if( !eventProcessing() ) return StatusCode::SUCCESS; // common processing
  counter("1-selected events") += 1;
  //
  LHCb::ProtoParticles* protos = get<LHCb::ProtoParticles> ( m_input );  
  for( LHCb::ProtoParticles::const_iterator p = protos->begin(); protos->end () != p ; ++p ){ 
    const LHCb::ProtoParticle* proto = *p;
    if( !caloElectron()->set(proto))continue;
    counter("2-selected protos")+=1;
    if( !acceptTrack( proto->track() ))continue;
    counter("3-selected tracks")+=1;
    LHCb::CaloHypo* hypo = caloElectron()->electron();
    if ( !hypoProcessing(hypo) ) continue;
    counter("4-selected hypos")+=1;
    LHCb::CaloCellID id = LHCb::CaloCellID();
    const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo);
    if( NULL != cluster)id=cluster->seed();    
    double eOp = caloElectron()->eOverP();
    if( !inRange( m_eop, eOp))continue;
    if( !inRange( m_dlle, proto->info(LHCb::ProtoParticle::CombDLLe , 9999.)))continue;
    if( !inRange( m_rdlle, proto->info(LHCb::ProtoParticle::RichDLLe, 9999.)))continue;
    if( !inRange( m_bMatch, proto->info(LHCb::ProtoParticle::CaloBremMatch, 9999.)))continue;
    if( !inRange( m_eMatch, proto->info(LHCb::ProtoParticle::CaloElectronMatch, 9999.)))continue;
    if( !inRange( m_cMatch, proto->info(LHCb::ProtoParticle::CaloTrMatch, 9999.)))continue;

    counter("5-selected electrons")+=1;

    // track extrapolation
    Gaudi::XYZPoint hypoR( hypo->position()->x(),hypo->position()->y(),hypo->position()->z());// hypo position
    Gaudi::XYZPoint clusR( cluster->position().x(),cluster->position().y(),cluster->position().z());// cluster          
    Gaudi::XYZPoint statR = caloElectron()->caloState().position();// caloState position
    Gaudi::XYZPoint cellR( calo()->cellX(id) , calo()->cellY(id), calo()->cellZ(id) );// seed position
    Gaudi::XYZPoint closR = caloElectron()->closestState().position();// closestState position

    Gaudi::LorentzVector cs(caloElectron()->caloState().momentum().x(),
                            caloElectron()->caloState().momentum().y(),
                            caloElectron()->caloState().momentum().z(),
                            caloElectron()->caloState().p());
    double theta = caloElectron()->caloState().momentum().Theta();

    Gaudi::LorentzVector t(proto->track()->momentum().x(),
                           proto->track()->momentum().y(),
                           proto->track()->momentum().z (),
                           proto->track()->p());
    StatusCode sc;


    // bremStrahlung extrapolation
    const LHCb::CaloHypo* brem = caloElectron()->bremstrahlung();
    bool hasBrem = ( brem == NULL ) ? false : true;
    if( m_brem && !hasBrem )continue; // is Brem explicitely requested

    
    double dist = -1.;    
    LHCb::CaloCellID bid = LHCb::CaloCellID();
    Gaudi::LorentzVector bP;
    Gaudi::XYZPoint bclusR;
    Gaudi::XYZPoint bcellR; 
    Gaudi::XYZPoint bremR;
    LHCb::CaloMomentum bremM ;
    
    // 

    if( hasBrem){
      bremM= caloElectron()->bremCaloMomentum();
      bP = Gaudi::LorentzVector ( bremM.momentum().x(),
                                  bremM.momentum().y(),
                                  bremM.momentum().z(),
                                  bremM.momentum().e());  
    
    // brem position(s)
      bremR =Gaudi::XYZPoint( brem->position()->x(),brem->position()->y(),brem->position()->z());// hypo 
      SmartRef<LHCb::CaloCluster> bcluster;
      if ( brem->clusters().size() > 0 ){
        bcluster= *(brem->clusters().begin());
        bid = (*bcluster).seed();      
      }
      bclusR=Gaudi::XYZPoint(bcluster->position().x(),bcluster->position().y(),bcluster->position().z());//clus
      bcellR=Gaudi::XYZPoint( calo()->cellX(bid) , calo()->cellY(bid), calo()->cellZ(bid) );// seed
      dist = sqrt(fabs(double(bid.row() - id.row())) * fabs(double(bid.row() - id.row()))+
                       fabs(double(bid.col() - id.col())) * fabs(double(bid.col() - id.col())));
      if( bid.area() == id.area() &&  !inRange( m_dist, dist) )continue; // brem-electron separation    
      counter("6-selected electron+Brem")+=1;
    }
    
    // perform electron pairing
    double mas = 9999.;
    if( m_pairing){
      for( LHCb::ProtoParticles::const_iterator pp = p+1 ; protos->end () != pp ; ++pp ){
        const LHCb::ProtoParticle* proto2 = *pp;
        if( !caloElectron()->set(proto2))continue;;

        LHCb::CaloHypo* hypo2 = caloElectron()->electron();
        if( hypo == hypo2 )continue;

        if( !hypoProcessing( hypo2 ) )continue;

        double eOp2 = caloElectron()->eOverP();
        if( !inRange( m_eop, eOp2))continue;
        if( !inRange( m_dlle, proto2->info(LHCb::ProtoParticle::CombDLLe, 0.)))continue;
        if( !inRange( m_rdlle, proto2->info(LHCb::ProtoParticle::RichDLLe, 0.)))continue;        
        // compute mass
        const LHCb::Track*  t1 = proto->track();
        const LHCb::Track*  t2 = proto2->track();
        if( !acceptTrack(t2))continue;
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
      LHCb::CaloMomentum momentum( hypo );
      sc=ntp->column("TrackMatch", proto->info(LHCb::ProtoParticle::CaloTrMatch, 9999.));
      sc=ntp->column("ElecMatch", proto->info(LHCb::ProtoParticle::CaloElectronMatch, 9999.));
      sc=ntp->column("BremMatch", proto->info(LHCb::ProtoParticle::CaloBremMatch, 9999.));
      sc=ntp->column("BremEleDist", dist);
      sc=ntp->column("Prs", toPrs()->energy ( *hypo , "Prs"  ) );
      sc=ntp->column("Spd", toSpd()->multiplicity(*hypo, "Spd") );
      sc=ntp->column("TrajectoryL", proto->info(LHCb::ProtoParticle::CaloTrajectoryL, 9999.));
      sc=ntp->column("VeloCharge", proto->info(LHCb::ProtoParticle::VeloCharge, -1.));
      sc=ntp->column("DLLe", proto->info(LHCb::ProtoParticle::CombDLLe, 0.));
      sc=ntp->column("RichDLLe", proto->info(LHCb::ProtoParticle::RichDLLe, 0.));

      // hypo info
      sc=ntp->column("EoP" , eOp );
      sc=ntp->column("hypoE", momentum.e()  );
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
      sc=ntp->column("caloStateErrX",sqrt( caloElectron()->caloState().covariance()(0,0)));
      sc=ntp->column("caloStateErrY",sqrt( caloElectron()->caloState().covariance()(1,1)));
      sc=ntp->column("caloStateErrTX",sqrt( caloElectron()->caloState().covariance()(2,2)));
      sc=ntp->column("caloStateErrTY",sqrt( caloElectron()->caloState().covariance()(3,3)));      
      sc=ntp->column("incidence",theta);


      // brem info
      sc=ntp->column("BremId", bid.index());
      sc=ntp->column("BremP", bP );
      sc=ntp->column("BremR", bremR  );
      sc=ntp->column("BremClusterR", bclusR  );
      sc=ntp->column("BremSeedR"   , bcellR  );

      double bStX=0;
      double bStY=0;
      double bStTX=0;
      double bStTY=0;
      double bStCX=0;
      double bStCY=0;
      
      if( hasBrem){
        LHCb::State* bState =  (LHCb::State*) proto->track()->stateAt(LHCb::State::BegRich1);
        if( bState == NULL ){
          bState = proto->track()->states()[0];
          debug() << proto->track()->states();
          Warning("BegRich1 state does not exists - used first state",StatusCode::SUCCESS).ignore();
        }
        bStX = sqrt( bState->covariance()(0,0) );
        bStY = sqrt( bState->covariance()(1,1) );
        bStTX = sqrt( bState->covariance()(2,2) );
        bStTY = sqrt( bState->covariance()(3,3) ) ;
        double dz = bremR.Z()-bState->z();
        bStCX =  bState->covariance()(0,0) +  bState->covariance()(2,2) * dz * dz +  bState->covariance()(0,2) * 2.*dz;
        bStCY =  bState->covariance()(1,1) +  bState->covariance()(3,3) * dz * dz +  bState->covariance()(1,3) * 2.*dz;
        bStCX = (bStCX > 0) ? sqrt( bStCX) : 0;
        bStCY = (bStCY > 0) ? sqrt( bStCY) : 0;
      }      
      sc=ntp->column("bStateErrX",bStX);
      sc=ntp->column("bStateErrY",bStY);
      sc=ntp->column("bStateErrTX",bStTX);
      sc=ntp->column("bStateErrTY",bStTY);
      sc=ntp->column("bCaloStateX",bStCX);
      sc=ntp->column("bCaloStateY",bStCY);
      // odin info
      sc=ntp->column("run"   , m_run         );
      sc=ntp->column("event" , (double) m_evt );
      sc=ntp->column("triggertype" , m_tty );
      sc=ntp->column("nSpd", nSpd() );
      sc=ntp->column("nTracks", nTracks() );
      sc=ntp->column("nVertices", nVertices() );      
      if(m_pairing)sc = ntp->column("MinMee",mas);      
      sc = ntp->write();
      counter("7-Events in ntuple")+=1;
    }
    
  
    // Histogramming
    if( !m_histo && !m_profil)return StatusCode::SUCCESS;

    std::string prof ="";
    

    counter("8-Events in histos")+=1;
    
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


    std::vector<std::string> bsplit;
    bsplit.push_back("all/");
    bsplit.push_back( barea+"/all/");
    bsplit.push_back( barea+"/"+charge+"/all/");
    bsplit.push_back( barea+"/"+bside+"/all/");
    bsplit.push_back( barea+"/"+bside+"/"+charge+"/all/");
    bsplit.push_back( barea+"/"+bside+"/"+ blevel +"/all/");
    bsplit.push_back( barea+"/"+bside+"/"+ blevel +"/"+charge+"/all/");
    bsplit.push_back( barea+"/"+blevel+"/all/");
    bsplit.push_back( barea+"/"+blevel+"/"+charge+"/all/");
    bsplit.push_back( barea+"/"+blevel+"/"+ bside +"/all/");
    bsplit.push_back( barea+"/"+blevel+"/"+ bside +"/"+charge+"/all/");
    

    std::vector<std::string> split;
    split.push_back("all/");
    split.push_back( area+"/all/");
    split.push_back( area+"/"+charge+"/all/");
    split.push_back( area+"/"+side+"/all/");
    split.push_back( area+"/"+side+"/"+charge+"/all/");
    split.push_back( area+"/"+side+"/"+ level +"/all/");
    split.push_back( area+"/"+side+"/"+ level +"/"+charge+"/all/");
    split.push_back( area+"/"+level+"/all/");
    split.push_back( area+"/"+level+"/"+charge+"/all/");
    split.push_back( area+"/"+level+"/"+ side +"/all/");
    split.push_back( area+"/"+level+"/"+ side +"/"+charge+"/all/");
    

    // ------ brem aligment (DeltaXX versus ThetaYY)
    double thMin = -0.30;
    double thMax = +0.30;
    double bthX= t.X()/t.Z();
    double bthY= t.Y()/t.Z();
    double step  = (thMax-thMin)/m_thBin;
    if( hasBrem ){
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
      const Gaudi::Plane3D   plane = calo()->plane(CaloPlane::ShowerMax);
      Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,point,mu);
      double bdpX= (bremR.X() - bthX*point.Z());
      double bdpY= (bremR.Y() - bthY*point.Z());    
      
      std::string bbinX = "bin"+Gaudi::Utils::toString( int((bthX-thMin)/step) );
      std::string bbinY = "bin"+Gaudi::Utils::toString( int((bthY-thMin)/step) );
      std::vector<std::string> bhat;
      bhat.push_back("");
      bhat.push_back(bbinX);
      bhat.push_back(bbinY);
      
      plot1D(eOp,"eOp control (all)", 0.5, 2.,100);
      for(std::vector<std::string>::iterator iq=q.begin();iq< q.end();++iq){
        std::string  base = "BremAlign/Delta/" + *iq + "/";
        if(m_histo){
          for(std::vector<std::string>::iterator i=bhat.begin();i< bhat.end();++i){
            plot1D( bdceX , base+"Cell/"+*i+"/dX"  , "dX  : Cell-Brem(CellZ)"    , m_min, m_max, m_bin);
            plot1D( bdceY , base+"Cell/"+*i+"/dY"  , "dY  : Cell-Brem(CellZ)"    , m_min, m_max, m_bin);
            plot1D( bdclX , base+"Cluster/"+*i+"/dX"  , "dX  : Cluster-Brem(ClusterZ)"    , m_min, m_max, m_bin);
            plot1D( bdclY , base+"Cluster/"+*i+"/dY"  , "dY  : Cluster-Brem(ClusterZ)"    , m_min, m_max, m_bin);
            plot1D( bdbX ,  base+"Hypo/"+*i+"/dX", "dX  : Hypo-Brem(HypoZ)"      , m_min, m_max, m_bin);
            plot1D( bdbY ,  base+"Hypo/"+*i+"/dY", "dY  : Hypo-Brem(HypoZ)"      , m_min, m_max, m_bin);
            plot1D( bdpX ,  base+"HypoSM/"+*i+"/dX"   , "dX  : Hypo-Brem(ShowerMax)"  , m_min, m_max, m_bin);
            plot1D( bdpY ,  base+"HypoSM//"+*i+"/dY"  , "dY  : Hypo-Brem(ShowerMax)"  , m_min, m_max, m_bin);
          }
        }
        if(m_profil){
          profile1D( bthX , bdceX ,base+"Cell/dX.thX"  ,"dX.vs.thX: Cell-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdceX ,base+"Cell/dX.thY"  ,"dX.vs.thY: Cell-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX ,bdclX ,base+"Cluster/dX.thX","dX.vs.thX: Cluster-Brem(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY ,bdclX ,base+"Cluster/dX.thY","dX.vs.thY: Cluster-Brem(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX , bdbX  ,base+"Hypo/dX.thX","dX.vs.thX: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdbX  ,base+"Hypo/dX.thY","dX.vs.thY: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX , bdpX  ,base+"HypoSM/dX.thX"  ,"dX.vs.thX: Hypo-Brem(ShoweMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdpX  ,base+"HypoSM/dX.thY"  ,"dX.vs.thY: Hypo-Brem(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX , bdceY ,base+"Cell/dY.thX"  ,"dY.vs.thX: Hypo-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdceY ,base+"Cell/dY.thY"  ,"dY.vs.thY: Hypo-Brem(CellZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX , bdclY,base+"Cluster/dY.thX","dY.vs.thX: Cluster-Brem(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdclY,base+"Cluster/dY.thY","dY.vs.thY: Cluster-Brem(ClusterZ)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX , bdbY  ,base+"Hypo/dY.thX"  ,"dY.vs.thX: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdbY  ,base+"Hypo/dY.thY"  ,"dY.vs.thY: Hypo-Brem(HypoZ)"    ,thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthX , bdpY  ,base+"HypoSM/dY.thX","dY.vs.thX: Hypo-Brem(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
          profile1D( bthY , bdpY  ,base+"HypoSM/dY.thY","dY.vs.thY: Hypo-Brem(ShowerMax)",thMin,thMax,m_thBin,prof,m_min,m_max);
        }
      }
      
      // S-shape
      double bsize = calo()->cellSize(bid);
      double batx =  (bthX*bremR.Z()  - bcellR.X())/bsize; // brem provides the 'true' position @ HypoZ
      double baty =  (bthY*bremR.Z()  - bcellR.Y())/bsize;
      double batxSM =(bthX*point.Z()  - bcellR.X())/bsize; // brem provides the 'true' position @ ShowerMax
      double batySM =(bthY*point.Z()  - bcellR.Y())/bsize;
      
      double babx =  (bclusR.X()      - bcellR.X())/bsize; // e-weighted barycenter (cluster)
      double baby =  (bclusR.Y()      - bcellR.Y())/bsize; 
      
      double bacx =  (bremR.X()       - bcellR.X())/bsize; // S-shape corrected barycenter (hypo)
      double bacy =  (bremR.Y()       - bcellR.Y())/bsize; 
      
      
      
      
      for(std::vector<std::string>::iterator is = bsplit.begin();is <bsplit.end() ; ++is){
        std::string base = "BremAlign/Sshape/" + *is;
        if(m_histo){
          plot2D(batx, babx , base + "Cluster/X"  , "bremX(HypoZ) / clusterX " ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
          plot2D(baty, baby , base + "Cluster/Y"  , "bremY(HypoZ) / clusterY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
          plot2D(batx, bacx , base + "Hypo/X"  , "bremX(HypoZ) / hypoX" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
          plot2D(baty, bacy , base + "Hypo/Y"  , "bremY(HypoZ) / hypoY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);    
          plot2D(batxSM, babx , base + "ClusterSM/X"  , "bremX(ShowerMax) / clusterX" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
          plot2D(batySM, baby , base + "ClusterSM/Y"  , "bremY(ShowerMax) / clusterY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
          plot2D(batxSM, bacx , base + "HypoSM/X"  , "bremX(ShowerMax) / hypoX" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
          plot2D(batySM, bacy , base + "HypoSM/Y"  , "bremY(ShowerMax) / hypoY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        }
        if(m_profil){
          base = "BremAlign/Sshape/" + *is;
          profile1D(batx, babx , base + "Cluster/profX"  , "bremX(HypoZ) / clusterX " ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);     
          profile1D(baty, baby , base + "Cluster/profY"  , "bremY(HypoZ) / clusterY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          profile1D(batx, bacx , base + "Hypo/profX"  , "bremX(HypoZ) / hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          profile1D(baty, bacy , base + "Hypo/profY"  , "bremY(HypoZ) / hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);    
          profile1D(batxSM, babx , base + "ClusterSM/profX", "bremX(ShowerMax) / clusterX" ,-1*m_r,+1*m_r, m_b ,prof,-1*m_r,1*m_r);
          profile1D(batySM, baby , base + "ClusterSM/profY", "bremY(ShowerMax) / clusterY" ,-1*m_r,+1*m_r, m_b ,prof,-1*m_r,1*m_r);
          profile1D(batxSM, bacx , base + "HypoSM/profX"  , "bremX(ShowerMax) / hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          profile1D(batySM, bacy , base + "HypoSM/profY"  , "bremY(ShowerMax) / hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          
          base = "BremAlign/iSshape/" + *is;
          profile1D(babx,batx,  base + "Cluster/profX"  , "bremX(HypoZ) .vs. clusterX " ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);     
          profile1D(baby,baty,  base + "Cluster/profY"  , "bremY(HypoZ) .vs. clusterY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          profile1D(bacx,batx,  base + "Hypo/profX"  , "bremX(HypoZ) .vs. hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          profile1D(bacy,baty,  base + "Hypo/profY"  , "bremY(HypoZ) .vs. hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);    
          profile1D(babx,batxSM, base + "ClusterSM/profX","bremX(ShowerMax) .vs. clusterX",-1*m_r,+1*m_r, m_b ,prof,-1*m_r,1*m_r);
          profile1D(baby,batySM, base + "ClusterSM/profY","bremY(ShowerMax) .vs. clusterY",-1*m_r,+1*m_r, m_b ,prof,-1*m_r,1*m_r);
          profile1D(bacx,batxSM, base + "HypoSM/profX"  , "bremX(ShowerMax) .vs. hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
          profile1D(bacy,batySM, base + "HypoSM/profY"  , "bremY(ShowerMax) .vs. hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);          
        }
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
    double size = calo()->cellSize(id);
    double atx =(  closR.X()       - cellR.X())/size; // track propagation provides the 'true' position @ HypoZ
    double aty =(  closR.Y()       - cellR.Y())/size; 
    double atxSM=(  statR.X()       - cellR.X())/size; // track propagation provides the 'true' position @ ShowerMax
    double atySM =(  statR.Y()       - cellR.Y())/size;
    double abx =(  clusR.X()       - cellR.X())/size; // e-weighted barycenter (cluster)
    double aby =(  clusR.Y()       - cellR.Y())/size;
    double acx =(  hypoR.X()       - cellR.X())/size; // S-shape corrected barycenter (hypo)
    double acy =(  hypoR.Y()       - cellR.Y())/size; 


    for(std::vector<std::string>::iterator is = split.begin();is <split.end() ; ++is){
      std::string base = "ElectronAlign/Sshape/" + *is;
      if(m_histo){
        plot2D(atx, abx , base + "Cluster/X"  , "trackX(bestZ) .vs. clusterX " ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        plot2D(aty, aby , base + "Cluster/Y"  , "trackY(bestZ) .vs. clusterY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        plot2D(atx, acx , base + "Hypo/X"  , "trackX(bestZ) .vs. hypoX" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        plot2D(aty, acy , base + "Hypo/Y"  , "trackY(bestZ) .vs. hypoY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);    
        plot2D(atxSM, abx , base + "ClusterSM/X"  , "trackX(ShowerMax) .vs. clusterX" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        plot2D(atySM, aby , base + "ClusterSM/Y"  , "trackY(ShowerMax) .vs. clusterY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        plot2D(atxSM, acx , base + "HypoSM/X"  , "trackX(ShowerMax) .vs. hypoX" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
        plot2D(atySM, acy , base + "HypoSM/Y"  , "trackY(ShowerMax) .vs. hypoY" ,-1*m_r,+1*m_r,-1*m_r,+1*m_r, m_b, m_b);
      }
      if(m_profil){
        profile1D(atx, abx , base + "Cluster/profX"  , "trackX(bestZ) .vs. clusterX " ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r); 
        profile1D(aty, aby , base + "Cluster/profY"  , "trackY(bestZ) .vs. clusterY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        profile1D(atx, acx , base + "Hypo/profX"  , "trackX(bestZ) .vs. hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        profile1D(aty, acy , base + "Hypo/profY"  , "trackY(bestZ) .vs. hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);    
        profile1D(atxSM, abx , base + "ClusterSM/profX","trackX(ShowerMax) .vs. clusterX" ,-1*m_r,+1*m_r,m_b ,prof,-1*m_r,1*m_r);
        profile1D(atySM, aby , base + "ClusterSM/profY","trackY(ShowerMax) .vs. clusterY" ,-1*m_r,+1*m_r,m_b ,prof,-1*m_r,1*m_r);
        profile1D(atxSM, acx , base + "HypoSM/profX"  , "trackX(ShowerMax) .vs. hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        profile1D(atySM, acy , base + "HypoSM/profY"  , "trackY(ShowerMax) .vs. hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        base = "ElectronAlign/iSshape/" + *is;
        profile1D(abx,atx,    base + "Cluster/profX"  , "trackX(bestZ) .vs. clusterX " ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r); 
        profile1D(aby,aty,    base + "Cluster/profY"  , "trackY(bestZ) .vs. clusterY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        profile1D(acx,atx,    base + "Hypo/profX"  , "trackX(bestZ) .vs. hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        profile1D(acy,aty,    base + "Hypo/profY"  , "trackY(bestZ) .vs. hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);    
        profile1D(abx,atxSM,  base + "ClusterSM/profX","trackX(ShowerMax) .vs. clusterX" ,-1*m_r,+1*m_r,m_b ,prof,-1*m_r,1*m_r);
        profile1D(aby,atySM,  base + "ClusterSM/profY","trackY(ShowerMax) .vs. clusterY" ,-1*m_r,+1*m_r,m_b ,prof,-1*m_r,1*m_r);
        profile1D(acx,atxSM,  base + "HypoSM/profX"  , "trackX(ShowerMax) .vs. hypoX" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
        profile1D(acy,atySM,  base + "HypoSM/profY"  , "trackY(ShowerMax) .vs. hypoY" ,-1*m_r,+1*m_r, m_b , prof,-1*m_r,1*m_r);
      }
    }
    
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
