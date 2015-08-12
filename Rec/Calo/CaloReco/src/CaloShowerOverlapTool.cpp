// Include files 

#include "CaloUtils/CaloAlgUtils.h"
#include "Event/CaloDataFunctor.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CaloMomentum.h"

// local
#include "CaloShowerOverlapTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloShowerOverlapTool
//
// 2014-06-02 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloShowerOverlapTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloShowerOverlapTool::CaloShowerOverlapTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
: GaudiTool ( type, name , parent ),
  m_a1(0),
  m_a2(0),
  m_s1(0),
  m_s2(0),
  m_detLoc(DeCalorimeterLocation::Ecal),
  m_weights(),
  m_minSize(2){
  declareInterface<ICaloShowerOverlapTool>(this);
  declareProperty("Detector",m_detLoc=DeCalorimeterLocation::Ecal);
  declareProperty("Profile",m_pcond="Conditions/Reco/Calo/PhotonShowerProfile");
  declareProperty("Verbose",m_verbose=false);
  declareProperty("ClusterMinSize",m_minSize = 2); // skip single-cell "clusters"

  // define type from instance name
  const std::string uName ( LHCb::CaloAlgUtils::toUpper( name ) ) ;
  if ( uName.find( "MERGED" )  != std::string::npos   ||  uName.find( "SPLITPHOTON" )  != std::string::npos )m_type = "SplitPhoton"; 
  else if (  uName.find( "PHOTON" ) )m_type="Photon";
  else   m_type="Photon"; // the default
}

StatusCode CaloShowerOverlapTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Initialize" << endmsg;
  m_det = getDet<DeCalorimeter> ( m_detLoc );

  m_shape = tool<CaloCorrectionBase>("CaloCorrectionBase","ShowerProfile",this);
  m_stool = tool<ICaloHypoTool>("CaloSCorrection",m_type+"SCorrection",this);
  m_ltool = tool<ICaloHypoTool>("CaloLCorrection",m_type+"LCorrection",this);
  sc = setProfile(m_pcond);
  return sc;
}


//=============================================================================
// Destructor
//=============================================================================
CaloShowerOverlapTool::~CaloShowerOverlapTool() {} 

//=============================================================================

StatusCode CaloShowerOverlapTool::setProfile(std::string prof){
  m_pcond=prof;
  StatusCode sc = m_shape->setConditionParams(m_pcond,true);
  return sc;
}

void CaloShowerOverlapTool::storeInitialWeights(const LHCb::CaloCluster* cl1,const LHCb::CaloCluster* cl2){
  m_weights.clear();
  std::map<LHCb::CaloCellID,double> weights;
  for( LHCb::CaloCluster::Entries::const_iterator i1 = cl1->entries().begin() ; cl1->entries().end() != i1 ; ++i1 ){
    weights[i1->digit()->cellID()]=i1->fraction();
    //info() << " part1 " << i1->digit()->cellID() << " : " << i1->fraction() << endmsg;
  }  
  for( LHCb::CaloCluster::Entries::const_iterator i2 = cl2->entries().begin() ; cl2->entries().end() != i2 ; ++i2 ){
    LHCb::CaloCellID id2=i2->digit()->cellID();
    std::map<LHCb::CaloCellID,double>::iterator it = weights.find(id2);
    //info() << " part2 " << i2->digit()->cellID() << " : " << i2->fraction() << endmsg;
    if( it == weights.end() )weights[ id2 ]=i2->fraction();
    else{
      double f1 = weights[ id2 ];
      weights[ id2 ] = f1 + i2->fraction();      
    }
  }  
  // check
  for( std::map<LHCb::CaloCellID,double>::iterator it = weights.begin() ; weights.end() != it ; ++it)  {
    if( it->second == 1.)continue;
    m_weights[it->first]=it->second;
    //info() << "storing weight " << it->first << " : " << it->second << endmsg;
  }
  //info() << " Stored weights " << m_weights.size() << endmsg;
}

double CaloShowerOverlapTool::getInitialWeight(const LHCb::CaloCellID id){
  if( m_weights.size() == 0)return 1.;
  std::map<LHCb::CaloCellID,double>::iterator it = m_weights.find(id);
  if( it == m_weights.end() )return 1.;
  return it->second;
}



void CaloShowerOverlapTool::process(const LHCb::CaloCluster* cl1 , const LHCb::CaloCluster* cl2 , 
                                    int spd, int niter,bool propagateInitialWeights){





  if( cl1->entries().size() < m_minSize || cl2->entries().size() < m_minSize ){
    counter("Overlap correction skipped due to cluster size") += 1;
    return;  // skip small clusters
  }

  m_a1=cl1->seed().area();
  m_a2=cl2->seed().area();
  m_s1= spd  /  10 ;
  m_s2= spd  %  10 ;

  m_weights.clear(); // clear initial weights
  if( propagateInitialWeights )storeInitialWeights(cl1,cl2);



  // 0 - evaluate parameters (applying photon hypo corrections for the position)
  LHCb::CaloCluster* w1 = (LHCb::CaloCluster*) cl1;
  LHCb::CaloCluster* w2 = (LHCb::CaloCluster*) cl2;
  evaluate(w1);
  evaluate(w2);

  if( w1->e() <= 0. || w2->e() <= 0){
    counter("Overlap correction skipped due to cluster energy") += 1;
    return;
  }


  if( m_verbose ){
    info() << " ======== Shower Overlap =======" << endmsg;
    info() << " CL1/CL2 : " << cl1->e() << " " << cl2->e() << " " << cl1->e()+cl2->e()<< endmsg;
    info() << " seed    : " << cl1->seed()  << " " << cl2->seed() << endmsg;
    info() << " area    : " << m_a1  << " " << m_a2 << endmsg;
    info() << " Spd     : " << spd << " " << m_s1  << " " << m_s2 << endmsg;
    info() << " params  : " << cl1->position().parameters() << " / " <<   cl2->position().parameters() << endmsg;
  }
  


  int iter = 0;

  // 1 - determine the energy fractions of each entry
  while( iter < niter ){

    if( m_verbose )info() << " ------ iter = " << iter << endmsg;

    subtract(w1,w2,propagateInitialWeights);

    if( m_verbose )info() << " >> CL1/CL2 : " << w1->e() << " " << w2->e() << "  " << w1->e() + w2->e() << endmsg;
    if( m_verbose )info() << " >> params  : " << w1->position().parameters() << " / " <<   w2->position().parameters() << endmsg;
    //
    if( m_verbose ){
      LHCb::CaloMomentum momentum;
      momentum.addCaloPosition(w1);
      momentum.addCaloPosition(w2);
      //info() << " >> Mass : "  << momentum.mass() << endmsg;
    }
    
    iter++;
  }
  // 3 - reset cluster-like parameters
  evaluate(w1,false);
  evaluate(w2,false);

  //delete w1;
  //delete w2;
}


double CaloShowerOverlapTool::fraction(LHCb::CaloCluster* cluster, LHCb::CaloDigit* digit,int flag){

  if( digit == NULL)return 1.;
  const LHCb::CaloCellID cellID=digit->cellID();
  

  double size  = m_det->cellSize( cellID ) ;
  double xd    = m_det->cellX   ( cellID ) ;
  double yd    = m_det->cellY   ( cellID ) ;
  double xc = cluster->position().parameters()(  LHCb::CaloPosition::X );
  double yc = cluster->position().parameters()(  LHCb::CaloPosition::Y );
  double zc = cluster->position().z();
  double zd = (xc*xc + yc*yc + zc*zc - xc*xd - yc* yd )/zc;
  double d3d = std::sqrt( (xd - xc)*(xd - xc ) +
                          (yd - yc)*(yd - yc ) +
                          (zd - zc)*(zd - zc ) )/size;
  int area = (flag == 1) ? m_a1 : m_a2;
  int spd  = (flag == 1) ? m_s1 : m_s2;
  double f = showerFraction( d3d, area, spd );
  double ed = digit->e();
  double ec = f * cluster->position().parameters()( LHCb::CaloPosition::E );
  double frac= (ed > ec ) ?  (ed - ec )/ ed : 0.;
  
  // info() << "        --> Digit : " << cellID << "  fraction : " << frac << endmsg;
  return frac;
}



void CaloShowerOverlapTool::subtract(LHCb::CaloCluster* cl1, LHCb::CaloCluster* cl2,bool propagateInitialWeight){

  // cluster1  -> cluster2 spillover
  //info() << "     --- 1st cluster overlap ----- " << cl1->seed() <<  endmsg;
  for( LHCb::CaloCluster::Entries::iterator i2 = cl2->entries().begin() ; cl2->entries().end() != i2 ; ++i2 ){
    if( (LHCb::CaloDigitStatus::UseForEnergy & i2->status())==0 &&
        (LHCb::CaloDigitStatus::UseForPosition & i2->status())==0 )continue;
    double initialWeight = propagateInitialWeight ? getInitialWeight( i2->digit()->cellID() ) : 1.;
    i2->setFraction( fraction( cl1,  i2->digit() , 1)*initialWeight );
    //info() << "cl1 -> 2 : " << i2->digit()->cellID() << "  " << i2->digit()->e() << " " << i2->fraction() << endmsg;
  }

  // re-evaluate cluster2 accordingly
  evaluate(cl2);
  if( cl2->e() < 0)return; // skip negative energy "clusters"

  //info() << "     --- 2nd cluster overlap ----- " << cl2->seed() << endmsg;
  // cluster2  -> cluster1 spillover
  for( LHCb::CaloCluster::Entries::iterator i1 = cl1->entries().begin() ; cl1->entries().end() != i1 ; ++i1 ){
    const LHCb::CaloDigit* dig1 = i1->digit();
    if( (LHCb::CaloDigitStatus::UseForEnergy & i1->status()) ==0 &&
        (LHCb::CaloDigitStatus::UseForPosition & i1->status())==0 )continue;
    double initialWeight = propagateInitialWeight ? getInitialWeight( i1->digit()->cellID() ) : 1.;
    i1->setFraction( fraction( cl2, i1->digit() , 2)*initialWeight );
    //info() << "cl2 -> 1 : " << i1->digit()->cellID() << "  " << i1->digit()->e() << " " << i1->fraction() << endmsg;
    double eps = 1.e-4;
    // normalize the sum of partial weights in case of  shared cells 
    for( LHCb::CaloCluster::Entries::iterator i2 = cl2->entries().begin() ; cl2->entries().end() != i2 ; ++i2 ){
      const LHCb::CaloDigit* dig2 = i2->digit();
      if( !(dig2->cellID() == dig1->cellID()) )continue;
      if( (LHCb::CaloDigitStatus::UseForEnergy & i2->status())==0 && 
           (LHCb::CaloDigitStatus::UseForPosition & i2->status())==0 )continue;
      double f1 = i1->fraction();
      double f2 = i2->fraction();
      double sum = f1 + f2 ;
      if( fabs( sum - initialWeight) > eps ){
        if( sum < initialWeight && f2 == 0. )i2->setFraction( initialWeight - f1 );
        else if ( sum < initialWeight && f1  == 0.)i1->setFraction (initialWeight - f2);
        else{
          i1->setFraction( initialWeight*f1/(f1+f2) );
          i2->setFraction( initialWeight*f2/(f1+f2) );
        }        
        //info() << "  -> SHARED " << f1 << " -> " << i1->fraction() << " | " << f2 << " -> " << i2->fraction() << " => " << i1->fraction()+i2->fraction() << endmsg;
      }    
    }
  }

  // reevaluate  cluster1 & 2 accordingly
  evaluate(cl1);
  evaluate(cl2);

}




double CaloShowerOverlapTool::showerFraction(double d3d, unsigned int area ,int spd){
  double frac = 0;
  LHCb::CaloCellID cellID(2,area,0,0); //fake cell
  frac = (spd) ?
    m_shape->getCorrection( CaloCorrection::profileC      , cellID , d3d ,0.) :
    m_shape->getCorrection( CaloCorrection::profile       , cellID , d3d ,0.) ;
  if( 0. > frac ) { frac=0.; }
  if( 1. < frac ) { frac=1.; }
  return frac ;
}


void CaloShowerOverlapTool::evaluate(LHCb::CaloCluster* cluster,bool hypoCorrection){


  // 0 - reset z-position of cluster
  LHCb::ClusterFunctors::ZPosition zPosition( m_det );
  cluster->position().setZ( zPosition( cluster )  );

  // 1 - 3x3 energy and energy-weighted barycenter
  double E, X, Y;
  StatusCode sc = LHCb::ClusterFunctors::calculateEXY( cluster->entries().begin() ,
                                                       cluster->entries().end  () ,
                                                       m_det , E , X , Y      );
  if( sc.isSuccess() ){
    cluster->position().parameters()( LHCb::CaloPosition::E ) = E ;
    cluster->position().parameters()( LHCb::CaloPosition::X ) = X ;
    cluster->position().parameters()( LHCb::CaloPosition::Y ) = Y ;
  }
  else
    Warning( " E,X and Y of cluster could not be evaluated!",StatusCode::SUCCESS,1).ignore();
  

  if( cluster->e() < 0)return; // skip correction for negative energy "clusters"

  //-------------------------------------------------------------------
  if( !hypoCorrection ) return; // do not apply 'photon' hypo correction  

  // 2 - apply 'photon hypothesis' corrections
  
  // create a fake CaloHypo
  LHCb::CaloHypo* hypo = new LHCb::CaloHypo();
  hypo -> setHypothesis ( LHCb::CaloHypo::Photon );      
  hypo -> addToClusters ( cluster );
  hypo -> setPosition   ( new LHCb::CaloPosition(cluster->position()) );    

  // Apply transversal corrections
  sc=m_stool->process(hypo);
  if( sc.isSuccess() ){
    cluster->position().parameters()( LHCb::CaloPosition::X ) = hypo->position()->parameters()( LHCb::CaloPosition::X) ;
    cluster->position().parameters()( LHCb::CaloPosition::Y ) = hypo->position()->parameters()( LHCb::CaloPosition::Y) ;
  }else
    Error(" SCorrection could not be evaluated!",sc,1).ignore();
  
  // Apply longitudinal correction
  sc=m_ltool->process(hypo);
  if( sc.isSuccess() ){
    cluster->position().setZ( hypo->position()->z() );
  }  else
    Error(" LCorrection could not be evaluated!",sc,1).ignore();

  // cleanup
  delete hypo;
}


