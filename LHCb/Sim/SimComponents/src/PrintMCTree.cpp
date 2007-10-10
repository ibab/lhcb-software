// $Id: PrintMCTree.cpp,v 1.1 2007-10-10 12:11:35 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// from LHCb
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

// local
#include "PrintMCTree.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintMCTree
//
// 2004-09-10 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( PrintMCTree );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintMCTree::PrintMCTree( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    ,  m_printMCTree(0)
    ,  m_particleNames()
    ,  m_particleIDs()
{
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "Depth", m_depth = -1 );
}
//=============================================================================
// Destructor
//=============================================================================
PrintMCTree::~PrintMCTree() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrintMCTree::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if (m_particleNames.empty()){
    fatal() << "You need to give a list of particles" << endreq;
    return StatusCode::FAILURE ;
  }
  
  m_printMCTree = tool<IPrintMCDecayTreeTool>( "PrintMCDecayTreeTool", this );

  IParticlePropertySvc *ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc");
  if( !ppSvc ) {
    fatal() << "Unable to locate Particle Property Service" << endmsg;
    return sc;
  }

  std::vector<std::string>::iterator PN;
  for (PN=m_particleNames.begin() ; PN!=m_particleNames.end() ; ++PN ){
    ParticleProperty *pp = ppSvc->find(*PN);
    if (!pp) {
      fatal() << " Unable to retrieve particle property for " 
              << *PN << endmsg;
      return StatusCode::FAILURE;
    }
    int pid = pp->jetsetID();
    m_particleIDs.push_back(pid);
  }

  info() << "Will print tree for MC particles " << m_particleNames << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintMCTree::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::MCParticles* kmcparts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !kmcparts ) {
    fatal() << "Unable to find MC particles at '" 
            << LHCb::MCParticleLocation::Default << "'" << endreq;
    return StatusCode::FAILURE;
  }
  std::vector<LHCb::MCParticle*> mcparts(kmcparts->begin(), kmcparts->end());
  debug() << "There are " <<  mcparts.size() << " MC particles" << endmsg;

  bool printed = false ;
  std::vector<LHCb::MCParticle*>::iterator MCP;
  for (MCP=mcparts.begin() ; MCP!=mcparts.end() ; ++MCP ){
    int pid = (*MCP)->particleID().pid() ;
    verbose() << "MC Particle is a " << pid << endreq ;
    std::vector<int>::iterator PID;
    for (PID=m_particleIDs.begin() ; PID!=m_particleIDs.end() ; ++PID ){
      if ( pid==(*PID)) {
        info() << "Printing MC tree for particle with ID " << pid << endreq ;
        m_printMCTree->printTree( (*MCP), m_depth ) ;
        printed = true ;
        break ;
      }  
    }
  }
  if (!printed) debug() << "No MC particles found to print in a tree" << endreq;
  
  setFilterPassed(printed);

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrintMCTree::finalize() {

  debug() << "==> Finalize" << endmsg;
  StatusCode sc = toolSvc()->releaseTool( m_printMCTree );
  if (!sc) return sc ;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
