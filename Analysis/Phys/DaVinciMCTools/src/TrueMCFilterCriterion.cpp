// $Id: TrueMCFilterCriterion.cpp,v 1.14 2007-01-12 14:03:53 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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

DECLARE_TOOL_FACTORY( TrueMCFilterCriterion );

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
  , m_pCl2MCPTable(0)
{
  declareInterface<IFilterCriterion>(this);
  declareProperty( "VetoSignal", m_filterOut );
  declareProperty( "ParticlePath", 
                   m_particlePaths );
  
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
                                    m_particlePaths);

  // for calo clusters -> MCPatricle association
  // Not actually used anywhere
//   m_pCl2MCPTable = get<Table>("Relations/" + 
//                               LHCb::CaloClusterLocation::Default);

  verbose() << "Initialised happily" << endreq ;

  return sc ;
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::testParticle( const LHCb::Particle* const & part ) {
 
  verbose() << "TrueMCFilterCriterion" << endreq ;

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

  return ( signal ) ? (!m_filterOut) : m_filterOut ;
  
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
