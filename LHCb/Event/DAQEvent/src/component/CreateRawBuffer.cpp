// $Id: CreateRawBuffer.cpp,v 1.1 2004-06-28 11:41:03 cattanem Exp $
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

  // Add the event header to the RawBuffer (will this be done by the DAQ?)
  // Suppose this information is manipulated by class ID 101 and has Source
  //(detector?) info 1;
  raw_int head[4];
  head[0] = raw_int(evt->runNum());
  head[1] = raw_int(evt->evtNum());
  head[2] = 0;  // reserved for event time
  head[3] = 0;  // reserved for event time

  // Add this block to RawBuffer:
  rawBuffer->addBank( 1, RawBuffer::Header, head, 2 );

  return StatusCode::SUCCESS;
};
