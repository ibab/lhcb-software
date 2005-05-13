//$Id: CondDBTestAlgorithm.cpp,v 1.7 2005-05-13 16:17:50 marcocle Exp $

#include "CondDBTestAlgorithm.h"
#include "DetDesc/Condition.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IAlignment.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISlowControl.h"
#include "DetDesc/LVolume.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/IUpdateManagerSvc.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<CondDBTestAlgorithm> Factory;
const IAlgFactory& CondDBTestAlgorithmFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
CondDBTestAlgorithm::CondDBTestAlgorithm ( const std::string& name, ISvcLocator* pSvcLocator )
  : Algorithm( name, pSvcLocator ),
  m_LHCb_cond(NULL),
  m_Hcal_cond(NULL),
  m_LHCb_temp(-1e20),
  m_Hcal_temp(-1e20),
  m_avg_temp(-1e20)
{
}

//----------------------------------------------------------------------------

/// Initialize the algorithm. 
StatusCode CondDBTestAlgorithm::initialize() {
  StatusCode sc;
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "*************** initialize() ***************" << endreq;
  log << MSG::INFO << "Retrieve the LHCb detector /dd/Structure/LHCb" << endreq;
  SmartDataPtr<DetectorElement> lhcb( detSvc(), "/dd/Structure/LHCb" );

  log << MSG::INFO << "*** register conditions ***" << endreq;
  IUpdateManagerSvc *ums;
  sc = serviceLocator()->service("UpdateManagerSvc",ums,true);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to find UpdateManagerSvc" <<endmsg;
    return sc;
  }
  ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&CondDBTestAlgorithm::updateCacheLHCb);
  ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&CondDBTestAlgorithm::updateCache);
  ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&CondDBTestAlgorithm::updateCacheHcal);
  ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&CondDBTestAlgorithm::updateCache);
  ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",NULL);
  ums->update(this);
  ums->release();
 
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Process one event.
StatusCode CondDBTestAlgorithm::execute() {

  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "*************** execute(): process new event ***************" << endreq;

  log << MSG::INFO << "-------------------------------------" << endmsg;
  log << MSG::INFO << "Temperature check: LHCb = " << m_LHCb_temp << endmsg;
  log << MSG::INFO << "                   Hcal = " << m_Hcal_temp << endmsg;
  log << MSG::INFO << "                   avg  = " << m_avg_temp << endmsg;
  log << MSG::INFO << "-------------------------------------" << endmsg;
  
  static int count = 0;
  
  ++count;

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
  if (count == 4){
    log << MSG::INFO << "(Force update!)" << endreq;
    scLHCb->forceUpdateMode();
  }
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
          << pPVDO->paramNames().size() << " params" << endmsg;
      log << MSG::INFO << pPVDO->printParams() << endmsg;
    }
    log << MSG::INFO << "Now update it" << endmsg;
    sc = pVDO->update();
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
          << pPVDO->paramNames().size() << " params" << endmsg;
      log << MSG::INFO << pPVDO->printParams() << endmsg;
    }
  }
  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------------------
StatusCode CondDBTestAlgorithm::updateCacheLHCb(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "updateCacheLHCb() called!" << endmsg;
  if (m_LHCb_cond == NULL){
    DataObject *pObj;
    detSvc()->retrieveObject("/dd/SlowControl/LHCb/scLHCb",pObj);
    m_LHCb_cond = dynamic_cast<Condition *>(pObj);
    if (m_LHCb_cond == NULL) {
      log << MSG::ERROR << "Failed to retrieve /dd/SlowControl/LHCb/scLHCb" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_LHCb_temp = m_LHCb_cond->param<double>("Temperature");
  return StatusCode::SUCCESS;
}
StatusCode CondDBTestAlgorithm::updateCacheHcal(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "updateCacheHcal() called!" << endmsg;
  if (m_Hcal_cond == NULL){
    DataObject *pObj;
    detSvc()->retrieveObject("/dd/SlowControl/Hcal/scHcal",pObj);
    m_Hcal_cond = dynamic_cast<Condition *>(pObj);
    if (m_Hcal_cond == NULL) {
      log << MSG::ERROR << "Failed to retrieve /dd/SlowControl/Hcal/scHcal" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_Hcal_temp = m_Hcal_cond->param<double>("Temperature");
  return StatusCode::SUCCESS;
}
StatusCode CondDBTestAlgorithm::updateCache(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "updateCache() called!" << endmsg;
  if (m_LHCb_cond == NULL){
    DataObject *pObj;
    detSvc()->retrieveObject("/dd/SlowControl/LHCb/scLHCb",pObj);
    m_LHCb_cond = dynamic_cast<Condition *>(pObj);
    if (m_LHCb_cond == NULL) {
      log << MSG::ERROR << "Failed to retrieve /dd/SlowControl/LHCb/scLHCb" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  if (m_Hcal_cond == NULL){
    DataObject *pObj;
    detSvc()->retrieveObject("/dd/SlowControl/Hcal/scHcal",pObj);
    m_Hcal_cond = dynamic_cast<Condition *>(pObj);
    if (m_Hcal_cond == NULL) {
      log << MSG::ERROR << "Failed to retrieve /dd/SlowControl/Hcal/scHcal" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_avg_temp = (m_LHCb_cond->param<double>("Temperature")+m_Hcal_cond->param<double>("Temperature"))/2.;
  return StatusCode::SUCCESS;
}

