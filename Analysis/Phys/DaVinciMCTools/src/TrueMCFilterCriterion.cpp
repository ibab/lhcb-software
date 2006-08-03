// $Id: TrueMCFilterCriterion.cpp,v 1.10 2006-08-03 14:39:37 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Kernel/IMCDecayFinder.h"
#include "DaVinciAssociators/Particle2MCLinker.h"

#include "Kernel/CaloCellID.h"
#include "Event/CaloCluster.h"
#include "CaloKernel/CaloVector.h"
#include "CaloUtils/Calo2MC.h"
//#include "Event/TrgCaloParticle.h"

// local
#include "TrueMCFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrueMCFilterCriterion
//
// 2004-09-13 : Patrick KOPPENBURG
// 
// Reviewed to have gammas 2005-03-20 : Luis Fernandez
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrueMCFilterCriterion>          s_factory ;
const        IToolFactory& TrueMCFilterCriterionFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrueMCFilterCriterion::TrueMCFilterCriterion( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : FilterCriterionBase ( type, name , parent )
  , m_pMCDecFinder(0)
  , m_pLinker(0)
  , m_filterOut(false)
  , m_gammaID(22)
  , m_pCl2MCPTable(0)
{
  declareInterface<IFilterCriterion>(this);
  declareProperty( "VetoSignal", m_filterOut );
  declareProperty( "CaloClustersPath", 
                   m_caloClustersPath = LHCb::CaloClusterLocation::Ecal );
  declareProperty( "ParticlePath", 
                   m_particlePath );
  declareProperty( "CaloCluster2MCParticlePath",
                   m_caloCluster2MCParticlePath);
  
}
//=============================================================================
// Destructor
//=============================================================================
TrueMCFilterCriterion::~TrueMCFilterCriterion() {}; 
//=============================================================================
// Initialize
//=============================================================================
StatusCode TrueMCFilterCriterion::initialize( ){

  const StatusCode sc = FilterCriterionBase::initialize() ;
  if ( !sc) return sc ;

  m_pMCDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this); 

  std::string MCDecay = m_pMCDecFinder->decay();
  if (MCDecay == "")  {
    fatal() << "MC decay is empty! " << endreq;
    return StatusCode::FAILURE;
  } else info() << "MC decay is " << MCDecay << endreq ;

  // For Particle -> MCParticle association  
  m_pLinker = new Particle2MCLinker(this,
                                    Particle2MCMethod::Chi2,
                                    m_particlePath);

  // for calo clusters -> MCPatricle association
  m_pCl2MCPTable = get<Table>("Relations/" + 
                              LHCb::CaloClusterLocation::Default + 
                              "CCs2MCPs");

  verbose() << "Initialised happily" << endreq ;

  return sc ;
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::testParticle( const LHCb::Particle* const & part ) {
 
  verbose() << "TrueMCFilterCriterion" << endreq ;

  return ( testNeutralsWithOrigin(part) || testTheRest(part) ) ?
    (!m_filterOut) : m_filterOut ;
  
}
//=============================================================================
bool TrueMCFilterCriterion::testTheRest( const LHCb::Particle* const & part ) 
{  

  const LHCb::MCParticle* MC = m_pLinker->firstMCP( part );
  
  if( NULL == MC ){
    debug() << "Empty association range" << endreq;
    debug() << "No association for " 
            << part->particleID().pid() << " " << part->momentum()  << endreq ;
    return m_filterOut ; // true if one wants to kill all, false else
  }

  debug() << "Particle      " << part->particleID().pid() << " " 
          << part->momentum() 
          << endreq ;

  bool signal = false;

  while ( NULL != MC ){

    debug() << "Associated to " << MC->particleID().pid() << " " 
            << MC->momentum() << endreq ;
    signal = findMCParticle(MC);
    
    if(signal) debug() << "which is a signal particle" << endreq;
    if(signal) break; // just take one associated MCParticle
    MC = m_pLinker->nextMCP();
  }
  return signal;

}
//=============================================================================
bool TrueMCFilterCriterion::testNeutralsWithOrigin( const LHCb::Particle* const & part ) {  

  if (NULL == part) return false;
  bool signal = false;
  /*

  /// @todo : Re-do Gamma->MCParticle association
  // Special case of online gammas (neutrals with origin)

  if(part->origin() && m_gammaID == part->particleID().pid()){ // neutrals with origin
         
    // Check that the gamma is made from a TrgCaloParticle
    const TrgCaloParticle* myTrgCaloPart = dynamic_cast<const TrgCaloParticle*>(part->origin());
    
    if(myTrgCaloPart){
      debug() << "Special case of online gammas" << endreq;

      std::vector<CaloCellID> ClusterSeed = myTrgCaloPart->cellIdVector();
      CaloCellID myTrgCaloPartCellID = ClusterSeed[0];
      
      verbose() << "There is a TrgCaloParticle with ID " << myTrgCaloPart->particleID().pid()
                << " and CellID[0] " << ClusterSeed[0] << endreq;
      
      // The Calo clusters
      LHCb::CaloClusters* myCaloClusters = get<LHCb::CaloClusters>(m_CaloClustersPath);
      
      // Create a CaloVector of CaloClusters for easy access and get rid of split clusters (from pi0)
      CaloVector<const LHCb::CaloCluster*>  CaloClustersVec;
      
      for(LHCb::CaloClusters::const_iterator icl = myCaloClusters->begin(); icl != myCaloClusters->end(); ++icl){
        // forget it if split cluster
        const LHCb::CaloCluster* cl = *icl;
        if (!(myCaloClusters == cl->parent())) continue;
        CaloClustersVec.addEntry(cl,  cl->seed());
      }
      
      verbose() << "CaloClustersVec size: " << CaloClustersVec.size() << endreq;
      
      // get the corresponding CaloCluster
      const LHCb::CaloCluster* ccluster = CaloClustersVec[myTrgCaloPartCellID];
      if(!ccluster) return Error("Cluster corresponding to the TrgCaloParticle not found");
      
      // Now the relation table and association
      if(false == m_pAsctCl2MCP->tableExists()){
        return Error("No table retrieved for CaloCluster2MCParticle associator");
      }
      const DirectType* table = m_pAsctCl2MCP->direct();
      if(!table) return Error("No valid direct table for CaloCluster2MCParticle associator");
      
      // Check the association
      const DirectType::Range r = table->relations(ccluster);

      if(r.empty()){
        debug() << "Empty association range" << endreq;
        debug() << "No association for " 
                << part->particleID().pid() << " " << part->momentum()  << endreq ;
        return m_filterOut ; // true if one wants to kill all, false else
      }

      debug() << "Particle      " << part->particleID().pid() << " " << part->momentum() 
              << endreq ;
  
      for(unsigned ii = 0 ; ii<r.size(); ++ii){
        MC = r[ii].to();
        if( !MC || part->particleID().pid() != MC->particleID().pid()) continue;

        debug() << "Associated to " << MC->particleID().pid() << " " << MC->momentum() << endreq ;
        
        signal = findMCParticle(MC);
        if(signal) verbose() << "Found association for online gamma" << endreq;
        if(signal) break; // just take one associated MCParticle
      } // ii

      if(signal){
        debug() << "which is a signal particle" << endreq;
        return (!m_filterOut) ; // false if one wants to kill all, true else
      }
      else return m_filterOut ; // true if one wants to kill all, false else
      
    } // if online gamma
  // end FIXME
  */

  // LF note : need to go AssociatorWeighted class to treat gammas and charged in the same way
  return signal;

}
//=============================================================================
// get MC particles
//=============================================================================
bool TrueMCFilterCriterion::findMCParticle( const LHCb::MCParticle* MC ) {

  verbose() << "TrueMCFilterCriterion find MC" << endreq ;

  // MC list
  LHCb::MCParticle::Vector mclist; // LF
  LHCb::MCParticles* kmcparts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !kmcparts ){
    fatal() << "Unable to find MC particles at '" << LHCb::MCParticleLocation::Default << "'" << endreq;
    return false;
  }

  LHCb::MCParticle::ConstVector MCHead;
  const LHCb::MCParticle* imc = NULL;
  while ( m_pMCDecFinder->findDecay(*kmcparts, imc ) ){
    const LHCb::MCParticle* jmc = imc;
    MCHead.push_back(jmc);
  }
   
  LHCb::MCParticle::ConstVector::const_iterator ihead;
  for( ihead = MCHead.begin(); ihead != MCHead.end(); ++ihead){
    const LHCb::MCParticle* mc = *ihead;
    // final states must be flagged
    m_pMCDecFinder->decayMembers(mc, mclist);
     
  }
  
  verbose() << "Found " << mclist.size() << " MC particles from true decay" << endreq ;
  if (mclist.empty()) return false ;

  verbose() << "Looking for " << MC->particleID().pid() << " " << MC->momentum() << endreq ;  
  LHCb::MCParticle::Vector::iterator mcp ; // LF
  for ( mcp = mclist.begin() ; mcp != mclist.end() ; ++mcp ){
    verbose() << "Looping on  " << (*mcp)->particleID().pid() << " " 
              << (*mcp)->momentum() << endreq ;  
    if ( (*mcp) == MC ) {
      verbose() << "Particle found" << endreq;
      return true ; // found 
    }
  }
  verbose() << "Particle not found" << endreq;
  return false ;
  
}
//=============================================================================
StatusCode TrueMCFilterCriterion::finalize(){
  
  if( NULL != m_pLinker ) delete m_pLinker;
  return GaudiTool::finalize() ;
  
}
