// $Id: Calo2MCTool.cpp,v 1.1 2009-09-16 16:07:06 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// from LHCb
#include "Relations/RelationWeighted1D.h"
// local
#include "Calo2MCTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Calo2MCTool
//
// 2009-07-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Calo2MCTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Calo2MCTool::Calo2MCTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICalo2MCTool>(this);

  declareProperty ( "Cluster2MCTable" , m_cluster2MCLoc ) ;
  declareProperty ( "Digit2MCTable"   , m_digit2MCLoc = "Relations/" + LHCb::CaloDigitLocation::Default ) ;

  // cluster2MC table location depends on context()

  std::string out( context() );
  std::transform( context().begin() , context().end() , out.begin () , ::toupper ) ;
  if( out == "HLT" ){
    m_cluster2MCLoc = "Relations/" + LHCb::CaloClusterLocation::DefaultHlt;
  }else{
    m_cluster2MCLoc = "Relations/" + LHCb::CaloClusterLocation::Default;
  }


}
//=============================================================================
// Destructor
//=============================================================================
Calo2MCTool::~Calo2MCTool() {} 

//=============================================================================


StatusCode Calo2MCTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  m_ppsvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true);
  if( m_ppsvc == NULL )return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}



ICalo2MCTool* Calo2MCTool::from(const LHCb::CaloCluster*   cluster  ){
  clear();
  m_clusters.push_back( cluster ) ;
  StatusCode sc = process();
  if ( sc.isFailure() )Warning("Processing Calo2MCTool from cluster failed").ignore();
  return this; 
}  

StatusCode Calo2MCTool::process(){

  m_sum = 0;

  if( !m_clusters.empty() ){
    // process clusters matching
    // 1 - get cluster->MC table
    //if( 0 == m_cluster2MC )
    m_cluster2MC  = get<LHCb::Calo2MC::IClusterTable>( m_cluster2MCLoc );
    if( 0 == m_cluster2MC ) 
      return Warning(" Cluster <-> MC relation table not found at " + m_cluster2MCLoc , StatusCode::FAILURE);
    
    // 2 - built (particle,weight) map
    for( std::vector<const LHCb::CaloCluster*>::const_iterator ic = m_clusters.begin() ; ic != m_clusters.end() ; ic++){
      const LHCb::CaloCluster* cluster = *ic;
      if( cluster == NULL )continue;
      LHCb::Calo2MC::IClusterTable::Range cluster2MCRange = m_cluster2MC->relations( cluster );
      for( LHCb::Calo2MC::IClusterTable::Range::const_iterator ir = cluster2MCRange.begin();cluster2MCRange.end()!=ir;ir++){
        const LHCb::MCParticle* mcpart = ir->to();
        m_mcMap[mcpart] += ir->weight();
      }
      m_sum += cluster->e();
      info() << "SUM CLUSTERS ENERGY " << m_sum << endreq;
    }
  }else if( !m_digits.empty() ){
    // process digits matching    
    // 1 - get digit->MC table
    //if( 0 == m_digit2MC )
    m_digit2MC  = get<LHCb::Calo2MC::DigitTable>( m_digit2MCLoc );
    if( 0 == m_digit2MC ) 
      return Warning(" Digit <-> MC relation table not found at " + m_digit2MCLoc , StatusCode::FAILURE);
    // 2 - built (particle,weight) map
    for( std::vector<const LHCb::CaloDigit*>::const_iterator id = m_digits.begin() ; id != m_digits.end() ; id++){
      const LHCb::CaloDigit* digit = *id;
      LHCb::Calo2MC::IDigitTable::Range digit2MCRange = m_digit2MC->relations( digit );
      for( LHCb::Calo2MC::IDigitTable::Range::const_iterator ir = digit2MCRange.begin();digit2MCRange.end()!=ir;ir++){
        const LHCb::MCParticle* mcpart = ir->to();
        m_mcMap[mcpart] += ir->weight();
      }
      m_sum += digit->e();
    }
  }else{
    return StatusCode::FAILURE;
  }
  mcDigest();

  info() << descriptor() << endmsg;

  return StatusCode::SUCCESS;
}

void Calo2MCTool::clear(){
  m_mcMap.clear();
  m_treeMap.clear();
  m_digits.clear();
  m_clusters.clear();
  m_hypos.clear();
  m_protos.clear();
  m_parts.clear();
}
  

void Calo2MCTool::mcDigest(){

  info() << "DIGEST" << endmsg;

  double mcMax = 0.;
  double mcBest = 0.;
  const LHCb::MCParticle* empty = NULL;
  m_maxMC  = empty ;
  m_bestMC = empty ;

  if( m_sum <= 0 )return;
  // loop over contributing particle :
  for( CaloMCTools::CaloMCMap::const_iterator imap = m_mcMap.begin() ; imap != m_mcMap.end() ; imap++){
    const LHCb::MCParticle* mcPart = imap->first;
    double w  = weight( mcPart );
    double q  = quality( mcPart );
    double m = mcPart->momentum().M();    

    // the most contributing MCParticle (with smallest mass when several MCPart with same weight)
    if( w >= mcMax ){
      bool ok = true;
      if( m_maxMC != NULL &&  w == mcMax && m > m_maxMC->momentum().M() )ok= false;
      if(ok){
        mcMax = w;
        m_maxMC = mcPart ;
      }      
    }
    // the best matching MCParticle
    if( q >= mcBest ){
      mcBest = q;
      m_bestMC = mcPart;
    }
    

  } // end loop over MCParticles

  info() << "MAX/BEST " << weight( m_maxMC ) << "/" << quality( m_bestMC) << endmsg;


  // build MC tree
  // 1- get related MC particle (seed) without any descendant listed in the related mcParticles 
  std::vector<const LHCb::MCParticle*> seeds;
  for( CaloMCTools::CaloMCMap::const_iterator imap = m_mcMap.begin() ; imap != m_mcMap.end() ; imap++){
    const LHCb::MCParticle* mcPart = imap->first;
    const SmartRefVector<LHCb::MCVertex>& vertices = mcPart->endVertices();
    int nProd = 0;
    for( SmartRefVector<LHCb::MCVertex>::const_iterator iv = vertices.begin();iv!=vertices.end();iv++){
      const  SmartRefVector<LHCb::MCParticle> products = (*iv)->products();
      for( SmartRefVector<LHCb::MCParticle>::const_iterator ip = products.begin();ip!=products.end();ip++){
        CaloMCTools::CaloMCMap::iterator it = m_mcMap.find( *ip );
        if( it != m_mcMap.end() )nProd += 1 ;
      }      
    }
    if( nProd == 0) seeds.push_back( mcPart );
  }
  

  // 2- build the seed upstream tree
  for( std::vector<const LHCb::MCParticle*>::const_iterator is = seeds.begin() ; is!= seeds.end() ; is++){
    const LHCb::MCParticle* seed = *is;
    std::vector<const LHCb::MCParticle*> tree;
    std::string sTree ;
    mcTree( seed , tree, sTree);
    std::stringstream frac("");
    std::stringstream qual("");
    frac << format(" %6.1f %% from : ", weight( seed ) *100. );
    qual << " ( " << format(" %6.1f %% of the MC particle energy in calo",
                            (weight( seed ) == 0)?0: quality(seed) /weight( seed )*100.) << " )";
    sTree = frac.str() + sTree + qual.str(); 
    m_treeMap[ sTree ] = tree;
  }  
}



void Calo2MCTool::mcTree(const LHCb::MCParticle* part, std::vector<const LHCb::MCParticle*>& tree , std::string& sTree){
  tree.push_back( part );
  const LHCb::ParticleProperty* prop = m_ppsvc->find( part->particleID() );
  sTree = prop->name() +  sTree;
  if( part->mother() != NULL){
    sTree = " -> " + sTree;
    mcTree( part->mother() , tree, sTree );
  }
}

double Calo2MCTool::weight(const LHCb::MCParticle* part){
  if( part == NULL )return 0.;
  if( m_sum <= 0 )return 0.;
  return m_mcMap[part]/m_sum;
}

double Calo2MCTool::quality(const LHCb::MCParticle* part){
  if( part == NULL )return 0.;
  return weight(part) * m_mcMap[part]/part->momentum().E();
}


std::string Calo2MCTool::descriptor(){
  std::stringstream ss("");
  ss  << std::endl;
  ss  << "     ---------- Calo MC contribution " ;
  if( !m_hypos.empty() ) ss << "to " << m_hypos.size() << " hypo(s) -------- " << std::endl;
  if( !m_digits.empty() ) ss << "to " << m_digits.size() << " digit(s) -------- " << std::endl;
  if( !m_clusters.empty() ) ss << "to " << m_clusters.size() << " cluster(s) ------- " << std::endl;
  ss  << "     ---- Total calo energy deposit : " << m_sum << " MeV " << std::endl;
  for( std::map<std::string,std::vector<const LHCb::MCParticle*> >::iterator im = m_treeMap.begin() ; m_treeMap.end()!=im;im++){
    ss  << "        -- " << (*im).first << std::endl;
  }
  return ss.str();
}


const LHCb::MCParticle* Calo2MCTool::findMC(std::string name, double threshold ){
  const LHCb::ParticleProperty* prop = m_ppsvc->find( name );
  if ( prop == NULL)return NULL;
  double t = threshold;
  LHCb::MCParticle* best  = NULL;
  for( CaloMCTools::CaloMCMap::const_iterator imap = m_mcMap.begin() ; imap != m_mcMap.end() ; imap++){
    const LHCb::MCParticle* mcPart = imap->first;
    if( mcPart->particleID().abspid() != prop->particleID().abspid() )continue;
    double q = quality( mcPart );
    if( q < t )continue;
    t = q;
    best = (LHCb::MCParticle*) mcPart;
  }
  return best;
}
const LHCb::MCParticle* Calo2MCTool::bestMC(){
  return m_bestMC;
}
const LHCb::MCParticle* Calo2MCTool::maxMC(){
  return m_maxMC;
}
CaloMCTools::CaloMCMap  Calo2MCTool::mcMap(){
  return m_mcMap;  
}


//int Calo2MCTool::mcCategory(){
//}



