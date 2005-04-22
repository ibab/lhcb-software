//$Id: CondDBTestAlgorithm.cpp,v 1.4 2005-04-22 15:17:54 marcocle Exp $

#include "CondDBTestAlgorithm.h"
#include "DetDesc/Condition.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IAlignment.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISlowControl.h"
#include "DetDesc/LVolume.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<CondDBTestAlgorithm> Factory;
const IAlgFactory& CondDBTestAlgorithmFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
CondDBTestAlgorithm::CondDBTestAlgorithm ( const std::string& name, ISvcLocator* pSvcLocator )
  : Algorithm( name, pSvcLocator )
{
}

//----------------------------------------------------------------------------

/// Initialize the algorithm. 
StatusCode CondDBTestAlgorithm::initialize() {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Process one event.
StatusCode CondDBTestAlgorithm::execute() {

  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "*************** execute(): process new event ***************" << endreq;

  // Retrieve the LHCb detector element
  log << MSG::INFO << "Retrieve the LHCb detector /dd/Structure/LHCb" << endreq;
  SmartDataPtr<DetectorElement> lhcb( detSvc(), "/dd/Structure/LHCb" );
  sc = i_analyse( lhcb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve alignment for the LHCb detector
  log << MSG::INFO << "Retrieve the alignment Condition for the LHCb detector" << endreq;
  Condition* alLHCb = lhcb->alignment()->condition();
  sc = i_analyse( alLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve slowControl for the LHCb detector
  log << MSG::INFO << "Retrieve the slowControl Condition for the LHCb detector" << endreq;
  Condition* scLHCb = lhcb->slowControl()->condition();
  sc = i_analyse( scLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve logical volume for the LHCb detector
  log << MSG::INFO << "Retrieve the logical volume for the LHCb detector" << endreq;
  LVolume* lvLHCb = dynamic_cast<LVolume*>( (ILVolume*)(lhcb->geometry()->lvolume()) );
  sc = i_analyse( lvLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve the Hcal detector element
  log << MSG::INFO << "Retrieve the Hcal detector /dd/Structure/LHCb/Hcal" << endreq;
  SmartDataPtr<DetectorElement> hcal( detSvc(), "/dd/Structure/LHCb/Hcal" );
  sc = i_analyse( hcal );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve slowControl for the Hcal detector
  log << MSG::INFO << "Retrieve the slowControl Condition for the Hcal detector" << endreq;
  Condition* scHcal = hcal->slowControl()->condition();
  sc = i_analyse( scHcal );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve the Hcal detector element
  log << MSG::INFO << "Test DummyDE detector /dd/Structure/LHCb/Dummy" << endreq;
  SmartDataPtr<DetectorElement> dummy( detSvc(), "/dd/Structure/LHCb/Dummy" );
  sc = i_analyse( dummy );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Event processing completed
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Finalize the algorithm. 
StatusCode CondDBTestAlgorithm::finalize( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode CondDBTestAlgorithm::i_analyse( DataObject* pObj ) {
  MsgStream log(msgSvc(), name());
  if( 0 != pObj ) {
    log << MSG::INFO << "Successfully retrieved" << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve this DataObject" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc;  
  ValidDataObject *pVDO= dynamic_cast<ValidDataObject*>( pObj );
  if( 0 != pVDO ) {
    log << MSG::INFO << "Before update:" << endmsg;
    log << MSG::INFO << "  -> this Object is valid in [" 
        << std::dec << pVDO->validSince().absoluteTime()
        << "(0x" << std::hex << pVDO->validSince().absoluteTime() << ")," 
        << std::dec  << pVDO->validTill().absoluteTime()
        << "(0x" << std::hex << pVDO->validTill().absoluteTime()  << ")]"
        << endmsg;
    ParamValidDataObject *pPVDO= dynamic_cast<ParamValidDataObject*>( pObj );
    if ( 0 != pPVDO ){
      log << MSG::INFO << "  -> it has "
          << pPVDO->params().size() << " params and "
          << pPVDO->paramVectors().size() << " paramVectors" << endmsg;
      log << MSG::INFO << pPVDO->printParams() << endmsg;
      log << MSG::INFO << pPVDO->printParamVectors() << endmsg;
    }
    log << MSG::INFO << "Now update it" << endmsg;
    sc = detSvc()->updateObject( pObj );
  } else { // it is not a ValidDataObject, so it does not implement update()
    log << MSG::INFO << "Now update it (via DataSvc)" << endmsg;
    sc = detSvc()->updateObject( pObj );
  }
  
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Successfully updated" << endreq;
  }

  pVDO = dynamic_cast<ValidDataObject*>( pObj );
  if( 0 != pVDO ) {
    log << MSG::INFO << "After update:" << endmsg;
    log << MSG::INFO << "  -> this Object is valid in [" 
        << std::dec << pVDO->validSince().absoluteTime()
        << "(0x" << std::hex << pVDO->validSince().absoluteTime() << ")," 
        << std::dec  << pVDO->validTill().absoluteTime()
        << "(0x" << std::hex << pVDO->validTill().absoluteTime()  << ")]"
        << endmsg;
    ParamValidDataObject *pPVDO= dynamic_cast<ParamValidDataObject*>( pObj );
    if ( 0 != pPVDO ){
      log << MSG::INFO << "  -> it has "
          << pPVDO->params().size() << " params and "
          << pPVDO->paramVectors().size() << " paramVectors" << endmsg;
      log << MSG::INFO << pPVDO->printParams() << endmsg;
      log << MSG::INFO << pPVDO->printParamVectors() << endmsg;
    }
  }
  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------------------

