// $Id: CreateRawBuffer.cpp,v 1.4 2005-03-08 13:16:48 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From LHCb event model
#include "Event/RawBuffer.h"
#include "Event/EventHeader.h"

// local
#include "CreateRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CreateRawBuffer
//
// 2004-06-28 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CreateRawBuffer>          s_factory ;
const        IAlgFactory& CreateRawBufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateRawBuffer::CreateRawBuffer( const std::string& name,
                                  ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateRawBuffer::execute() {

  // Get the event header
  EventHeader* evt = get<EventHeader>( EventHeaderLocation::Default );

  // Create and register in the TES a new RawBuffer:
  RawBuffer * rawBuffer = new RawBuffer();
  if (NULL == rawBuffer ) {
    return Error( " Unable to allocate memory to RawBuffer" );
  }

  put( rawBuffer, RawBufferLocation::Default );

  // Add a DAQ bank, with zero missing sources. Assume SourceID is 1
  raw_int head[4];
  head[0] = raw_int(evt->evtNum());
  head[1] = raw_int(evt->runNum());
  head[2] = 0;  // Queuing time in SFC
  head[3] = 0;  // Number of missing sources

  // Add this block to RawBuffer:
  rawBuffer->addBank( 1, RawBuffer::DAQ, head, 4, 0 );

  return StatusCode::SUCCESS;
};
