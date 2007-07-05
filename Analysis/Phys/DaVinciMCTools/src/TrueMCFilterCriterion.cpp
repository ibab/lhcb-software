// $Id: TrueMCFilterCriterion.cpp,v 1.18 2007-07-05 13:09:58 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "MCInterfaces/IMCDecayFinder.h"
#include "Kernel/Particle2MCLinker.h"

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
    fatal() << "MC decay is empty! " << endmsg;
    return StatusCode::FAILURE;
  } else info() << "MC decay is " << MCDecay << endmsg ;

  // For Particle -> MCParticle association  
  m_pLinker = new Particle2MCLinker(this,
                                    Particle2MCMethod::Chi2,
                                    m_particlePaths);

  // for calo clusters -> MCPatricle association
  // Not actually used anywhere
//   m_pCl2MCPTable = get<Table>("Relations/" + 
//                               LHCb::CaloClusterLocation::Default);

  if (msgLevel(MSG::VERBOSE)) verbose() << "Initialised happily" << endmsg ;
  setActive(); 

  return sc ;
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::testParticle( const LHCb::Particle* const & part ) {
 
  if (msgLevel(MSG::VERBOSE)) verbose() << "TrueMCFilterCriterion" << endmsg ;

  const LHCb::MCParticle* MC = m_pLinker->firstMCP( part );
  
  if( NULL == MC ){
    if (msgLevel(MSG::DEBUG)) debug() << "Empty association range" << endmsg;
    if (msgLevel(MSG::DEBUG)) debug() << "No association for " << part->key() << " "
                                      << part->particleID().pid() << " " << part->momentum()  << endmsg ;
    return m_filterOut ; // true if one wants to kill all, false else
  }

  if ( msgLevel ( MSG::DEBUG )) debug() << "Particle      " << part->key() << " " 
                                        << part->particleID().pid() << " " 
                                        << part->momentum() 
                                        << endmsg ;

  bool signal = false;

  while ( NULL != MC ){

    if (msgLevel(MSG::DEBUG)) debug() << "Associated to " << MC->key() << " " 
                                      << MC->particleID().pid() << " " 
                                      << MC->momentum() << endmsg ;
    signal = findMCParticle(MC);
    
    if(signal) if (msgLevel(MSG::DEBUG)) debug() << "which is a signal particle" << endmsg;
    if(signal) break; // just take one associated MCParticle
    MC = m_pLinker->nextMCP();
  }

  return ( signal ) ? (!m_filterOut) : m_filterOut ;
  
}
//=============================================================================
// get MC particles
//=============================================================================
bool TrueMCFilterCriterion::findMCParticle( const LHCb::MCParticle* MC ) {

  if (msgLevel(MSG::VERBOSE)) verbose() << "TrueMCFilterCriterion find MC" << endmsg ;

  // MC list
  LHCb::MCParticle::Vector mclist; // LF
  LHCb::MCParticles* kmcparts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !kmcparts ){
    fatal() << "Unable to find MC particles at '" << LHCb::MCParticleLocation::Default << "'" << endmsg;
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
  
  if (msgLevel(MSG::VERBOSE)) verbose() << "Found " << mclist.size() << " MC particles from true decay" << endmsg ;
  if (mclist.empty()) return false ;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Looking for " << MC->key() 
                                        << " " << MC->particleID().pid() << " " << MC->momentum() << endmsg ;  
  LHCb::MCParticle::Vector::iterator mcp ; // LF
  for ( mcp = mclist.begin() ; mcp != mclist.end() ; ++mcp ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Looping on  " << (*mcp)->key() 
                                          << " " << (*mcp)->particleID().pid() << " " 
                                          << (*mcp)->momentum() << endmsg ;  
    if ( (*mcp) == MC ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Particle found" << endmsg;
      return true ; // found 
    }
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Particle not found" << endmsg;
  return false ;
  
}
//=============================================================================
StatusCode TrueMCFilterCriterion::finalize(){
  
  if( NULL != m_pLinker ) delete m_pLinker;
  return GaudiTool::finalize() ;
  
}
