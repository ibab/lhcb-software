// $Id: FlavourTaggingDebug.cpp,v 1.1 2002-09-20 12:09:05 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/FlavourTag.h"

#include "DaVinciMCTools/IDebugTool.h"

// local
#include "FlavourTaggingDebug.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTaggingDebug
//
// 2002-09-12 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FlavourTaggingDebug>          s_factory ;
const        IAlgFactory& FlavourTaggingDebugFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTaggingDebug::FlavourTaggingDebug( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ), m_pAsctLinks(0), m_debug(0)
{

}

//=============================================================================
// Destructor
//=============================================================================
FlavourTaggingDebug::~FlavourTaggingDebug() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode FlavourTaggingDebug::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  //  Load all necessary tools via the base 
  StatusCode sc = loadTools();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "   Unable to load tools" << endreq;
    return StatusCode::FAILURE;
  }

  sc = toolSvc()->retrieveTool( "Particle2MCAsct", m_pAsctLinks );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to retrieve the Link Associator tool."
        << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool( "DebugTool", m_debug );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to retrieve the Debug tool." << endreq;
    return sc;
  }  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FlavourTaggingDebug::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  SmartDataPtr<FlavourTags> tags(eventSvc(), FlavourTagLocation::User);
  if( tags == 0 || tags->size() == 0)
    return StatusCode::SUCCESS;

  // Fill the PhysDesktop particle and vertex vectors.  
  StatusCode scDesktop = desktop()->getInput();
  if( scDesktop.isFailure() ) {
    log << MSG::ERROR << "    not able to fill PhysDesktop " << endreq;
    return StatusCode::FAILURE;
  }

  FlavourTags::const_iterator ti;
  for( ti = tags->begin(); ti != tags->end(); ti++ )
    m_debug->printTree( (*ti)->taggedB() );
  
  // Retrieve the particles and vertices from PhysDesktop 
  const ParticleVector& parts = desktop()->particles();
  
  m_debug->printEventAsList( parts, m_pAsctLinks );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FlavourTaggingDebug::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
