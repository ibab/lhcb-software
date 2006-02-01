// $Id: CondDBTestAlgorithm.cpp,v 1.17 2006-02-01 19:50:30 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DetDesc/Condition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/LVolume.h"

// local
#include "CondDBTestAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBTestAlgorithm
//
// 2005-06-23 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CondDBTestAlgorithm );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBTestAlgorithm::CondDBTestAlgorithm( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_LHCb_cond(NULL),
    m_Hcal_cond(NULL),
    m_LHCb_temp(-1e20),
    m_Hcal_temp(-1e20),
    m_avg_temp(-1e20),
    m_evtCount(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
CondDBTestAlgorithm::~CondDBTestAlgorithm() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CondDBTestAlgorithm::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  info() << "*** register conditions ***" << endmsg;
  try {

    // tell the update manager that we need a condition object, which pointer to set and which method to call
    registerCondition("/dd/SlowControl/LHCb/scLHCb", m_LHCb_cond, &CondDBTestAlgorithm::i_updateCacheLHCb);
    registerCondition("/dd/SlowControl/Hcal/scHcal", m_Hcal_cond, &CondDBTestAlgorithm::i_updateCacheHcal);

    // we already told the update mgr which pointers to set, we do not need to do it twice
    registerCondition("/dd/SlowControl/LHCb/scLHCb", &CondDBTestAlgorithm::i_updateCache);
    registerCondition("/dd/SlowControl/Hcal/scHcal", &CondDBTestAlgorithm::i_updateCache);

    // this line tells the update mgr that we want the following condition to be always up-to-date,
    // but we do not need to call any special method when it is updated (is it really useful?)
    registerCondition<CondDBTestAlgorithm>("/dd/SlowControl/LHCb/scLHCb");

    // we want that the following TabulatedProperty is updated and the pointer to it is put
    // in m_TabProp
    registerCondition<CondDBTestAlgorithm>("/dd/Properties/TestFunction", m_TabProp);

    registerCondition<CondDBTestAlgorithm>("/dd/Alignment/Velo/Module01", m_m01);
    registerCondition<CondDBTestAlgorithm>("/dd/Alignment/Velo/Module12", m_m12);

    m_dds = svc<IDetDataSvc>("DetectorDataSvc",true);

  }
  catch (GaudiException &e){
    fatal() << e << endmsg;
    return StatusCode::FAILURE;
  }
  
  return runUpdate();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CondDBTestAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;

  info() << "-------------------------------------" << endmsg;
  info() << "Event" << m_dds->eventTime() << endmsg;
  info() << "Temperature check: LHCb = " << m_LHCb_temp << endmsg;
  info() << "                   Hcal = " << m_Hcal_temp << endmsg;
  info() << "                   avg  = " << m_avg_temp << endmsg;
  info() << *m_TabProp << endmsg;
  info() << "-------------------------------------" << endmsg;
  
  ++m_evtCount;

  StatusCode sc;
  
  // Retrieve the LHCb detector element
  info() << "Retrieve the LHCb detector /dd/Structure/LHCb" << endmsg;
  DetectorElement *lhcb = getDet<DetectorElement>( "/dd/Structure/LHCb" );
  sc = i_analyse( lhcb );
  if( !sc.isSuccess() ) return sc;

  // Retrieve alignment for the LHCb detector
  info() << "Retrieve the alignment Condition for the LHCb detector" << endmsg;
  Condition* alLHCb = lhcb->condition("Alignment");
  sc = i_analyse( alLHCb );
  if( !sc.isSuccess() ) return sc;

  // Retrieve slowControl for the LHCb detector
  info() << "Retrieve the slowControl Condition for the LHCb detector" << endmsg;
  Condition* scLHCb = lhcb->condition("SlowControl");
  sc = i_analyse( scLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;
  if (m_evtCount == 4){
    // change the value and trigger a re-cache
    info() << "Forcing a re-cache..." << endmsg;
    scLHCb->param<double>("Temperature") = scLHCb->param<double>("Temperature")/2.0;
    info() << "Temperature set to " << scLHCb->param<double>("Temperature") << endmsg;
    info() << "UpdateManagerSvc::invalidate() ==> Condition data are considered no more valid" << endmsg;
    updMgrSvc()->invalidate(scLHCb);
  } else if (m_evtCount == 5){
    // trigger a re-load from CondDB
    info() << "Forcing a re-load from CondDB..." << endmsg;
    info() << "ValidDataObject::forceUpdateMode() + UpdateManagerSvc::invalidate()" << endmsg;
    scLHCb->forceUpdateMode();
    updMgrSvc()->invalidate(scLHCb);
  }

  // Retrieve logical volume for the LHCb detector
  info() << "Retrieve the logical volume for the LHCb detector" << endmsg;
  LVolume* lvLHCb = dynamic_cast<LVolume*>( (ILVolume*)(lhcb->geometry()->lvolume()) );
  sc = i_analyse( lvLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve the Hcal detector element
  info() << "Retrieve the Hcal detector /dd/Structure/LHCb/Hcal" << endmsg;
  DetectorElement *hcal = getDet<DetectorElement>( "/dd/Structure/LHCb/Hcal" );
  sc = i_analyse( hcal );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve slowControl for the Hcal detector
  info() << "Retrieve the slowControl Condition for the Hcal detector" << endmsg;
  Condition* scHcal = hcal->condition("SlowControl");
  sc = i_analyse( scHcal );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve the Dummy detector element
  info() << "Test DummyDE detector /dd/Structure/LHCb/Dummy" << endmsg;
  DetectorElement *dummy = getDet<DetectorElement>( "/dd/Structure/LHCb/Dummy" );
  sc = i_analyse( dummy );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Test alignment (and cool::ChannelId)
  info() << "Test AlignmentConditions detector /dd/Alignment/Velo/ModuleXX" << endmsg;

  //  info() << m_m01->name() << " dPosXYZ = " << m_m01->param<std::vector<double> >("dPosXYZ")
  //         << " transformation = \n"
  info() << m_m01->name() << ":\n"
         << m_m01->printParams()
         << "\n transformation =  \n"
         << m_m01->matrix()
         << endmsg;

  info() << m_m12->name() << ":\n"
         << m_m12->printParams()
         << "\n transformation =  \n"
         << m_m12->matrix()
         << endmsg;

  info() << "Test COOL FolderSets mapping to catalogs (get /dd/CondDBRoot/OnLine/Cave)" << endmsg;
  DataObject* dataObj = getDet<DataObject>("/dd/CondDBRoot/OnLine/Cave");
  
  if ( dataObj ) {
    info() << "Found the DataObject " << dataObj->registry()->identifier() << endmsg;
  } else {
    error() << "DataObject not found!" << endmsg;
    return StatusCode::FAILURE;
  }

  info() << "Test self-referencing XML string (get /dd/CondDBRoot/TestFolder/TestSubFolder/TestCondition)" << endmsg;
  Condition* testCond = getDet<Condition>("/dd/CondDBRoot/TestFolder/TestSubFolder/TestCondition");

  sc = i_analyse( testCond );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;
  
  // Event processing completed
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CondDBTestAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Anlyze the contnt of a DataObject
//=========================================================================
StatusCode CondDBTestAlgorithm::i_analyse( DataObject* pObj ) {
  if( 0 != pObj ) {
    info() << "Successfully retrieved" << endmsg;
  } else {
    error() << "Could not retrieve this DataObject" << endmsg;
    return StatusCode::FAILURE;
  }

  ValidDataObject *pVDO= dynamic_cast<ValidDataObject*>( pObj );

  if( 0 != pVDO ) {
    info() << "Object is valid in ["
           << std::dec << pVDO->validSince()
      //<< "(0x" << std::hex << pVDO->validSince() << "),"
           << "," 
           << std::dec << pVDO->validTill()
      //<< "(0x" << std::hex << pVDO->validTill() << ")"
           << "[" << endmsg;
    
    if ( pVDO->isValid( m_dds->eventTime() ) ) {
      info() << "(currently valid)" << endmsg;
    }
    else {
      error() << "Object is not valid at the current event time! (" << m_dds->eventTime()
              << ")" << endmsg;
      return StatusCode::FAILURE;
    }
    ParamValidDataObject *pPVDO= dynamic_cast<ParamValidDataObject*>( pObj );
    if ( 0 != pPVDO ){
      info() << "It is a ParamValidDataObject with " << pPVDO->paramNames().size() << " params" << endmsg;
      info() << "Parameters:\n" << pPVDO->printParams() << endmsg;
    }
  } else {
    info() << "(not a ValidDataObject)" << endmsg;
  }
  
  return StatusCode::SUCCESS;  
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBTestAlgorithm::i_updateCacheLHCb ( ) {
  info() << "i_updateCacheLHCb() called!" << endmsg;
  m_LHCb_temp = m_LHCb_cond->param<double>("Temperature");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBTestAlgorithm::i_updateCacheHcal ( ) {
  info() << "i_updateCacheHcal() called!" << endmsg;
  m_Hcal_temp = m_Hcal_cond->param<double>("Temperature");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBTestAlgorithm::i_updateCache ( ) {
  info() << "i_updateCache() called!" << endmsg;
  m_avg_temp = (m_LHCb_cond->param<double>("Temperature")+m_Hcal_cond->param<double>("Temperature"))/2.;
  return StatusCode::SUCCESS;
}
//=============================================================================
