// $Id: CreateL1Buffer.cpp,v 1.1 2004-06-28 11:41:03 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From LHCb event model
#include "Event/L1Buffer.h"

// local
#include "CreateL1Buffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CreateL1Buffer
//
// 2004-06-28 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CreateL1Buffer>          s_factory ;
const        IAlgFactory& CreateL1BufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateL1Buffer::CreateL1Buffer( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateL1Buffer::execute() {

  // Create and register in the TES the  L1Buffer
  L1Buffer* l1Buffer = new L1Buffer();
  if( NULL == l1Buffer ) {
    return Error( "Unable to allocate memory to L1Buffer" );
  }
  
  return put( l1Buffer, L1BufferLocation::Default );
};
