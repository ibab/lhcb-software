// $Id: MCParticleArrayFilterAlg.cpp,v 1.1 2008-04-14 15:23:46 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/IMCParticleArrayFilter.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include "Event/MCParticle.h"
// local
#include "MCParticleArrayFilterAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCParticleArrayFilterAlg
//
// 2008-04-09 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCParticleArrayFilterAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleArrayFilterAlg::MCParticleArrayFilterAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_filter(0),
  m_filterType(""),
  m_inputLocation(LHCb::MCParticleLocation::Default),
  m_outputLocation(""),
  m_ppSvc(0)
{
  declareProperty("IMCParticleArrayFilter", m_filterType);
  declareProperty("InputLocation", m_inputLocation); 
  declareProperty("OutputLocation", m_outputLocation); 
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleArrayFilterAlg::~MCParticleArrayFilterAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCParticleArrayFilterAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_filter = tool<IMCParticleArrayFilter>(m_filterType, this);

  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);

  return  (0!=m_filter || 0!= m_ppSvc) 
    ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleArrayFilterAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::MCParticle::Container* mcParts_ = 
    get<LHCb::MCParticle::Container>(m_inputLocation);

  LHCb::MCParticle::ConstVector mcParts(mcParts_->size(), 0);
  std::copy(mcParts_->begin(), mcParts_->end(), mcParts.begin());
  
  LHCb::MCParticle::ConstVector decays;
  
  m_filter->filter(mcParts, decays);

  LHCb::MCParticle::Container* x = new LHCb::MCParticle::Container();

  x->reserve(decays.size());

  for (LHCb::MCParticle::ConstVector::const_iterator i = decays.begin();
       i != decays.end(); ++i) {
    x->insert((LHCb::MCParticle*)*i);
  }
  
  put(x, m_outputLocation);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCParticleArrayFilterAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
