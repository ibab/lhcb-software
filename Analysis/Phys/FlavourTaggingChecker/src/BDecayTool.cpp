#include "BDecayTool.h"
#include "GaudiKernel/ToolFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BDecayTool v1.0
//
// 2013-04-23 : M. Calvo 
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( BDecayTool )

//=============================================================================
BDecayTool::BDecayTool( const std::string& type,
				    const std::string& name,
				    const IInterface* parent ) :
  GaudiTool ( type, name, parent ) 
  , m_mcDecay(0){ 
  declareInterface<IBDecayTool>(this);
}
BDecayTool::~BDecayTool(){}

//=============================================================================
StatusCode BDecayTool::initialize() { 

  m_mcDecay = tool<IMCDecayFinder>("MCDecayFinder","MCDecayFinder",this);

  return StatusCode::SUCCESS; 
}

StatusCode BDecayTool::finalize() { return StatusCode::SUCCESS; }

//=============================================================================

const MCParticle* BDecayTool::forcedB() {

  //check what is the B forced to decay
  const MCParticle *BS = 0;

  //const LHCb::MCParticle* head = 0 ;
  if(m_mcDecay->findDecay( BS)){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Particle matching the decay. ID = "<<BS->particleID().pid()<< endmsg;
  }
  
  return BS;

}

//=======================================================================

//IMCDecayFinder* BDecayTool::mcDecayFinder() const {return m_mcDecay;}

