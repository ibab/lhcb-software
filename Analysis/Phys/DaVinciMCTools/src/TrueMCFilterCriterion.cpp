// $Id: TrueMCFilterCriterion.cpp,v 1.5 2005-02-08 17:12:38 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "MCTools/IMCDecayFinder.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

// local
#include "TrueMCFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrueMCFilterCriterion
//
// 2004-09-13 : Patrick KOPPENBURG
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
  : GaudiTool ( type, name , parent )
  , m_pMCDecFinder(0)
  , m_pAsct(0)
  , m_filterOut(false)
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

  StatusCode sc = GaudiTool::initialize() ;
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

  return sc ;
  
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::isSatisfied( const Particle* const & part ) {
 
  verbose() << "TrueMCFilterCriterion" << endreq ;
  // associated MC particle
  MCParticle *MC = m_pAsct->associatedFrom( part );
  if (!MC) {
    debug() << "No association for " 
            << part->particleID().pid() << " " << part->momentum()  << endreq ;
    return m_filterOut ; // true if one wants to kill all, false else
  } else {
    debug() << "Particle      " << part->particleID().pid() << " " << part->momentum() 
            << endreq ;    
    debug() << "Associated to " << MC->particleID().pid() << " " << MC->momentum() << endreq ;
  }
  if (findMCParticle(MC)) {
    debug() << "which is a signal particle" << endreq ;    
    return (!m_filterOut) ; // false if one wants to kill all, true else
  }
  else return m_filterOut ;     // true if one wants to kill all, false else   
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
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::operator()( const Particle* const & part ) {

  return this->isSatisfied( part );

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
