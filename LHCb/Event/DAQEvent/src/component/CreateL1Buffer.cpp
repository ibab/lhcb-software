// $Id: CreateL1Buffer.cpp,v 1.2 2005-03-08 13:16:48 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From LHCb event model
#include "Event/L1Buffer.h"
#include "Event/EventHeader.h"

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
  // Get the event header
  EventHeader* evt = get<EventHeader>( EventHeaderLocation::Default );

  // Add a DAQ bank, with zero missing sources
  l1_int head[7];
  head[0] = l1_int( (evt->evtNum()>>16)&0xFFFF );
  head[1] = l1_int( (evt->evtNum()&0xFFFF) );
  head[2] = l1_int( (evt->runNum()>>16)&0xFFFF );
  head[3] = l1_int( (evt->runNum()&0xFFFF) );
  head[4] = 0;  // Queuing time in SFC
  head[5] = 0;  // Queuing time in SFC
  head[6] = 0;  // Number of missing sources

  // Add this block to L1Buffer:
  l1Buffer->addBank( L1Buffer::SFCID, L1Buffer::DAQ, head, 7 );
  
  return put( l1Buffer, L1BufferLocation::Default );
};
