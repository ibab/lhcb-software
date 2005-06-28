// $Id: TrueMCFilterCriterion.cpp,v 1.7 2005-06-28 15:37:27 lazzecri Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "MCTools/IMCDecayFinder.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

#include "CaloKernel/CaloVector.h"
#include "Event/TrgCaloParticle.h"

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
    , m_pAsct(0)
    , m_filterOut(false)
    , m_gammaID(22)
    , m_pAsctCl2MCP(0)
    , m_CaloClustersPath(CaloClusterLocation::Ecal)
{
  declareInterface<IFilterCriterion>(this);
  declareProperty( "VetoSignal", m_filterOut );
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
  if(!m_pMCDecFinder){
    fatal() << "Unable to retrieve MCDecayFinder tool" << endreq;
    return sc;
  }
  std::string MCDecay = m_pMCDecFinder->decay();
  if (MCDecay == "")  {
    fatal() << "MC decay is empty! " << endreq;
    return StatusCode::FAILURE;
  } else info() << "MC decay is " << MCDecay << endreq ;
  
  m_pAsct = tool<Particle2MCLinksAsct::IAsct>("Particle2MCLinksAsct", this);
  if(!m_pAsct){
    fatal() << "Unable to retrieve the Particle2MCLinks associator" << endreq;
    return sc;
  }

  // for calo clusters association
  m_pAsctCl2MCP = tool<IAsctCl2MCP>("AssociatorWeighted<CaloCluster,MCParticle,float>", "CCs2MCPs");
  if(!m_pAsctCl2MCP){
    err() << "Unable to retrieve the AssociatorWeighted<CaloCluster,MCParticle,float>" << endreq;
    return sc;
  }
  
  return sc ;
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::testParticle( const Particle* const & part ) {
 
  verbose() << "TrueMCFilterCriterion" << endreq ;

  MCParticle* MC = 0;
  bool signal = false;

  // beg FIXME : for now the association of online gammas requires offline clusters
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
      CaloClusters* myCaloClusters = get<CaloClusters>(m_CaloClustersPath);
      
      // Create a CaloVector of CaloClusters for easy access and get rid of split clusters (from pi0)
      CaloVector<const CaloCluster*>  CaloClustersVec;
      
      for(CaloClusters::const_iterator icl = myCaloClusters->begin(); icl != myCaloClusters->end(); ++icl){
        // forget it if split cluster
        const CaloCluster* cl = *icl;
        if (!(myCaloClusters == cl->parent())) continue;
        CaloClustersVec.addEntry(cl,  cl->seed());
      }
      
      verbose() << "CaloClustersVec size: " << CaloClustersVec.size() << endreq;
      
      // get the corresponding CaloCluster
      const CaloCluster* ccluster = CaloClustersVec[myTrgCaloPartCellID];
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
  } // neutrals with origin
  // end FIXME

  // LF note : need to go AssociatorWeighted class to treat gammas and charged in the same way
  AssociatorWeighted<Particle,MCParticle,double>::IAsct *pAsso = m_pAsct;

  // Range
  AssociatorWeighted<Particle,MCParticle,double>::ToRange mcPartAssoRange = pAsso->rangeFrom(part);
  
  // Iterator
  AssociatorWeighted<Particle,MCParticle,double>::ToIterator mcPartAssoIt;
  
  // Not association and not online gamma (because return before the AssociatorWeighted part)
  if(mcPartAssoRange.empty()){
    debug() << "Empty association range" << endreq;
    debug() << "No association for " 
            << part->particleID().pid() << " " << part->momentum()  << endreq ;
    return m_filterOut ; // true if one wants to kill all, false else
  }

  verbose() << " ... rangeFrom size = " << mcPartAssoRange.size() << endreq;

  debug() << "Particle      " << part->particleID().pid() << " " << part->momentum() 
          << endreq ;

  MC = 0;  
  signal = false;

  for(mcPartAssoIt = mcPartAssoRange.begin(); mcPartAssoIt != mcPartAssoRange.end(); mcPartAssoIt++){
    MC = mcPartAssoIt->to();
    // this should never happen since there is a range ...
    if(!MC) continue;

    debug() << "Associated to " << MC->particleID().pid() << " " << MC->momentum() << endreq ;
    signal = findMCParticle(MC);
    
    if(signal) debug() << "which is a signal particle" << endreq;
    if(signal) break; // just take one associated MCParticle
  }

  if(signal){
    return (!m_filterOut) ; // false if one wants to kill all, true else
  }
  else return m_filterOut ; // true if one wants to kill all, false else

}
//=============================================================================
// get MC particles
//=============================================================================
bool TrueMCFilterCriterion::findMCParticle( const MCParticle* MC ) {

  // MC list
  // std::vector<const MCParticle*> mclist;
  std::vector<MCParticle*> mclist; // LF
  MCParticles* kmcparts = get<MCParticles>(MCParticleLocation::Default );
  if( !kmcparts ){
    fatal() << "Unable to find MC particles at '" << MCParticleLocation::Default << "'" << endreq;
    return false;
  }

  std::vector<MCParticle*> MCHead;
  const MCParticle* imc = NULL;
  while ( m_pMCDecFinder->findDecay(*kmcparts, imc ) ){
    MCParticle* jmc = const_cast<MCParticle*>(imc);
    MCHead.push_back(jmc);
  }
   
  std::vector<MCParticle*>::const_iterator ihead;
  for( ihead = MCHead.begin(); ihead != MCHead.end(); ++ihead){
    const MCParticle* mc = *ihead;
    // final states must be flagged
    m_pMCDecFinder->decayMembers(mc, mclist);
     
  }
  
  verbose() << "Found " << mclist.size() << " MC particles from true decay" << endreq ;
  if (mclist.empty()) return false ;

  verbose() << "Looking for " << MC->particleID().pid() << " " << MC->momentum() << endreq ;  
  // std::vector<const MCParticle*>::const_iterator mcp ;
  std::vector<MCParticle*>::iterator mcp ; // LF
  for ( mcp = mclist.begin() ; mcp != mclist.end() ; ++mcp ){
    verbose() << "Looping on  " << (*mcp)->particleID().pid() << " " << (*mcp)->momentum() << endreq ;  
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
  
  StatusCode sc = toolSvc()->releaseTool(m_pAsct);
  if(sc.isFailure()){
    fatal() << "Unable to release the Particle2MCLinks associator" << endreq;
    return sc;
  }

  return GaudiTool::finalize() ;
  
}
