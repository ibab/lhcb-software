// $Id: CopyFlavourTag.cpp,v 1.2 2008-08-08 12:41:34 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/FlavourTag.h"
// from MicroDST
#include "MicroDST/ICloneFlavourTag.h"
// local
#include "CopyFlavourTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyFlavourTag
//
// 2008-04-16 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyFlavourTag );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyFlavourTag::CopyFlavourTag( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : 
  MicroDSTAlgorithm ( name , pSvcLocator ),
  m_cloner(0),
  m_tagClonerName("FlavourTagCloner")
{
  declareProperty( "ICloneFlavourTag", m_tagClonerName );
}
//=============================================================================
// Destructor
//=============================================================================
CopyFlavourTag::~CopyFlavourTag() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyFlavourTag::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_cloner = tool<ICloneFlavourTag>(m_tagClonerName, this);

  if (m_cloner) {
    debug() << "Found ICloneFlavourTag" << endmsg;
  } else {
    error() << "Failed to find ICloneFlavourTag" << endmsg;
  }

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::FlavourTagLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyFlavourTag::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store FlavourTag bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return 
    (0!=copyKeyedContainer<LHCb::FlavourTags,ICloneFlavourTag>( inputTESLocation(),
                                                                m_cloner) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyFlavourTag::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
