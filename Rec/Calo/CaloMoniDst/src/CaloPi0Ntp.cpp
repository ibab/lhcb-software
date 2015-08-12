#include <vector>
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloMomentum.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/ODIN.h" 
#include "CaloPi0Ntp.h"
#include "Event/RecVertex.h"
#include "Event/L0DUReport.h"

DECLARE_ALGORITHM_FACTORY( CaloPi0Ntp )


CaloPi0Ntp::CaloPi0Ntp( const std::string &name, ISvcLocator *pSvcLocator )
  : GaudiTupleAlg( name, pSvcLocator )
  , m_odin(NULL)
  , m_calo(NULL)
  , m_toSpd(NULL)
  , m_toPrs(NULL){ 
  declareProperty( "PhotonPt"      , m_ppt      = std::make_pair(250. , 15000.) );
  declareProperty( "Isolation"     , m_isol     = std::make_pair(0.   , 9999. ) ); // Warning  : a cut biases the pi0 mass
  declareProperty( "Conversion"    , m_conv     = std::make_pair(1    , 1 ) );
  declareProperty( "PrsE"          , m_prsE     = std::make_pair(0.   , 9999. ) );
  declareProperty( "Pt"            , m_pt       = std::make_pair(200.   , 15000 ) );
  declareProperty( "E"             , m_e        = std::make_pair(0.   , 500000) );
  declareProperty( "Mass"          , m_mass     = std::make_pair(50.  , 900.  ) );
  declareProperty( "Background"    , m_bkg      = false);
  declareProperty( "hMin"    , m_hMin      = 0.);
  declareProperty( "hMax"    , m_hMax      = 900.);
  declareProperty( "hBin"    , m_hBin      = 450);
  declareProperty( "etBin"    , m_etBin      = 150.);
  declareProperty( "leBin"    , m_leBin      = 0.25);
  declareProperty( "thBin"    , m_thBin      = 0.005); // (x1 inner, x2 middle, x4 outer)
  declareProperty( "spdBin"   , m_spdBin     = 50   );
  declareProperty( "Tuple"    , m_tuple = true);
  declareProperty( "Histo"    , m_histo = true);
  declareProperty( "Trend"    , m_trend = false);
  declareProperty( "VertexLoc", m_vertLoc = "");
  declareProperty( "UsePV3D", m_usePV3D = false);
}

CaloPi0Ntp::~CaloPi0Ntp(){}


StatusCode CaloPi0Ntp::initialize(){
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
  m_calo = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal);
  // get tools
  m_toSpd = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Spd" , this );
  m_toSpd->setCalos( "Ecal" ,"Spd");
  m_toPrs = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Prs" , this );
  m_toPrs->setCalos( "Ecal" ,"Prs");
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);

  // set vertex location
  if(m_vertLoc == "")
    m_vertLoc=(m_usePV3D) ? LHCb::RecVertexLocation::Velo3D :  LHCb::RecVertexLocation::Primary;

  /*
  const CaloVector<CellParam>& cells = m_calo->cellParams();
  for( CaloVector<CellParam>::const_iterator ic = cells.begin() ; ic != cells.end() ; ++ic){
    if( !ic->valid() )continue;
    if( ic->cellID().isPin() )continue;
    debug() << "       " << ic->cellID().index() << "   " << CaloCellCode::caloArea(2,ic->cellID().area() ) << endmsg;
  }
  */

  return StatusCode::SUCCESS;
}


// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloPi0Ntp::execute(){

  typedef LHCb::CaloHypo::Container    Hypos;
  typedef LHCb::CaloHypo               Photon;




  Tuple ntp = NULL;
  if( m_tuple)ntp = nTuple(500, "pi0_tupling" ,CLID_ColumnWiseTuple);


  if( !exist<Hypos>(LHCb::CaloHypoLocation::Photons) )return Error("Photon hypo container not found", StatusCode::SUCCESS );
  Hypos* hypos = get<Hypos>( LHCb::CaloHypoLocation::Photons );
  if ( 0 == hypos ) return StatusCode::SUCCESS;

  m_spdMult = 0;
  std::string l0loc = LHCb::L0DUReportLocation::Default;
  if( exist<LHCb::L0DUReport>( l0loc)){
    LHCb::L0DUReport* l0   = get<LHCb::L0DUReport>( l0loc );
    m_spdMult = (int) l0->dataValue("Spd(Mult)");
  }
  

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




  bool ok = false;
  StatusCode sc;
  for( Hypos::iterator g1 = hypos->begin(); hypos->end() != g1; ++g1 ){
    Photon* p1 = *g1;
    if ( NULL == p1 ) continue;

    
    /*
    info() << "------- P1 " << (*(p1->clusters().begin()))->seed()<< endmsg;
    const std::vector<LHCb::CaloDigit*>& spds = m_toSpd->digits(*p1,"Spd");
    for ( std::vector<LHCb::CaloDigit*>::const_iterator is = spds.begin() ; is != spds.end() ;++is){
      info() << "m_toSpd : " << (*is)->cellID() << " " << (*is)->e() << endmsg;
    }
    info() << "m_toSpd->multiplicity "<< m_toSpd->multiplicity ( *p1 , "Spd") << endmsg;
    const SmartRefVector<LHCb::CaloDigit>& digs = p1->digits();
    for ( SmartRefVector<LHCb::CaloDigit>::const_iterator iss = digs.begin() ; iss != digs.end()  ; ++iss){
      info() << "extra digits : " << (*iss)->cellID() << " " << (*iss)->e() << endmsg;
    }   
    */

    LHCb::CaloMomentum mt1( p1 );
    if( !inRange( m_ppt ,  mt1.pt() ) )continue;
    if( !inRange( m_conv , m_toSpd->multiplicity ( *p1 , "Spd") ) )continue;
    if( !inRange( m_prsE , m_toPrs->energy ( *p1 , "Prs"  ) ) )continue;



    Gaudi::LorentzVector v1( mt1.momentum() );
    Gaudi::XYZPoint point1( p1->position()->x() , p1->position()->y() , p1->position()->z() );

    double prs1 = m_toPrs->energy ( *p1 , "Prs"  );
    int spd1 = m_toSpd->multiplicity ( *p1 , "Spd");

    LHCb::CaloCellID id1 = LHCb::CaloCellID();
    SmartRef<LHCb::CaloCluster> cl1;
    if ( p1->clusters().size() > 0 )cl1= *(p1->clusters().begin());
    if( cl1 != 0 )id1 = cl1->seed();      
    if( cl1 == 0)continue;
    
    Gaudi::LorentzVector c1( cl1->position().x(), cl1->position().y(), cl1->position().z(), cl1->e() );
    Gaudi::XYZVector pos1( cl1->position().x(),cl1->position().y(),cl1->position().z());
    Gaudi::LorentzVector cc1(  cl1->e()*pos1.X()/pos1.R(),cl1->e()*pos1.Y()/pos1.R(), cl1->e()*pos1.Z()/pos1.R(),cl1->e() );
    Gaudi::LorentzVector ccc1( cl1->e()*point1.X()/point1.R(),
                               cl1->e()*point1.Y()/point1.R(), cl1->e()*point1.Z()/point1.R(),cl1->e() );
    
    
    // loop over the second photon
    for( Hypos::iterator g2 = g1 + 1; hypos->end() != g2; ++g2 ){
      Photon* p2 = *g2; 
     if ( 0 == p2 ) continue;

     /*
       info() << "------- P2 " << (*(p2->clusters().begin()))->seed()<< endmsg;
       const std::vector<LHCb::CaloDigit*>& spds = m_toSpd->digits(*p2,"Spd");
       for ( std::vector<LHCb::CaloDigit*>::const_iterator is = spds.begin() ; is != spds.end() ;++is){
       info() << "m_toSpd : " << (*is)->cellID() << " " << (*is)->e() << endmsg;
       }
       info() << "m_toSpd->multiplicity "<< m_toSpd->multiplicity ( *p2 , "Spd") << endmsg;
       const SmartRefVector<LHCb::CaloDigit>& digs = p2->digits();
       for ( SmartRefVector<LHCb::CaloDigit>::const_iterator iss = digs.begin() ; iss != digs.end()  ; ++iss){
       info() << "extra digits : " << (*iss)->cellID() << " " << (*iss)->e() << endmsg;
       }
     */

     LHCb::CaloMomentum mt2( p2 );
     if( !inRange( m_ppt ,  mt2.pt() ) )continue;
     if( !inRange( m_conv , m_toSpd->multiplicity ( *p2 , "Spd") ) )continue;
     if( !inRange( m_prsE , m_toPrs->energy ( *p2 , "Prs"  ) ) )continue;

     Gaudi::LorentzVector v2( mt2.momentum() );
     Gaudi::XYZPoint point2( p2->position()->x() , p2->position()->y() , p2->position()->z() );
     
     Gaudi::LorentzVector pi0( v1 + v2 );
     
     // background shape from (x,y)->(-x,-y) symmetrized g2
     Gaudi::LorentzVector v2Sym( v2 );
     v2Sym.SetPx( -v2.Px() );
     v2Sym.SetPy( -v2.Py() );
     Gaudi::LorentzVector bkg( v1 + v2Sym );
     Gaudi::XYZPoint p2Sym( -p2->position()->x() , -p2->position()->y() , p2->position()->z() );
     
     bool isBkg = false;
      if(inRange( m_e   , bkg.e()    ) &&
         inRange( m_pt  , bkg.pt()    ) &&
         inRange( m_mass, bkg.mass() ) ) isBkg = true;
      bool isPi0 = false;
      if(inRange( m_e   , pi0.e()    ) &&
         inRange( m_pt  , pi0.pt()    ) &&
         inRange( m_mass, pi0.mass() ) ) isPi0 = true;
      
      
      
      if( !isPi0 && !isBkg)continue;

      // Get cellIDs
      LHCb::CaloCellID id2 = LHCb::CaloCellID();
      SmartRef<LHCb::CaloCluster> cl2;
      if ( p2->clusters().size() > 0 )cl2= *(p2->clusters().begin());
      if( cl2 != 0 )id2 = cl2->seed();      
      if( cl2 == 2)continue;



      // define pi0 area
      LHCb::CaloCellID id = (id1.area() == id2.area() ) ? id1 : LHCb::CaloCellID();

      // isolation criteria

      Gaudi::XYZVector vec    = point2 - point1;
      


      Gaudi::XYZVector vecSym = p2Sym - point1;
      double cSize = m_calo->cellSize( id1 );
      if( m_calo->cellSize( id2 ) >  cSize ) cSize = m_calo->cellSize( id2 );
      double isol = (cSize > 0) ?  vec.Rho() / cSize : 0;
      double isolSym = (cSize > 0) ?  vecSym.Rho() / cSize : 0;


      isPi0 = isPi0 &&  inRange( m_isol, isol );
      isBkg = isBkg &&  inRange( m_isol, isolSym );
      
      if( (isPi0 ) || (m_bkg && isBkg ) ){

        double prs2 = m_toPrs->energy ( *p2 , "Prs"  );
        int spd = 0;
        int spd2 = m_toSpd->multiplicity ( *p2 , "Spd");
        if(  spd2 >0 )spd +=1;
        if(  spd1 >0 )spd +=2;
        Gaudi::LorentzVector c2( cl2->position().x(), cl2->position().y(), cl2->position().z(), cl2->e() );
        double pi0m = isPi0 ? pi0.mass() : 0. ;
        double bkgm = isBkg ? bkg.mass() : 0. ;
        int typ = 0;
        if( isPi0) typ += 1;
        if( isBkg) typ += 2;
        // cluster mass
        Gaudi::XYZVector pos2( cl2->position().x(),cl2->position().y(),cl2->position().z());
        Gaudi::LorentzVector cc2(  cl2->e()*pos2.X()/pos2.R(),cl2->e()*pos2.Y()/pos2.R(), cl2->e()*pos2.Z()/pos2.R(),cl2->e() );
        Gaudi::LorentzVector ccc2( cl2->e()*point2.X()/point2.R(),
                                   cl2->e()*point2.Y()/point2.R(), cl2->e()*point2.Z()/point2.R(),cl2->e() );
        Gaudi::LorentzVector cc(cc1 + cc2);
        Gaudi::LorentzVector ccc(ccc1 + ccc2);
        double ccmas = (isPi0) ? cc.mass() : 0;
        double cccmas = (isPi0) ? ccc.mass() : 0;
        

        counter("candidates for #PV="+ Gaudi::Utils::toString(nVert) + " ["+ m_vertLoc+"]")+=1;


        if( m_tuple){
          sc=ntp->column("p1" , v1);
          sc=ntp->column("p2" , v2);
          sc=ntp->column("r1" , point1);
          sc=ntp->column("r2" , point2);
          sc=ntp->column("prs1", prs1);
          sc=ntp->column("prs2", prs2);
          sc=ntp->column("spd", spd );
          sc=ntp->column("id1", id1.index() );
          sc=ntp->column("id2", id2.index() );
          sc=ntp->column("cl1", c1);
          sc=ntp->column("cl2", c2);
          sc=ntp->column("mass",pi0m);
          if(m_bkg)sc=ntp->column("bkg",bkgm );
          sc=ntp->column("type", typ);
          sc=ntp->column("p", pi0  );
          sc=ntp->column("clmass",ccmas);
          sc=ntp->column("clEmass",cccmas);
          // odin info
          sc=ntp->column("run"   , run         );
          sc=ntp->column("event" , (double) evt );
          sc=ntp->column("triggertype" , tty );
          // #vertices
          sc=ntp->column("Nvertices", nVert);
          // #SpdMult
          sc=ntp->column("spdMult", m_spdMult);
          sc=ntp->write();
          ok = true;
        }        
        if( !isPi0) continue;
        // histograms for tuning
        if( m_histo){
          hTuning("Cluster", spd ,prs1,prs2, ccc1, id1 , ccc2 , id2,nVert);
          hTuning("Corrected", spd , prs1,prs2,v1, id1 , v2 , id2,nVert);
        }
        if ( m_trend && spd == 0){
          std::string sNpv = "PV" + Gaudi::Utils::toString( nVert ) +"/";
          std::string sRun  =  "r" + Gaudi::Utils::toString( run   ) +"/";
          std::string base = "Trend/";
          plot1D(pi0m, base+"allPV/allRun/mass","di-photon mass spectrum for all run & allPV" , m_hMin, m_hMax, m_hBin);
          plot1D(pi0m, base+"allPV/"+sRun+"mass","di-photon mass spectrum for all PV & run = "+sRun , m_hMin, m_hMax, m_hBin); 
          if(id1.area() == id2.area()){
              std::string sarea = id1.areaName() ;
              plot1D(pi0m, base+"allPV/"+sRun+sarea,"di-photon mass spectrum for all PV & run = "+sRun , m_hMin, m_hMax, m_hBin); 
          }
          plot1D(pi0m, base+sNpv+sRun+"mass","di-photon mass spectrum for PV="+sNpv+" (run = "+sRun+")" , m_hMin, m_hMax, m_hBin);
          plot1D(pi0m, base+sNpv+"allRun/mass","di-photon mass spectrum for PV="+sNpv+" (all run)" , m_hMin, m_hMax, m_hBin);
        }
      }
    }
  }
  if(ok)counter("Events in tuple") += 1;
  if( sc.isFailure())Warning("Error with ntupling",StatusCode::SUCCESS).ignore();
  return StatusCode::SUCCESS;
}

bool CaloPi0Ntp::inRange( std::pair<double,double> range, double value){
  return ( value >= range.first) && (value <= range.second);
}

bool CaloPi0Ntp::inRange( std::pair<int,int> range, int value){
  return ( value == 0 &&   range.first == 1 ) || (value > 0 && range.second == 1);
}


std::vector<double> CaloPi0Ntp::toVector(const Gaudi::LorentzVector& vec){
  std::vector<double> temp;
  temp.push_back( vec.E() );
  temp.push_back( vec.Px() );
  temp.push_back( vec.Py() );
  temp.push_back( vec.Pz() );
  return temp;
}

std::vector<double> CaloPi0Ntp::toVector(const Gaudi::XYZPoint& point){
  std::vector<double> temp;
  temp.push_back( point.X() );
  temp.push_back( point.Y() );
  temp.push_back( point.Z() );
  return temp;  
}





void CaloPi0Ntp::hTuning(std::string base, int spd,double prs1, double prs2,
                         const Gaudi::LorentzVector c1 , const LHCb::CaloCellID id1, 
                         const Gaudi::LorentzVector c2 , const LHCb::CaloCellID id2,int nVert){


  std::string s1 = id1.areaName() ;
  std::string s2 = id2.areaName() ;
  std::string sarea = ( id1.area() > id2.area() )? s1+s2 : s2+s1;
  // log(E) bins
  int ble1 = int( (log( c1.e() )/m_leBin) );
  int ble2 = int( (log( c2.e() )/m_leBin) );
  // Et bins
  int bet1 = int( c1.Pt() / m_etBin );
  int bet2 = int( c2.Pt() / m_etBin );
  // theta bins
  int bth1 = int( c1.Theta() / m_thBin/pow(2.,(double)2-id1.area() ));
  int bth2 = int( c2.Theta() / m_thBin/pow(2.,(double)2-id2.area() ));

  // spd bins
  int spdslot = int( m_spdMult / m_spdBin);
  
  std::string sspd;
  if ( spd==0 ) sspd = "gg";
  else if (spd==1 || spd == 2) sspd = "gee";
  else if( spd==3) sspd = "eeee";

  int prs = 0;
  if( prs1 > 5)prs += 1;
  if( prs2 > 5)prs += 1;

  std::string sprs = "prs" + Gaudi::Utils::toString( prs );


  Gaudi::LorentzVector di( c1+c2);

  // pi0->gg versus nPV
  std::string sVert = "PV" + Gaudi::Utils::toString( nVert );
  plot1D(di.mass(), base+"/"+sVert+"/all" , base+"/all  #PV="+Gaudi::Utils::toString( nVert ) , m_hMin, m_hMax, m_hBin);
  plot1D(di.mass(), base+"/"+sVert+"/"+sspd+"/all" , 
         base+"/"+sspd+"/all  #PV="+Gaudi::Utils::toString( nVert ) , m_hMin, m_hMax, m_hBin);
  double Y1 = m_calo->cellCenter( id1 ).Y();
  double Y2 = m_calo->cellCenter( id2 ).Y();
  if( fabs(Y1)<300 && fabs(Y2)<300){
    plot1D(di.mass(), base+"/"+sVert+"/band" , base+"/band  #PV="+Gaudi::Utils::toString( nVert ) , m_hMin, m_hMax, m_hBin);
    plot1D(di.mass(), base+"/"+sVert+"/"+sspd+"/band" , 
           base+"/"+sspd+"/band  #PV="+Gaudi::Utils::toString( nVert ) , m_hMin, m_hMax, m_hBin);
  }
  


  // pi0->gg versus spdMult
  std::string nSpd = "Spd" + Gaudi::Utils::toString( spdslot*m_spdBin ); 
  plot1D(di.mass(), base+"/"+nSpd+"/all" , base+"/all  #Spd="+Gaudi::Utils::toString( spdslot*m_spdBin ),m_hMin,m_hMax,m_hBin);
  plot1D(di.mass(), base+"/"+nSpd+"/"+sspd+"/all" , 
         base+"/"+sspd+"/all  #Spd="+Gaudi::Utils::toString( spdslot*m_spdBin ),m_hMin,m_hMax,m_hBin);
    
  if( fabs(Y1)<300 && fabs(Y2)<300){
    plot1D(di.mass(),base+"/"+nSpd+"/band",base+"/band  #Spd="+Gaudi::Utils::toString( spdslot*m_spdBin ),m_hMin,m_hMax,m_hBin);
    plot1D(di.mass(),base+"/"+nSpd+"/"+sspd+"/band",
           base+"/"+sspd+"/band  #Spd="+Gaudi::Utils::toString( spdslot*m_spdBin ),m_hMin,m_hMax,m_hBin);
  }  



  
  // highPt selection
  if( spd == 0 && di.Pt() > 2000 && c1.Pt()>800 && c2.Pt() >800)
    plot1D(di.mass(), base+"/all_highPt_sel1" , base+"/all highPt  sel1 spd == 0" , m_hMin, m_hMax, m_hBin);
  if( spd == 0 &&  ((c1.Pt()>1050 && c2.Pt() >250) || (c2.Pt()>1050 && c1.Pt() >250)) )
    plot1D(di.mass(), base+"/all_highPt_sel2" , base+"/all highPt sel2 spd == 0" , m_hMin, m_hMax, m_hBin);
    

  plot1D(di.mass(), base+"/all" , base+"/all" , m_hMin, m_hMax, m_hBin);
  plot1D(di.mass(), base+"/"+sarea+"/all" , base+"/"+sarea+"/all" , m_hMin, m_hMax, m_hBin);
  plot1D(di.mass(), base+"/"+sarea+"/"+sspd+"/all" , base+"/"+sarea+"/"+sspd+"/all" , m_hMin, m_hMax, m_hBin);
  plot1D(di.mass(), base+"/"+sarea+"/"+sspd+"/"+sprs+"/all" , base+"/"+sarea+"/"+sspd+"/"+sprs+"/all" , m_hMin, m_hMax, m_hBin);
  

  std::string u = base +"/" + sarea +"/"+ sspd +"/" + sprs +"/Energy/all";
  std::string u1 = base +"/" + sarea +"/"+ sspd +"/" + sprs +"/Energy/b"+Gaudi::Utils::toString( (double) ble1 * m_leBin );
  std::string u2 = base +"/" + sarea +"/"+ sspd +"/" + sprs +"/Energy/b"+Gaudi::Utils::toString( (double) ble2 * m_leBin );
  plot1D( di.mass(), u1 , u1 , m_hMin, m_hMax, m_hBin);
  if( u2 != u1) plot1D( di.mass(), u2,u2,m_hMin, m_hMax, m_hBin);
  plot1D( di.mass(), u , u , m_hMin, m_hMax, m_hBin);
  
  if( (prs1 < 5 && spd == 0 ) || (prs2<5 && spd == 0 ) ){
    std::string tu = base +"/" + sarea +"/EcalTuning/Energy/";
    std::string bin;
    if( prs1 < 5 )bin = Gaudi::Utils::toString( (double) ble1*m_leBin );
    else if( prs2<5 )bin = Gaudi::Utils::toString( (double) ble2*m_leBin );
    plot1D( di.mass(), tu+bin , tu+bin , m_hMin, m_hMax, m_hBin);
    plot1D( di.mass(), tu+"all" , tu+"all" , m_hMin, m_hMax, m_hBin);
  }
  
  
  
  std::string uu = base +"/" + sarea +"/"+ sspd +"/" + sprs+"/Pt/all";
  std::string uu1 = base +"/" + sarea +"/"+ sspd +"/" + sprs +"/Pt/b"+Gaudi::Utils::toString( (double) bet1 * m_etBin );
  std::string uu2 = base +"/" + sarea +"/"+ sspd +"/" + sprs +"/Pt/b"+Gaudi::Utils::toString( (double) bet2 * m_etBin );
  plot1D( di.mass(), uu1 , uu1 , m_hMin, m_hMax, m_hBin);
  if( uu2 != uu1) plot1D( di.mass(), uu2,uu2,m_hMin, m_hMax, m_hBin);
  plot1D( di.mass(), uu , uu , m_hMin, m_hMax, m_hBin);
  if( (prs1 < 5 && spd == 0) || (prs2<5 && spd == 0) ){
    std::string tu = base +"/" + sarea +"/EcalTuning/Pt/";
    std::string bin;
    if( prs1 < 5  )bin = Gaudi::Utils::toString( (double) bet1*m_etBin );
    else if( prs2<5 )bin = Gaudi::Utils::toString( (double) bet2*m_etBin );
    plot1D( di.mass(), tu+bin , tu+bin , m_hMin, m_hMax, m_hBin);
    plot1D( di.mass(), tu+"all" , tu+"all" , m_hMin, m_hMax, m_hBin);
  }
      

  
  std::string uuu = base +"/" + sarea +"/"+ sspd +"/" + sprs +"/Theta/all";
  std::string uuu1 = base +"/" + sarea +"/"+ sspd +"/" + sprs+
    "/Theta/b"+Gaudi::Utils::toString( (double) bth1 * m_thBin*pow(2.,(double)2-id1.area()));
  std::string uuu2 = base +"/" + sarea +"/"+ sspd +"/" + sprs +
    "/Theta/b"+Gaudi::Utils::toString( (double) bth2 * m_thBin*pow(2.,(double)2-id2.area()));
  plot1D( di.mass(), uuu1 , uuu1 , m_hMin, m_hMax, m_hBin);
  if( uu2 != uu1) plot1D( di.mass(), uuu2,uuu2,m_hMin, m_hMax, m_hBin);
  plot1D( di.mass(), uuu , uuu , m_hMin, m_hMax, m_hBin);
  if( (prs1 < 5 && spd == 0) || (prs2<5 && spd == 0  ) ){
    std::string tu = base +"/" + sarea +"/EcalTuning/Theta/";
    std::string bin;
    if( prs1 < 5  )bin = Gaudi::Utils::toString( (double)  bth1 * m_thBin*pow(2.,(double) 2-id1.area()) );
    else if( prs2<5 )bin = Gaudi::Utils::toString( (double) bth2 * m_thBin*pow(2.,(double) 2-id2.area()) );
    plot1D( di.mass(), tu+bin , tu+bin , m_hMin, m_hMax, m_hBin);
    plot1D( di.mass(), tu+"all" , tu+"all" , m_hMin, m_hMax, m_hBin);
  }
}
