// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <GaudiKernel/DataObject.h>
// local
#include "MoveDataObject.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MoveDataObject
//
// 2010-09-28 : Juan Palacios
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MoveDataObject::MoveDataObject( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{ }
//=============================================================================
// Destructor
//=============================================================================
MoveDataObject::~MoveDataObject() {}
//=============================================================================
// Main execution
//=============================================================================
StatusCode MoveDataObject::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  typedef std::vector<std::string>::const_iterator stringIter;
  stringIter iLoc = this->inputTESLocations().begin();
  stringIter locEnd = this->inputTESLocations().end();


  for (; iLoc != locEnd; ++iLoc) {

    const std::string inputLocation = niceLocationName(*iLoc);

    executeLocation(inputLocation);

  }

  return StatusCode::SUCCESS;

}
//=============================================================================
void MoveDataObject::executeLocation(const std::string& objectLocation)
{

  const std::string outputLocation = this->outputTESLocation( objectLocation );

  verbose() << "Going to move DataObject from " << objectLocation
            << " to " << outputLocation << endmsg;

  DataObject* pObj = 0;

  StatusCode sc = eventSvc()->retrieveObject(objectLocation,
                                             (DataObject*&)pObj);

  if (sc.isFailure() ) {

    Error("Failed to retrieve DataObject from "+
          objectLocation,
          StatusCode::FAILURE, 0).ignore();
    return;

  } else if ( pObj)  {

    sc = eventSvc()->unregisterObject(pObj);

    if ( sc.isSuccess() )  {

      sc = eventSvc()->registerObject(outputLocation, pObj);

    } else {

      Error("Failed to move DataObject from "+
            objectLocation+ " to " + outputLocation,
            StatusCode::FAILURE, 0).ignore();

    }

    pObj->release();

  }

}
//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MoveDataObject )
